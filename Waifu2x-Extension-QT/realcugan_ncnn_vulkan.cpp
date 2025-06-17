#include "mainwindow.h"
#include <QDebug>

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
    int noise_level = ui->spinBox_DenoiseLevel_image->value(); // General denoise, might need RealCUGAN specific UI
    // RealCUGAN model dir is typically 'models-se', 'models-pro', 'models-nose'.
    // This needs a dedicated ComboBox in UI, e.g. ui->comboBox_ModelDir_RealCUGAN
    QString model_dir_name_read = Settings_Read_value("/settings/RealCUGANModelDir").toString(); // Placeholder
    if (model_dir_name_read.isEmpty()) model_dir_name_read = "models-se";
    QString model_dir_name = model_dir_name_read;
    int scale = ui->doubleSpinBox_ScaleRatio_image->value();
    int gpu_id_read = Settings_Read_value("/settings/RealCUGANGpuId").toInt(); // Placeholder, e.g. ui->comboBox_GPUID_RealCUGAN
    // Assuming -1 or default int() if conversion fails is acceptable.
    int gpu_id = gpu_id_read;
    QString format = ui->comboBox_ImageSaveFormat->currentText();
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


void MainWindow::Realcugan_NCNN_Vulkan_Video_BySegment(int)
{
    qDebug() << "Realcugan_NCNN_Vulkan_Video_BySegment stub";
}

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
    int noise_level = ui->spinBox_DenoiseLevel_video->value(); // General denoise for video
    QString model_dir_name_read = Settings_Read_value("/settings/RealCUGANModelDirVideo").toString(); // Placeholder
    if (model_dir_name_read.isEmpty()) model_dir_name_read = "models-se";
    QString model_dir_name = model_dir_name_read;
    int scale = ui->doubleSpinBox_ScaleRatio_video->value();
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
    int noise_level = ui->spinBox_DenoiseLevel_gif->value(); // General denoise for gif
    QString model_dir_name_read = Settings_Read_value("/settings/RealCUGANModelDirGif").toString(); // Placeholder
    if (model_dir_name_read.isEmpty()) model_dir_name_read = "models-se";
    QString model_dir_name = model_dir_name_read;
    int scale = ui->doubleSpinBox_ScaleRatio_gif->value();
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
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int)
{
}

void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings()
{
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

