/*
    Copyright (C) 2025  beyawnko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/beyawnko
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RealCuganProcessor.h"
#include "VideoProcessor.h"
#include "ProcessRunner.h"
#include "GpuManager.h"
#include "UiController.h"
#include "LiquidGlassWidget.h"
#include "anime4kprocessor.h"
#include "realesrganprocessor.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

// Includes for various Qt modules
#include <QGridLayout>
#include <QCheckBox>
#include <QApplication>
#include <QEventLoop>
#include <QMessageBox>
#include <QThreadPool>
#include <QThread>
#include <QEvent>
#include <QCloseEvent>
#include <QList>
#include <QMap>
#include <QDebug>
#include <QFileDialog>
#include <QImageReader>
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QTextCodec>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardItem>
#include <algorithm>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>

MainWindow::MainWindow(int maxThreadsOverride, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize state and members
    m_currentState = ProcessingState::Idle;
    isProcessing = false;
    currentProcess = nullptr;
    Processing_Status = PROCESS_TYPE_NONE;
    current_File_Row_Number = -1;
    TempDir_Path = Current_Path + "/temp";
    FFMPEG_EXE_PATH_Waifu2xEX = Current_Path + "/ffmpeg/ffmpeg.exe";

    // Setup Processors that actually exist
    m_anime4kProcessor = new Anime4KProcessor(this);
    connect(m_anime4kProcessor, &Anime4KProcessor::logMessage, this, &MainWindow::TextBrowser_NewMessage);
    connect(m_anime4kProcessor, &Anime4KProcessor::statusChanged, this, &MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString);
    connect(m_anime4kProcessor, &Anime4KProcessor::processingFinished, this, &MainWindow::onProcessingFinished);

    m_realEsrganProcessor = new RealEsrganProcessor(this);
    connect(m_realEsrganProcessor, &RealEsrganProcessor::logMessage, this, &MainWindow::TextBrowser_NewMessage);
    connect(m_realEsrganProcessor, &RealEsrganProcessor::statusChanged, this, &MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString);
    connect(m_realEsrganProcessor, &RealEsrganProcessor::fileProgress, this, &MainWindow::onFileProgress);
    connect(m_realEsrganProcessor, &RealEsrganProcessor::processingFinished, this, &MainWindow::onProcessingFinished);

    realCuganProcessor = new RealCuganProcessor(this);
    connect(realCuganProcessor, &RealCuganProcessor::logMessage, this, &MainWindow::TextBrowser_NewMessage);
    connect(realCuganProcessor, &RealCuganProcessor::statusChanged, this, &MainWindow::Table_video_ChangeStatus_rowNumInt_statusQString);
    connect(realCuganProcessor, &RealCuganProcessor::fileProgress, this, &MainWindow::onFileProgress);
    connect(realCuganProcessor, &RealCuganProcessor::processingFinished, this, &MainWindow::onProcessingFinished);

    // Setup other components
    videoProcessor = new VideoProcessor(this);
    qRegisterMetaType<FileMetadata>("FileMetadata");

    // Thread pool setup
    QVariant maxThreadCountSetting = Settings_Read_value("/settings/MaxThreadCount");
    globalMaxThreadCount = maxThreadCountSetting.isValid() ? maxThreadCountSetting.toInt() : 0;
    if (maxThreadsOverride > 0) globalMaxThreadCount = maxThreadsOverride;
    if (globalMaxThreadCount <= 0) {
        int cores = QThread::idealThreadCount();
        globalMaxThreadCount = (cores > 1) ? cores : 2;
    }
    QThreadPool::globalInstance()->setMaxThreadCount(globalMaxThreadCount);
    ui->spinBox_ThreadNum_image->setMaximum(globalMaxThreadCount);
    ui->spinBox_ThreadNum_gif_internal->setMaximum(globalMaxThreadCount);
    ui->spinBox_ThreadNum_video_internal->setMaximum(globalMaxThreadCount);
    if (ui->spinBox_NumOfThreads_VFI) {
        ui->spinBox_NumOfThreads_VFI->setMaximum(globalMaxThreadCount);
    }

    setWindowTitle(QStringLiteral("Beya_Waifu %1 by beyawnko").arg(VERSION));
    translator = new QTranslator(this);

    // Hide the clutter panel below the text browser
    if (widget_RealCUGAN_Hidden) { // Add null check for safety
        widget_RealCUGAN_Hidden->setVisible(false);
    }

    // Configure the main splitter for better resizing behavior
    ui->homeMainSplitter->setStretchFactor(0, 1); // Give file list side priority on expanding
    ui->homeMainSplitter->setStretchFactor(1, 0); // Keep settings side a more fixed size
    ui->homeMainSplitter->setSizes(QList<int>() << this->width() * 0.6 << this->width() * 0.4); // Set initial sizes relative to window width

    ApplyDarkStyle();
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- CORE ASYNC PROCESSING LOGIC ---
void MainWindow::on_pushButton_Start_clicked()
{
    if (m_currentState != ProcessingState::Idle) {
        TextBrowser_NewMessage(tr("Already processing. Please stop the current job first."));
        return;
    }
    m_currentState = ProcessingState::Processing;
    pushButton_Start_setEnabled_self(false);
    pushButton_Stop_setEnabled_self(true);
    m_jobQueue.clear();
    m_FinishedProc = 0;
    m_ErrorProc = 0;
    for (int i = 0; i < Table_model_image->rowCount(); ++i) {
        if (Table_model_image->item(i, 1)->text() == tr("Waiting")) {
            m_jobQueue.enqueue({i, PROCESS_TYPE_IMAGE});
        }
    }
    for (int i = 0; i < Table_model_gif->rowCount(); ++i) {
        if (Table_model_gif->item(i, 1)->text() == tr("Waiting")) {
            m_jobQueue.enqueue({i, PROCESS_TYPE_GIF});
        }
    }
    for (int i = 0; i < Table_model_video->rowCount(); ++i) {
        if (Table_model_video->item(i, 1)->text() == tr("Waiting")) {
            m_jobQueue.enqueue({i, PROCESS_TYPE_VIDEO});
        }
    }
    m_TotalNumProc = m_jobQueue.count();
    UpdateProgressBar();
    if (m_jobQueue.isEmpty()) {
        TextBrowser_NewMessage(tr("No files are waiting for processing."));
        m_currentState = ProcessingState::Idle;
        pushButton_Start_setEnabled_self(true);
        pushButton_Stop_setEnabled_self(false);
        return;
    }
    TextBrowser_NewMessage(tr("Starting processing for %1 file(s)...").arg(m_TotalNumProc));
    int maxThreads = ui->spinBox_ThreadNum_image->value();
    for (int i = 0; i < maxThreads; ++i) {
        tryStartNextFile();
    }
}

void MainWindow::tryStartNextFile()
{
    if (m_currentState == ProcessingState::Stopping) {
        if (ThreadNumRunning == 0) Waifu2x_Finished_manual();
        return;
    }
    if (m_currentState != ProcessingState::Processing || m_jobQueue.isEmpty()) {
        if (ThreadNumRunning == 0 && m_currentState == ProcessingState::Processing) Waifu2x_Finished();
        return;
    }
    (void)QtConcurrent::run([this]() { this->startNextFileProcessing(); });
}

void MainWindow::startNextFileProcessing()
{
    static QMutex queueMutex;
    QMutexLocker locker(&queueMutex);

    if (m_currentState != ProcessingState::Processing || m_jobQueue.isEmpty()) {
        return;
    }
    ProcessJob job = m_jobQueue.dequeue();
    ThreadNumRunning++;
    current_File_Row_Number = job.rowNum;

    if (job.type == PROCESS_TYPE_IMAGE) {
        Table_image_ChangeStatus_rowNumInt_statusQString(job.rowNum, tr("Processing..."));
        QString engine = ui->comboBox_Engine_Image->currentText();
        // --- THIS IS THE ENGINE DISPATCH LOGIC ---
        if (engine == "RealESRGAN-NCNN-Vulkan") {
            RealESRGAN_NCNN_Vulkan_Image(job.rowNum, false);
        } else if (engine == "RealCUGAN-NCNN-Vulkan") {
            Realcugan_NCNN_Vulkan_Image(job.rowNum, false, false);
        } else if (engine == "Anime4K") {
            Anime4k_Image(job.rowNum, false);
        } else {
            // Add other engine stubs here if needed
            TextBrowser_NewMessage(tr("Error: Unknown or unsupported image engine selected: %1").arg(engine));
            onProcessingFinished(job.rowNum, false, PROCESS_TYPE_IMAGE);
        }
    } else if (job.type == PROCESS_TYPE_GIF) {
        Table_gif_ChangeStatus_rowNumInt_statusQString(job.rowNum, tr("Processing..."));
        QString engine = ui->comboBox_Engine_GIF->currentText();
        // TODO: Implement GIF processing logic similar to image processing
        // For now, simulate error for GIF
        TextBrowser_NewMessage(tr("Error: GIF processing not yet implemented for engine: %1").arg(engine));
        onProcessingFinished(job.rowNum, false, PROCESS_TYPE_GIF);
    } else if (job.type == PROCESS_TYPE_VIDEO) {
        Table_video_ChangeStatus_rowNumInt_statusQString(job.rowNum, tr("Processing..."));
        QString engine = ui->comboBox_Engine_Video->currentText();
        if (engine == "RealESRGAN-NCNN-Vulkan") {
            RealESRGAN_NCNN_Vulkan_Video(job.rowNum);
        } else if (engine == "RealCUGAN-NCNN-Vulkan") {
            Realcugan_NCNN_Vulkan_Video(job.rowNum);
        } else {
            TextBrowser_NewMessage(tr("Error: Unknown or unsupported video engine selected: %1").arg(engine));
            onProcessingFinished(job.rowNum, false, PROCESS_TYPE_VIDEO);
        }
    }
}

void MainWindow::onProcessingFinished(int rowNum, bool success, ProcessJobType jobType)
{
    if (m_currentState == ProcessingState::Idle) return;
    // Update status in the correct table based on jobType
    // This is already handled by the specific processor signals connected to Table_xxx_ChangeStatus_rowNumInt_statusQString
    // However, if a process finishes due to an error *before* the processor starts (e.g. unknown engine),
    // we might need to manually set a final status like "Error" here.
    // For now, the existing status update mechanism via signals should cover most cases.
    // The main purpose here is to decrement ThreadNumRunning and try the next file.
    if (success) { m_FinishedProc++; } else { m_ErrorProc++; }
    ThreadNumRunning--;
    UpdateProgressBar();
    tryStartNextFile();
}

void MainWindow::onFileProgress(int rowNum, int percent)
{
    if (rowNum == current_File_Row_Number) {
        SetCurrentFileProgressBarValue(percent, 100);
    }
}

// --- WRAPPER IMPLEMENTATIONS FOR REFACtORED ENGINES ---
void MainWindow::Anime4k_Image(int rowNum, bool){
    // This is a wrapper. The actual logic is in Anime4KProcessor.
    Anime4kSettings settings;
    settings.programPath = Current_Path + "/anime4k-cli/Anime4KCPP_CLI.exe"; // Example path
    // Populate settings from UI:
    settings.passes = ui->spinBox_Passes_Anime4K->value();
    settings.pushColorCount = ui->spinBox_PushColorCount_Anime4K->value();
    settings.pushColorStrength = ui->doubleSpinBox_PushColorStrength_Anime4K->value();
    settings.pushGradientStrength = ui->doubleSpinBox_PushGradientStrength_Anime4K->value();
    settings.fastMode = ui->checkBox_FastMode_Anime4K->isChecked();
    settings.hdnMode = ui->checkBox_HDNMode_Anime4k->isChecked();
    settings.acNet = ui->checkBox_ACNet_Anime4K->isChecked(); // Ensure this matches UI
    settings.gpuMode = ui->checkBox_GPUMode_Anime4K->isChecked();
    settings.gpgpuModel = ui->comboBox_GPGPUModel_A4k->currentText(); // Ensure this matches UI

    settings.preProcessing = ui->checkBox_EnablePreProcessing_Anime4k->isChecked();
    if(settings.preProcessing) {
        // settings.preFilters = ui->lineEdit_PreFilters_Anime4k->text(); // This LineEdit does not exist
        QStringList preFilterParts;
        if (ui->checkBox_MedianBlur_Pre_Anime4k->isChecked()) preFilterParts << "medianBlur";
        if (ui->checkBox_MeanBlur_Pre_Anime4k->isChecked()) preFilterParts << "meanBlur";
        if (ui->checkBox_CASSharping_Pre_Anime4k->isChecked()) preFilterParts << "CAS";
        if (ui->checkBox_GaussianBlurWeak_Pre_Anime4k->isChecked()) preFilterParts << "gaussianBlurWeak";
        if (ui->checkBox_GaussianBlur_Pre_Anime4k->isChecked()) preFilterParts << "gaussianBlur";
        if (ui->checkBox_BilateralFilter_Pre_Anime4k->isChecked()) preFilterParts << "bilateralFilter";
        if (ui->checkBox_BilateralFilterFaster_Pre_Anime4k->isChecked()) preFilterParts << "bilateralFilterFaster";
        settings.preFilters = preFilterParts.join(':');
    }
    settings.postProcessing = ui->checkBox_EnablePostProcessing_Anime4k->isChecked();
    if(settings.postProcessing) {
        // settings.postFilters = ui->lineEdit_PostFilters_Anime4k->text(); // This LineEdit does not exist
        QStringList postFilterParts;
        if (ui->checkBox_MedianBlur_Post_Anime4k->isChecked()) postFilterParts << "medianBlur";
        if (ui->checkBox_MeanBlur_Post_Anime4k->isChecked()) postFilterParts << "meanBlur";
        if (ui->checkBox_CASSharping_Post_Anime4k->isChecked()) postFilterParts << "CAS";
        if (ui->checkBox_GaussianBlurWeak_Post_Anime4k->isChecked()) postFilterParts << "gaussianBlurWeak";
        if (ui->checkBox_GaussianBlur_Post_Anime4k->isChecked()) postFilterParts << "gaussianBlur";
        if (ui->checkBox_BilateralFilter_Post_Anime4k->isChecked()) postFilterParts << "bilateralFilter";
        if (ui->checkBox_BilateralFilterFaster_Post_Anime4k->isChecked()) preFilterParts << "bilateralFilterFaster";
        settings.postFilters = postFilterParts.join(':');
    }

    settings.specifyGpu = ui->checkBox_SpecifyGPU_Anime4k->isChecked();
    if(settings.specifyGpu) {
        settings.gpuString = ui->lineEdit_GPUs_Anime4k->text(); // Corrected name
    }
    // settings.commandQueues = ui->spinBox_CommandQueues_Anime4k->value(); // If such UI exists
    // settings.parallelIo = ui->checkBox_ParallelIO_Anime4k->isChecked(); // If such UI exists

    QString sourceFile = Table_model_image->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "anime4k");

    disconnect(m_anime4kProcessor, &Anime4KProcessor::processingFinished, this, &MainWindow::onProcessingFinished);
    connect(m_anime4kProcessor, &Anime4KProcessor::processingFinished, this, [this](int rN, bool suc){ this->onProcessingFinished(rN, suc, PROCESS_TYPE_IMAGE); });
    m_anime4kProcessor->processImage(rowNum, sourceFile, destFile, settings);
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int rowNum, bool)
{
    QString sourceFile = Table_model_image->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realesrgan");
    RealEsrganSettings settings;
    settings.programPath = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";
    // Read from the new RealESRGAN tab controls
    settings.modelName = ui->comboBox_Model_RealESRGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_image->value(); // This is a general setting, remains
    settings.tileSize = ui->spinBox_TileSize_RealESRGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealESRGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealESRGAN->currentText();
    // TODO: Add multi-GPU settings if ui->checkBox_MultiGPU_RealESRGANEngine is checked, similar to other engines.
    // For now, assuming single GPU mode based on original structure.
    if (settings.modelName.contains("x4")) settings.modelNativeScale = 4;
    else if (settings.modelName.contains("x2")) settings.modelNativeScale = 2;
    else settings.modelNativeScale = 1;

    disconnect(m_realEsrganProcessor, SIGNAL(processingFinished(int,bool)), this, SLOT(onProcessingFinished(int,bool)));
    connect(m_realEsrganProcessor, &RealEsrganProcessor::processingFinished, this, [this](int rN, bool suc){ this->onProcessingFinished(rN, suc, PROCESS_TYPE_IMAGE); });
    m_realEsrganProcessor->processImage(rowNum, sourceFile, destFile, settings);
}

void MainWindow::Realcugan_NCNN_Vulkan_Image(int rowNum, bool, bool)
{
    QString sourceFile = Table_model_image->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realcugan");
    RealCuganSettings settings;
    settings.programPath = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealCUGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_image->value();
    settings.denoiseLevel = ui->spinBox_DenoiseLevel_image->value();
    settings.tileSize = ui->spinBox_TileSize_RealCUGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealCUGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealCUGAN->currentText();
    realCuganProcessor->processImage(rowNum, sourceFile, destFile, settings);
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Video(int rowNum)
{
    QString sourceFile = Table_model_video->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realesrgan_video");
    RealEsrganSettings settings;
    settings.programPath = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";
    // Read from the new RealESRGAN tab controls
    settings.modelName = ui->comboBox_Model_RealESRGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_video->value(); // General setting
    settings.tileSize = ui->spinBox_TileSize_RealESRGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealESRGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealESRGAN->currentText();
    // TODO: Add multi-GPU settings if ui->checkBox_MultiGPU_RealESRGANEngine is checked.
    if (settings.modelName.contains("x4")) settings.modelNativeScale = 4;
    else if (settings.modelName.contains("x2")) settings.modelNativeScale = 2;
    else settings.modelNativeScale = 1;

    disconnect(m_realEsrganProcessor, SIGNAL(processingFinished(int,bool)), this, SLOT(onProcessingFinished(int,bool)));
    connect(m_realEsrganProcessor, &RealEsrganProcessor::processingFinished, this, [this](int rN, bool suc){ this->onProcessingFinished(rN, suc, PROCESS_TYPE_VIDEO); });
    m_realEsrganProcessor->processVideo(rowNum, sourceFile, destFile, settings);
}

void MainWindow::Realcugan_NCNN_Vulkan_Video(int rowNum)
{
    QString sourceFile = Table_model_video->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realcugan_video");
    RealCuganSettings settings;
    settings.programPath = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealCUGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_video->value();
    settings.denoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    settings.tileSize = ui->spinBox_TileSize_RealCUGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealCUGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealCUGAN->currentText();

    disconnect(realCuganProcessor, &RealCuganProcessor::processingFinished, this, &MainWindow::onProcessingFinished);
    connect(realCuganProcessor, &RealCuganProcessor::processingFinished, this, [this](int rN, bool suc){ this->onProcessingFinished(rN, suc, PROCESS_TYPE_VIDEO); });
    realCuganProcessor->processVideo(rowNum, sourceFile, destFile, settings);
}

// --- Moved from CompatibilityTest.cpp ---
int MainWindow::Simple_Compatibility_Test()
{
    QString realcuganExe = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    QString realesrganExe = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";

    isCompatible_RealCUGAN_NCNN_Vulkan = QFile::exists(realcuganExe);
    emit Send_Add_progressBar_CompatibilityTest();

    isCompatible_RealESRGAN_NCNN_Vulkan = QFile::exists(realesrganExe);
    emit Send_Add_progressBar_CompatibilityTest();

    QMetaObject::invokeMethod(this, "Finish_progressBar_CompatibilityTest", Qt::QueuedConnection);
    emit Send_Waifu2x_Compatibility_Test_finished();
    return 0;
}

void MainWindow::waitForCompatibilityTest()
{
    if (compatibilityTestFuture.isRunning())
    {
        compatibilityTestFuture.waitForFinished();
    }
}

// --- Implementation for Generate_Output_Path ---
QString MainWindow::Generate_Output_Path(const QString& original_filePath, const QString& suffix)
{
    QFileInfo originalFileInfo(original_filePath);
    QString baseName = originalFileInfo.completeBaseName();
    QString originalExt = originalFileInfo.suffix().toLower();
    QString newBaseName = baseName + "_" + suffix;

    QString outputDir;
    if (ui->checkBox_OutPath_isEnabled->isChecked() && !ui->lineEdit_outputPath->text().isEmpty()) {
        outputDir = ui->lineEdit_outputPath->text();
        QDir dir(outputDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
    } else {
        outputDir = originalFileInfo.absolutePath();
    }

    QString newExt = originalExt;

    const QStringList imageExts = {"png", "jpg", "jpeg", "bmp", "webp", "tif", "tiff"};
    const QStringList videoExts = {"mp4", "mkv", "mov", "avi", "webm", "flv", "mpg", "mpeg", "wmv", "3gp", "vob"};
    const QStringList gifExts = {"gif"};

    bool isOriginalImage = imageExts.contains(originalExt);
    bool isOriginalVideo = videoExts.contains(originalExt);
    bool isOriginalGif = gifExts.contains(originalExt);

    if (isOriginalImage && !isOriginalGif) {
        newExt = ui->comboBox_ImageSaveFormat->currentText().toLower();
        if (newExt.isEmpty() || newExt.length() > 5) {
            newExt = originalExt;
        }
    } else if (isOriginalGif) {
        newExt = "gif";
    } else if (isOriginalVideo) {
        // Keep originalExt
    }
    return QDir(outputDir).filePath(newBaseName + "." + newExt);
}

// --- START OF STUB IMPLEMENTATION SECTION ---
void MainWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); /* STUB */ }
void MainWindow::toggleLiquidGlass(bool enabled) { if (glassWidget) glassWidget->setVisible(enabled); /* STUB */ }
int MainWindow::Waifu2x_Caffe_Image(int r, bool) { qDebug() << "STUB: Waifu2x_Caffe_Image"; onProcessingFinished(r, false); return 0; }
int MainWindow::Waifu2x_Converter_Image(int r, bool) { qDebug() << "STUB: Waifu2x_Converter_Image"; onProcessingFinished(r, false); return 0; }
void MainWindow::changeEvent(QEvent *e) { QMainWindow::changeEvent(e); /* STUB */ }
void MainWindow::Set_Font_fixed() { /* STUB */ }
bool MainWindow::SystemPrefersDark() const { return false; /* STUB */ }
void MainWindow::ApplyDarkStyle() { /* STUB */ }
bool MainWindow::runProcess(QProcess*, const QString&, QByteArray*, QByteArray*) { return false; /* STUB */ }
void MainWindow::ShellMessageBox(const QString&, const QString&, QMessageBox::Icon) { /* STUB */ }
void MainWindow::setImageEngineIndex(int) { /* STUB */ }
void MainWindow::setGifEngineIndex(int) { /* STUB */ }
void MainWindow::setVideoEngineIndex(int) { /* STUB */ }
void MainWindow::pushButton_Start_setEnabled_self(bool e) { if (ui) ui->pushButton_Start->setEnabled(e); /* STUB */ }
void MainWindow::pushButton_Stop_setEnabled_self(bool e) { if (ui) ui->pushButton_Stop->setEnabled(e); /* STUB */ }
void MainWindow::closeEvent(QCloseEvent *event) { QMainWindow::closeEvent(event); /* STUB */ }
bool MainWindow::ReplaceOriginalFile(QString, QString) { return false; /* STUB */ }
void MainWindow::MoveFileToOutputPath(QString, QString) { /* STUB */ }
QMap<QString, int> MainWindow::Image_Gif_Read_Resolution(QString) { return QMap<QString, int>(); /* STUB */ }
void MainWindow::Delay_sec_sleep(int) { /* STUB */ }
void MainWindow::Delay_msec_sleep(int) { /* STUB */ }
void MainWindow::ExecuteCMD_batFile(QString, bool) { /* STUB */ }
void MainWindow::on_groupBox_FrameInterpolation_toggled(bool) { /* STUB */ }
int MainWindow::Waifu2x_DetectGPU_finished() { return 0; /* STUB */ }
int MainWindow::Realsr_ncnn_vulkan_DetectGPU_finished() { return 0; /* STUB */ }
int MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_finished() { return 0; /* STUB */ }
int MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU_finished() { return 0; /* STUB */ }
void MainWindow::SRMD_DetectGPU_finished() { /* STUB */ }
int MainWindow::Waifu2x_DumpProcessorList_converter_finished() { return 0; /* STUB */ }
void MainWindow::Set_checkBox_DisableResize_gif_Checked() { /* STUB */ }
void MainWindow::Table_image_insert_fileName_fullPath(const FileLoadInfo& fileInfo) {
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(fileInfo.fileName); // Col 0: File Name
    rowItems << new QStandardItem(fileInfo.status.isEmpty() ? tr("Waiting") : fileInfo.status); // Col 1: Status
    rowItems << new QStandardItem(fileInfo.fullPath); // Col 2: Full Path
    QString resolution = (!fileInfo.customResolutionWidth.isEmpty() || !fileInfo.customResolutionHeight.isEmpty()) ?
                         fileInfo.customResolutionWidth + "x" + fileInfo.customResolutionHeight : "";
    rowItems << new QStandardItem(resolution); // Col 3: Resolution
    rowItems << new QStandardItem(""); // Col 4: Output Path (empty for now)
    rowItems << new QStandardItem(""); // Col 5: Engine Settings (empty for now)
    Table_model_image->appendRow(rowItems);
    qDebug() << "[Debug] Table_image_insert_fileName_fullPath: Appended. New rowCount:" << Table_model_image->rowCount() << "columnCount:" << Table_model_image->columnCount();
}

