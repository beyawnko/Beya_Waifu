#include "mainwindow.h"
#include "RealCuganProcessor.h" // Ensure this is included
#include "RealcuganJobManager.h"
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

// MainWindow::Realcugan_NCNN_Vulkan_PrepareArguments has been removed.

/**
 * @brief Run RealCUGAN on a single image.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Image(int file_list_row_number, bool isBatch, bool /*unused_flag*/)
{
    if (isProcessing && !isBatch) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }
    if(isBatch && isProcessing) return;

    isProcessing = true;

    realCuganProcessor->readSettings();

    QString input_path = table_image_item_fullpath.at(file_list_row_number);
    QString original_fileName = table_image_item_fileName.at(file_list_row_number);
    // Use Generate_Output_Path from MainWindow, ensure it's suitable or adjust suffix
    QString output_path = Generate_Output_Path(original_fileName, QString("RealCUGAN_%1_x%2_denoise%3")
                                               .arg(m_realcugan_Model)
                                               .arg(m_realcugan_Scale)
                                               .arg(m_realcugan_DenoiseLevel));


    int current_scale = m_realcugan_Scale; // Base scale from settings
    // Check if there's a specific UI element for image scale that might override
    if (ui->spinBox_Scale_RealCUGAN) { // Assuming spinBox_Scale_RealCUGAN holds the desired scale for the current operation
        current_scale = ui->spinBox_Scale_RealCUGAN->value();
    } else if (ui->doubleSpinBox_ScaleRatio_image) { // Fallback or alternative
         current_scale = static_cast<int>(ui->doubleSpinBox_ScaleRatio_image->value());
    }

    QString outputFormat = ui->comboBox_ImageSaveFormat ? ui->comboBox_ImageSaveFormat->currentText().toLower() : "png";

    QString jobsStr = Settings_Read_value("/settings/RealCUGANJobsImage",
                                         QString("1:1:1")).toString();
    QString syncGapStr = Settings_Read_value("/settings/RealCUGANSyncGapImage",
                                          QString("3")).toString();
    // bool verboseLog = Settings_Read_value("/settings/RealCUGANVerboseLog",
    //                                      QVariant(false)).toBool();
    bool verboseLog = false; // Defaulting to false, or read from settings if still needed by prepareArguments

    bool isMultiGpu = ui->checkBox_MultiGPU_RealCUGAN ? ui->checkBox_MultiGPU_RealCUGAN->isChecked() : false;
    QString multiGpuJobArgsString;
    if(isMultiGpu) {
        multiGpuJobArgsString = RealcuganNcnnVulkan_MultiGPU();
    }

    QStringList arguments = realCuganProcessor->prepareArguments(
        input_path,
        output_path,
        current_scale,
        m_realcugan_Model,
        m_realcugan_DenoiseLevel,
        m_realcugan_TileSize,
        m_realcugan_GPUID,
        m_realcugan_TTA,
        outputFormat,
        isMultiGpu,
        multiGpuJobArgsString,
        false, // experimental flag
        jobsStr,
        syncGapStr,
        verboseLog
    );

    QString executableFullPath = realCuganProcessor->executablePath(false);
    QDir engineDir = QFileInfo(executableFullPath).absoluteDir();

    if (!currentProcess) currentProcess = new QProcess(this);
    currentProcess->disconnect();

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_finished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError()));

    qDebug() << "[MW::Realcugan_Image] Starting RealCUGAN:" << executableFullPath;
    qDebug() << "[MW::Realcugan_Image] Args:" << arguments.join(" ");

    currentProcess->setWorkingDirectory(engineDir.absolutePath());
    currentProcess->start(executableFullPath, arguments);

    Set_Progress_Bar_Value(0, 0);
    Set_Current_File_Progress_Bar_Value(0,0);
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    if (!isBatch) {
        pushButton_Start_setEnabled_self(false);
    }
    pushButton_Stop_setEnabled_self(true);
    Processing_Status = PROCESS_TYPE_IMAGE;
    current_File_Row_Number = file_list_row_number;
    Table_image_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, tr("Processing..."));
}


