#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QDirIterator>
#include <QRandomGenerator>

// --- RealESRGAN Specific Helper Functions ---

// CalculateRealESRGANScaleSequence: Determines how many times to apply a model
// based on its native scale to reach or exceed the targetScale.
// It aims to get scale >= targetScale using the fewest model applications.
// Downscaling (if currentPipelineScale > targetScale) is handled as a post-process step if necessary.
QList<int> MainWindow::CalculateRealESRGANScaleSequence(int targetScale, int modelNativeScale) {
    QList<int> sequence;
    if (targetScale <= 0 || modelNativeScale <= 0) {
        sequence.append(1); // Default to 1x (no effective scaling or error)
        qWarning() << "CalculateRealESRGANScaleSequence: Invalid targetScale or modelNativeScale. Defaulting to 1x.";
        return sequence;
    }

    if (modelNativeScale == 1) { // If model is 1x (e.g. denoise only model, not typical for RealESRGAN but possible)
        sequence.append(1);
        return sequence;
    }

    double currentTotalScale = 1.0;
    while (currentTotalScale < targetScale) {
        sequence.append(modelNativeScale);
        currentTotalScale *= modelNativeScale;
    }

    if (sequence.isEmpty()) { // Should only happen if targetScale is 1
        sequence.append(1); // Represents a 1x operation (e.g. run model for denoising if applicable)
    }

    qDebug() << "CalculateRealESRGANScaleSequence: Target" << targetScale << "ModelNative" << modelNativeScale << "Sequence" << sequence;
    return sequence;
}

// --- RealESRGAN Core Processing Functions ---

void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings() {
    QString modelComboText = ui->comboBox_Model_RealESRGAN->currentText();
    // -n {MODEL_NAME} -s {SCALE}
    // Example models: "realesrgan-x4plus", "realesrgan-x4plus-anime", "realesr-animevideov3-x2"
    // For "realesr-animevideov3-x2", MODEL_NAME is "realesr-animevideov3", SCALE is 2.

    if (modelComboText == "realesrgan-x4plus") {
        m_realesrgan_ModelName = "realesrgan-x4plus";
        m_realesrgan_ModelNativeScale = 4;
    } else if (modelComboText == "realesrgan-x4plus-anime") {
        m_realesrgan_ModelName = "realesrgan-x4plus-anime";
        m_realesrgan_ModelNativeScale = 4;
    } else if (modelComboText == "realesr-animevideov3-x2") {
        m_realesrgan_ModelName = "realesr-animevideov3"; // Base model name for the executable
        m_realesrgan_ModelNativeScale = 2;
    } else if (modelComboText == "realesr-animevideov3-x3") {
        m_realesrgan_ModelName = "realesr-animevideov3";
        m_realesrgan_ModelNativeScale = 3;
    } else if (modelComboText == "realesr-animevideov3-x4") {
        m_realesrgan_ModelName = "realesr-animevideov3";
        m_realesrgan_ModelNativeScale = 4;
    } else {
        // Fallback to a default if somehow the combobox text is unexpected
        m_realesrgan_ModelName = "realesrgan-x4plus";
        m_realesrgan_ModelNativeScale = 4;
        qWarning() << "RealESRGAN_ReadSettings: Unknown model text" << modelComboText << ", defaulting to realesrgan-x4plus.";
    }

    m_realesrgan_TileSize = ui->spinBox_TileSize_RealESRGAN->value();
    m_realesrgan_TTA = ui->checkBox_TTA_RealESRGAN->isChecked();
    m_realesrgan_GPUID = ui->comboBox_GPUID_RealESRGAN->currentText(); // Full "ID: Name" string

    qDebug() << "RealESRGAN Settings Read: ModelName:" << m_realesrgan_ModelName
             << "ModelNativeScale:" << m_realesrgan_ModelNativeScale
             << "TileSize:" << m_realesrgan_TileSize
             << "TTA:" << m_realesrgan_TTA
             << "GPUID (Single):" << m_realesrgan_GPUID;
}

QStringList MainWindow::RealESRGAN_NCNN_Vulkan_PrepareArguments(
    const QString &inputFile, const QString &outputFile, int currentPassScaleForExe, /* This is the scale passed to -s for the exe */
    const QString &modelNameForExe, /* This is the name passed to -n for the exe */
    int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob,
    bool ttaEnabled, const QString &outputFormat)
{
    QStringList arguments;
    // EXE Path is: Current_Path + "/realesrgan-ncnn-vulkan-20220424-windows/realesrgan-ncnn-vulkan.exe"
    // Models path is relative to EXE: "./models"
    // Command: {EXE_PATH} -i {INPUT} -o {OUTPUT} -s {SCALE_FOR_EXE} -n {MODEL_NAME_FOR_EXE} -t {TILE_SIZE} -g {GPU_ID_PART} [-j {JOB_THREADS}] {TTA_FLAG} -f {FORMAT}

    arguments << "-i" << QDir::toNativeSeparators(inputFile)
              << "-o" << QDir::toNativeSeparators(outputFile)
              << "-s" << QString::number(currentPassScaleForExe) // Scale for this specific pass by the model
              << "-n" << modelNameForExe; // Model name (e.g., realesrgan-x4plus, realesr-animevideov3)

    if (tileSize > 0) { // 0 means auto for many ncnn tools
        arguments << "-t" << QString::number(tileSize);
    }

    if (isMultiGPUJob) {
        // gpuIdOrJobConfig should be like "-g 0,1 -j 1:2:1,1:2:1" or "-g 0,1 -j 2,2"
        // The RealesrganNcnnVulkan_MultiGPU() or _ReadSettings_Video_GIF should provide this.
        arguments << gpuIdOrJobConfig.split(" ");
    } else {
        arguments << "-g" << gpuIdOrJobConfig.split(" ")[0]; // Single GPU ID part
    }

    if (ttaEnabled) {
        arguments << "-x";
    }
    arguments << "-f" << outputFormat.toLower();

    return arguments;
}