void MainWindow::Table_gif_insert_fileName_fullPath(const FileLoadInfo& fileInfo) {
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(fileInfo.fileName); // Col 0: File Name
    rowItems << new QStandardItem(fileInfo.status.isEmpty() ? tr("Waiting") : fileInfo.status); // Col 1: Status
    rowItems << new QStandardItem(fileInfo.fullPath); // Col 2: Full Path
    QString resolution = (!fileInfo.customResolutionWidth.isEmpty() || !fileInfo.customResolutionHeight.isEmpty()) ?
                         fileInfo.customResolutionWidth + "x" + fileInfo.customResolutionHeight : "";
    rowItems << new QStandardItem(resolution); // Col 3: Resolution
    rowItems << new QStandardItem(""); // Col 4: Output Path (empty for now)
    rowItems << new QStandardItem(""); // Col 5: Engine Settings (empty for now)
    Table_model_gif->appendRow(rowItems);
    qDebug() << "[Debug] Table_gif_insert_fileName_fullPath: Appended. New rowCount:" << Table_model_gif->rowCount() << "columnCount:" << Table_model_gif->columnCount();
}

void MainWindow::Table_video_insert_fileName_fullPath(const FileLoadInfo& fileInfo) {
    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem(fileInfo.fileName); // Col 0: File Name
    rowItems << new QStandardItem(fileInfo.status.isEmpty() ? tr("Waiting") : fileInfo.status); // Col 1: Status
    rowItems << new QStandardItem(fileInfo.fullPath); // Col 2: Full Path
    QString resolution = (!fileInfo.customResolutionWidth.isEmpty() || !fileInfo.customResolutionHeight.isEmpty()) ?
                         fileInfo.customResolutionWidth + "x" + fileInfo.customResolutionHeight : "";
    rowItems << new QStandardItem(resolution); // Col 3: Resolution
    rowItems << new QStandardItem(""); // Col 4: FPS (empty for now)
    rowItems << new QStandardItem(""); // Col 5: Duration (empty for now)
    rowItems << new QStandardItem(""); // Col 6: Output Path (empty for now)
    rowItems << new QStandardItem(""); // Col 7: Engine Settings (empty for now)
    Table_model_video->appendRow(rowItems);
    qDebug() << "[Debug] Table_video_insert_fileName_fullPath: Appended. New rowCount:" << Table_model_video->rowCount() << "columnCount:" << Table_model_video->columnCount();
}