void MainWindow::Realcugan_NCNN_Vulkan_Video_BySegment(int rowNum)
{
    bool delOriginal = ui->checkBox_DelOriginal->isChecked() ||
                       ui->checkBox_ReplaceOriginalFile->isChecked();
    int segmentDuration = ui->spinBox_VideoSplitDuration ? ui->spinBox_VideoSplitDuration->value() : 10;

    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
    QString srcPath = Table_model_video->item(rowNum, 1)->text();
    if (!QFile::exists(srcPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") + srcPath + tr("]. Error: [File does not exist.]"));
        Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); // Direct call
        Send_progressbar_Add(); // Direct call
        return;
    }

    QFileInfo finfo(srcPath);
    QString baseName = file_getBaseName(srcPath);
    QString suffix = finfo.suffix();
    QString folder = file_getFolderPath(finfo);

    QString cfrVideo = video_To_CFRMp4(srcPath);
    if (!QFile::exists(cfrVideo))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") + srcPath + tr("]. Error: [Cannot convert video format to mp4.]"));
        Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        Send_progressbar_Add();
        return;
    }

    QString audioPath = folder + "/Audio_" + baseName + "_" + suffix + "_W2xEX.wav";
    if (!QFile::exists(audioPath))
        video_get_audio(cfrVideo, audioPath);

    QString framesPath = folder + "/" + baseName + "_" + suffix + "_SplitFrames_W2xEX";
    QString clipsPath;
    QString dateStr;
    do {
        dateStr = video_getClipsFolderNo();
        clipsPath = folder + "/" + dateStr + "_VideoClipsWaifu2xEX";
    } while (file_isDirExist(clipsPath));
    QString clipsName = dateStr + "_VideoClipsWaifu2xEX";

    int duration = video_get_duration(cfrVideo);
    int startTime = 0;
    int clipIndex = 0;

    if (ui->checkBox_ShowInterPro && ui->checkBox_ShowInterPro->isChecked() && duration > segmentDuration)
        emit Send_CurrentFileProgress_Start(finfo.fileName(), duration);

    realCuganProcessor->readSettingsVideoGif(0);

    QString jobsStr = Settings_Read_value("/settings/RealCUGANJobsVideo",
                                         QString("1:2:2")).toString();
    QString syncGapStr = Settings_Read_value("/settings/RealCUGANSyncGapVideo",
                                          QString("3")).toString();
    // bool verboseLog = Settings_Read_value("/settings/RealCUGANVerboseLog",
    //                                      QVariant(false)).toBool();
    bool verboseLog = false; // Defaulting to false, or read from settings if still needed by prepareArguments

    bool isMultiGpu = ui->checkBox_MultiGPU_RealCUGAN ? ui->checkBox_MultiGPU_RealCUGAN->isChecked() : false;
    QString multiGpuJobArgsString;
    if(isMultiGpu) {
        multiGpuJobArgsString = RealcuganNcnnVulkan_MultiGPU();
    }

    int videoScale = m_realcugan_Scale;
    if (ui->spinBox_Scale_RealCUGAN) { // General RealCUGAN scale setting
         videoScale = ui->spinBox_Scale_RealCUGAN->value();
    }
    // If there's a video-specific scale UI, it should be preferred:
    // if (ui->doubleSpinBox_ScaleRatio_video) videoScale = static_cast<int>(ui->doubleSpinBox_ScaleRatio_video->value());


    while (duration > startTime)
    {
        int timeLeft = duration - startTime;
        int segDur = timeLeft >= segmentDuration ? segmentDuration : timeLeft;

        if (file_isDirExist(framesPath)) file_DelDir(framesPath);
        file_mkDir(framesPath);

        video_video2images_ProcessBySegment(cfrVideo, framesPath, startTime, segDur);
        QStringList frameNames = file_getFileNames_in_Folder_nofilter(framesPath);
        if (frameNames.isEmpty()) { /* ... error handling ... */ Send_progressbar_Add(); return; }

        QString scaledFramesPath = framesPath + "_Scaled";
        file_mkDir(scaledFramesPath);

        for (const QString &frame : frameNames) {
            QString inFile = framesPath + "/" + frame;
            QString outFile = scaledFramesPath + "/" + frame;

            // Realcugan_ProcessSingleFileIteratively is in mainwindow.cpp
            // It will use realCuganProcessor->prepareArguments itself.
            // We must ensure the parameters passed here are consistent with its needs.
            // The jobsStr, syncGapStr, verboseLog are now part of prepareArguments,
            // so Realcugan_ProcessSingleFileIteratively needs to be updated to pass them.
            if (!Realcugan_ProcessSingleFileIteratively(inFile, outFile, videoScale, 0,0,
                                                       m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
                                                       m_realcugan_GPUID, // This is the single GPU ID
                                                       isMultiGpu, // This indicates if multi-GPU mode is generally on
                                                       m_realcugan_TTA, "png",
                                                       false, // experimental
                                                       rowNum))
            {
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
                Send_progressbar_Add();
                return;
            }
        }

        if (!file_isDirExist(clipsPath)) file_mkDir(clipsPath);
        clipIndex++;
        QString clip = clipsPath + "/" + QString::number(clipIndex, 10) + ".mp4";
        video_images2video(cfrVideo, clip, scaledFramesPath, "", false, 1, 1, false); // Args might need review for custom res
        if (!QFile::exists(clip)) { /* ... error handling ... */ Send_progressbar_Add(); return; }

        if (ui->checkBox_ShowInterPro && ui->checkBox_ShowInterPro->isChecked())
            emit Send_CurrentFileProgress_progressbar_Add_SegmentDuration(segDur);

        startTime += segDur;
        file_DelDir(framesPath);
        file_DelDir(scaledFramesPath);
    }

    if (ui->checkBox_ShowInterPro && ui->checkBox_ShowInterPro->isChecked()) emit Send_CurrentFileProgress_Stop();

    QString finalVideoPathSuffix = QString("RealCUGAN_%1_x%2_denoise%3").arg(m_realcugan_Model).arg(videoScale).arg(m_realcugan_DenoiseLevel);
    QString finalVideo = Generate_Output_Path(finfo.fileName(), finalVideoPathSuffix);


    QFile::remove(finalVideo); // Remove if exists before assembling
    video_AssembleVideoClips(clipsPath, clipsName, finalVideo, audioPath);
    if (!QFile::exists(finalVideo)) { /* ... error handling ... */ Send_progressbar_Add(); return; }

    if (delOriginal) ReplaceOriginalFile(srcPath, finalVideo);

    Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
    Send_progressbar_Add();
}