bool MainWindow::RealESRGAN_ProcessSingleFileIteratively(
    const QString &inputFile, const QString &outputFile, int targetOverallScale,
    int modelNativeScale, /* Pass the native scale of the selected model */
    const QString &modelName, /* Pass the name for -n argument */
    int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob,
    bool ttaEnabled, const QString &outputFormat, int rowNumForStatusUpdate)
{
    qDebug() << "RealESRGAN_ProcessSingleFileIteratively: Input" << inputFile << "Output" << outputFile
             << "TargetOverallScale" << targetOverallScale << "ModelNativeScale" << modelNativeScale << "ModelName" << modelName;

    QFileInfo finalOutFileInfo(outputFile);
    QString tempSubFolder = QDir::tempPath() + "/W2XEX_RealESRGAN_Iter/" + QFileInfo(inputFile).completeBaseName() + "_" + QString::number(QRandomGenerator::global()->generate());
    QDir tempDir(tempSubFolder);
    if (!tempDir.mkpath(".")) {
        qDebug() << "RealESRGAN: Failed to create temporary directory:" << tempSubFolder;
        if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Temp dir failed"), "ERROR");
        return false;
    }
    QString tempPathBase = tempDir.filePath(finalOutFileInfo.completeBaseName() + "_pass_");

    QList<int> scaleSequence = CalculateRealESRGANScaleSequence(targetOverallScale, modelNativeScale);

    int currentPipelineScale = 1;
    for(int s_pass : scaleSequence) currentPipelineScale *= s_pass; // s_pass here is modelNativeScale

    if (scaleSequence.isEmpty()) {
        qDebug() << "RealESRGAN_ProcessSingle: Empty scale sequence.";
        if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Scale sequence error"), "ERROR");
        tempDir.removeRecursively();
        return false;
    }

    QString currentIterInputFile = inputFile;
    bool success = true;

    for (int i = 0; i < scaleSequence.size(); ++i) {
        if (Stopping) { success = false; break; }

        int scaleForThisPassExe = scaleSequence[i]; // This is the model's native scale, used for -s
        QString currentIterOutputFile;
        bool isLastModelPass = (i == scaleSequence.size() - 1);

        if (isLastModelPass && currentPipelineScale == targetOverallScale) {
            currentIterOutputFile = outputFile;
        } else {
            currentIterOutputFile = tempPathBase + QString::number(i) + "." + outputFormat.toLower();
        }

        QFileInfo currentIterOutInfo(currentIterOutputFile);
        QDir outputDirForPass(currentIterOutInfo.path());
        if (!outputDirForPass.exists() && !outputDirForPass.mkpath(".")) {
             qDebug() << "RealESRGAN ProcessSingle: Failed to create output directory for pass:" << currentIterOutInfo.path();
             success = false; break;
        }

        QStringList arguments = RealESRGAN_NCNN_Vulkan_PrepareArguments(
            currentIterInputFile, currentIterOutputFile, scaleForThisPassExe,
            modelName, tileSize, gpuIdOrJobConfig, isMultiGPUJob, ttaEnabled, outputFormat
        );

        if (rowNumForStatusUpdate != -1) {
             UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Processing (Pass %1/%2)").arg(i + 1).arg(scaleSequence.size()), "INFO");
        }

        QProcess process;
        QString exePath = Current_Path + "/realesrgan-ncnn-vulkan-20220424-windows/realesrgan-ncnn-vulkan.exe";

        qDebug() << "RealESRGAN ProcessSingle Pass" << i+1 << "Cmd:" << exePath << arguments.join(" ");
        process.start(exePath, arguments);

        if (!process.waitForStarted(10000)) {
            qDebug() << "RealESRGAN ProcessSingle: Process failed to start for pass" << i+1 << process.errorString();
            success = false; break;
        }
        if (!process.waitForFinished(-1)) {
            qDebug() << "RealESRGAN ProcessSingle: Process timed out or crashed for pass" << i+1 << process.errorString();
            success = false; break;
        }

        QByteArray stdOut = process.readAllStandardOutput();
        QByteArray stdErr = process.readAllStandardError();
        if (!stdOut.isEmpty()) qDebug() << "RealESRGAN Pass" << i+1 << "STDOUT:" << QString::fromLocal8Bit(stdOut);
        if (!stdErr.isEmpty()) qDebug() << "RealESRGAN Pass" << i+1 << "STDERR:" << QString::fromLocal8Bit(stdErr);

        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            qDebug() << "RealESRGAN ProcessSingle: Pass" << i+1 << "failed. ExitCode:" << process.exitCode() << "Error:" << process.errorString();
            success = false; break;
        }
        if (!QFile::exists(currentIterOutputFile)) {
            qDebug() << "RealESRGAN ProcessSingle: Output file for pass" << i+1 << "not found:" << currentIterOutputFile;
            success = false; break;
        }
        currentIterInputFile = currentIterOutputFile;
    }

    if (success && currentPipelineScale != targetOverallScale && !Stopping) {
        if (currentPipelineScale > targetOverallScale) {
            qDebug() << "RealESRGAN: Downscaling needed from" << currentPipelineScale << "x to" << targetOverallScale << "x for" << outputFile;
            if (currentIterInputFile != outputFile) {
                if (QFile::exists(outputFile)) QFile::remove(outputFile);
                if (!QFile::copy(currentIterInputFile, outputFile)) {
                    qDebug() << "RealESRGAN: Failed to copy for eventual downscaling."; success = false;
                }
            }
            // Actual downscaling step is conceptually here. The main app would call a resizer.
            // This function's responsibility ends with outputting the model-scaled image.
            // ShowMessageBox("Information", tr("RealESRGAN: Output is at %1x, further downscaling to %2x is required (not automatically performed by this function).").arg(currentPipelineScale).arg(targetOverallScale), QMessageBox::Information);
             emit Send_TextBrowser_NewMessage(tr("RealESRGAN: Output is at %1x, user requested %2x. Manual/post-process downscale may be needed.").arg(currentPipelineScale).arg(targetOverallScale));

        } else { // currentPipelineScale < targetOverallScale (should not happen with current CalculateRealESRGANScaleSequence)
             qDebug() << "RealESRGAN: Pipeline scale" << currentPipelineScale << "is less than target" << targetOverallScale << ". Logic error or unsupported scenario.";
             success = false; // This is an unexpected state
        }
    }

    if (tempDir.exists()) { /* ... cleanup ... */ tempDir.removeRecursively(); }
    if (Stopping && success) { return false; }
    if (!success && rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error during processing"), "ERROR");
    return success;
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int rowNum, bool ReProcess_MissingAlphaChannel) {
    Q_UNUSED(ReProcess_MissingAlphaChannel);
    if (Stopping == true) return;

    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    if (!item_InFile || !item_OutFile || !item_Status) { /* ... error ... */ return; }
    item_Status->setText(tr("Processing")); qApp->processEvents();

    QString inputFile = item_InFile->text();
    QString outputFile = item_OutFile->text();

    // --- handle alpha channel by splitting it before processing ---
    // QImage supports PNG and WebP input so the split works for both formats
    QTemporaryDir alphaTempDir;
    QString alphaFilePath;
    QString rgbInputPath = inputFile;
    QString rgbOutputPath = outputFile;
    QImage alphaImage;
    QImage sourceImage(inputFile);
    bool hasAlpha = sourceImage.hasAlphaChannel();
    if (hasAlpha && alphaTempDir.isValid()) {
        alphaImage = sourceImage.alphaChannel();
        QImage rgbImage = sourceImage.convertToFormat(QImage::Format_RGB888);
        rgbInputPath = alphaTempDir.filePath("rgb_in.png");
        rgbOutputPath = alphaTempDir.filePath("rgb_out.png");
        alphaFilePath = alphaTempDir.filePath("alpha.png");
        rgbImage.save(rgbInputPath);
        alphaImage.save(alphaFilePath);
    }

    RealESRGAN_NCNN_Vulkan_ReadSettings();

    int uiTargetScale = ui->doubleSpinBox_ScaleRatio_image->value();
    if (uiTargetScale <=0) uiTargetScale = m_realesrgan_ModelNativeScale;

    QString gpuConfigToUse = m_realesrgan_GPUID;
    bool isMultiConfig = false;
    if (ui->checkBox_MultiGPU_RealESRGAN->isChecked()){
        gpuConfigToUse = RealesrganNcnnVulkan_MultiGPU();
        isMultiConfig = !gpuConfigToUse.startsWith("-g "); // A bit of a heuristic: if it's complex, it's multi
    }

    // This function is called by Waifu2xMainThread which should wrap it in QtConcurrent::run
    bool success = RealESRGAN_ProcessSingleFileIteratively(
        rgbInputPath, rgbOutputPath, uiTargetScale,
        m_realesrgan_ModelNativeScale, m_realesrgan_ModelName, m_realesrgan_TileSize,
        gpuConfigToUse, isMultiConfig, m_realesrgan_TTA,
        ui->comboBox_OutFormat_Image->currentText(),
        rowNum
    );

    if (success && hasAlpha && QFile::exists(rgbOutputPath)) {
        QImage processedImage(rgbOutputPath);
        processedImage = processedImage.convertToFormat(QImage::Format_ARGB32);
        // Reattach the saved alpha so transparency is kept for PNG/WebP outputs
        processedImage.setAlphaChannel(alphaImage);
        processedImage.save(outputFile);
    }

    if (success && !Stopping) {
        item_Status->setText(tr("Finished"));
        LoadScaledImageToLabel(outputFile, ui->label_Preview_Main);
        UpdateTotalProcessedFilesCount();
    } else if (!Stopping && !success) { // Already updated by helper if rowNumForStatusUpdate != -1
        if(item_Status->text() != tr("Error during processing")) item_Status->setText(tr("Error"));
    } else if (Stopping) {
        item_Status->setText(tr("Stopped"));
    }
    // ProcessNextFile(); // Managed by Waifu2xMainThread
}

