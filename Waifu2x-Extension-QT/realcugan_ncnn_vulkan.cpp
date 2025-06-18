#include "mainwindow.h"
#include "RealcuganJobManager.h"
#include <QDebug>
#include <QSettings>

// Stub implementations to satisfy the build
// Note: The incorrect static definitions that were here previously have been removed.
// Instance members are declared in mainwindow.h and initialized in mainwindow.cpp constructor.
// Functions defined as MainWindow::FunctionName should have their declarations in mainwindow.h.
// Removing runProcess stub from here as it's defined in mainwindow.cpp

QStringList MainWindow::Realcugan_NCNN_Vulkan_PrepareArguments(
    const QString& input_path,
    const QString& output_path,
    int noise_level,
    const QString& model_dir_name, // This is just the name like "models-se", not the full path yet
    int scale,
    int gpu_id,
    const QString& format,
    bool tta_mode,
    const QString& tile_size_str,
    bool verbose_log,
    const QString& syncgap_str, // syncgap mode as string
    bool isVideoOrGif // Used to potentially adjust job settings, not a direct engine arg
)
{
    QStringList args;

    // -i input-path
    args << "-i" << input_path;

    // -o output-path
    args << "-o" << output_path;

    // -n noise-level
    args << "-n" << QString::number(noise_level);

    // -s scale
    args << "-s" << QString::number(scale);

    // -t tile-size
    if (!tile_size_str.isEmpty() && tile_size_str != "0" && tile_size_str != "auto") {
        args << "-t" << tile_size_str;
    } else {
        args << "-t" << "0"; // Auto tile size
    }

    // -m model-path (This should be the path to the model directory, e.g. "models-se")
    // Similar to RealESRGAN, this needs to be correctly resolved relative to the executable.
    // The parameter model_dir_name is just the directory name.
    args << "-m" << model_dir_name;


    // -g gpu-id
    args << "-g" << QString::number(gpu_id);

    // -j load:proc:save
    // Use different job settings for video/GIF vs static images if needed,
    // otherwise, use a general setting.
    QString jobs_key = isVideoOrGif ? "/settings/RealCUGANJobsVideo" : "/settings/RealCUGANJobsImage";
    QString default_jobs = isVideoOrGif ? "1:2:2" : "1:1:1"; // Example: more save/load threads for video
    QString jobs_read = Settings_Read_value(jobs_key).toString();
    if (jobs_read.isEmpty()) jobs_read = default_jobs;
    QString jobs = jobs_read;
    args << "-j" << jobs;

    // -c syncgap-mode
    if (!syncgap_str.isEmpty()) {
        bool ok;
        int syncgap_val = syncgap_str.toInt(&ok);
        if (ok && syncgap_val >= 0 && syncgap_val <=3) {
             args << "-c" << syncgap_str;
        } else {
            args << "-c" << "3"; // Default if invalid
            qDebug() << "Invalid syncgap value provided:" << syncgap_str << ". Defaulting to 3.";
        }
    } else {
        args << "-c" << "3"; // Default if empty
    }


    // -f format
    if (!format.isEmpty()) {
        args << "-f" << format;
    }

    // -x tta_mode
    if (tta_mode) {
        args << "-x";
    }

    // -v verbose_log
    if (verbose_log) {
        args << "-v";
    }

    return args;
}

