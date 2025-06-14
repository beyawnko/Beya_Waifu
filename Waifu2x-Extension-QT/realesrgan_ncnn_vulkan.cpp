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

// Utility to warn when an external process fails
static bool warnProcessFailure(QWidget *parent, QProcess &proc,
                               const QString &context)
{
    if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        QString msg = QObject::tr("%1 failed with exit code %2")
                          .arg(context)
                          .arg(proc.exitCode());
        QMessageBox::warning(parent, QObject::tr("Process Failure"), msg);
        return false;
    }
    return true;
}

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

bool MainWindow::RealESRGAN_SetupTempDir(const QString &inputFile, const QString &outputFile, QDir &tempDir, QString &tempPathBase) {
    QFileInfo outInfo(outputFile);
    QString tempSubFolder = QDir::tempPath() + "/W2XEX_RealESRGAN_Iter/" + QFileInfo(inputFile).completeBaseName() + "_" + QString::number(QRandomGenerator::global()->generate());
    tempDir = QDir(tempSubFolder);
    if (!tempDir.mkpath(".")) {
        qDebug() << "RealESRGAN: Failed to create temporary directory:" << tempSubFolder;
        return false;
    }
    tempPathBase = tempDir.filePath(outInfo.completeBaseName() + "_pass_");
    return true;
}

