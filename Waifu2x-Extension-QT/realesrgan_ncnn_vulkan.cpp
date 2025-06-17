#include "mainwindow.h"
#include <QDebug>

QStringList MainWindow::RealESRGAN_NCNN_Vulkan_PrepareArguments(
    const QString& input_path,
    const QString& output_path,
    int scale_arg, // Note: scale is often part of the model name for RealESRGAN
    const QString& model_name, // This is -n
    int gpu_id,
    const QString& format,
    bool tta_mode,
    bool verbose_log,
    const QString& tile_size_str)
{
    QStringList args;

    // -i input-path
    args << "-i" << input_path;

    // -o output-path
    args << "-o" << output_path;

    // -n model-name (This is the primary model selector)
    args << "-n" << model_name;

    // -s scale (The actual scale factor, RealESRGAN models often have scale in their name,
    // but the -s param might still be needed if the model is generic or for verification)
    // For many RealESRGAN models, the scale is implicit in the model chosen with -n.
    // However, some models like realesr-animevideov3 use -s to select a submodel file.
    // Example: realesr-animevideov3-x2.bin, realesr-animevideov3-x4.bin
    // If model_name already contains scale (e.g. "realesrgan-x4plus"), -s might be redundant or used differently.
    // For now, we pass it as provided.
    args << "-s" << QString::number(scale_arg);

    // -t tile-size
    if (!tile_size_str.isEmpty() && tile_size_str != "0" && tile_size_str != "auto") {
        args << "-t" << tile_size_str;
    } else {
        args << "-t" << "0"; // Auto tile size
    }

    // -m model-path (Assuming models are in a directory relative to the engine)
    // This path needs to be determined correctly, possibly from settings or a fixed relative path.
    // For now, let's assume "models" is a subdirectory where the executable is.
    // This will be refined when we know where the executables and models are deployed.
    args << "-m" << "models"; // Placeholder - this needs to be correct relative to exe

    // -g gpu-id
    args << "-g" << QString::number(gpu_id);

    // -j load:proc:save (Using default values for now, can be configurable later)
    // Example: "1:2:2" (1 load thread, 2 proc threads, 2 save threads)
    // Proc threads can be per-GPU if multiple GPUs are used, e.g., "1:2,2:2" for 2 GPUs
    // This might come from global settings.
    QString jobs_read = Settings_Read_value("/settings/RealESRGANJobs").toString();
    if (jobs_read.isEmpty()) jobs_read = "1:2:2"; // Default value
    QString jobs = jobs_read;
    args << "-j" << jobs;


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

void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int file_list_row_number, bool isBatch)
{
    if (isProcessing) {
        if (isBatch) return; // Don't start if already processing in batch mode
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }

    isProcessing = true;
    currentProcess = new QProcess(this);

    // Determine executable path (adjust as per actual deployment)
    QString enginePath = Current_Path + "/Engines/realesrgan-ncnn-vulkan/";
    QString executableName = "realesrgan-ncnn-vulkan.exe"; // Assuming Windows, adjust for other platforms
    QString executableFullPath = enginePath + executableName;

    // --- Get parameters from UI/Settings ---
    // These would typically be read from ui->comboBox_Engine_Image, ui->spinBox_ScaleRatio_image, etc.
    // Using placeholder values for now.
    QString input_path = table_image_item_fullpath.at(file_list_row_number);
    QString original_fileName = table_image_item_fileName.at(file_list_row_number);
    QString output_path = Generate_Output_Path(original_fileName, "RealESRGAN-NCNN-Vulkan");

    int scale = ui->doubleSpinBox_ScaleRatio_image->value(); // Placeholder for actual UI element
    QString model_name = ui->comboBox_Model_RealsrNCNNVulkan->currentText(); // Placeholder
    int gpu_id = ui->comboBox_GPUID_RealsrNCNNVulkan->currentText().toInt(); // Placeholder
    QString format = ui->comboBox_ImageSaveFormat->currentText(); // Placeholder
    bool tta_mode = ui->checkBox_TTA_RealsrNCNNVulkan->isChecked(); // Placeholder
    bool verbose_log = true; // Or from a setting
    QString tile_size_str = ui->spinBox_TileSize_RealsrNCNNVulkan->text(); // Placeholder

    // Update model path to be relative to the engine executable
    QString model_path_abs = enginePath + "models";
    // The -m parameter for realesrgan expects just the folder name if it's standard, or full path
    // For now, assuming the executable handles finding "models" if it's in the same dir or sub dir.
    // If realesrgan requires an absolute path to models, then model_path_abs should be passed.
    // The current PrepareArguments uses a relative "models".

    QStringList arguments = RealESRGAN_NCNN_Vulkan_PrepareArguments(
        input_path, output_path, scale, model_name, gpu_id, format, tta_mode, verbose_log, tile_size_str
    );

    // Ensure the model path argument is correct if it needs to be absolute
    // This might involve modifying PrepareArguments or adjusting it here.
    // For now, we assume PrepareArguments handles it with the "models" relative path.
    // If RealESRGAN needs an absolute model path:
    // int model_arg_idx = arguments.indexOf("-m");
    // if(model_arg_idx != -1 && model_arg_idx + 1 < arguments.length()){
    //     arguments[model_arg_idx+1] = model_path_abs;
    // }


    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessStdOut()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onProcessStdErr()));

    qDebug() << "Starting RealESRGAN for image:" << input_path;
    qDebug() << "Arguments:" << arguments.join(" ");
    qDebug() << "Executable:" << executableFullPath;

    currentProcess->setWorkingDirectory(enginePath); // Important for finding models if relative paths are used
    currentProcess->start(executableFullPath, arguments);

    // Update UI: progress bar, status text, disable start button, etc.
    Set_Progress_Bar_Value(0, 0); // Overall progress
    Set_Current_File_Progress_Bar_Value(0,0); // Current file progress
    ui->label_ETA->setText(tr("ETA: Calculating..."));
    ui->pushButton_Start->setEnabled(false);
    ui->pushButton_Stop->setEnabled(true);
    Processing_Status = PROCESS_TYPE_IMAGE; // Set current process type
    current_File_Row_Number = file_list_row_number; // Store current file being processed
}