void MainWindow::Realcugan_NCNN_Vulkan_Video(int file_list_row_number)
{
    if (isProcessing) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }
    isProcessing = true;

    realCuganProcessor->readSettingsVideoGif(0);

    QString input_path_frames_dir = TempDir_Path + "/" + table_video_item_fileName.at(file_list_row_number) + "_frames/";
    QString original_fileName = table_video_item_fileName.at(file_list_row_number);
    QString output_path_frames_dir = TempDir_Path + "/" + original_fileName + "_frames_RealCUGAN_NCNN_Vulkan/";

    int scale = m_realcugan_Scale;
     if (ui->spinBox_Scale_RealCUGAN){
        scale = ui->spinBox_Scale_RealCUGAN->value();
     } else if (ui->doubleSpinBox_ScaleRatio_video){ // Fallback or alternative video specific scale
        scale = static_cast<int>(ui->doubleSpinBox_ScaleRatio_video->value());
     }

    QString outputFormat = "png";

    QString jobsStr = Settings_Read_value("/settings/RealCUGANJobsVideo",
                                         QString("1:2:2")).toString();
    QString syncGapStr = Settings_Read_value("/settings/RealCUGANSyncGapVideo",
                                          QString("3")).toString();
    bool verboseLog = Settings_Read_value("/settings/RealCUGANVerboseLog",
                                         QVariant(false)).toBool();

    bool isMultiGpu = ui->checkBox_MultiGPU_RealCUGAN ? ui->checkBox_MultiGPU_RealCUGAN->isChecked() : false;
    QString multiGpuJobArgsString;
    if(isMultiGpu) {
        multiGpuJobArgsString = RealcuganNcnnVulkan_MultiGPU();
    }

    QStringList arguments = realCuganProcessor->prepareArguments(
        input_path_frames_dir,
        output_path_frames_dir,
        scale,
        m_realcugan_Model,
        m_realcugan_DenoiseLevel,
        m_realcugan_TileSize,
        m_realcugan_GPUID,
        m_realcugan_TTA,
        outputFormat,
        isMultiGpu,
        multiGpuJobArgsString,
        false, // experimental
        jobsStr,
        syncGapStr,
        verboseLog
    );

    QString executableFullPath = realCuganProcessor->executablePath(false);
    QDir engineDir = QFileInfo(executableFullPath).absoluteDir();

    if (!currentProcess) currentProcess = new QProcess(this);
    currentProcess->disconnect();

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_finished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError()));

    qDebug() << "[MW::Realcugan_Video] Starting RealCUGAN for video frames:" << input_path_frames_dir;
    qDebug() << "[MW::Realcugan_Video] Args:" << arguments.join(" ");

    currentProcess->setWorkingDirectory(engineDir.absolutePath());
    currentProcess->start(executableFullPath, arguments);

    Set_Progress_Bar_Value(0, 0);
    Set_Current_File_Progress_Bar_Value(0,0);
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    pushButton_Start_setEnabled_self(false);
    pushButton_Stop_setEnabled_self(true);
    Processing_Status = PROCESS_TYPE_VIDEO;
    current_File_Row_Number = file_list_row_number;
    Table_video_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, tr("Processing Frames..."));
}


