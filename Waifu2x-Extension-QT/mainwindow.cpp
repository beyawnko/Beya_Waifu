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
    // TODO: Add loops for GIF and Video tables
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
    Table_image_ChangeStatus_rowNumInt_statusQString(job.rowNum, tr("Processing..."));

    if (job.type == PROCESS_TYPE_IMAGE) {
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
            onProcessingFinished(job.rowNum, false);
        }
    }
    // TODO: else if for GIF and Video jobs
}

void MainWindow::onProcessingFinished(int /*rowNum*/, bool success)
{
    if (m_currentState == ProcessingState::Idle) return;
    if (success) { m_FinishedProc++; } else { m_ErrorProc++; }
    ThreadNumRunning--;
    UpdateProgressBar();
    tryStartNextFile();
}

void MainWindow::onFileProgress(int rowNum, int percent)
{
    if (rowNum == current_File_Row_Number) {
        Set_Current_File_Progress_Bar_Value(percent, 100);
    }
}

// --- WRAPPER IMPLEMENTATIONS FOR REFACtORED ENGINES ---
void MainWindow::Anime4k_Image(int rowNum, bool){
    // This is a wrapper. The actual logic is in Anime4KProcessor.
    // Assuming m_anime4kProcessor is initialized in constructor.
    // Settings would be gathered from UI and passed to the processor.
    // m_anime4kProcessor->processImage(...);
    // For now, just a stub to allow compilation.
    qDebug() << "STUB: Anime4k_Image called for row" << rowNum;
    onProcessingFinished(rowNum, false); // Fail the job to prevent hanging
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int rowNum, bool)
{
    QString sourceFile = Table_model_image->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realesrgan");
    RealEsrganSettings settings;
    settings.programPath = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealsrNCNNVulkan->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_image->value();
    settings.tileSize = ui->spinBox_TileSize_RealsrNCNNVulkan->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealsrNCNNVulkan->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealsrNCNNVulkan->currentText();
    if (settings.modelName.contains("x4")) settings.modelNativeScale = 4;
    else if (settings.modelName.contains("x2")) settings.modelNativeScale = 2;
    else settings.modelNativeScale = 1;
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
    QString destFile = Generate_Output_Path(sourceFile, "realesrgan-video.mp4");
    RealEsrganSettings settings;
    settings.programPath = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealsrNCNNVulkan->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_video->value();
    settings.tileSize = ui->spinBox_TileSize_RealsrNCNNVulkan->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealsrNCNNVulkan->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealsrNCNNVulkan->currentText();
    if (settings.modelName.contains("x4")) settings.modelNativeScale = 4;
    else if (settings.modelName.contains("x2")) settings.modelNativeScale = 2;
    else settings.modelNativeScale = 1;
    m_realEsrganProcessor->processVideo(rowNum, sourceFile, destFile, settings);
}

void MainWindow::Realcugan_NCNN_Vulkan_Video(int rowNum)
{
    QString sourceFile = Table_model_video->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realcugan-video.mp4");
    RealCuganSettings settings;
    settings.programPath = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealCUGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_video->value();
    settings.denoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    settings.tileSize = ui->spinBox_TileSize_RealCUGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealCUGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealCUGAN->currentText();
    realCuganProcessor->processVideo(rowNum, sourceFile, destFile, settings);
}

// --- ALL OTHER FUNCTION IMPLEMENTATIONS ARE STUBBED BELOW ---
// This is to prevent "undefined reference" and "multiple definition" errors.
// Their actual logic is either obsolete or will be added back in future refactors.