void MainWindow::RealESRGAN_NCNN_Vulkan_GIF(int rowNum) {
    // Structure similar to RealCUGAN_NCNN_Vulkan_GIF
    if (Stopping == true) return;
    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    if (!item_InFile || !item_OutFile || !item_Status) { /* ... */ return; }
    item_Status->setText(tr("Processing")); qApp->processEvents();

    QString sourceFileFullPath = item_InFile->text();
    QString resultFileFullPath = item_OutFile->text();
    QFileInfo sourceFileInfo(sourceFileFullPath);
    QString baseName = sourceFileInfo.completeBaseName();
    QString splitFramesFolder = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_GIF_split_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString scaledFramesFolder = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_GIF_scaled_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(splitFramesFolder); QDir().mkpath(scaledFramesFolder);

    emit Send_TextBrowser_NewMessage(tr("Splitting GIF: %1 (RealESRGAN)").arg(sourceFileFullPath));
    Gif_splitGif(sourceFileFullPath, splitFramesFolder);
    QStringList framesList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if (framesList.isEmpty()) { /* ... */ QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); return; }

    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0);

    int targetScale = ui->spinBox_scaleRatio_gif->value();
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale;

    bool allOk = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName(), framesList.count());
    for (int i = 0; i < framesList.count(); ++i) { /* ... frame processing loop from RealCUGAN GIF ... */
        if (Stopping) { allOk = false; break; }
        CurrentFileProgress_progressbar_Add();
        emit Send_TextBrowser_NewMessage(tr("Processing GIF frame %1/%2 (RealESRGAN)").arg(i+1).arg(framesList.count()));
        QString inputFrame = QDir(splitFramesFolder).filePath(framesList.at(i));
        QString outputFrame = QDir(scaledFramesFolder).filePath(framesList.at(i));
        if (!RealESRGAN_ProcessSingleFileIteratively(inputFrame, outputFrame, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName, m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(), m_realesrgan_TTA, QFileInfo(framesList.at(i)).suffix().isEmpty() ? "png" : QFileInfo(framesList.at(i)).suffix() )) {
            allOk = false; break;
        }
    }
    CurrentFileProgress_Stop();
    /* ... rest of GIF assembly and cleanup ... */
    if (allOk && !Stopping) { /* ... assemble ... */ Gif_assembleGif(resultFileFullPath, scaledFramesFolder, Gif_getDuration(sourceFileFullPath), false, 0, 0, false, sourceFileFullPath); /* ... update status ... */ }
    else if (!Stopping) item_Status->setText(tr("Error: Frame processing failed")); else item_Status->setText(tr("Stopped"));
    QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Video(int rowNum) {
    // Structure similar to RealCUGAN_NCNN_Vulkan_Video
    if (Stopping == true) return;
    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);
    if (!item_InFile || !item_OutFile || !item_Status) { /* ... */ return; }
    item_Status->setText(tr("Processing")); qApp->processEvents();
    QString sourceFileFullPath = item_InFile->text();
    QString resultFileFullPath = item_OutFile->text();
    QFileInfo sourceFileInfo(sourceFileFullPath);
    QString baseName = sourceFileInfo.completeBaseName();
    QString splitFramesFolder = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_Vid_split_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString scaledFramesFolder = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_Vid_scaled_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString audioPath = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_Vid_audio.m4a";
    QDir().mkpath(splitFramesFolder); QDir().mkpath(scaledFramesFolder); QFile::remove(audioPath);
    emit Send_TextBrowser_NewMessage(tr("Extracting video frames & audio: %1 (RealESRGAN)").arg(sourceFileFullPath));
    video_video2images(sourceFileFullPath, splitFramesFolder, audioPath);
    QStringList framesList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if (framesList.isEmpty()) { /* ... */ QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath); return; }
    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0);
    int targetScale = ui->spinBox_scaleRatio_video->value();
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale;
    bool allOk = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName(), framesList.count());
    for (int i = 0; i < framesList.count(); ++i) { /* ... frame processing loop ... */
        if (Stopping) { allOk = false; break; }
        CurrentFileProgress_progressbar_Add();
        emit Send_TextBrowser_NewMessage(tr("Processing video frame %1/%2 (RealESRGAN)").arg(i+1).arg(framesList.count()));
        QString inputFrame = QDir(splitFramesFolder).filePath(framesList.at(i));
        QString outputFrame = QDir(scaledFramesFolder).filePath(framesList.at(i));
        if (!RealESRGAN_ProcessSingleFileIteratively(inputFrame, outputFrame, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName, m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(), m_realesrgan_TTA, QFileInfo(framesList.at(i)).suffix().isEmpty() ? "png" : QFileInfo(framesList.at(i)).suffix() )) {
            allOk = false; break;
        }
    }
    CurrentFileProgress_Stop();
    /* ... rest of video assembly and cleanup ... */
    if (allOk && !Stopping) { /* ... assemble ... */ video_images2video(sourceFileFullPath, resultFileFullPath, scaledFramesFolder, audioPath, false, 0, 0, false); /* ... update status ... */ }
    else if (!Stopping) item_Status->setText(tr("Error: Frame processing failed")); else item_Status->setText(tr("Stopped"));
    QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath);
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Video_BySegment(int rowNum) {
    // Structure similar to RealCUGAN_NCNN_Vulkan_Video_BySegment
    if (Stopping == true) return;
    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);
    if (!item_InFile || !item_OutFile || !item_Status) { /* ... */ return; }
    item_Status->setText(tr("Processing (Segmented)")); qApp->processEvents();
    QString sourceFileFullPath = item_InFile->text();
    QString finalResultFileFullPath = item_OutFile->text();
    QFileInfo sourceFileInfo(sourceFileFullPath);
    QString sourceFileNameNoExt = sourceFileInfo.completeBaseName();
    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0);
    int targetScale = ui->spinBox_scaleRatio_video->value();
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale;
    int totalDurationSec = video_get_duration(sourceFileFullPath);
    int segmentDurationSec = ui->spinBox_SegmentDuration->value();
    if (segmentDurationSec <= 0) segmentDurationSec = 600;
    int numSegments = (totalDurationSec + segmentDurationSec - 1) / segmentDurationSec;
    QString mainTempFolder = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealESRGAN_VidSeg_Main_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(mainTempFolder);
    QStringList processedVideoSegmentsPaths;
    QString fullAudioPath = QDir(mainTempFolder).filePath(sourceFileNameNoExt + "_full_audio.m4a");
    emit Send_TextBrowser_NewMessage(tr("Extracting full audio track... (RealESRGAN Segmented)"));
    video_get_audio(sourceFileFullPath, fullAudioPath);
    if (!QFile::exists(fullAudioPath)) { /* ... error ... */ QDir(mainTempFolder).removeRecursively(); return; }
    bool overallSuccess = true;
    for (int i = 0; i < numSegments; ++i) { /* ... per segment: extract frames, process, assemble segment ... */
        if (Stopping) { overallSuccess = false; break; }
        // (Code from RealCUGAN's VideoBySegment, adapted for RealESRGAN)
        // ...
        QString segmentName = QString("segment_%1").arg(i, 3, 10, QChar('0'));
        QString splitFramesFolder = QDir(mainTempFolder).filePath(segmentName + "_split");
        QString scaledFramesFolder = QDir(mainTempFolder).filePath(segmentName + "_scaled");
        QDir().mkpath(splitFramesFolder); QDir().mkpath(scaledFramesFolder);
        int startTime = i * segmentDurationSec;
        int currentSegmentDuration = qMin(segmentDurationSec, totalDurationSec - startTime);
        QProcess ffmpegFrameExtract; QStringList extractArgs;
        QString frameOutputPattern = QDir(splitFramesFolder).filePath("frame_%0"+QString::number(CalNumDigits(video_get_frameNum(sourceFileFullPath)))+"d.png");
        extractArgs << "-nostdin" << "-y" << "-ss" << QString::number(startTime) << "-i" << sourceFileFullPath << "-t" << QString::number(currentSegmentDuration) << "-vf" << "fps=" + video_get_fps(sourceFileFullPath) << frameOutputPattern;
        ffmpegFrameExtract.start(FFMPEG_EXE_PATH_Waifu2xEX, extractArgs);
        if (!ffmpegFrameExtract.waitForStarted(5000) || !ffmpegFrameExtract.waitForFinished(-1) || ffmpegFrameExtract.exitCode() != 0) { overallSuccess = false; break; }
        QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
        // ... loop frames ... call RealESRGAN_ProcessSingleFileIteratively ...
        // ... assemble segment video (processedSegmentVideoPath) ...
        // processedVideoSegmentsPaths.append(processedSegmentVideoPath);
        // QDir(scaledFramesFolder).removeRecursively();
    }
    if (overallSuccess && !Stopping) { /* ... concatenate segments, mux audio ... */ }
    /* ... final status update & cleanup ... */
    QDir(mainTempFolder).removeRecursively();
}