void MainWindow::Realcugan_NCNN_Vulkan_GIF(int file_list_row_number)
{
    if (isProcessing) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }
    isProcessing = true;

    realCuganProcessor->readSettingsVideoGif(0);

    QString input_path_frames_dir = TempDir_Path + "/" + table_gif_item_fileName.at(file_list_row_number) + "_frames/";
    QString original_fileName = table_gif_item_fileName.at(file_list_row_number);
    QString output_path_frames_dir = TempDir_Path + "/" + original_fileName + "_frames_RealCUGAN_NCNN_Vulkan/";

    int scale = m_realcugan_Scale;
    if(ui->spinBox_Scale_RealCUGAN) { // General RealCUGAN scale
        scale = ui->spinBox_Scale_RealCUGAN->value();
    } else if(ui->doubleSpinBox_ScaleRatio_gif) { // GIF specific scale
        scale = static_cast<int>(ui->doubleSpinBox_ScaleRatio_gif->value());
    }

    QString outputFormat = "png";

    QString jobsStr = Settings_Read_value("/settings/RealCUGANJobsGif",
                                         QString("1:2:2")).toString();
    QString syncGapStr = Settings_Read_value("/settings/RealCUGANSyncGapGif",
                                          QString("3")).toString();
    // bool verboseLog = Settings_Read_value("/settings/RealCUGANVerboseLog",
    //                                      QVariant(false)).toBool();
    bool verboseLog = false; // Defaulting to false, or read from settings if still needed by prepareArguments

    bool isMultiGpu = ui->checkBox_MultiGPU_RealCUGAN ? ui->checkBox_MultiGPU_RealCUGAN->isChecked() : false;
    QString multiGpuJobArgsString;
    if(isMultiGpu) {
        multiGpuJobArgsString = RealcuganNcnnVulkan_MultiGPU();
    }

    QStringList arguments = realCuganProcessor->prepareArguments(
        input_path_frames_dir,
        output_path_frames_dir,
        scale,
        m_realcugan_Model,
        m_realcugan_DenoiseLevel,
        m_realcugan_TileSize,
        m_realcugan_GPUID,
        m_realcugan_TTA,
        outputFormat,
        isMultiGpu,
        multiGpuJobArgsString,
        false, // experimental
        jobsStr,
        syncGapStr,
        verboseLog
    );

    QString executableFullPath = realCuganProcessor->executablePath(false);
    QDir engineDir = QFileInfo(executableFullPath).absoluteDir();

    if (!currentProcess) currentProcess = new QProcess(this);
    currentProcess->disconnect();

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_finished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError()));

    qDebug() << "[MW::Realcugan_GIF] Starting RealCUGAN for GIF frames:" << input_path_frames_dir;
    qDebug() << "[MW::Realcugan_GIF] Args:" << arguments.join(" ");

    currentProcess->setWorkingDirectory(engineDir.absolutePath());
    currentProcess->start(executableFullPath, arguments);

    Set_Progress_Bar_Value(0, 0);
    Set_Current_File_Progress_Bar_Value(0,0);
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    pushButton_Start_setEnabled_self(false);
    pushButton_Stop_setEnabled_self(true);
    Processing_Status = PROCESS_TYPE_GIF;
    current_File_Row_Number = file_list_row_number;
    Table_gif_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, tr("Processing Frames..."));
}