void MainWindow::RealESRGAN_CleanupTempDir(const QDir &tempDir) {
    if (tempDir.exists()) {
        tempDir.removeRecursively();
    }
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
    int originalWidth, int originalHeight, /* Added original dimensions */
    int modelNativeScale, /* Pass the native scale of the selected model */
    const QString &modelName, /* Pass the name for -n argument */
    int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob,
    bool ttaEnabled, const QString &outputFormat, int rowNumForStatusUpdate)
{
    qDebug() << "RealESRGAN_ProcessSingleFileIteratively: Input" << inputFile << "Output" << outputFile
             << "TargetOverallScale" << targetOverallScale << "OrigSize:" << originalWidth << "x" << originalHeight
             << "ModelNativeScale:" << modelNativeScale << "ModelName" << modelName;

    QDir tempDir;
    QString tempPathBase;
    if (!RealESRGAN_SetupTempDir(inputFile, outputFile, tempDir, tempPathBase)) {
        if (rowNumForStatusUpdate != -1)
            UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Temp dir failed"), "ERROR");
        return false;
    }

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
    QString lastPassOutputFile = inputFile; // Initialize with original input

    for (int pass = 0; pass < scaleSequence.size() && success && !Stopping; ++pass) {
        // Determine if this is the last AI pass based on sequence, not yet considering final resampling.
        // The output of the last AI pass might be a temporary file.
        bool isLastAiPass = (pass == scaleSequence.size() - 1);
        // If it's the last AI pass AND no resampling will be needed AND temp files are used, then write to final `outputFile`.
        // However, it's simpler to always write AI passes to temp files (or `outputFile` if only one pass and no resampling).
        // Let's keep intermediate files in tempDir.
        lastPassOutputFile = tempPathBase + QString::number(pass) + "." + outputFormat.toLower();
        if (scaleSequence.size() == 1) { // Only one pass, could directly use outputFile if no resampling expected
            // To simplify, always use temp for AI output, then resample/copy.
            // This avoids complex conditions here.
        }


        QFileInfo outInfo(lastPassOutputFile);
        QDir outDir(outInfo.path());
        if (!outDir.exists() && !outDir.mkpath(".")) {
            qDebug() << "RealESRGAN ProcessSingle: Failed to create output directory for pass:" << outInfo.path();
            success = false;
            break;
        }

        QStringList args = RealESRGAN_NCNN_Vulkan_PrepareArguments(
            currentIterInputFile, lastPassOutputFile, scaleSequence[pass], // Use lastPassOutputFile for the current pass's output
            modelName, tileSize, gpuIdOrJobConfig, isMultiGPUJob, ttaEnabled, outputFormat
        );


        if (rowNumForStatusUpdate != -1) {
            UpdateTableWidget_Status(rowNumForStatusUpdate,
                                     tr("Processing (Pass %1/%2)").arg(pass + 1).arg(scaleSequence.size()),
                                     "INFO");
        }

        QProcess proc;
        QString exePath = Current_Path + "/realesrgan-ncnn-vulkan-20220424-windows/realesrgan-ncnn-vulkan.exe";
        qDebug() << "RealESRGAN Pass" << pass+1 << "Cmd:" << exePath << args.join(" ");
        proc.start(exePath, args);

        if (!proc.waitForStarted(10000)) {
            qDebug() << "RealESRGAN Pass" << pass+1 << "failed to start." << proc.errorString();
            QMessageBox::warning(this, tr("Process Failure"),
                                 tr("RealESRGAN failed to start for pass %1").arg(pass + 1));
            success = false;
            break;
        }

        while (proc.state() != QProcess::NotRunning) {
            if (Stopping) {
                proc.terminate();
                if (!proc.waitForFinished(1500)) {
                    proc.kill();
                    proc.waitForFinished();
                }
                success = false;
                break;
            }
            if (proc.waitForFinished(100)) {
                break;
            }
        }
        if (!success) break;


        QByteArray stdOut = proc.readAllStandardOutput();
        QByteArray stdErr = proc.readAllStandardError();
        if (!stdOut.isEmpty()) qDebug() << "RealESRGAN ProcessSingle Pass" << pass+1 << "STDOUT:" << QString::fromLocal8Bit(stdOut);
        if (!stdErr.isEmpty()) qDebug() << "RealESRGAN ProcessSingle Pass" << pass+1 << "STDERR:" << QString::fromLocal8Bit(stdErr);

        if (!warnProcessFailure(this, proc, tr("RealESRGAN pass %1").arg(pass + 1))) {
            qDebug() << "RealESRGAN ProcessSingle: Pass" << pass+1 << "failed. ExitCode:" << proc.exitCode();
            success = false;
            break;
        }
        if (!QFile::exists(lastPassOutputFile)) {
            qDebug() << "RealESRGAN ProcessSingle: Output file for pass" << pass+1 << "not found:" << lastPassOutputFile;
            success = false; break;
        }
        currentIterInputFile = lastPassOutputFile; // Output of this pass is input for the next
    }

    if (success && !Stopping) {
        QImage imageAfterAiPasses(currentIterInputFile); // Load the result of the last AI pass
        if (imageAfterAiPasses.isNull()) {
            qDebug() << "RealESRGAN: Failed to load image after AI passes from" << currentIterInputFile;
            success = false;
        } else {
            int finalTargetWidth = qRound(static_cast<double>(originalWidth) * targetOverallScale);
            int finalTargetHeight = qRound(static_cast<double>(originalHeight) * targetOverallScale);

            if (imageAfterAiPasses.width() != finalTargetWidth || imageAfterAiPasses.height() != finalTargetHeight) {
                qDebug() << "RealESRGAN: Resampling from" << imageAfterAiPasses.size()
                         << "to target" << QSize(finalTargetWidth, finalTargetHeight) << "for final output" << outputFile;
                QImage resampledImage = imageAfterAiPasses.scaled(finalTargetWidth, finalTargetHeight,
                                                               this->CustRes_AspectRatioMode, // Use MainWindow's member
                                                               Qt::SmoothTransformation);
                if (!resampledImage.save(outputFile)) {
                    qDebug() << "RealESRGAN: Failed to save resampled image to" << outputFile;
                    success = false;
                }
            } else { // Dimensions are already correct
                if (currentIterInputFile != outputFile) { // If last AI pass output to a temp file
                    if (QFile::exists(outputFile)) QFile::remove(outputFile); // Ensure target is clean
                    if (!QFile::copy(currentIterInputFile, outputFile)) {
                        qDebug() << "RealESRGAN: Failed to copy final image from" << currentIterInputFile << "to" << outputFile;
                        success = false;
                    }
                }
                // If currentIterInputFile IS outputFile, it's already in place.
            }
        }
    }
    // The old message about manual downscaling is removed as this resampling step handles it.
    // The `currentPipelineScale` vs `targetOverallScale` check might still be useful for logging/debugging.
    if (currentPipelineScale != targetOverallScale && success && !Stopping) {
         qDebug() << "RealESRGAN: AI pipeline scale was" << currentPipelineScale << "x, user requested " << targetOverallScale << "x. Resampling applied.";
         // emit Send_TextBrowser_NewMessage(tr("RealESRGAN: AI output at %1x, resampled to user requested %2x.").arg(currentPipelineScale).arg(targetOverallScale));
    }


    RealESRGAN_CleanupTempDir(tempDir);
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

    AlphaInfo alphaInfo = PrepareAlpha(inputFile); // This might put RGB data in a temp file
    QString rgbInputPath = alphaInfo.rgbPath; // This is the path to the RGB data (original or temp)
    QString rgbOutputPath = alphaInfo.hasAlpha ? QDir(alphaInfo.tempDir).filePath("rgb_final_preswap.png") : outputFile; // Final RGB before potential alpha merge

    QImage originalFullImage(inputFile); // Load original user-provided file for dimensions
    int originalImgWidth = originalFullImage.width();
    int originalImgHeight = originalFullImage.height();
    originalFullImage = QImage(); // Release memory

    if (originalImgWidth == 0 || originalImgHeight == 0) {
        qDebug() << "RealESRGAN_Image: Failed to load original dimensions for" << inputFile;
        item_Status->setText(tr("Error: Reading original dimensions failed"));
        if(alphaInfo.hasAlpha) QDir(alphaInfo.tempDir).removeRecursively();
        // ProcessNextFile(); // Or handle error state
        return;
    }


    RealESRGAN_NCNN_Vulkan_ReadSettings();

    // Use doubleSpinBox_ScaleRatio_image for target scale
    double targetOverallScaleDouble = ui->doubleSpinBox_ScaleRatio_image->value();
    if (targetOverallScaleDouble <= 0) targetOverallScaleDouble = 1.0; // Default to 1x if invalid

    // RealESRGAN_ProcessSingleFileIteratively expects int for targetOverallScale.
    // If targetOverallScaleDouble is not an integer, the resampling step is crucial.
    // For the AI passes, we might pass a rounded or ceiling scale, or use modelNativeScale.
    // Let's pass the double to ProcessSingleFileIteratively and let it handle rounding for AI passes if needed,
    // but use the precise double for final resampling target.
    // However, the function signature expects int targetOverallScale.
    // For now, let's assume user wants integer scaling for AI part, and resampling handles the rest.
    // Or, better, the targetOverallScale for ProcessSingleFileIteratively should be the final multiplier.
    // The current ProcessSingleFileIteratively takes int targetOverallScale.
    // Let's use the doubleSpinBox value directly for calculating final dimensions,
    // and use an int (e.g. ceil or round) for the AI pass planning.
    // The subtask asks for `targetOverallScale` to be passed. This should be the user's desired scale.
    // Let's assume `ui->doubleSpinBox_ScaleRatio_image->value()` can be passed as `targetOverallScale`.
    // The function signature change to `int targetOverallScale` is fine.

    int integerTargetScaleForAiPasses = qCeil(targetOverallScaleDouble);
    if (integerTargetScaleForAiPasses <= 0) integerTargetScaleForAiPasses = 1;


    QString gpuConfigToUse = m_realesrgan_GPUID; // Single GPU ID string
    bool isMultiConfig = false;
    if (ui->checkBox_MultiGPU_RealESRGAN->isChecked()){
        gpuConfigToUse = RealesrganNcnnVulkan_MultiGPU(); // Full multi-GPU job string
        isMultiConfig = true; // Mark that it's a multi-GPU config string
    }

    // This function is called by Waifu2xMainThread which should wrap it in QtConcurrent::run
    bool success = RealESRGAN_ProcessSingleFileIteratively(
        rgbInputPath, rgbOutputPath, targetOverallScaleDouble, // Pass the double scale for accurate final resampling
        originalImgWidth, originalImgHeight, // Pass original dimensions
        m_realesrgan_ModelNativeScale, m_realesrgan_ModelName, m_realesrgan_TileSize,
        gpuConfigToUse, isMultiConfig, m_realesrgan_TTA,
        ui->comboBox_OutFormat_Image->currentText(), // Output format for intermediate files if any, and final if not resampled
        rowNum
    );

    if (success && alphaInfo.hasAlpha && QFile::exists(rgbOutputPath)) { // rgbOutputPath now contains the resampled image
        RestoreAlpha(alphaInfo, rgbOutputPath, outputFile);
    }
    else if (!success && alphaInfo.hasAlpha) {
        QDir(alphaInfo.tempDir).removeRecursively();
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


// Processes a directory of images through RealESRGAN AI passes.
// finalAIOutputDir will be updated to the path of the directory containing the last AI pass results.
bool MainWindow::RealESRGAN_ProcessDirectoryIteratively(
    const QString &initialInputDir, QString &finalAIOutputDir, // finalAIOutputDir is an out-parameter
    int targetOverallScale, int modelNativeScale,
    const QString &modelName, // Model name for -n argument
    int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob,
    bool ttaEnabled, const QString &outputFormat)
{
    qDebug() << "RealESRGAN_ProcessDirectoryIteratively: InputDir" << initialInputDir
             << "TargetOverallScale" << targetOverallScale << "ModelNativeScale" << modelNativeScale;

    QList<int> scaleSequence = CalculateRealESRGANScaleSequence(targetOverallScale, modelNativeScale);
    if (scaleSequence.isEmpty()) {
        qDebug() << "RealESRGAN_ProcessDirectoryIteratively: Empty scale sequence generated.";
        emit Send_TextBrowser_NewMessage(tr("Error: Could not determine AI scaling passes for RealESRGAN."));
        return false;
    }
    // If targetOverallScale is 1, sequence might be [1]. If modelNativeScale is also 1, it's a 1x pass.
    // If targetOverallScale is 1 but modelNativeScale > 1, sequence might be [modelNativeScale],
    // which means it will upscale then require downscale. The resampling step after this function handles it.

    QString currentPassInputDir = initialInputDir;
    // Create a unique main temporary directory for this entire operation
    QString mainProcessingTempDir = QDir::tempPath() + "/W2XEX_RealESRGAN_DirIter_Main_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir mainTempD(mainProcessingTempDir);
    if (!mainTempD.mkpath(".")) {
        qDebug() << "RealESRGAN_ProcessDirectoryIteratively: Failed to create main temp directory " << mainProcessingTempDir;
        return false;
    }
    QList<QString> tempPassOutputDirs; // To clean up intermediate pass outputs in case of error

    bool success = true;
    for (int i = 0; i < scaleSequence.size(); ++i) {
        if (Stopping) { success = false; break; }

        int currentPassScaleForExe = scaleSequence[i]; // This is modelNativeScale for each pass in the sequence
        QString passOutputDir = QDir(mainProcessingTempDir).filePath(QString("pass_%1_out").arg(i));
        QDir().mkpath(passOutputDir); // Ensure current pass output dir is created
        tempPassOutputDirs.append(passOutputDir);

        QStringList arguments = RealESRGAN_NCNN_Vulkan_PrepareArguments(
            currentPassInputDir, passOutputDir, currentPassScaleForExe,
            modelName, tileSize, gpuIdOrJobConfig, isMultiGPUJob,
            ttaEnabled, outputFormat // For directory, outputFormat is for frames
        );

        emit Send_TextBrowser_NewMessage(tr("Starting RealESRGAN directory pass %1/%2 (Model Scale: %3x)...").arg(i + 1).arg(scaleSequence.size()).arg(currentPassScaleForExe));

        QProcess process;
        QString exePath = Current_Path + "/realesrgan-ncnn-vulkan-20220424-windows/realesrgan-ncnn-vulkan.exe";
        qDebug() << "RealESRGAN_ProcessDirectoryIteratively Pass" << i + 1 << "Cmd:" << exePath << arguments.join(" ");

        process.start(exePath, arguments);
        if (!process.waitForStarted(10000)) {
            qDebug() << "RealESRGAN_ProcessDirectoryIteratively: Process failed to start for pass" << i + 1;
            QMessageBox::warning(this, tr("Process Failure"),
                                 tr("RealESRGAN failed to start for directory pass %1").arg(i + 1));
            success = false;
            break;
        }

        while (process.state() != QProcess::NotRunning) {
            if (Stopping) { // Use the global/member stopping flag
                process.terminate();
                if (!process.waitForFinished(1500)) {
                    process.kill();
                    process.waitForFinished();
                }
                success = false;
                break;
            }
            if (process.waitForFinished(100)) { // Check every 100ms
                break;
            }
        }
        if (!success) break; // If stopped or failed in loop, break outer

        if (!warnProcessFailure(this, process,
                                tr("RealESRGAN directory pass %1").arg(i + 1))) {
            qDebug() << "RealESRGAN_ProcessDirectoryIteratively: Pass" << i + 1 << "failed. ExitCode:" << process.exitCode() << "Error:" << process.errorString();
            qDebug() << "STDERR:" << QString::fromLocal8Bit(process.readAllStandardError());
            qDebug() << "STDOUT:" << QString::fromLocal8Bit(process.readAllStandardOutput());
            success = false;
            break;
        }

        QDir checkOutDir(passOutputDir);
        if (checkOutDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot).isEmpty()) {
             qDebug() << "RealESRGAN_ProcessDirectoryIteratively: Output directory for pass " << i+1 << " is empty: " << passOutputDir;
             if (!QDir(currentPassInputDir).entryInfoList(QDir::Files | QDir::NoDotAndDotDot).isEmpty()){
                success = false; break;
             }
        }

        // Cleanup previous pass's output dir if it was a temporary one (i.e., not the initialInputDir)
        if (i > 0) { // For pass 0, currentPassInputDir is initialInputDir
             QDir prevPassDir(currentPassInputDir); // currentPassInputDir here is tempPassOutputDirs.at(i-1)
             if(prevPassDir.exists()) prevPassDir.removeRecursively();
        }
        currentPassInputDir = passOutputDir; // Output of this pass is input for the next
    }

    if (success && !Stopping) {
        finalAIOutputDir = currentPassInputDir; // This is the directory with the final AI pass results
    } else {
        // If failed or stopped, clean up ALL created temporary pass directories including currentPassInputDir if it's a temp
        for (const QString &dirPath : tempPassOutputDirs) {
            QDir d(dirPath);
            if(d.exists()) d.removeRecursively();
        }
        // Also remove the main processing temp dir
        if(mainTempD.exists()) mainTempD.removeRecursively();
        finalAIOutputDir.clear();
        return false;
    }
    // Caller is responsible for cleaning up mainProcessingTempDir eventually,
    // AFTER it's done with finalAIOutputDir (which is inside mainProcessingTempDir).
    return true;
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
    if (framesList.isEmpty()) { /* ... error handling ... */ QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); return; }

    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Sets m_realesrgan_gpuJobConfig_temp

    int targetScale = ui->spinBox_scaleRatio_gif->value();
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale; // Fallback to model's native if UI is 0 or less

    // --- Alpha Preparation ---
    emit Send_TextBrowser_NewMessage(tr("Preparing GIF frames (RGB/Alpha separation)... (RealESRGAN)"));
    QString rgbFramesTempDir = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_GIF_rgb_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString alphaBackupTempDir = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_GIF_alpha_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(rgbFramesTempDir); QDir().mkpath(alphaBackupTempDir);
    QMap<QString, AlphaInfo> frameAlphaInfosGIF;
    bool prepSuccessGIF = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Prep Alpha)"), framesList.count());
    for (const QString &frameFileName : framesList) {
        if (Stopping) { prepSuccessGIF = false; break; }
        CurrentFileProgress_progressbar_Add();
        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        AlphaInfo alphaInfo = PrepareAlpha(inputFramePath);
        frameAlphaInfosGIF.insert(frameFileName, alphaInfo);
        QString rgbFrameDestPath = QDir(rgbFramesTempDir).filePath(frameFileName);
        if (alphaInfo.rgbPath != inputFramePath) { // rgbPath is a temp from PrepareAlpha
            if (!QFile::copy(alphaInfo.rgbPath, rgbFrameDestPath)) { prepSuccessGIF = false; break; }
            if (alphaInfo.tempDir.startsWith(QDir::tempPath())) QDir(alphaInfo.tempDir).removeRecursively();
        } else { // No alpha, rgbPath is same as input
            if (!QFile::copy(inputFramePath, rgbFrameDestPath)) { prepSuccessGIF = false; break; }
        }
        if (alphaInfo.hasAlpha) {
            QString alphaBackupPath = QDir(alphaBackupTempDir).filePath(frameFileName);
            if (!QFile::copy(alphaInfo.alphaPath, alphaBackupPath)) { prepSuccessGIF = false; break; }
        }
    }
    CurrentFileProgress_Stop();
    if (!prepSuccessGIF || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error preparing alpha"));
        QDir(splitFramesFolder).removeRecursively(); QDir(rgbFramesTempDir).removeRecursively(); QDir(alphaBackupTempDir).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        return; // ProcessNextFile() will be called by the main thread manager if part of batch
    }

    // --- AI Processing on RGB frames directory ---
    QString scaledRgbFramesAIDirGIF;
    emit Send_TextBrowser_NewMessage(tr("Starting AI processing for GIF frames... (RealESRGAN)"));
    bool aiProcessingSuccessGIF = RealESRGAN_ProcessDirectoryIteratively(
        rgbFramesTempDir, scaledRgbFramesAIDirGIF, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName,
        m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(),
        m_realesrgan_TTA, "png" // Output AI pass as PNG
    );
    QDir(rgbFramesTempDir).removeRecursively();
    if (!aiProcessingSuccessGIF || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in AI processing"));
        QDir(splitFramesFolder).removeRecursively(); QDir(alphaBackupTempDir).removeRecursively(); QDir(scaledRgbFramesAIDirGIF).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        return;
    }

    // --- Alpha Restoration Loop ---
    emit Send_TextBrowser_NewMessage(tr("Restoring alpha to GIF frames... (RealESRGAN)"));
    QString combinedFramesAIDirGIF = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_GIF_combined_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(combinedFramesAIDirGIF);
    bool restoreAlphaSuccessGIF = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Restore Alpha)"), framesList.count());
    for (const QString &originalFrameFileName : framesList) {
        if (Stopping) { restoreAlphaSuccessGIF = false; break; }
        CurrentFileProgress_progressbar_Add();
        QString scaledRgbPath = QDir(scaledRgbFramesAIDirGIF).filePath(originalFrameFileName);
        QString combinedDestPath = QDir(combinedFramesAIDirGIF).filePath(originalFrameFileName);
        AlphaInfo currentAlphaInfo = frameAlphaInfosGIF.value(originalFrameFileName);
        if (currentAlphaInfo.hasAlpha) {
            QString alphaBackupPath = QDir(alphaBackupTempDir).filePath(originalFrameFileName);
            if (!QFile::exists(alphaBackupPath)) { QFile::copy(scaledRgbPath, combinedDestPath); continue; }
            AlphaInfo tempRestore = {true, scaledRgbPath, alphaBackupPath, "", false};
            if (!RestoreAlpha(tempRestore, scaledRgbPath, combinedDestPath, true, targetScale)) { restoreAlphaSuccessGIF = false; break; }
        } else {
            if (!QFile::copy(scaledRgbPath, combinedDestPath)) { restoreAlphaSuccessGIF = false; break; }
        }
    }
    CurrentFileProgress_Stop();
    QDir(scaledRgbFramesAIDirGIF).removeRecursively(); QDir(alphaBackupTempDir).removeRecursively();
    if (!restoreAlphaSuccessGIF || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error restoring alpha"));
        QDir(splitFramesFolder).removeRecursively(); QDir(combinedFramesAIDirGIF).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        return;
    }

    // --- Resampling Loop ---
    emit Send_TextBrowser_NewMessage(tr("Resampling final GIF frames... (RealESRGAN)"));
    QSize originalGifSize;
    if (!framesList.isEmpty()) { QImage ff(QDir(splitFramesFolder).filePath(framesList.first())); if(!ff.isNull()) originalGifSize = ff.size(); }
    if(originalGifSize.isEmpty()){ item_Status->setText(tr("Error: Get GIF original size failed")); QDir(splitFramesFolder).removeRecursively(); QDir(combinedFramesAIDirGIF).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); return;}
    int targetFrameWidth = qRound(static_cast<double>(originalGifSize.width()) * targetScale);
    int targetFrameHeight = qRound(static_cast<double>(originalGifSize.height()) * targetScale);
    QStringList combinedFramesForResample = file_getFileNames_in_Folder_nofilter(combinedFramesAIDirGIF);
    bool resamplingSuccessGIF = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Resample Output)"), combinedFramesForResample.count());
    for (const QString &frameName : combinedFramesForResample) {
        if (Stopping) { resamplingSuccessGIF = false; break; }
        CurrentFileProgress_progressbar_Add();
        QString combinedPath = QDir(combinedFramesAIDirGIF).filePath(frameName);
        QImage img(combinedPath); if (img.isNull()) { resamplingSuccessGIF = false; continue; }
        QImage resampled = img.scaled(targetFrameWidth, targetFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
        QString finalPath = QDir(scaledFramesFolder).filePath(frameName);
        QFileInfo finalPathInfo(finalPath); if(!QDir(finalPathInfo.path()).exists()) QDir().mkpath(finalPathInfo.path());
        if (!resampled.save(finalPath)) { resamplingSuccessGIF = false; break; }
    }
    CurrentFileProgress_Stop();
    QDir(combinedFramesAIDirGIF).removeRecursively();
    if (!resamplingSuccessGIF || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in final resampling"));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        return;
    }

    // --- GIF Assembly ---
    if (!Stopping) { Gif_assembleGif(resultFileFullPath, scaledFramesFolder, Gif_getDuration(sourceFileFullPath), false, 0, 0, false, sourceFileFullPath); /* ... update status ... */ }
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
    QString baseName = sourceFileInfo.completeBaseName(); // Used for temp folder naming
    QString splitFramesFolder = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_Vid_split_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString scaledFramesFolder = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_Vid_scaled_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz"); // Final resampled frames
    QString audioPath = Current_Path + "/temp_W2xEX/" + baseName + "_RealESRGAN_Vid_audio.m4a";
    QDir().mkpath(splitFramesFolder); QDir().mkpath(scaledFramesFolder); QFile::remove(audioPath);

    emit Send_TextBrowser_NewMessage(tr("Extracting video frames & audio: %1 (RealESRGAN)").arg(sourceFileFullPath));
    video_video2images(sourceFileFullPath, splitFramesFolder, audioPath);
    QStringList framesList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if (framesList.isEmpty()) { /* ... error handling ... */ return; }

    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Sets m_realesrgan_gpuJobConfig_temp
    int targetScale = ui->spinBox_scaleRatio_video->value();
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale;

    // --- AI Processing (Directory) ---
    QString scaledFramesFolderAI; // Output from directory processing
    emit Send_TextBrowser_NewMessage(tr("Starting AI processing for video frames... (RealESRGAN)"));
    bool aiSuccess = RealESRGAN_ProcessDirectoryIteratively(
        splitFramesFolder, scaledFramesFolderAI, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName,
        m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(),
        m_realesrgan_TTA, "png" // Output AI pass as PNG for quality
    );

    if (!aiSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in AI processing"));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolderAI).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath);
        return; // ProcessNextFile handled by caller/thread manager
    }

    // --- Resampling Loop ---
    emit Send_TextBrowser_NewMessage(tr("Resampling AI processed video frames... (RealESRGAN)"));
    QSize originalVideoSize = video_get_Resolution(sourceFileFullPath);
    if(originalVideoSize.isEmpty()){ item_Status->setText(tr("Error: Get original video size failed")); QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolderAI).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath); return;}
    int targetFrameWidth = qRound(static_cast<double>(originalVideoSize.width()) * targetScale);
    int targetFrameHeight = qRound(static_cast<double>(originalVideoSize.height()) * targetScale);
    QStringList aiFrames = file_getFileNames_in_Folder_nofilter(scaledFramesFolderAI);
    bool resamplingOk = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Resampling)"), aiFrames.count());
    for (const QString &frameName : aiFrames) {
        if (Stopping) { resamplingOk = false; break; }
        CurrentFileProgress_progressbar_Add();
        QString aiPath = QDir(scaledFramesFolderAI).filePath(frameName);
        QImage img(aiPath); if(img.isNull()){ resamplingOk = false; continue;}
        QImage resampled = img.scaled(targetFrameWidth, targetFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
        QString finalPath = QDir(scaledFramesFolder).filePath(frameName); // Save to final assembly folder
        QFileInfo finalPathInfo(finalPath); if(!QDir(finalPathInfo.path()).exists()) QDir().mkpath(finalPathInfo.path());
        if(!resampled.save(finalPath)) { resamplingOk = false; break;}
    }
    CurrentFileProgress_Stop();
    QDir(scaledFramesFolderAI).removeRecursively(); // Clean up intermediate AI frames

    if(!resamplingOk || Stopping){
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in resampling"));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath);
        return;
    }

    // --- Video Assembly ---
    if (!Stopping) { /* ... assemble ... */ video_images2video(sourceFileFullPath, resultFileFullPath, scaledFramesFolder, audioPath, false, 0, 0, false); /* ... update status ... */ }
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
        emit Send_TextBrowser_NewMessage(tr("Extracting frames for segment %1/%2... (RealESRGAN)").arg(i+1).arg(numSegments));
        ffmpegFrameExtract.start(FFMPEG_EXE_PATH_Waifu2xEX, extractArgs);
        if (!ffmpegFrameExtract.waitForStarted(5000) ||
            !ffmpegFrameExtract.waitForFinished(-1) ||
            !warnProcessFailure(this, ffmpegFrameExtract,
                                tr("FFmpeg frame extraction for segment %1").arg(i + 1))) {
            qDebug() << "RealESRGAN VideoBySegment: Failed to extract frames for segment" << i+1;
            overallSuccess = false;
            break;
        }
        QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
        if (framesFileName_qStrList.isEmpty() && currentSegmentDuration > 0) { /* Allow empty if duration is 0 */
             qDebug() << "RealESRGAN VideoBySegment: No frames extracted for segment" << i+1 << "though duration was" << currentSegmentDuration;
             // Depending on strictness, this could be an error or just an empty segment.
        }

        // --- AI Processing for the current segment's frames ---
        QString segmentScaledFramesFolderAI; // Output from directory processing
        emit Send_TextBrowser_NewMessage(tr("Starting AI processing for segment %1/%2 frames... (RealESRGAN)").arg(i+1).arg(numSegments));
        bool aiSegmentSuccess = RealESRGAN_ProcessDirectoryIteratively(
            splitFramesFolder, segmentScaledFramesFolderAI, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName,
            m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(),
            m_realesrgan_TTA, "png"
        );

        if (!aiSegmentSuccess || Stopping) {
            qDebug() << "RealESRGAN VideoBySegment: AI processing failed or stopped for segment" << i+1;
            overallSuccess = false; break;
        }

        // --- Resampling Loop for the current segment's AI processed frames ---
        emit Send_TextBrowser_NewMessage(tr("Resampling AI frames for segment %1/%2... (RealESRGAN)").arg(i+1).arg(numSegments));
        QSize originalVideoSizeSeg = video_get_Resolution(sourceFileFullPath);
        if(originalVideoSizeSeg.isEmpty()){
            qDebug() << "RealESRGAN VideoBySegment: Failed to get original video dimensions for resampling segment" << i+1;
            overallSuccess = false; break;
        }
        int targetSegFrameWidth = qRound(static_cast<double>(originalVideoSizeSeg.width()) * targetScale);
        int targetSegFrameHeight = qRound(static_cast<double>(originalVideoSizeSeg.height()) * targetScale);

        QStringList aiSegmentFramesList = file_getFileNames_in_Folder_nofilter(segmentScaledFramesFolderAI);
         if (aiSegmentFramesList.isEmpty() && !framesFileName_qStrList.isEmpty()) {
             qDebug() << "RealESRGAN VideoBySegment: AI processed folder for segment " << i+1 << " is empty, but original frames existed.";
             overallSuccess = false; break;
        }

        bool segResamplingSuccess = true;
        CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Seg %1 Resample)").arg(i+1), aiSegmentFramesList.count());
        for (const QString &aiFrameFileName : aiSegmentFramesList) {
            if (Stopping) { segResamplingSuccess = false; break; }
            CurrentFileProgress_progressbar_Add();
            QString aiFramePath = QDir(segmentScaledFramesFolderAI).filePath(aiFrameFileName);
            QImage aiImage(aiFramePath);
            if (aiImage.isNull()) { segResamplingSuccess = false; continue; }
            QImage resampledImage = aiImage.scaled(targetSegFrameWidth, targetSegFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
            QString finalSegFramePath = QDir(scaledFramesFolder).filePath(aiFrameFileName); // scaledFramesFolder is segment_XXX_scaled
            QFileInfo finalSegFrameInfo(finalSegFramePath);
            QDir finalSegFrameDir(finalSegFrameInfo.path());
            if(!finalSegFrameDir.exists()) finalSegFrameDir.mkpath(".");
            if (!resampledImage.save(finalSegFramePath)) { segResamplingSuccess = false; break; }
        }
        CurrentFileProgress_Stop();
        QDir(segmentScaledFramesFolderAI).removeRecursively(); // Clean up intermediate AI output for segment

        if (!segResamplingSuccess || Stopping) {
            qDebug() << "RealESRGAN VideoBySegment: Resampling failed or stopped for segment" << i+1;
            overallSuccess = false; break;
        }

        // --- Assemble video segment ---
        QString processedSegmentVideoPath = QDir(mainTempFolder).filePath(segmentName + "_processed.mp4");
        emit Send_TextBrowser_NewMessage(tr("Assembling video segment %1/%2... (RealESRGAN)").arg(i + 1).arg(numSegments));
        video_images2video(sourceFileFullPath, processedSegmentVideoPath, scaledFramesFolder, "", false, 0, 0, false);
        if (!QFile::exists(processedSegmentVideoPath)) {
            qDebug() << "RealESRGAN VideoBySegment: Failed to assemble video segment" << i+1;
            overallSuccess = false; break;
        }
        processedVideoSegmentsPaths.append(processedSegmentVideoPath);
        QDir(splitFramesFolder).removeRecursively(); // Clean up original split frames for the segment
        // scaledFramesFolder (containing resampled frames) for the segment is kept until final concatenation
    }

    if (overallSuccess && !Stopping) {
        // ... (concatenation and muxing logic remains the same) ...
        emit Send_TextBrowser_NewMessage(tr("Concatenating processed video segments... (RealESRGAN)"));
        QString concatFilePath = QDir(mainTempFolder).filePath("concat_list.txt");
        QFile concatFile(concatFilePath);
        if (concatFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&concatFile);
            for (const QString &segmentPath : processedVideoSegmentsPaths) {
                out << "file '" << QDir::toNativeSeparators(segmentPath) << "'\n";
            }
            concatFile.close();
        } else { overallSuccess = false; }

        if(overallSuccess) {
            QString concatenatedVideoPath = QDir(mainTempFolder).filePath(sourceFileNameNoExt + "_concat_noaudio.mp4");
            QProcess ffmpegConcat;
            QStringList concatArgs;
            concatArgs << "-f" << "concat" << "-safe" << "0" << "-i" << concatFilePath << "-c" << "copy" << concatenatedVideoPath;
            ffmpegConcat.start(FFMPEG_EXE_PATH_Waifu2xEX, concatArgs);
            if (!ffmpegConcat.waitForStarted(5000) ||
                !ffmpegConcat.waitForFinished(-1) ||
                !warnProcessFailure(this, ffmpegConcat, tr("FFmpeg concat"))) {
                overallSuccess = false;
            } else {
                emit Send_TextBrowser_NewMessage(tr("Muxing final video with audio... (RealESRGAN)"));
                QProcess ffmpegMux;
                QStringList muxArgs;
                muxArgs << "-i" << concatenatedVideoPath << "-i" << fullAudioPath << "-c:v" << "copy" << "-c:a" << "aac" << "-b:a" << ui->spinBox_bitrate_audio->text()+"k" << "-shortest" << finalResultFileFullPath;
                ffmpegMux.start(FFMPEG_EXE_PATH_Waifu2xEX, muxArgs);
                if (!ffmpegMux.waitForStarted(5000) ||
                    !ffmpegMux.waitForFinished(-1) ||
                    !warnProcessFailure(this, ffmpegMux, tr("FFmpeg mux"))) {
                    overallSuccess = false;
                }
            }
        }
    }
    /* ... final status update & cleanup ... */
    // Cleanup scaledFramesFolder for each segment, which were kept for concatenation
    for (int k=0; k<numSegments; ++k) {
        QString segScaledPath = QDir(mainTempFolder).filePath(QString("segment_%1").arg(k, 3, 10, QChar('0')) + "_scaled");
        QDir(segScaledPath).removeRecursively();
    }
    QDir(mainTempFolder).removeRecursively();
}