QStringList MainWindow::getImageFullPaths() const {
    QStringList paths;
    for (int i = 0; i < Table_model_image->rowCount(); ++i) {
        paths.append(Table_model_image->item(i, 2)->text()); // Assuming column 2 is full path
    }
    return paths;
}

void MainWindow::Batch_Table_Update_slots(const QList<FileLoadInfo>& imagesToAdd, const QList<FileLoadInfo>& gifsToAdd, const QList<FileLoadInfo>& videosToAdd, bool doAddNewImage, bool doAddNewGif, bool doAddNewVideo)
{
    qDebug() << "[Debug] Batch_Table_Update_slots: Called with";
    qDebug() << "[Debug]   doAddNewImage:" << doAddNewImage << "imagesToAdd count:" << imagesToAdd.count();
    qDebug() << "[Debug]   doAddNewGif:" << doAddNewGif << "gifsToAdd count:" << gifsToAdd.count();
    qDebug() << "[Debug]   doAddNewVideo:" << doAddNewVideo << "videosToAdd count:" << videosToAdd.count();

    // ui_tableViews_setUpdatesEnabled(false); // This was a STUB, removing call

    if (doAddNewImage && Table_model_image) {
        qDebug() << "[Debug] Batch_Table_Update_slots: Processing" << imagesToAdd.count() << "images.";
        for (const auto& fileInfo : imagesToAdd) {
            qDebug() << "[Debug] Batch_Table_Update_slots: Adding image - Name:" << fileInfo.fileName << "Path:" << fileInfo.fullPath;
            Table_image_insert_fileName_fullPath(fileInfo);
        }
    }
    if (doAddNewGif && Table_model_gif) {
        qDebug() << "[Debug] Batch_Table_Update_slots: Processing" << gifsToAdd.count() << "GIFs.";
        for (const auto& fileInfo : gifsToAdd) {
            qDebug() << "[Debug] Batch_Table_Update_slots: Adding GIF - Name:" << fileInfo.fileName << "Path:" << fileInfo.fullPath;
            Table_gif_insert_fileName_fullPath(fileInfo);
        }
    }
    if (doAddNewVideo && Table_model_video) {
        qDebug() << "[Debug] Batch_Table_Update_slots: Processing" << videosToAdd.count() << "videos.";
        for (const auto& fileInfo : videosToAdd) {
            qDebug() << "[Debug] Batch_Table_Update_slots: Adding video - Name:" << fileInfo.fileName << "Path:" << fileInfo.fullPath;
            Table_video_insert_fileName_fullPath(fileInfo);
        }
    }

    // ui_tableViews_setUpdatesEnabled(true); // This was a STUB, removing call

    // Explicitly tell views that layout might have changed, though rowsInserted should cover it.
    // This is more of a "just in case" or if updates were somehow suppressed.
    // However, QStandardItemModel::appendRow should correctly notify the view.
    // Forcing viewport update is usually not necessary if model/view is set up correctly.
    // If issues persist, this might indicate a deeper problem with view updates.
    // if (doAddNewImage && ui && ui->tableView_image) ui->tableView_image->viewport()->update();
    // if (doAddNewGif && ui && ui->tableView_gif) ui->tableView_gif->viewport()->update();
    // if (doAddNewVideo && ui && ui->tableView_video) ui->tableView_video->viewport()->update();

    Table_FileCount_reload(); // This is a STUB but called as per original logic flow
}