/**
 * @brief Run RealCUGAN on a single image.
 *
 * Falls back to stored settings when widgets are absent.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Image(int file_list_row_number, bool isBatch, bool /*unused_flag*/)
{
    if (isProcessing) {
        if (isBatch) return;
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }

    isProcessing = true;
    currentProcess = new QProcess(this);

    QString enginePath = Current_Path + "/Engines/realcugan-ncnn-vulkan/";
    QString executableName = "realcugan-ncnn-vulkan.exe"; // Assuming Windows
    QString executableFullPath = enginePath + executableName;

    QString input_path = table_image_item_fullpath.at(file_list_row_number);
    QString original_fileName = table_image_item_fileName.at(file_list_row_number);
    QString output_path = Generate_Output_Path(original_fileName, "RealCUGAN-NCNN-Vulkan");

    // --- Get parameters from UI/Settings (Placeholders for RealCUGAN specific UI) ---
    int noise_level = 0;
    if (ui->spinBox_DenoiseLevel_image)
        noise_level = ui->spinBox_DenoiseLevel_image->value();
    else
        noise_level = Settings_Read_value("/settings/RealCUGAN_DenoiseLevel").toInt();
    // RealCUGAN model dir is typically 'models-se', 'models-pro', 'models-nose'.
    // This needs a dedicated ComboBox in UI, e.g. ui->comboBox_ModelDir_RealCUGAN
    QString model_dir_name_read = Settings_Read_value("/settings/RealCUGANModelDir").toString(); // Placeholder
    if (model_dir_name_read.isEmpty()) model_dir_name_read = "models-se";
    QString model_dir_name = model_dir_name_read;
    int scale = 2;
    if (ui->doubleSpinBox_ScaleRatio_image)
        scale = ui->doubleSpinBox_ScaleRatio_image->value();
    else
        scale = Settings_Read_value("/settings/ImageScaleRatio").toInt();
    int gpu_id_read = Settings_Read_value("/settings/RealCUGANGpuId").toInt(); // Placeholder, e.g. ui->comboBox_GPUID_RealCUGAN
    // Assuming -1 or default int() if conversion fails is acceptable.
    int gpu_id = gpu_id_read;
    QString format = "png";
    if (ui->comboBox_ImageSaveFormat)
        format = ui->comboBox_ImageSaveFormat->currentText();
    else
        format = Settings_Read_value("/settings/ImageEXT").toString();
    bool tta_mode_read = Settings_Read_value("/settings/RealCUGANTTAMode").toBool(); // Placeholder, e.g. ui->checkBox_TTA_RealCUGAN
    // Assuming false if conversion fails is acceptable.
    bool tta_mode = tta_mode_read;
    QString tile_size_str_read = Settings_Read_value("/settings/RealCUGANTileSize").toString(); // Placeholder
    if (tile_size_str_read.isEmpty()) tile_size_str_read = "0";
    QString tile_size_str = tile_size_str_read;
    bool verbose_log = true;
    QString syncgap_str_read = Settings_Read_value("/settings/RealCUGANSyncGap").toString(); // Placeholder
    if (syncgap_str_read.isEmpty()) syncgap_str_read = "3";
    QString syncgap_str = syncgap_str_read;

    QStringList arguments = Realcugan_NCNN_Vulkan_PrepareArguments(
        input_path, output_path, noise_level, model_dir_name, scale, gpu_id, format,
        tta_mode, tile_size_str, verbose_log, syncgap_str, false /*isVideoOrGif=false*/
    );

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessStdOut()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onProcessStdErr()));

    qDebug() << "Starting RealCUGAN for image:" << input_path;
    qDebug() << "Arguments:" << arguments.join(" ");
    qDebug() << "Executable:" << executableFullPath;

    currentProcess->setWorkingDirectory(enginePath);
    currentProcess->start(executableFullPath, arguments);

    Set_Progress_Bar_Value(0, 0);
    Set_Current_File_Progress_Bar_Value(0,0);
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
    Processing_Status = PROCESS_TYPE_IMAGE;
    current_File_Row_Number = file_list_row_number;
}