void MainWindow::APNG_RealESRGANCNNVulkan(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath) {
    Q_UNUSED(sourceFileFullPath); Q_UNUSED(resultFileFullPath);
    qDebug() << "APNG_RealESRGANCNNVulkan started. Input:" << splitFramesFolder << "Output:" << scaledFramesFolder;
    RealESRGAN_NCNN_Vulkan_ReadSettings();
    RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(0);
    int targetScale = ui->spinBox_Scale_RealESRGAN->value(); // Use RealESRGAN tab's scale or general image scale
    if (targetScale <= 0) targetScale = m_realesrgan_ModelNativeScale;

    // Ensure final output scaledFramesFolder exists (it's an output param for this func, APNG_Main creates it)
    QDir finalOutputDirCheck(scaledFramesFolder);
    if (!finalOutputDirCheck.exists() && !finalOutputDirCheck.mkpath(".")) {
        qDebug() << "APNG_RealESRGAN: Failed to create final output directory:" << scaledFramesFolder;
        return;
    }

    QString sourceFileNameNoExt = QFileInfo(sourceFileFullPath).completeBaseName();

    // --- Alpha Preparation ---
    emit Send_TextBrowser_NewMessage(tr("Preparing APNG frames (RGB/Alpha separation)... (RealESRGAN)"));
    QString rgbFramesTempDirAPNG = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RES_APNG_rgb_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString alphaBackupTempDirAPNG = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RES_APNG_alpha_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(rgbFramesTempDirAPNG); QDir().mkpath(alphaBackupTempDirAPNG);
    QMap<QString, AlphaInfo> frameAlphaInfosAPNG;
    bool prepSuccessAPNG = true;
    qDebug() << "APNG_RealESRGAN: Starting alpha prep for" << framesFileName_qStrList.count() << "frames.";
    for (const QString &frameFileName : framesFileName_qStrList) {
        if (Stopping) { prepSuccessAPNG = false; break; }
        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        AlphaInfo alphaInfo = PrepareAlpha(inputFramePath);
        frameAlphaInfosAPNG.insert(frameFileName, alphaInfo);
        QString rgbFrameDestPath = QDir(rgbFramesTempDirAPNG).filePath(frameFileName);
        if (alphaInfo.rgbPath != inputFramePath) {
            if (!QFile::copy(alphaInfo.rgbPath, rgbFrameDestPath)) { prepSuccessAPNG = false; break; }
            if (alphaInfo.tempDir.startsWith(QDir::tempPath())) QDir(alphaInfo.tempDir).removeRecursively();
        } else {
            if (!QFile::copy(inputFramePath, rgbFrameDestPath)) { prepSuccessAPNG = false; break; }
        }
        if (alphaInfo.hasAlpha) {
            QString alphaBackupPath = QDir(alphaBackupTempDirAPNG).filePath(frameFileName);
            if (!QFile::copy(alphaInfo.alphaPath, alphaBackupPath)) { prepSuccessAPNG = false; break; }
        }
    }
    if (!prepSuccessAPNG || Stopping) {
        qDebug() << "APNG_RealESRGAN: Error during alpha prep or stopped.";
        QDir(rgbFramesTempDirAPNG).removeRecursively(); QDir(alphaBackupTempDirAPNG).removeRecursively();
        return;
    }

    // --- AI Processing ---
    QString scaledRgbFramesAIDirAPNG;
    emit Send_TextBrowser_NewMessage(tr("Starting AI processing for APNG frames... (RealESRGAN)"));
    bool aiProcessingSuccessAPNG = RealESRGAN_ProcessDirectoryIteratively(
        rgbFramesTempDirAPNG, scaledRgbFramesAIDirAPNG, targetScale, m_realesrgan_ModelNativeScale, m_realesrgan_ModelName,
        m_realesrgan_TileSize, m_realesrgan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealESRGAN->isChecked(),
        m_realesrgan_TTA, "png"
    );
    QDir(rgbFramesTempDirAPNG).removeRecursively();
    if (!aiProcessingSuccessAPNG || Stopping) {
        qDebug() << "APNG_RealESRGAN: Error during AI processing or stopped.";
        QDir(alphaBackupTempDirAPNG).removeRecursively(); QDir(scaledRgbFramesAIDirAPNG).removeRecursively();
        return;
    }

    // --- Alpha Restoration ---
    emit Send_TextBrowser_NewMessage(tr("Restoring alpha to APNG frames... (RealESRGAN)"));
    QString combinedFramesAIDirAPNG = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RES_APNG_combined_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(combinedFramesAIDirAPNG);
    bool restoreAlphaSuccessAPNG = true;
    for (const QString &originalFrameFileName : framesFileName_qStrList) {
        if (Stopping) { restoreAlphaSuccessAPNG = false; break; }
        QString scaledRgbPath = QDir(scaledRgbFramesAIDirAPNG).filePath(originalFrameFileName);
        QString combinedDestPath = QDir(combinedFramesAIDirAPNG).filePath(originalFrameFileName);
        AlphaInfo currentAlphaInfo = frameAlphaInfosAPNG.value(originalFrameFileName);
        if (currentAlphaInfo.hasAlpha) {
            QString alphaBackupPath = QDir(alphaBackupTempDirAPNG).filePath(originalFrameFileName);
            if (!QFile::exists(alphaBackupPath)) { QFile::copy(scaledRgbPath, combinedDestPath); continue; }
            AlphaInfo tempRestore = {true, scaledRgbPath, alphaBackupPath, "", false};
            if (!RestoreAlpha(tempRestore, scaledRgbPath, combinedDestPath, true, targetScale)) { restoreAlphaSuccessAPNG = false; break; }
        } else {
            if (!QFile::copy(scaledRgbPath, combinedDestPath)) { restoreAlphaSuccessAPNG = false; break; }
        }
    }
    QDir(scaledRgbFramesAIDirAPNG).removeRecursively(); QDir(alphaBackupTempDirAPNG).removeRecursively();
    if (!restoreAlphaSuccessAPNG || Stopping) {
        qDebug() << "APNG_RealESRGAN: Error during alpha restoration or stopped.";
        QDir(combinedFramesAIDirAPNG).removeRecursively(); return;
    }

    // --- Resampling Loop ---
    emit Send_TextBrowser_NewMessage(tr("Resampling final APNG frames... (RealESRGAN)"));
    QSize originalApngSize;
    if (!framesFileName_qStrList.isEmpty()) { QImage ff(QDir(splitFramesFolder).filePath(framesFileName_qStrList.first())); if(!ff.isNull()) originalApngSize = ff.size(); }
    if(originalApngSize.isEmpty()){ qDebug() << "APNG_RealESRGAN: Failed to get original APNG dimensions."; QDir(combinedFramesAIDirAPNG).removeRecursively(); return; }
    int targetFrameWidth = qRound(static_cast<double>(originalApngSize.width()) * targetScale);
    int targetFrameHeight = qRound(static_cast<double>(originalApngSize.height()) * targetScale);
    QStringList combinedFramesForResampleAPNG = file_getFileNames_in_Folder_nofilter(combinedFramesAIDirAPNG);
    bool resamplingSuccessAPNG = true;
    for (const QString &frameName : combinedFramesForResampleAPNG) {
        if (Stopping) { resamplingSuccessAPNG = false; break; }
        QString combinedPath = QDir(combinedFramesAIDirAPNG).filePath(frameName);
        QImage img(combinedPath); if (img.isNull()) { resamplingSuccessAPNG = false; continue; }
        QImage resampled = img.scaled(targetFrameWidth, targetFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
        QString finalPath = QDir(scaledFramesFolder).filePath(frameName); // scaledFramesFolder is the function's final output dir
        QFileInfo finalPathInfo(finalPath); if(!QDir(finalPathInfo.path()).exists()) QDir().mkpath(finalPathInfo.path());
        if (!resampled.save(finalPath)) { resamplingSuccessAPNG = false; break; }
    }
    QDir(combinedFramesAIDirAPNG).removeRecursively();
    if (!resamplingSuccessAPNG || Stopping) {
        qDebug() << "APNG_RealESRGAN: Error during resampling or stopped."; return;
    }

    qDebug() << "APNG_RealESRGANCNNVulkan: All frame processing completed successfully.";
    emit Send_TextBrowser_NewMessage(tr("All APNG frames processed and resampled (RealESRGAN)."));
    // APNG_Main will use frames from scaledFramesFolder.
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
    ui->listWidget_GPUList_MultiGPU_RealESRGAN->addItem(QString("ID: %1, Name: %2, Threads: %3, Tile: %4 (%5)")
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