void MainWindow::on_pushButton_CustRes_apply_clicked() { /* STUB */ }
void MainWindow::on_pushButton_CustRes_cancel_clicked() { /* STUB */ }
void MainWindow::on_pushButton_about_clicked() { /* STUB */ }
void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int) { /* STUB */ }
void MainWindow::on_pushButton_Save_GlobalFontSize_clicked() { /* STUB */ }
void MainWindow::on_pushButton_HideTextBro_clicked() { /* STUB */ }
void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int) { /* STUB */ }
void MainWindow::on_Ext_image_textChanged(const QString &) { /* STUB */ }
void MainWindow::on_Ext_video_textChanged(const QString &) { /* STUB */ }
void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_pushButton_ResetVideoSettings_clicked() { /* STUB */ }
void MainWindow::on_pushButton_Stop_clicked() { /* STUB */ }
void MainWindow::Waifu2x_Finished() { /* STUB */ }
void MainWindow::Waifu2x_Finished_manual() { /* STUB */ }
void MainWindow::on_pushButton_ReadMe_clicked() { /* STUB */ }
void MainWindow::on_pushButton_wiki_clicked() { /* STUB */ }
void MainWindow::SetCurrentFileProgressBarValue(int val, int max_val) { if(ui && ui->progressBar_CurrentFile) { ui->progressBar_CurrentFile->setMaximum(max_val); ui->progressBar_CurrentFile->setValue(val); } }
void MainWindow::UpdateProgressBar() { /* STUB */ }
void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_groupBox_video_settings_clicked() { /* STUB */ }
void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_DelOriginal_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_AudioDenoise_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_ShowInterPro_stateChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int) { /* STUB */ }
void MainWindow::Add_progressBar_CompatibilityTest() { /* STUB */ }
void MainWindow::TimeSlot() { /* STUB */ }
int MainWindow::Waifu2x_Compatibility_Test_finished() { return 0; /* STUB */ }
void MainWindow::Read_Input_paths_BrowserFile(QStringList Input_path_List)
{
    // Clear AddNew flags before processing
    AddNew_image = false;
    AddNew_video = false;
    AddNew_gif = false;

    // Get current items in tables to avoid duplicates efficiently
    QSet<QString> existingImagePaths_set;
    for (int i = 0; i < Table_model_image->rowCount(); ++i) {
        existingImagePaths_set.insert(Table_model_image->item(i, 2)->text());
    }
    QSet<QString> existingGifPaths_set;
    for (int i = 0; i < Table_model_gif->rowCount(); ++i) {
        existingGifPaths_set.insert(Table_model_gif->item(i, 2)->text());
    }
    QSet<QString> existingVideoPaths_set;
    for (int i = 0; i < Table_model_video->rowCount(); ++i) {
        existingVideoPaths_set.insert(Table_model_video->item(i, 2)->text());
    }

    QList<FileLoadInfo> imagesToAdd_info;
    QList<FileLoadInfo> gifsToAdd_info;
    QList<FileLoadInfo> videosToAdd_info;

    bool localAddNewImage = false;
    bool localAddNewGif = false;
    bool localAddNewVideo = false;

    qDebug() << "[Debug] Read_Input_paths_BrowserFile: Processing" << Input_path_List.count() << "input paths.";

    for (const QString &path : Input_path_List)
    {
        qDebug() << "[Debug] Read_Input_paths_BrowserFile: Current path:" << path;
        QFileInfo fileInfo(path);
        if (fileInfo.isDir())
        {
            qDebug() << "[Debug] Read_Input_paths_BrowserFile: Path is a directory:" << path;
            QDir dir(path);
            QStringList fileEntries = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
            qDebug() << "[Debug] Read_Input_paths_BrowserFile: Found" << fileEntries.count() << "files in directory.";
            for (const QString &fileName : fileEntries) {
                QString fullPath = dir.filePath(fileName);
                qDebug() << "[Debug] Read_Input_paths_BrowserFile: Processing file from dir:" << fullPath;
                if (!Deduplicate_filelist_worker(fullPath, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set)) {
                     QList<QPair<QString, QString>> img_tmp, gif_tmp, vid_tmp; // These are unused by FileList_Add's current signature
                     bool laImg = false, laGif = false, laVid = false; // Initialize local flags for FileList_Add
                     FileList_Add(fileName, fullPath, img_tmp, gif_tmp, vid_tmp, laImg, laGif, laVid, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
                     qDebug() << "[Debug] Read_Input_paths_BrowserFile: FileList_Add returned: laImg=" << laImg << "laGif=" << laGif << "laVid=" << laVid << "for" << fullPath;
                     if (laImg) {
                         imagesToAdd_info.append({fileName, fullPath, tr("Waiting"), "", ""});
                         localAddNewImage = true;
                         qDebug() << "[Debug] Read_Input_paths_BrowserFile: Added to imagesToAdd_info:" << fullPath;
                     }
                     if (laGif) {
                         gifsToAdd_info.append({fileName, fullPath, tr("Waiting"), "", ""});
                         localAddNewGif = true;
                         qDebug() << "[Debug] Read_Input_paths_BrowserFile: Added to gifsToAdd_info:" << fullPath;
                     }
                     if (laVid) {
                         videosToAdd_info.append({fileName, fullPath, tr("Waiting"), "", ""});
                         localAddNewVideo = true;
                         qDebug() << "[Debug] Read_Input_paths_BrowserFile: Added to videosToAdd_info:" << fullPath;
                     }
                } else {
                    qDebug() << "[Debug] Read_Input_paths_BrowserFile: Skipped (duplicate):" << fullPath;
                }
            }
        }
        else if (fileInfo.isFile())
        {
            qDebug() << "[Debug] Read_Input_paths_BrowserFile: Path is a file:" << path;
            if (!Deduplicate_filelist_worker(path, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set)) {
                QList<QPair<QString, QString>> img_tmp, gif_tmp, vid_tmp; // Unused
                bool laImg = false, laGif = false, laVid = false; // Initialize
                FileList_Add(fileInfo.fileName(), path, img_tmp, gif_tmp, vid_tmp, laImg, laGif, laVid, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
                qDebug() << "[Debug] Read_Input_paths_BrowserFile: FileList_Add returned: laImg=" << laImg << "laGif=" << laGif << "laVid=" << laVid << "for" << path;
                if (laImg) {
                    imagesToAdd_info.append({fileInfo.fileName(), path, tr("Waiting"), "", ""});
                    localAddNewImage = true;
                    qDebug() << "[Debug] Read_Input_paths_BrowserFile: Added to imagesToAdd_info:" << path;
                }
                if (laGif) {
                    gifsToAdd_info.append({fileInfo.fileName(), path, tr("Waiting"), "", ""});
                    localAddNewGif = true;
                    qDebug() << "[Debug] Read_Input_paths_BrowserFile: Added to gifsToAdd_info:" << path;
                }
                if (laVid) {
                    videosToAdd_info.append({fileInfo.fileName(), path, tr("Waiting"), "", ""});
                    localAddNewVideo = true;
                    qDebug() << "[Debug] Read_Input_paths_BrowserFile: Added to videosToAdd_info:" << path;
                }
            } else {
                qDebug() << "[Debug] Read_Input_paths_BrowserFile: Skipped (duplicate):" << path;
            }
        }
    }

    qDebug() << "[Debug] Read_Input_paths_BrowserFile: Before calling Batch_Table_Update_slots:";
    qDebug() << "[Debug]   imagesToAdd_info count:" << imagesToAdd_info.count() << "localAddNewImage:" << localAddNewImage;
    qDebug() << "[Debug]   gifsToAdd_info count:" << gifsToAdd_info.count() << "localAddNewGif:" << localAddNewGif;
    qDebug() << "[Debug]   videosToAdd_info count:" << videosToAdd_info.count() << "localAddNewVideo:" << localAddNewVideo;
    Batch_Table_Update_slots(imagesToAdd_info, gifsToAdd_info, videosToAdd_info, localAddNewImage, localAddNewGif, localAddNewVideo);

    if (localAddNewImage) AddNew_image = true;
    if (localAddNewGif) AddNew_gif = true;
    if (localAddNewVideo) AddNew_video = true;
}

int MainWindow::on_pushButton_RemoveItem_clicked()
{
    QTableView *currentTableView = nullptr;
    QStandardItemModel *currentModel = nullptr;

    int currentTabIndex = ui->fileListTabWidget->currentIndex();

    if (currentTabIndex == 0) { // Images tab
        currentTableView = ui->tableView_image;
        currentModel = Table_model_image;
    } else if (currentTabIndex == 1) { // GIF/APNG tab
        currentTableView = ui->tableView_gif;
        currentModel = Table_model_gif;
    } else if (currentTabIndex == 2) { // Videos tab
        currentTableView = ui->tableView_video;
        currentModel = Table_model_video;
    }

    if (currentTableView && currentModel) {
        if (currentModel->rowCount() == 0) {
            TextBrowser_NewMessage(tr("File list is empty. Nothing to remove."));
            return 0; // Indicate no action
        }
        QModelIndexList selectedRows = currentTableView->selectionModel()->selectedRows();
        if (!selectedRows.isEmpty()) {
            // Remove rows in reverse order to avoid issues with changing row indices
            for (int i = selectedRows.count() - 1; i >= 0; --i) {
                currentModel->removeRow(selectedRows.at(i).row());
            }
            Table_FileCount_reload(); // Update file count
            return 1; // Indicate success
        } else {
            TextBrowser_NewMessage(tr("No item selected to remove."));
        }
    }
    return 0; // Indicate failure or no action
}
void MainWindow::on_pushButton_Report_clicked() { /* STUB */ }
void MainWindow::on_pushButton_clear_textbrowser_clicked() { /* STUB */ }
void MainWindow::on_pushButton_HideSettings_clicked() { /* STUB */ }
void MainWindow::on_pushButton_ReadFileList_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Read File List"),
                                                    QDir::homePath(),
                                                    tr("Text Files (*.txt);;All Files (*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        TextBrowser_NewMessage(tr("Error: Could not open file for reading: %1").arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QStringList imagePaths, gifPaths, videoPaths;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("IMAGE:")) {
            imagePaths.append(line.mid(6));
        } else if (line.startsWith("GIF:")) {
            gifPaths.append(line.mid(4));
        } else if (line.startsWith("VIDEO:")) {
            videoPaths.append(line.mid(6));
        } else if (!line.trimmed().isEmpty()){
            // Try to auto-detect based on common extensions if no prefix
            QFileInfo fileInfo(line.trimmed());
            QString suffix = fileInfo.suffix().toLower();
            QStringList imgExt = ui->Ext_image->text().split(':', Qt::SkipEmptyParts);
            QStringList vidExt = ui->Ext_video->text().split(':', Qt::SkipEmptyParts);

            if (imgExt.contains(suffix)) {
                imagePaths.append(line.trimmed());
            } else if (vidExt.contains(suffix)) {
                videoPaths.append(line.trimmed());
            } else if (suffix == "gif" || suffix == "apng") {
                gifPaths.append(line.trimmed());
            } else {
                TextBrowser_NewMessage(tr("Warning: Unknown file type or invalid line in list: %1").arg(line));
            }
        }
    }
    file.close();

    bool filesAdded = false;
    if (!imagePaths.isEmpty()) {
        Read_Input_paths_BrowserFile(imagePaths);
        filesAdded = true;
    }
    if (!gifPaths.isEmpty()) {
        Read_Input_paths_BrowserFile(gifPaths);
        filesAdded = true;
    }
    if (!videoPaths.isEmpty()) {
        Read_Input_paths_BrowserFile(videoPaths);
        filesAdded = true;
    }

    if (filesAdded) {
        Table_FileCount_reload();
        TextBrowser_NewMessage(tr("File list loaded from: %1").arg(fileName));
    } else {
        TextBrowser_NewMessage(tr("No valid files found in: %1").arg(fileName));
    }
}
void MainWindow::on_Ext_image_editingFinished() { /* STUB */ }
void MainWindow::on_Ext_video_editingFinished() { /* STUB */ }
void MainWindow::on_checkBox_AutoSaveSettings_clicked() { /* STUB */ }
void MainWindow::on_pushButton_BrowserFile_clicked()
{
    QString imageExtensions = ui->Ext_image->text().replace(":", " *.");
    QString videoExtensions = ui->Ext_video->text().replace(":", " *.");
    QString gifExtensions = "gif apng"; // QFileDialog uses space separation for individual wildcards

    QStringList filters;
    filters << tr("Supported Files (*.%1 *.%2 *.%3)").arg(imageExtensions).arg(videoExtensions).arg(gifExtensions.replace(" ", " *."));
    filters << tr("Image Files (*.%1)").arg(imageExtensions);
    filters << tr("Video Files (*.%2)").arg(videoExtensions);
    filters << tr("Animated Files (*.%1)").arg(gifExtensions.replace(" ", " *."));
    filters << tr("All Files (*)");

    QStringList selectedFiles = QFileDialog::getOpenFileNames(
        this,
        tr("Browse and Add Files"),
        QDir::homePath(), // Default to home path or last used path
        filters.join(";;"));

    if (!selectedFiles.isEmpty())
    {
        // The Read_Input_paths_BrowserFile function seems designed for this.
        // It internally calls FileList_Add which then calls Batch_Table_Update_slots.
        Read_Input_paths_BrowserFile(selectedFiles);
        Table_FileCount_reload(); // Update the file count display
    }
}
void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &) { /* STUB */ }
void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &) { /* STUB */ }
void MainWindow::on_lineEdit_pixformat_textChanged(const QString &) { /* STUB */ }
void MainWindow::on_pushButton_encodersList_clicked() { /* STUB */ }
void MainWindow::on_pushButton_ForceRetry_clicked() { /* STUB */ }
void MainWindow::on_pushButton_PayPal_clicked() { /* STUB */ }
void MainWindow::on_tabWidget_currentChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_FFprobe_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_SoX_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked() { /* STUB */ }
void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked() { /* STUB */ }
void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked() { /* STUB */ }
void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int) { /* STUB */ }
void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int) { /* STUB */ }
void MainWindow::on_pushButton_TurnOffScreen_clicked() { /* STUB */ }
void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked() { /* STUB */ }
void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked() { /* STUB */ }
void MainWindow::on_pushButton_DetectGPU_VFI_clicked() { /* STUB */ }
void MainWindow::on_lineEdit_MultiGPU_IDs_VFI_editingFinished() { /* STUB */ }
void MainWindow::on_checkBox_MultiGPU_VFI_stateChanged(int) { /* STUB */ }
void MainWindow::on_groupBox_FrameInterpolation_clicked() { /* STUB */ }
void MainWindow::on_checkBox_EnableVFI_Home_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_RifeNcnnVulkan_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_CainNcnnVulkan_clicked() { /* STUB */ }
void MainWindow::on_checkBox_isCompatible_DainNcnnVulkan_clicked() { /* STUB */ }
void MainWindow::on_comboBox_Engine_VFI_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_pushButton_Verify_MultiGPU_VFI_clicked() { /* STUB */ }
void MainWindow::on_checkBox_MultiThread_VFI_stateChanged(int) { /* STUB */ }
void MainWindow::on_checkBox_MultiThread_VFI_clicked() { /* STUB */ }
void MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked() { /* STUB */ }
void MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged(int) { /* STUB */ }
void MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked() { /* STUB */ }
void MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked() { /* STUB */ }
void MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked() { /* STUB */ }
void MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked() { /* STUB */ }
void MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked() { /* STUB */ }
void MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged(int) { /* STUB */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_finished(int, QProcess::ExitStatus) { /* STUB */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput() { /* STUB */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError() { /* STUB */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError) { /* STUB */ }
void MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError) { /* STUB */ }
void MainWindow::on_pushButton_SaveFileList_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File List"),
                                                    QDir::homePath(),
                                                    tr("Text Files (*.txt);;All Files (*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        TextBrowser_NewMessage(tr("Error: Could not open file for writing: %1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);

    // Save Images
    for (int i = 0; i < Table_model_image->rowCount(); ++i) {
        QString path = Table_model_image->item(i, 2)->text(); // Column 2 is FullPath
        out << "IMAGE:" << path << "\n";
    }
    // Save GIFs
    for (int i = 0; i < Table_model_gif->rowCount(); ++i) {
        QString path = Table_model_gif->item(i, 2)->text(); // Column 2 is FullPath
        out << "GIF:" << path << "\n";
    }
    // Save Videos
    for (int i = 0; i < Table_model_video->rowCount(); ++i) {
        QString path = Table_model_video->item(i, 2)->text(); // Column 2 is FullPath
        out << "VIDEO:" << path << "\n";
    }

    file.close();
    TextBrowser_NewMessage(tr("File list saved to: %1").arg(fileName));
}
void MainWindow::on_tableView_image_doubleClicked(const QModelIndex &) { /* STUB */ }
void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex &) { /* STUB */ }
void MainWindow::on_tableView_video_doubleClicked(const QModelIndex &) { /* STUB */ }
void MainWindow::on_tableView_image_pressed(const QModelIndex &) { /* STUB */ }
void MainWindow::on_tableView_gif_pressed(const QModelIndex &) { /* STUB */ }
void MainWindow::on_tableView_video_pressed(const QModelIndex &) { /* STUB */ }
void MainWindow::TextBrowser_StartMes() { /* STUB */ }
void MainWindow::on_pushButton_Patreon_clicked() { /* STUB */ }
void MainWindow::on_pushButton_SupportersList_clicked() { /* STUB */ }
void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int) { /* STUB */ }
void MainWindow::on_pushButton_CheckUpdate_clicked() { /* STUB */ }
void MainWindow::StartFullCompatibilityTest()
{
    ui->textBrowser->append(tr("Starting compatibility test..."));

    // Disable all checkboxes first and reset progress
    QList<QCheckBox*> checkboxes = ui->groupBox_CompatibilityTestRes->findChildren<QCheckBox*>();
    for (QCheckBox* cb : checkboxes) {
        cb->setChecked(false);
        cb->setEnabled(false); // Keep them disabled until test result is known
    }
    ui->progressBar_CompatibilityTest->setValue(0);
    // Define the total number of tests
    // This should match the number of calls to TestEngineCommand + other simple checks
    // Initial estimate, will adjust as tests are added
    int totalTests = 19; // From UI, let's aim for this.
    ui->progressBar_CompatibilityTest->setMaximum(totalTests);

    // Run the tests in a separate thread to avoid freezing the UI
    if (compatibilityTestFuture.isRunning()) {
        TextBrowser_NewMessage(tr("Compatibility test is already running."));
        return;
    }
    ui->pushButton_compatibilityTest->setEnabled(false);
    compatibilityTestFuture = QtConcurrent::run([this] { this->ExecuteCompatibilityTests(); });
}