void MainWindow::APNG_RealESRGANCNNVulkan(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath) {
    Q_UNUSED(sourceFileFullPath); Q_UNUSED(resultFileFullPath);
    qDebug() << "APNG_RealESRGANCNNVulkan started. Input:" << splitFramesFolder << "Output:" << scaledFramesFolder;
    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0);
    int targetScale = ui->spinBox_Scale_RealESRGAN->value(); // Use RealESRGAN tab's scale or general image scale
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale;
    QDir outputDir(scaledFramesFolder);
    if (!outputDir.exists() && !outputDir.mkpath(".")) { return; }
    bool allOk = true;
    for (const QString &frameFileName : framesFileName_qStrList) { /* ... frame processing loop from RealCUGAN APNG ... */
        if (Stopping) { allOk = false; break; }
        emit Send_TextBrowser_NewMessage(tr("Processing APNG frame: %1 (RealESRGAN)").arg(frameFileName));
        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        QString outputFramePath = QDir(scaledFramesFolder).filePath(frameFileName);
        if (!RealESRGAN_ProcessSingleFileIteratively(inputFramePath, outputFramePath, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName, m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(), m_realesrgan_TTA, QFileInfo(frameFileName).suffix().isEmpty() ? "png" : QFileInfo(frameFileName).suffix() )) {
            allOk = false; break;
        }
    }
    /* ... log success/failure ... */
}