void MainWindow::Realcugan_NCNN_Vulkan_Video_BySegment(int rowNum)
{
    bool delOriginal = ui->checkBox_DelOriginal->isChecked() ||
                       ui->checkBox_ReplaceOriginalFile->isChecked();
    int segmentDuration = ui->spinBox_SegmentDuration->value();

    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum,
                                                               "Processing");
    QString srcPath = Table_model_video->item(rowNum, 2)->text();
    if (!QFile::exists(srcPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") +
                                         srcPath + tr("]. Error: [File does not exist.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum,
                                                                   "Failed");
        emit Send_progressbar_Add();
        return;
    }

    QFileInfo finfo(srcPath);
    QString baseName = file_getBaseName(srcPath);
    QString suffix = finfo.suffix();
    QString folder = file_getFolderPath(finfo);

    QString cfrVideo = video_To_CFRMp4(srcPath);
    if (!QFile::exists(cfrVideo))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") +
                                         srcPath +
                                         tr("]. Error: [Cannot convert video format to mp4.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum,
                                                                   "Failed");
        emit Send_progressbar_Add();
        return;
    }

    QString audioPath = folder + "/Audio_" + baseName + "_" + suffix + "_W2xEX.wav";
    if (!QFile::exists(audioPath))
        video_get_audio(cfrVideo, audioPath);

    QString framesPath = folder + "/" + baseName + "_" + suffix + "_SplitFrames_W2xEX";
    QString clipsPath;
    QString dateStr;
    do
    {
        dateStr = video_getClipsFolderNo();
        clipsPath = folder + "/" + dateStr + "_VideoClipsWaifu2xEX";
    }
    while (file_isDirExist(clipsPath));
    QString clipsName = dateStr + "_VideoClipsWaifu2xEX";

    int duration = video_get_duration(cfrVideo);
    int startTime = 0;
    int clipIndex = 0;

    if (ui->checkBox_ShowInterPro->isChecked() && duration > segmentDuration)
        emit Send_CurrentFileProgress_Start(baseName + "." + suffix, duration);

    while (duration > startTime)
    {
        int timeLeft = duration - startTime;
        int segDur = timeLeft >= segmentDuration ? segmentDuration : timeLeft;

        if (file_isDirExist(framesPath))
            file_DelDir(framesPath);
        file_mkDir(framesPath);

        video_video2images_ProcessBySegment(cfrVideo, framesPath, startTime, segDur);
        QStringList frameNames = file_getFileNames_in_Folder_nofilter(framesPath);
        if (frameNames.isEmpty())
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") +
                                             srcPath + tr("]. Error: [Unable to split video into pictures.]"));
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum,
                                                                       "Failed");
            emit Send_progressbar_Add();
            return;
        }

        QString scaledFramesPath = framesPath + "_Scaled";
        file_mkDir(scaledFramesPath);
        for (const QString &frame : frameNames)
        {
            QString inFile = framesPath + "/" + frame;
            QString outFile = scaledFramesPath + "/" + frame;
            if (!Realcugan_ProcessSingleFileIteratively(inFile,
                                                       outFile,
                                                       m_realcugan_Scale,
                                                       0,
                                                       0,
                                                       m_realcugan_Model,
                                                       m_realcugan_DenoiseLevel,
                                                       m_realcugan_TileSize,
                                                       m_realcugan_GPUID,
                                                       false,
                                                       m_realcugan_TTA,
                                                       "png",
                                                       false,
                                                       rowNum))
            {
                emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum,
                                                                           "Failed");
                emit Send_progressbar_Add();
                return;
            }
        }

        if (!file_isDirExist(clipsPath))
            file_mkDir(clipsPath);
        clipIndex++;
        QString clip = clipsPath + "/" + QString::number(clipIndex, 10) + ".mp4";
        video_images2video(cfrVideo, clip, scaledFramesPath, "", false, 1, 1, false);
        if (!QFile::exists(clip))
        {
            emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") +
                                             srcPath + tr("]. Error: [Unable to assemble pictures into videos.]"));
            emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum,
                                                                       "Failed");
            emit Send_progressbar_Add();
            return;
        }

        if (ui->checkBox_ShowInterPro->isChecked())
            emit Send_CurrentFileProgress_progressbar_Add_SegmentDuration(segDur);

        startTime += segDur;
        file_DelDir(framesPath);
        file_DelDir(scaledFramesPath);
    }

    emit Send_CurrentFileProgress_Stop();

    QString finalVideo = folder + "/" + baseName + "_RealCUGAN_" + suffix + ".mp4";
    QFile::remove(finalVideo);
    video_AssembleVideoClips(clipsPath, clipsName, finalVideo, audioPath);
    if (!QFile::exists(finalVideo))
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [") +
                                         srcPath + tr("]. Error: [Unable to assemble video clips.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        emit Send_progressbar_Add();
        return;
    }

    if (delOriginal)
        ReplaceOriginalFile(srcPath, finalVideo);

    emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
    emit Send_progressbar_Add();
}