void MainWindow::RealESRGAN_NCNN_Vulkan_GIF(int file_list_row_number)
{
    if (isProcessing) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }

    isProcessing = true;
    currentProcess = new QProcess(this);

    QString enginePath = Current_Path + "/Engines/realesrgan-ncnn-vulkan/";
    QString executableName = "realesrgan-ncnn-vulkan.exe";
    QString executableFullPath = enginePath + executableName;

    // Assuming input_path for GIF is a directory of frames extracted by another function
    QString input_path = TempDir_Path + "/" + table_gif_item_fileName.at(file_list_row_number) + "_frames/";
    QString original_fileName = table_gif_item_fileName.at(file_list_row_number);
    // Output path should also be a directory for processed frames
    QString output_path = TempDir_Path + "/" + original_fileName + "_frames_RealESRGAN_NCNN_Vulkan/";

    int scale = ui->doubleSpinBox_ScaleRatio_gif->value(); // Placeholder
    QString model_name = ui->comboBox_Model_RealsrNCNNVulkan->currentText(); // Placeholder
    int gpu_id = ui->comboBox_GPUID_RealsrNCNNVulkan->currentText().toInt(); // Placeholder
    QString format = "png"; // Frames should be processed losslessly
    bool tta_mode = ui->checkBox_TTA_RealsrNCNNVulkan->isChecked(); // Placeholder
    bool verbose_log = true;
    QString tile_size_str = ui->spinBox_TileSize_RealsrNCNNVulkan->text(); // Placeholder

    QStringList arguments = RealESRGAN_NCNN_Vulkan_PrepareArguments(
        input_path, output_path, scale, model_name, gpu_id, format, tta_mode, verbose_log, tile_size_str
    );

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessStdOut()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onProcessStdErr()));

    qDebug() << "Starting RealESRGAN for GIF (frames):" << input_path;
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

void MainWindow::RealESRGAN_NCNN_Vulkan_Video(int file_list_row_number)
{
    if (isProcessing) {
        QMessageBox::information(this, tr("Error"), tr("Another process is already running. Please wait."));
        return;
    }

    isProcessing = true;
    currentProcess = new QProcess(this);

    QString enginePath = Current_Path + "/Engines/realesrgan-ncnn-vulkan/";
    QString executableName = "realesrgan-ncnn-vulkan.exe";
    QString executableFullPath = enginePath + executableName;

    // Assuming input_path for Video is a directory of frames
    QString input_path = TempDir_Path + "/" + table_video_item_fileName.at(file_list_row_number) + "_frames/";
    QString original_fileName = table_video_item_fileName.at(file_list_row_number);
    QString output_path = TempDir_Path + "/" + original_fileName + "_frames_RealESRGAN_NCNN_Vulkan/";

    int scale = ui->doubleSpinBox_ScaleRatio_video->value(); // Placeholder
    QString model_name = ui->comboBox_Model_RealsrNCNNVulkan->currentText(); // Placeholder
    int gpu_id = ui->comboBox_GPUID_RealsrNCNNVulkan->currentText().toInt(); // Placeholder
    QString format = "png"; // Frames processed losslessly
    bool tta_mode = ui->checkBox_TTA_RealsrNCNNVulkan->isChecked(); // Placeholder
    bool verbose_log = true;
    QString tile_size_str = ui->spinBox_TileSize_RealsrNCNNVulkan->text(); // Placeholder

    QStringList arguments = RealESRGAN_NCNN_Vulkan_PrepareArguments(
        input_path, output_path, scale, model_name, gpu_id, format, tta_mode, verbose_log, tile_size_str
    );

    connect(currentProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int, QProcess::ExitStatus)));
    connect(currentProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
    connect(currentProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(onProcessStdOut()));
    connect(currentProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(onProcessStdErr()));

    qDebug() << "Starting RealESRGAN for Video (frames):" << input_path;
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

void MainWindow::RealESRGAN_NCNN_Vulkan_Video_BySegment(int)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_Video_BySegment stub";
}

void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings()
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(int)
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_PreLoad_Settings()
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_CleanupTempFiles(const QString&, int, bool, const QString&)
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

void MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU()
{
    if (!pushButton_DetectGPU_RealsrNCNNVulkan)
        return;

    pushButton_DetectGPU_RealsrNCNNVulkan->setEnabled(false);
    pushButton_DetectGPU_RealsrNCNNVulkan->setText(tr("Detecting..."));

    QString exePath = Current_Path + "/Engines/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan";
#ifdef Q_OS_WIN
    exePath += ".exe";
#endif
    QProcess proc;
    QByteArray out, err;
    runProcess(&proc, QString("\"%1\" -h").arg(exePath), &out, &err);
    QString text = QString::fromLocal8Bit(out + err);
    Available_GPUID_RealESRGAN_ncnn_vulkan = parseVulkanDeviceList(text);

    RealESRGAN_ncnn_vulkan_DetectGPU_finished();
}

void MainWindow::RealESRGAN_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)
{
}