void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum) {
    Q_UNUSED(ThreadNum); // ThreadNum not used for now, assumes all GPUs for one task
    RealESRGAN_NCNN_Vulkan_ReadSettings();

    QString gpuJobConfig;
    if (ui->checkBox_MultiGPU_RealESRGAN->isChecked() && !GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.isEmpty()) {
        QStringList gpuIDs, jobThreadsPerGPU;
        for(const auto& gpu : GPUIDs_List_MultiGPU_RealesrganNcnnVulkan) {
            if(gpu.value("isEnabled", "true") == "true") { // Check if GPU is enabled in the list
                 gpuIDs.append(gpu.value("ID"));
                 // RealESRGAN's -j often takes a load:proc:save format, or just proc threads
                 // Using "Threads" from UI as proc threads for now.
                 // Example: 1 (load):N (proc):1 (save)
                 QString procThreads = gpu.value("Threads", "1"); // Default to 1 processing thread
                 jobThreadsPerGPU.append(QString("1:%1:1").arg(procThreads));
            }
        }
        if (!gpuIDs.isEmpty()) {
            gpuJobConfig = "-g " + gpuIDs.join(",");
            gpuJobConfig += " -j " + jobThreadsPerGPU.join(",");
        } else { // No enabled GPUs in list, fallback to single selected
            gpuJobConfig = "-g " + m_realesrgan_GPUID.split(" ")[0];
        }
    } else { // Single GPU mode
        gpuJobConfig = "-g " + m_realesrgan_GPUID.split(" ")[0];
    }
    m_realesrgan_gpuJobConfig_temp = gpuJobConfig;
    qDebug() << "RealESRGAN_ReadSettings_Video_GIF for ThreadNum" << ThreadNum << "GPU/Job Config:" << gpuJobConfig;
}