/**
 * @brief Process a video by applying RealCUGAN to extracted frames.
 *
 * Settings provide defaults when widgets are not available.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Video(int file_list_row_number)
{
    if (isProcessing) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }

    isProcessing = true;
    currentProcess = new QProcess(this);

    QString enginePath = Current_Path + "/Engines/realcugan-ncnn-vulkan/";
    QString executableName = "realcugan-ncnn-vulkan.exe"; // Assuming Windows
    QString executableFullPath = enginePath + executableName;

    QString input_path = TempDir_Path + "/" + table_video_item_fileName.at(file_list_row_number) + "_frames/";
    QString original_fileName = table_video_item_fileName.at(file_list_row_number);
    QString output_path = TempDir_Path + "/" + original_fileName + "_frames_RealCUGAN_NCNN_Vulkan/";

    // --- Get parameters from UI/Settings (Placeholders for RealCUGAN specific UI) ---
    int noise_level = 0;
    if (ui->spinBox_DenoiseLevel_video)
        noise_level = ui->spinBox_DenoiseLevel_video->value();
    else
        noise_level = Settings_Read_value("/settings/VideoDenoiseLevel").toInt();
    QString model_dir_name_read = Settings_Read_value("/settings/RealCUGANModelDirVideo").toString(); // Placeholder
    if (model_dir_name_read.isEmpty()) model_dir_name_read = "models-se";
    QString model_dir_name = model_dir_name_read;
    int scale = 2;
    if (ui->doubleSpinBox_ScaleRatio_video)
        scale = ui->doubleSpinBox_ScaleRatio_video->value();
    else
        scale = Settings_Read_value("/settings/VideoScaleRatio").toInt();
    int gpu_id_read = Settings_Read_value("/settings/RealCUGANGpuIdVideo").toInt(); // Placeholder
    int gpu_id = gpu_id_read;
    QString format = "png"; // Process frames losslessly
    bool tta_mode_read = Settings_Read_value("/settings/RealCUGANTTAModeVideo").toBool(); // Placeholder
    bool tta_mode = tta_mode_read;
    QString tile_size_str_read = Settings_Read_value("/settings/RealCUGANTileSizeVideo").toString(); // Placeholder
    if (tile_size_str_read.isEmpty()) tile_size_str_read = "0";
    QString tile_size_str = tile_size_str_read;
    bool verbose_log = true;
    QString syncgap_str_read = Settings_Read_value("/settings/RealCUGANSyncGapVideo").toString(); // Placeholder
    if (syncgap_str_read.isEmpty()) syncgap_str_read = "3";
    QString syncgap_str = syncgap_str_read;

    QStringList arguments = Realcugan_NCNN_Vulkan_PrepareArguments(
        input_path, output_path, noise_level, model_dir_name, scale, gpu_id, format,
        tta_mode, tile_size_str, verbose_log, syncgap_str, true /*isVideoOrGif=true*/
    );

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessStdOut()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onProcessStdErr()));

    qDebug() << "Starting RealCUGAN for video (frames):" << input_path;
    qDebug() << "Arguments:" << arguments.join(" ");
    qDebug() << "Executable:" << executableFullPath;

    currentProcess->setWorkingDirectory(enginePath);
    currentProcess->start(executableFullPath, arguments);

    Set_Progress_Bar_Value(0, 0);
    Set_Current_File_Progress_Bar_Value(0,0);
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
    Processing_Status = PROCESS_TYPE_VIDEO;
    current_File_Row_Number = file_list_row_number;
}

/**
 * @brief Upscale GIF frames using RealCUGAN.
 *
 * Uses stored settings if corresponding widgets are missing.
 */
void MainWindow::Realcugan_NCNN_Vulkan_GIF(int file_list_row_number)
{
    if (isProcessing) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }

    isProcessing = true;
    currentProcess = new QProcess(this);

    QString enginePath = Current_Path + "/Engines/realcugan-ncnn-vulkan/";
    QString executableName = "realcugan-ncnn-vulkan.exe"; // Assuming Windows
    QString executableFullPath = enginePath + executableName;

    // Assuming input_path for GIF is a directory of frames
    QString input_path = TempDir_Path + "/" + table_gif_item_fileName.at(file_list_row_number) + "_frames/";
    QString original_fileName = table_gif_item_fileName.at(file_list_row_number);
    QString output_path = TempDir_Path + "/" + original_fileName + "_frames_RealCUGAN_NCNN_Vulkan/";

    // --- Get parameters from UI/Settings (Placeholders for RealCUGAN specific UI) ---
    int noise_level = 0;
    if (ui->spinBox_DenoiseLevel_gif)
        noise_level = ui->spinBox_DenoiseLevel_gif->value();
    else
        noise_level = Settings_Read_value("/settings/GIFDenoiseLevel").toInt();
    QString model_dir_name_read = Settings_Read_value("/settings/RealCUGANModelDirGif").toString(); // Placeholder
    if (model_dir_name_read.isEmpty()) model_dir_name_read = "models-se";
    QString model_dir_name = model_dir_name_read;
    int scale = 2;
    if (ui->doubleSpinBox_ScaleRatio_gif)
        scale = ui->doubleSpinBox_ScaleRatio_gif->value();
    else
        scale = Settings_Read_value("/settings/GIFScaleRatio").toInt();
    int gpu_id_read = Settings_Read_value("/settings/RealCUGANGpuIdGif").toInt(); // Placeholder
    int gpu_id = gpu_id_read;
    QString format = "png"; // Process frames losslessly
    bool tta_mode_read = Settings_Read_value("/settings/RealCUGANTTAModeGif").toBool(); // Placeholder
    bool tta_mode = tta_mode_read;
    QString tile_size_str_read = Settings_Read_value("/settings/RealCUGANTileSizeGif").toString(); // Placeholder
    if (tile_size_str_read.isEmpty()) tile_size_str_read = "0";
    QString tile_size_str = tile_size_str_read;
    bool verbose_log = true;
    QString syncgap_str_read = Settings_Read_value("/settings/RealCUGANSyncGapGif").toString(); // Placeholder
    if (syncgap_str_read.isEmpty()) syncgap_str_read = "3";
    QString syncgap_str = syncgap_str_read;

    QStringList arguments = Realcugan_NCNN_Vulkan_PrepareArguments(
        input_path, output_path, noise_level, model_dir_name, scale, gpu_id, format,
        tta_mode, tile_size_str, verbose_log, syncgap_str, true /*isVideoOrGif=true*/
    );

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessStdOut()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onProcessStdErr()));

    qDebug() << "Starting RealCUGAN for GIF (frames):" << input_path;
    qDebug() << "Arguments:" << arguments.join(" ");
    qDebug() << "Executable:" << executableFullPath;

    currentProcess->setWorkingDirectory(enginePath);
    currentProcess->start(executableFullPath, arguments);

    Set_Progress_Bar_Value(0, 0);
    Set_Current_File_Progress_Bar_Value(0,0);
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
    Processing_Status = PROCESS_TYPE_GIF;
    current_File_Row_Number = file_list_row_number;
}