void MainWindow::ExecuteCompatibilityTests()
{
    // Paths to executables - these should be verified or made configurable if possible
    // For now, assume they are in subdirectories of Current_Path

    // Test Waifu2x-ncnn-vulkan (Latest)
    TestEngineCommand("Waifu2x-ncnn-vulkan (Latest)",
                      Current_Path + "/waifu2x-ncnn-vulkan/waifu2x-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW);

    // Test Waifu2x-ncnn-vulkan (FP16P) - Assuming same executable, different model perhaps, or different build.
    // For a simple command test, -h should suffice. If different exe, path needs change.
    TestEngineCommand("Waifu2x-ncnn-vulkan (20200414(fp16p))",
                      Current_Path + "/waifu2x-ncnn-vulkan-20200414-fp16p/waifu2x-ncnn-vulkan.exe", // Example path
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P);

    // Test Waifu2x-ncnn-vulkan (OLD) - Assuming another path or build
    TestEngineCommand("Waifu2x-ncnn-vulkan (OLD)",
                      Current_Path + "/waifu2x-ncnn-vulkan-old/waifu2x-ncnn-vulkan.exe", // Example path
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD);

    // Test SRMD-ncnn-vulkan
    TestEngineCommand("SRMD-ncnn-vulkan",
                      Current_Path + "/srmd-ncnn-vulkan/srmd-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_SRMD_NCNN_Vulkan);

    // Test RealESRGAN-NCNN-Vulkan
    TestEngineCommand("RealESRGAN-NCNN-Vulkan",
                      Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_Realsr_NCNN_Vulkan); // Note: UI checkbox is named Realsr, might need to create a new one or rename for RealESRGAN

    // Test RealCUGAN-NCNN-Vulkan
     TestEngineCommand("RealCUGAN-NCNN-Vulkan",
                      Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      nullptr); // No specific checkbox in UI for RealCUGAN compatibility test result, will add to log.
                                // For now, will use a temporary placeholder or just log.
                                // This highlights a mismatch between test plan and UI. The UI has isCompatible_Realsr_NCNN_Vulkan
                                // and isCompatible_RealCUGAN_NCNN_Vulkan in mainwindow.h but not in UI for test results.
                                // Let's assume we need to add these checkboxes to the UI later or map them correctly.
                                // For now, I will map RealESRGAN to isCompatible_Realsr_NCNN_Vulkan as it was before.
                                // And for RealCUGAN, I'll need a new checkbox or log only.
                                // The plan mentions isCompatible_RealESRGAN_NCNN_Vulkan and isCompatible_RealCUGAN_NCNN_Vulkan bools.
                                // The UI has checkBox_isCompatible_Realsr_NCNN_Vulkan.
                                // Let's assume checkBox_isCompatible_Realsr_NCNN_Vulkan is for RealESRGAN for now.
                                // And will need to add one for RealCUGAN.
                                // For now, I will test RealESRGAN against checkBox_isCompatible_Realsr_NCNN_Vulkan and manually create bool for RealCUGAN.
    isCompatible_RealESRGAN_NCNN_Vulkan = TestEngineCommand("RealESRGAN-NCNN-Vulkan", Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe", QStringList() << "-h", ui->checkBox_isCompatible_Realsr_NCNN_Vulkan);
    isCompatible_RealCUGAN_NCNN_Vulkan = TestEngineCommand("RealCUGAN-NCNN-Vulkan", Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe", QStringList() << "-h", nullptr); // No checkbox yet


    // Test FFmpeg
    TestEngineCommand("FFmpeg",
                      FFMPEG_EXE_PATH_Waifu2xEX, // Using the defined path
                      QStringList() << "-version",
                      ui->checkBox_isCompatible_FFmpeg);

    // Test FFprobe
    TestEngineCommand("FFprobe",
                      Current_Path + "/ffmpeg/ffprobe.exe", // Assuming it's alongside ffmpeg
                      QStringList() << "-version",
                      ui->checkBox_isCompatible_FFprobe);

    // ImageMagick (convert) - typically just 'convert' if in PATH, or specify full path
    // For Windows, it's often 'magick convert' or just 'magick'
    // This test is more platform-dependent if relying on PATH.
    // A bundled ImageMagick would be Current_Path + "/ImageMagick/convert.exe" or similar
    TestEngineCommand("ImageMagick",
                      "magick", // Or Current_Path + "/ImageMagick/magick.exe"
                      QStringList() << "-version",
                      ui->checkBox_isCompatible_ImageMagick);

    // Gifsicle
    TestEngineCommand("Gifsicle",
                      Current_Path + "/gifsicle/gifsicle.exe",
                      QStringList() << "--version",
                      ui->checkBox_isCompatible_Gifsicle);

    // SoX
    TestEngineCommand("SoX",
                      Current_Path + "/sox/sox.exe",
                      QStringList() << "--version",
                      ui->checkBox_isCompatible_SoX);

    // Anime4KCPP_CLI (CPU is default, GPU needs specific args)
    TestEngineCommand("Anime4K (CPU)",
                      Current_Path + "/anime4k-cli/Anime4KCPP_CLI.exe",
                      QStringList() << "-h", // Help command implies CPU mode is testable
                      ui->checkBox_isCompatible_Anime4k_CPU);

    TestEngineCommand("Anime4K (GPU)",
                      Current_Path + "/anime4k-cli/Anime4KCPP_CLI.exe",
                      QStringList() << "-h" << "-q", // -q might list platforms/devices if GPU is supported
                      ui->checkBox_isCompatible_Anime4k_GPU);


    // Waifu2x-Converter (waifu2x-converter-cpp)
    TestEngineCommand("Waifu2x-converter",
                      Current_Path + "/waifu2x-converter-cpp/waifu2x-converter-cpp.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_Waifu2x_Converter);

    // Waifu2x-Caffe - this one is more complex as it might need model files etc.
    // For a simple test, check executable existence and perhaps -h.
    // CPU
    TestEngineCommand("Waifu2x-caffe (CPU)",
                      Current_Path + "/waifu2x-caffe/waifu2x-caffe-cui.exe",
                      QStringList() << "-h", // Assuming -h exists
                      ui->checkBox_isCompatible_Waifu2x_Caffe_CPU);
    // GPU
    TestEngineCommand("Waifu2x-caffe (GPU)",
                      Current_Path + "/waifu2x-caffe/waifu2x-caffe-cui.exe",
                      QStringList() << "-h" << "-p" << "gpu", // Example args
                      ui->checkBox_isCompatible_Waifu2x_Caffe_GPU);
    // cuDNN
    TestEngineCommand("Waifu2x-caffe (cuDNN)",
                      Current_Path + "/waifu2x-caffe/waifu2x-caffe-cui.exe",
                      QStringList() << "-h" << "-p" << "cudnn", // Example args
                      ui->checkBox_isCompatible_Waifu2x_Caffe_cuDNN);


    // RIFE-NCNN-Vulkan
    TestEngineCommand("RIFE-NCNN-Vulkan",
                      Current_Path + "/rife-ncnn-vulkan/rife-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_RifeNcnnVulkan);

    // CAIN-NCNN-Vulkan
    TestEngineCommand("CAIN-NCNN-Vulkan",
                      Current_Path + "/cain-ncnn-vulkan/cain-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_CainNcnnVulkan);

    // DAIN-NCNN-Vulkan
    TestEngineCommand("DAIN-NCNN-Vulkan",
                      Current_Path + "/dain-ncnn-vulkan/dain-ncnn-vulkan.exe",
                      QStringList() << "-h",
                      ui->checkBox_isCompatible_DainNcnnVulkan);


    // Ensure all tests contribute to the progress bar even if they are simple file checks
    // For any remaining tests that were part of the original 19 but not covered by command tests:
    // Example: SRMD CUDA (if it's just a file check for now)
    // bool srmdCudaExists = QFile::exists(Current_Path + "/srmd-cuda/srmd-cuda.exe");
    // UpdateCompatibilityCheckbox(ui->checkBox_isCompatible_SRMD_CUDA, srmdCudaExists, "SRMD-CUDA");


    // Fill remaining progress bar slots if totalTests was an estimate
    int currentProgress = ui->progressBar_CompatibilityTest->value();
    int maxProgress = ui->progressBar_CompatibilityTest->maximum();
    for (int i = currentProgress; i < maxProgress; ++i) {
        QMetaObject::invokeMethod(this, "Add_progressBar_CompatibilityTest", Qt::QueuedConnection);
    }


    QMetaObject::invokeMethod(this, "Finish_progressBar_CompatibilityTest", Qt::QueuedConnection);
    QMetaObject::invokeMethod(ui->pushButton_compatibilityTest, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
    TextBrowser_NewMessage(tr("Compatibility test finished."));
    emit Send_Waifu2x_Compatibility_Test_finished();
}

bool MainWindow::TestEngineCommand(const QString& engineName, const QString& executablePath, const QStringList& arguments, QCheckBox* checkBox)
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels); // Combine stdout and stderr

    if (!QFile::exists(executablePath)) {
        QString logMsg = tr("%1: Executable not found at %2").arg(engineName, executablePath);
        QMetaObject::invokeMethod(this, "TextBrowser_NewMessage", Qt::QueuedConnection, Q_ARG(QString, logMsg));
        if (checkBox) {
            // UpdateCompatibilityCheckbox(checkBox, false, engineName); // This function is not defined yet
             QMetaObject::invokeMethod(checkBox, "setChecked", Qt::QueuedConnection, Q_ARG(bool, false));
             QMetaObject::invokeMethod(checkBox, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
        }
        QMetaObject::invokeMethod(this, "Add_progressBar_CompatibilityTest", Qt::QueuedConnection);
        return false;
    }

    process.start(executablePath, arguments);
    bool success = process.waitForFinished(15000); // Wait 15 seconds

    QString statusMsg;
    bool isCompatible = false;

    if (!success) {
        process.kill();
        process.waitForFinished(1000); // Ensure it's killed
        statusMsg = tr("%1: Test timed out or crashed.").arg(engineName);
    } else if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        statusMsg = tr("%1: Compatible (Exit code 0).").arg(engineName);
        isCompatible = true;
    } else {
        statusMsg = tr("%1: Not compatible or error (Exit code %2, Status %3). Output: %4")
                        .arg(engineName)
                        .arg(process.exitCode())
                        .arg(process.exitStatus())
                        .arg(QString::fromLocal8Bit(process.readAll()).trimmed());
    }

    QMetaObject::invokeMethod(this, "TextBrowser_NewMessage", Qt::QueuedConnection, Q_ARG(QString, statusMsg));
    if (checkBox) {
        // UpdateCompatibilityCheckbox(checkBox, isCompatible, engineName); // This function is not defined yet
        QMetaObject::invokeMethod(checkBox, "setChecked", Qt::QueuedConnection, Q_ARG(bool, isCompatible));
        QMetaObject::invokeMethod(checkBox, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
    }
    QMetaObject::invokeMethod(this, "Add_progressBar_CompatibilityTest", Qt::QueuedConnection);
    return isCompatible;
}


void MainWindow::on_pushButton_compatibilityTest_clicked()
{
    StartFullCompatibilityTest();
}

void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason) { /* STUB */ }
void MainWindow::progressbar_setRange_min_max_slots(int, int) { /* STUB */ }
void MainWindow::progressbar_Add_slots() { /* STUB */ }
void MainWindow::Unable2Connect_RawGithubusercontentCom() { /* STUB */ }
void MainWindow::SetEnable_pushButton_ForceRetry_self() { /* STUB */ }
void MainWindow::SystemTray_hide_self() { /* STUB */ }
void MainWindow::SystemTray_showNormal_self() { /* STUB */ }
void MainWindow::SystemTray_showDonate() { /* STUB */ }
void MainWindow::SystemTray_NewMessage(QString) { /* STUB */ }
void MainWindow::EnableBackgroundMode_SystemTray() { /* STUB */ }
void MainWindow::OpenSelectedFilesFolder_FilesList() { /* STUB */ }
void MainWindow::OpenSelectedFile_FilesList() { /* STUB */ }
void MainWindow::OpenOutputFolder() { /* STUB */ }
void MainWindow::RemoveALL_image_slot() { /* STUB */ }
void MainWindow::RemoveALL_gif_slot() { /* STUB */ }
void MainWindow::RemoveALL_video_slot() { /* STUB */ }
void MainWindow::Apply_CustRes_QAction_FileList_slot() { /* STUB */ }
void MainWindow::Cancel_CustRes_QAction_FileList_slot() { /* STUB */ }
void MainWindow::FinishedProcessing_DN() { /* STUB */ }
int MainWindow::Table_FileCount_reload() { return 0; }
void MainWindow::Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int, QString, QString) { /* STUB */ }
void MainWindow::Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int, QString, QString) { /* STUB */ }
void MainWindow::Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int, QString, QString) { /* STUB */ }
int MainWindow::Table_Read_Saved_Table_Filelist_Finished(QString) { return 0; }
int MainWindow::Table_Save_Current_Table_Filelist_Finished() { return 0; }
void MainWindow::on_pushButton_ClearList_clicked()
{
    // The existing Table_Clear() method handles clearing models,
    // re-initializing headers, and reloading file counts.
    Table_Clear();
}
bool MainWindow::SystemShutDown() { return false; }
void MainWindow::Read_urls_finfished() { /* STUB */ }
void MainWindow::video_write_VideoConfiguration(QString, int, int, bool, int, int, QString, bool, QString, QString, bool, int) { /* STUB */ }
int MainWindow::Settings_Save() { return 0; }
void MainWindow::video_write_Progress_ProcessBySegment(QString, int, bool, bool, int, int) { /* STUB */ }
void MainWindow::CurrentFileProgress_Start(QString, int) { /* STUB */ }
void MainWindow::CurrentFileProgress_Stop() { /* STUB */ }
void MainWindow::CurrentFileProgress_progressbar_Add() { /* STUB */ }
void MainWindow::CurrentFileProgress_progressbar_Add_SegmentDuration(int) { /* STUB */ }
void MainWindow::CurrentFileProgress_progressbar_SetFinishedValue(int) { /* STUB */ }
void MainWindow::CurrentFileProgress_WatchFolderFileNum(QString) { /* STUB */ }
void MainWindow::CurrentFileProgress_WatchFolderFileNum_Textbrower(QString, QString, int) { /* STUB */ }
void MainWindow::Donate_ReplaceQRCode(QString) { /* STUB */ }
void MainWindow::on_checkBox_BanGitee_clicked() { /* STUB */ }
int MainWindow::CheckUpdate_NewUpdate(QString, QString) { return 0; }
FileMetadata MainWindow::getOrFetchMetadata(QString const&) { return FileMetadata(); /* STUB */ }
void MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString(int, QString) { /* STUB */ }
void MainWindow::Table_video_ChangeStatus_rowNumInt_statusQString(int, QString) { /* STUB */ }
void MainWindow::Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString) { /* STUB */ }
void MainWindow::TextBrowser_NewMessage(QString msg) { if (ui) ui->textBrowser->append(msg); }

int MainWindow::FrameInterpolation_DetectGPU_finished() { /* STUB */ return 0; }
void MainWindow::Finish_progressBar_CompatibilityTest() { /* STUB */ }

// --- END OF STUB IMPLEMENTATION SECTION ---