QString MainWindow::RealesrganNcnnVulkan_MultiGPU() {
    if (!ui->checkBox_MultiGPU_RealESRGAN->isChecked() || GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.isEmpty()) {
        return "-g " + m_realesrgan_GPUID.split(" ")[0];
    }
    QStringList gpuIDs, jobThreads;
    for (const auto& gpuMap : GPUIDs_List_MultiGPU_RealesrganNcnnVulkan) {
        if(gpuMap.value("isEnabled", "true") == "true") {
            gpuIDs.append(gpuMap.value("ID"));
            QString procThreads = gpuMap.value("Threads", "1");
            jobThreads.append(QString("1:%1:1").arg(procThreads)); // Assuming load:proc:save
        }
    }
    if (gpuIDs.isEmpty()) return "-g " + m_realesrgan_GPUID.split(" ")[0]; // Fallback
    return QString("-g %1 -j %2").arg(gpuIDs.join(","), jobThreads.join(","));
}

void MainWindow::AddGPU_MultiGPU_RealesrganNcnnVulkan(QString GPUID_Name) {
    if (GPUID_Name.isEmpty() || GPUID_Name.contains("No available") || GPUID_Name.contains("Default")) return;
    QStringList parts = GPUID_Name.split(":");
    QString id = parts.first();
    QString name = parts.mid(1).join(":").trimmed();
    // Use threads from the dedicated multi-GPU threads spinbox if it exists, or a default like 1 or 2
    int threads = ui->spinBox_Threads_MultiGPU_RealESRGAN->value(); // Assuming spinBox_Threads_MultiGPU_RealESRGAN exists

    for (const auto& map : GPUIDs_List_MultiGPU_RealesrganNcnnVulkan) {
        if (map.value("ID") == id) { ShowMessageBox("Info", "GPU already added for RealESRGAN.", QMessageBox::Information); return; }
    }
    QMap<QString, QString> newGPU;
    newGPU.insert("ID", id); newGPU.insert("Name", name);
    newGPU.insert("Threads", QString::number(threads)); // This "Threads" will be used as "proc" in "load:proc:save"
    newGPU.insert("isEnabled", "true"); // Default to enabled
    // Tile size per GPU is not standard for RealESRGAN, usually global. Storing it if UI supports it.
    newGPU.insert("TileSize", QString::number(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->value()));


    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.append(newGPU);
    ui->listWidget_GPUList_MultiGPU_RealESRGAN->addItem(QString("ID: %1, Name: %2, Threads: %3, Tile: %4 (Enabled)")
                                                     .arg(id, name, QString::number(threads), newGPU.value("TileSize")));
}