void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings()
{
    if (realCuganProcessor)
        realCuganProcessor->readSettingsVideoGif(0);

    if (ui->doubleSpinBox_ScaleRatio_image)
        m_realcugan_Scale = static_cast<int>(ui->doubleSpinBox_ScaleRatio_image->value());
    else
        m_realcugan_Scale = Settings_Read_value("/settings/ImageScaleRatio").toInt();

    qDebug() << "Realcugan_NCNN_Vulkan_ReadSettings: Model:" << m_realcugan_Model
             << "Scale:" << m_realcugan_Scale
             << "Denoise:" << m_realcugan_DenoiseLevel
             << "Tile:" << m_realcugan_TileSize
             << "GPUID:" << m_realcugan_GPUID;
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int threadNum)
{
    if (realCuganProcessor)
        realCuganProcessor->readSettingsVideoGif(threadNum);

    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");

    QVariant listVar = settings.value("RealCUGAN_GPUJobConfig_MultiGPU");
    GPUIDs_List_MultiGPU_RealCUGAN =
        listVar.isValid() ? listVar.value<QList<QMap<QString, QString>>>()
                           : QList<QMap<QString, QString>>();

    bool multiEnabled = settings.value("RealCUGAN_MultiGPU_Enabled", false).toBool();
    QString fallbackId = settings.value("RealCUGAN_GPUID", "0").toString();

    RealcuganJobManager jobMgr;
    QString gpuConfig = jobMgr.buildGpuJobString(multiEnabled,
                                                 GPUIDs_List_MultiGPU_RealCUGAN,
                                                 fallbackId);

    m_realcugan_gpuJobConfig_temp = GPUIDs_List_MultiGPU_RealCUGAN;

    QString jobsVideo = settings.value("RealCUGANJobsVideo", "1:2:2").toString();
    qDebug() << "Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF GPU config" << gpuConfig
             << "JobsVideo" << jobsVideo;
    settings.endGroup();
}

void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings()
{
    if (realCuganProcessor)
        realCuganProcessor->preLoadSettings();

    Realcugan_NCNN_Vulkan_ReadSettings();
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0);
    qDebug() << "Realcugan_NCNN_Vulkan_PreLoad_Settings completed.";
}

void MainWindow::Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString&, int, bool, const QString&)
{
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
    // Skip when the RealCUGAN widgets are not present. In that case,
    // GPU detection for Frame Interpolation handles the list.
    if (!pushButton_DetectGPU_RealCUGAN)
        return;

    pushButton_DetectGPU_RealCUGAN->setEnabled(false);
    pushButton_DetectGPU_RealCUGAN->setText(tr("Detecting..."));

    QString exePath = Current_Path + "/Engines/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan";
#ifdef Q_OS_WIN
    exePath += ".exe";
#endif
    QProcess proc;
    QByteArray out, err;
    runProcess(&proc, QString("\"%1\" -h").arg(exePath), &out, &err);
    QString text = QString::fromLocal8Bit(out + err);
    Available_GPUID_RealCUGAN = parseVulkanDeviceList(text);

    Realcugan_NCNN_Vulkan_DetectGPU_finished();
}

void MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)
{
}