void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings()
{
    if (realCuganProcessor)
        realCuganProcessor->readSettings();

    if (this->ui->spinBox_Scale_RealCUGAN) // Check if ui and spinBox are valid
         m_realcugan_Scale = this->ui->spinBox_Scale_RealCUGAN->value();
    else
         m_realcugan_Scale =
             Settings_Read_value("RealCUGAN_Scale", QVariant(2)).toInt();


    qDebug() << "[MW::R_ReadSettings] Model:" << m_realcugan_Model
             << "Scale:" << m_realcugan_Scale
             << "Denoise:" << m_realcugan_DenoiseLevel
             << "Tile:" << m_realcugan_TileSize
             << "GPUID:" << m_realcugan_GPUID;
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int threadNum)
{
    if (realCuganProcessor)
        realCuganProcessor->readSettingsVideoGif(threadNum);
    qDebug() << "[MW::R_ReadSettings_VG] (Thread " << threadNum << ") Settings re-read by RealCuganProcessor.";
}

void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings()
{
    if (realCuganProcessor)
        realCuganProcessor->preLoadSettings();
    qDebug() << "[MW::R_PreLoad] RealCuganProcessor::preLoadSettings() called.";
}

void MainWindow::Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString &tempPathBase,
                                                        int /*maxPassIndex*/,
                                                        bool keepFinal,
                                                        const QString &finalFile)
{
    if (keepFinal && !finalFile.isEmpty() && QFile::exists(finalFile) )
        return;

    const QString framesDir = tempPathBase + "_frames";
    const QString scaledDir = framesDir + "_RealCUGAN_NCNN_Vulkan";

    const QString clipsDir = tempPathBase + "_VideoClipsWaifu2xEX";

    auto removeDirSafely = [this](const QString &path) {
        if (file_isDirExist(path)) {
            if (!file_DelDir(path))
                qDebug() << "Failed to delete directory:" << path;
        } else {
            //qDebug() << "Path does not exist, skipping delete:" << path;
        }
    };

    removeDirSafely(framesDir);
    removeDirSafely(scaledDir);
    removeDirSafely(clipsDir);

    if (!keepFinal && !finalFile.isEmpty() && QFile::exists(finalFile)) {
        if (!QFile::remove(finalFile))
            qDebug() << "Failed to delete final (intermediate) file:" << finalFile;
    }
}

static QStringList parseVulkanDeviceList(const QString &output)
{
    QStringList list;
    QRegularExpression re(QStringLiteral("^\\s*(?:GPU device\\s*)?\\[?(\\d+)\\]?[:\\-]?\\s*(.+)$"),
                           QRegularExpression::CaseInsensitiveOption |
                           QRegularExpression::MultilineOption);
    auto it = re.globalMatch(output);
    while (it.hasNext())
    {
        auto m = it.next();
        list << QStringLiteral("%1: %2").arg(m.captured(1).trimmed(), m.captured(2).trimmed());
    }
    return list;
}

void MainWindow::Realcugan_ncnn_vulkan_DetectGPU()
{
    QPushButton* detectButton = ui->pushButton_DetectGPU_RealCUGAN ? ui->pushButton_DetectGPU_RealCUGAN : ui->pushButton_DetectGPU_VFI;
    if (!detectButton) return;

    detectButton->setEnabled(false);
    detectButton->setText(tr("Detecting..."));

    QString exePath = realCuganProcessor->executablePath(false);

    QProcess proc;
    QByteArray out, err;
    runProcess(&proc, QString("\"%1\" -h").arg(exePath), &out, &err);
    QString text = QString::fromLocal8Bit(out + err);
    Available_GPUID_RealCUGAN = parseVulkanDeviceList(text);

    Realcugan_NCNN_Vulkan_DetectGPU_finished();
}

void MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error)
{
    qWarning() << "Error during RealCUGAN GPU detection, process error:" << error;
    QPushButton* detectButton = ui->pushButton_DetectGPU_RealCUGAN ? ui->pushButton_DetectGPU_RealCUGAN : ui->pushButton_DetectGPU_VFI;
    if (detectButton){
        detectButton->setEnabled(true);
        detectButton->setText(tr("Detect GPU"));
    }
}