void MainWindow::RealESRGAN_NCNN_Vulkan_PreLoad_Settings() {
    QSettings settings(Current_Path + "/settings.ini", QSettings::IniFormat); // Use global settings file
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    settings.beginGroup("RealESRGAN_NCNN_Vulkan");
    ui->comboBox_Model_RealESRGAN->setCurrentText(settings.value("Model", "realesrgan-x4plus").toString());
    ui->spinBox_TileSize_RealESRGAN->setValue(settings.value("TileSize", 0).toInt());
    ui->checkBox_TTA_RealESRGAN->setChecked(settings.value("TTA", false).toBool());

    // Load Available GPUs first, then set current text for single GPU
    Available_GPUID_RealESRGAN_ncnn_vulkan = settings.value("AvailableGPUs").toStringList();
    if (!Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty()) {
        ui->comboBox_GPUID_RealESRGAN->clear();
        ui->comboBox_GPUID_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
        ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->clear();
        ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
    }
    ui->comboBox_GPUID_RealESRGAN->setCurrentText(settings.value("GPUID", "0: Default GPU 0").toString());

    ui->checkBox_MultiGPU_RealESRGAN->setChecked(settings.value("MultiGPUEnabled", false).toBool());
    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan = settings.value("MultiGPU_List").value<QList_QMap_QStrQStr>();
    ui->listWidget_GPUList_MultiGPU_RealESRGAN->clear();
    for(const auto& gpuMap : GPUIDs_List_MultiGPU_RealesrganNcnnVulkan) {
        QString status = gpuMap.value("isEnabled", "true") == "true" ? "Enabled" : "Disabled";
        ui->listWidget_GPUList_MultiGPU_RealESRGAN->addItem(QString("ID: %1, Name: %2, Threads: %3, Tile: %4 (%5)")
            .arg(gpuMap.value("ID"), gpuMap.value("Name"), gpuMap.value("Threads"), gpuMap.value("TileSize"), status));
    }
    settings.endGroup();
    RealESRGAN_NCNN_Vulkan_ReadSettings();
    on_checkBox_MultiGPU_RealESRGAN_stateChanged(ui->checkBox_MultiGPU_RealESRGAN->isChecked() ? Qt::Checked : Qt::Unchecked); // Update UI state
}

void MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU() {
    Available_GPUID_RealESRGAN_ncnn_vulkan.clear();
    ui->comboBox_GPUID_RealESRGAN->clear();
    ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->clear();

    QProcess *process = new QProcess(this);
    process->setObjectName("RealESRGAN_DetectGPU");
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(RealESRGAN_ncnn_vulkan_DetectGPU_finished(int,QProcess::ExitStatus)));
    connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(RealESRGAN_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)));

    QString exePath = Current_Path + "/realesrgan-ncnn-vulkan-20220424-windows/realesrgan-ncnn-vulkan.exe";
    QFileInfo exeCheck(exePath);
    if (!exeCheck.exists() || !exeCheck.isExecutable()) {
        QMessageBox::critical(this, tr("Error"), tr("RealESRGAN executable not found at %1").arg(exePath));
        process->deleteLater(); return;
    }
    process->start(exePath, QStringList());
    ui->pushButton_DetectGPU_RealESRGAN->setEnabled(false);
}

void MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) return;
    QString output = QString::fromLocal8Bit(process->readAllStandardOutput() + process->readAllStandardError());
    qDebug() << "RealESRGAN DetectGPU Output:" << output;

    QRegularExpression gpuPattern("^\\s*(\\d+)\\s+(.+)$"); // Example: " 0  NVIDIA GeForce..."
    QStringList lines = output.split(QRegExp("[\\r\\n]"), Qt::SkipEmptyParts);
    Available_GPUID_RealESRGAN_ncnn_vulkan.clear();

    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        QRegularExpressionMatch match = gpuPattern.match(trimmedLine);
        if (match.hasMatch()) {
            QString gpuID = match.captured(1).trimmed();
            QString gpuName = match.captured(2).trimmed();
            if (!gpuName.toLower().contains("subdevice")) {
                 Available_GPUID_RealESRGAN_ncnn_vulkan.append(QString("%1: %2").arg(gpuID, gpuName));
            }
        }
    }
    if (Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty()) {
        Available_GPUID_RealESRGAN_ncnn_vulkan.append("0: Default (Not detected/parsed)");
        QMessageBox::warning(this, tr("GPU Detection"), tr("No GPUs parsed for RealESRGAN. Check console. Using default."));
    } else {
        QMessageBox::information(this, tr("GPU Detection"), tr("RealESRGAN GPUs detected."));
    }

    ui->comboBox_GPUID_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
    ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);

    QSettings settings(Current_Path + "/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    settings.beginGroup("RealESRGAN_NCNN_Vulkan");
    settings.setValue("AvailableGPUs", Available_GPUID_RealESRGAN_ncnn_vulkan);
    settings.endGroup();

    ui->pushButton_DetectGPU_RealESRGAN->setEnabled(true);
    process->deleteLater();
    emit Send_Realesrgan_ncnn_vulkan_DetectGPU_finished();
}

void MainWindow::RealESRGAN_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error) {
    Q_UNUSED(error);
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) return;
    qDebug() << "RealESRGAN DetectGPU error:" << process->errorString();
    QMessageBox::critical(this, tr("GPU Detection Error"), tr("RealESRGAN GPU detection process failed: %1").arg(process->errorString()));
    ui->pushButton_DetectGPU_RealESRGAN->setEnabled(true);
    Available_GPUID_RealESRGAN_ncnn_vulkan.append("0: Error (Detection failed)");
    ui->comboBox_GPUID_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
    ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
    process->deleteLater();
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_finished() { /* Similar to RealCUGAN, but for RealESRGAN QProcess chain if used */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error) { /* ... */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_finished() { /* General fallback if direct QProcess used and not iterative chain */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_errorOccurred(QProcess::ProcessError error) { /* ... */ }


// --- UI Interaction Slots for RealESRGAN (Copied from existing file, ensure they are connected) ---
void MainWindow::on_pushButton_DetectGPU_RealESRGAN_clicked()
{
    qDebug() << "on_pushButton_DetectGPU_RealESRGAN_clicked";
    RealESRGAN_ncnn_vulkan_DetectGPU();
}

void MainWindow::on_comboBox_Model_RealESRGAN_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    qDebug() << "RealESRGAN model changed to:" << ui->comboBox_Model_RealESRGAN->currentText();
    RealESRGAN_NCNN_Vulkan_ReadSettings(); // Update member vars like m_realesrgan_ModelNativeScale
}

void MainWindow::on_pushButton_TileSize_Add_RealESRGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealESRGAN) return;
    ui->spinBox_TileSize_RealESRGAN->setValue(AddTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealESRGAN->value()));
}