// --- STUBBED / EMPTY FUNCTIONS ---
void MainWindow::resizeEvent(QResizeEvent *event) { QMainWindow::resizeEvent(event); }
void MainWindow::toggleLiquidGlass(bool enabled) { if (glassWidget) glassWidget->setVisible(enabled); }
int MainWindow::Waifu2x_Caffe_Image(int r, bool) { qDebug() << "STUB: Waifu2x_Caffe_Image"; onProcessingFinished(r, false); return 0; }
int MainWindow::Waifu2x_Converter_Image(int r, bool) { qDebug() << "STUB: Waifu2x_Converter_Image"; onProcessingFinished(r, false); return 0; }
void MainWindow::changeEvent(QEvent *e) { QMainWindow::changeEvent(e); }
void MainWindow::Set_Font_fixed() { /* STUB */ }
bool MainWindow::SystemPrefersDark() const { return false; }
void MainWindow::ApplyDarkStyle() { /* STUB */ }
bool MainWindow::runProcess(QProcess*, const QString&, QByteArray*, QByteArray*) { return false; }
void MainWindow::ShellMessageBox(const QString&, const QString&, QMessageBox::Icon) { /* STUB */ }
void MainWindow::setImageEngineIndex(int) { /* STUB */ }
void MainWindow::setGifEngineIndex(int) { /* STUB */ }
void MainWindow::setVideoEngineIndex(int) { /* STUB */ }
void MainWindow::pushButton_Start_setEnabled_self(bool e) { if (ui) ui->pushButton_Start->setEnabled(e); }
void MainWindow::pushButton_Stop_setEnabled_self(bool e) { if (ui) ui->pushButton_Stop->setEnabled(e); }
void MainWindow::closeEvent(QCloseEvent *event) { QMainWindow::closeEvent(event); }
bool MainWindow::ReplaceOriginalFile(QString, QString) { return false; }
void MainWindow::MoveFileToOutputPath(QString, QString) { /* STUB */ }
QMap<QString, int> MainWindow::Image_Gif_Read_Resolution(QString) { return QMap<QString, int>(); }
void MainWindow::Delay_sec_sleep(int) { /* STUB */ }
void MainWindow::Delay_msec_sleep(int) { /* STUB */ }
void MainWindow::ExecuteCMD_batFile(QString, bool) { /* STUB */ }
void MainWindow::on_groupBox_FrameInterpolation_toggled(bool) { /* STUB */ }
int MainWindow::Waifu2x_DetectGPU_finished() { return 0; }
int MainWindow::Realsr_ncnn_vulkan_DetectGPU_finished() { return 0; }
int MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_finished() { return 0; }
int MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU_finished() { return 0; }
void MainWindow::SRMD_DetectGPU_finished() { /* STUB */ }
int MainWindow::Waifu2x_DumpProcessorList_converter_finished() { return 0; }
void MainWindow::Set_checkBox_DisableResize_gif_Checked() { /* STUB */ }
void MainWindow::Table_image_insert_fileName_fullPath(const FileLoadInfo&) { /* STUB */ }
void MainWindow::Table_gif_insert_fileName_fullPath(const FileLoadInfo&) { /* STUB */ }
void MainWindow::Table_video_insert_fileName_fullPath(const FileLoadInfo&) { /* STUB */ }
QStringList MainWindow::getImageFullPaths() const { return QStringList(); }
void MainWindow::Batch_Table_Update_slots(const QList<FileLoadInfo>&, const QList<FileLoadInfo>&, const QList<FileLoadInfo>&, bool, bool, bool) { /* STUB */ }
// ... Add empty stubs for ALL other missing functions from your log ...
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
// etc... for every single on_... clicked/changed slot.
void MainWindow::on_pushButton_Stop_clicked() { /* STUB */ }
// Make sure to add stubs for ALL functions that the build log complains about.
// This is tedious but necessary.
void MainWindow::Waifu2x_Finished() { /* STUB */ }
void MainWindow::Waifu2x_Finished_manual() { /* STUB */ }
void MainWindow::on_pushButton_ReadMe_clicked() { /* STUB */ }
void MainWindow::on_pushButton_wiki_clicked() { /* STUB */ }
void MainWindow::TextBrowser_NewMessage(QString msg) { if (ui) ui->textBrowser->append(msg); } // Matched to header QString by value
void MainWindow::on_pushButton_SaveSettings_clicked() { /* STUB */ }
void MainWindow::on_pushButton_ResetSettings_clicked() { /* STUB */ }