void MainWindow::on_pushButton_TileSize_Minus_RealESRGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealESRGAN) return;
    ui->spinBox_TileSize_RealESRGAN->setValue(MinusTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealESRGAN->value()));
}

void MainWindow::on_checkBox_MultiGPU_RealESRGAN_stateChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    ui->groupBox_GPUSettings_MultiGPU_RealESRGAN->setEnabled(enabled);
    ui->comboBox_GPUID_RealESRGAN->setDisabled(enabled);
    qDebug() << "RealESRGAN Multi-GPU" << (enabled ? "enabled" : "disabled");
}

void MainWindow::on_comboBox_GPUIDs_MultiGPU_RealESRGAN_currentIndexChanged(int index)
{
    if(index < 0 || index >= GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.size() || !ui->checkBox_MultiGPU_RealESRGAN->isChecked())
    {
        ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setChecked(false);
        ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setValue(0); // Default or disable
        return;
    }
    QMap<QString,QString> CurrentGPU_info = GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.at(index);
    ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setChecked(CurrentGPU_info.value("isEnabled", "true") == "true");
    ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setValue(CurrentGPU_info.value("TileSize", "0").toInt());
}

void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN_clicked()
{
    int currentIndex = ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->currentIndex();
    if(currentIndex < 0 || currentIndex >= GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.size()) return;

    MultiGPU_QMutex_RealesrganNcnnVulkan.lock();
    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan[currentIndex]["isEnabled"] = ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->isChecked() ? "true" : "false";
    MultiGPU_QMutex_RealesrganNcnnVulkan.unlock();
    // Update listWidget display
    QListWidgetItem* item = ui->listWidget_GPUList_MultiGPU_RealESRGAN->item(currentIndex); // Assuming direct mapping
    if(item) {
        QMap<QString,QString> gpuInfo = GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.at(currentIndex);
        QString status = gpuInfo.value("isEnabled", "true") == "true" ? "Enabled" : "Disabled";
        item->setText(QString("ID: %1, Name: %2, Threads: %3, Tile: %4 (%5)")
            .arg(gpuInfo.value("ID"), gpuInfo.value("Name"), gpuInfo.value("Threads"), gpuInfo.value("TileSize"), status));
    }
}

void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN_valueChanged(int arg1)
{
    int currentIndex = ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->currentIndex();
    if(currentIndex < 0 || currentIndex >= GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.size()) return;

    MultiGPU_QMutex_RealesrganNcnnVulkan.lock();
    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan[currentIndex]["TileSize"] = QString::number(arg1);
    MultiGPU_QMutex_RealesrganNcnnVulkan.unlock();
     QListWidgetItem* item = ui->listWidget_GPUList_MultiGPU_RealESRGAN->item(currentIndex);
    if(item) {
        QMap<QString,QString> gpuInfo = GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.at(currentIndex);
        QString status = gpuInfo.value("isEnabled", "true") == "true" ? "Enabled" : "Disabled";
        item->setText(QString("ID: %1, Name: %2, Threads: %3, Tile: %4 (%5)")
            .arg(gpuInfo.value("ID"), gpuInfo.value("Name"), gpuInfo.value("Threads"), gpuInfo.value("TileSize"), status));
    }
}

void MainWindow::on_pushButton_ShowMultiGPUSettings_RealESRGAN_clicked()
{
    QString Settings_str = "RealESRGAN Multi-GPU Config:\n";
    MultiGPU_QMutex_RealesrganNcnnVulkan.lock();
    for(int i = 0; i < GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.size(); i++)
    {
        QMap<QString,QString> CurrentGPU_info = GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.at(i);
        Settings_str.append(tr("GPU ID: ")+CurrentGPU_info.value("ID")+"  "+
                            tr("Name: ")+CurrentGPU_info.value("Name")+"  "+
                            tr("Enabled: ")+CurrentGPU_info.value("isEnabled", "true")+"  "+
                            tr("Threads: ")+CurrentGPU_info.value("Threads", "1")+"  "+
                            tr("Tile size: ")+CurrentGPU_info.value("TileSize", "0")+"\n");
    }
    MultiGPU_QMutex_RealesrganNcnnVulkan.unlock();
    ShowMessageBox(tr("Current Multi-GPU Settings (RealESRGAN)"), Settings_str, QMessageBox::Information);
}

// These also need to be connected in mainwindow.cpp constructor for RealESRGAN
// on_pushButton_AddGPU_MultiGPU_RealESRGAN_clicked()
// on_pushButton_RemoveGPU_MultiGPU_RealESRGAN_clicked()
// on_pushButton_ClearGPU_MultiGPU_RealESRGAN_clicked()
