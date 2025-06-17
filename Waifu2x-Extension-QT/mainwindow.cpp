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

MainWindow::MainWindow(int maxThreadsOverride, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FFMPEG_EXE_PATH_Waifu2xEX = Current_Path + "/ffmpeg/ffmpeg.exe";
    realCuganProcessor = new RealCuganProcessor(this);
    videoProcessor = new VideoProcessor(this);
    qRegisterMetaType<FileMetadataCache>("FileMetadataCache");

    QVariant maxThreadCountSetting = Settings_Read_value("/settings/MaxThreadCount");
    globalMaxThreadCount = maxThreadCountSetting.isValid() ? maxThreadCountSetting.toInt() : 0;
    if (maxThreadsOverride > 0) {
        globalMaxThreadCount = maxThreadsOverride;
    }
    if (globalMaxThreadCount <= 0) {
        int cores = QThread::idealThreadCount();
        if (cores < 1) {
            cores = 1;
        }
        globalMaxThreadCount = cores * 2;
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

    glassWidget = new LiquidGlassWidget(ui->centralwidget);
    glassWidget->setBackground(QImage(":/new/prefix1/icon/BackgroudMode.png"));
    glassWidget->hide();
    glassWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    glassWidget->setGeometry(ui->centralwidget->rect());

    if (auto grid = ui->groupBox_Setting->findChild<QGridLayout*>("gridLayout_4")) {
        QCheckBox *check = new QCheckBox(tr("Enable Liquid Glass"), this);
        grid->addWidget(check, grid->rowCount(), 0, 1, grid->columnCount());
        connect(check, &QCheckBox::toggled, this, &MainWindow::toggleLiquidGlass);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (glassWidget) {
        glassWidget->setGeometry(ui->centralwidget->rect());
    }
    QMainWindow::resizeEvent(event);
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange && isMinimized()) {
        if (ui->checkBox_MinimizeToTaskbar->isChecked()) {
            hide();
        }
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::Set_Font_fixed()
{
    uiController.setFontFixed(ui->checkBox_isCustFontEnable,
                              ui->fontComboBox_CustFont,
                              ui->spinBox_GlobalFontSize);
}

bool MainWindow::SystemPrefersDark() const
{
    return uiController.systemPrefersDark();
}

void MainWindow::ApplyDarkStyle()
{
    QVariant darkModeSetting = Settings_Read_value("/settings/DarkMode");
    uiController.applyDarkStyle(
        darkModeSetting.isValid() && !darkModeSetting.isNull() ? darkModeSetting.toInt() : 1);
}

bool MainWindow::runProcess(QProcess *process, const QString &cmd,
                            QByteArray *stdOut, QByteArray *stdErr)
{
    QEventLoop loop;
    if (stdOut) {
        stdOut->clear();
    }
    if (stdErr) {
        stdErr->clear();
    }
    if (stdOut) {
        QObject::connect(process, &QProcess::readyReadStandardOutput,
                         [&]() { stdOut->append(process->readAllStandardOutput()); });
    }
    if (stdErr) {
        QObject::connect(process, &QProcess::readyReadStandardError,
                         [&]() { stdErr->append(process->readAllStandardError()); });
    }
    QObject::connect(process,
                     QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     &loop,
                     &QEventLoop::quit);
    QObject::connect(process, &QProcess::errorOccurred, &loop, &QEventLoop::quit);
    process->start(cmd);
    loop.exec();
    return process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0;
}

void MainWindow::toggleLiquidGlass(bool enabled)
{
    glassEnabled = enabled;
    if (glassWidget) {
        glassWidget->setVisible(enabled);
    }
}

void MainWindow::ShellMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon)
{
    QMessageBox msg(icon, title, text, QMessageBox::Ok, this);
    msg.exec();
}

void MainWindow::setImageEngineIndex(int index)
{
    ui->comboBox_Engine_Image->setCurrentIndex(index);
}

void MainWindow::setGifEngineIndex(int index)
{
    ui->comboBox_Engine_GIF->setCurrentIndex(index);
}

void MainWindow::setVideoEngineIndex(int index)
{
    ui->comboBox_Engine_Video->setCurrentIndex(index);
}

void MainWindow::pushButton_Start_setEnabled_self(bool isEnabled)
{
    ui->pushButton_Start->setEnabled(isEnabled);
    Start_SystemTrayIcon->setEnabled(isEnabled);
    ui->pushButton_Start->setVisible(isEnabled);
}

void MainWindow::pushButton_Stop_setEnabled_self(bool isEnabled)
{
    ui->pushButton_Stop->setEnabled(isEnabled);
    Pause_SystemTrayIcon->setEnabled(isEnabled);
    ui->pushButton_Stop->setVisible(isEnabled);
}

// Stubs from previous successful steps (eventFilter, closeEvent)
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // TODO: Add custom event filtering logic if needed
    qDebug() << "STUB: MainWindow::eventFilter called for object" << obj << "event type" << event->type();
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow closing";
    QMainWindow::closeEvent(event);
    qApp->quit();
}

// Stubs for general utility functions that should be in mainwindow.cpp
FileMetadataCache MainWindow::getOrFetchMetadata(const QString &filePath) { qDebug() << "STUB: MainWindow::getOrFetchMetadata called for" << filePath; return FileMetadataCache(); }
bool MainWindow::ReplaceOriginalFile(QString original_fullpath, QString output_fullpath) { qDebug() << "STUB: MainWindow::ReplaceOriginalFile called for" << original_fullpath << "and" << output_fullpath; return false; }
void MainWindow::MoveFileToOutputPath(QString Orginal, QString SourceFilePath) { qDebug() << "STUB: MainWindow::MoveFileToOutputPath called for" << Orginal << "and" << SourceFilePath; }
QMap<QString,int> MainWindow::Image_Gif_Read_Resolution(QString SourceFileFullPath) { qDebug() << "STUB: MainWindow::Image_Gif_Read_Resolution called for" << SourceFileFullPath; return QMap<QString,int>(); }
void MainWindow::Delay_sec_sleep(int time) { qDebug() << "STUB: MainWindow::Delay_sec_sleep called with" << time; QThread::sleep(time); }
void MainWindow::Delay_msec_sleep(int time) { qDebug() << "STUB: Delay_msec_sleep called with" << time; QThread::msleep(time); }
void MainWindow::ExecuteCMD_batFile(QString cmd_str, bool requestAdmin) { Q_UNUSED(requestAdmin); qDebug() << "STUB: ExecuteCMD_batFile called for" << cmd_str; }

// Stubs for _finished signals/slots and other functions expected in mainwindow.cpp (not causing multiple defs)
int MainWindow::Waifu2x_DetectGPU_finished() { qDebug() << "STUB: Waifu2x_DetectGPU_finished called"; return 0; }
int MainWindow::Realsr_ncnn_vulkan_DetectGPU_finished() { qDebug() << "STUB: Realsr_ncnn_vulkan_DetectGPU_finished called"; return 0; }
int MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_finished()
{
    ui->pushButton_DetectGPU_RealCUGAN->setEnabled(true);
    ui->pushButton_DetectGPU_RealCUGAN->setText(tr("Detect GPU"));
    ui->comboBox_GPUID_RealCUGAN->clear();
    for (const QString &gpu : Available_GPUID_RealCUGAN)
    {
        ui->comboBox_GPUID_RealCUGAN->addItem(gpu);
    }
    return 0;
}

int MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU_finished()
{
    ui->pushButton_DetectGPU_RealsrNCNNVulkan->setEnabled(true);
    ui->pushButton_DetectGPU_RealsrNCNNVulkan->setText(tr("Detect GPU"));
    ui->comboBox_GPUID_RealsrNCNNVulkan->clear();
    for (const QString &gpu : Available_GPUID_RealESRGAN_ncnn_vulkan)
    {
        ui->comboBox_GPUID_RealsrNCNNVulkan->addItem(gpu);
    }
    return 0;
}
void MainWindow::SRMD_DetectGPU_finished() { qDebug() << "STUB: SRMD_DetectGPU_finished called"; }
int MainWindow::Waifu2x_DumpProcessorList_converter_finished() { qDebug() << "STUB: Waifu2x_DumpProcessorList_converter_finished called"; return 0; }
void MainWindow::Set_checkBox_DisableResize_gif_Checked() { qDebug() << "STUB: Set_checkBox_DisableResize_gif_Checked called"; }
void MainWindow::Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath) { Q_UNUSED(SourceFile_fullPath); qDebug() << "STUB: Table_image_insert_fileName_fullPath called for" << fileName; }
void MainWindow::Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath) { Q_UNUSED(SourceFile_fullPath); qDebug() << "STUB: Table_gif_insert_fileName_fullPath called for" << fileName; }
void MainWindow::Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath) { Q_UNUSED(SourceFile_fullPath); qDebug() << "STUB: Table_video_insert_fileName_fullPath called for" << fileName; }

// Stubs for remaining undefined references from linker errors
void MainWindow::on_pushButton_CustRes_apply_clicked() { qDebug() << "STUB: on_pushButton_CustRes_apply_clicked called"; }
void MainWindow::on_pushButton_CustRes_cancel_clicked() { qDebug() << "STUB: on_pushButton_CustRes_cancel_clicked called"; }
void MainWindow::comboBox_UpdateChannel_setCurrentIndex_self(int index) { qDebug() << "STUB: comboBox_UpdateChannel_setCurrentIndex_self called with index" << index; }
void MainWindow::on_comboBox_language_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_language_currentIndexChanged called with index" << index; }
void MainWindow::on_pushButton_about_clicked() { qDebug() << "STUB: on_pushButton_about_clicked called"; }
void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int arg1) { qDebug() << "STUB: on_spinBox_textbrowser_fontsize_valueChanged called with" << arg1; }
void MainWindow::on_pushButton_Save_GlobalFontSize_clicked() { qDebug() << "STUB: on_pushButton_Save_GlobalFontSize_clicked called"; }
void MainWindow::on_pushButton_HideTextBro_clicked() { qDebug() << "STUB: on_pushButton_HideTextBro_clicked called"; }
void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_AlwaysHideTextBrowser_stateChanged called with" << arg1; }
void MainWindow::on_Ext_image_textChanged(const QString &arg1) { qDebug() << "STUB: on_Ext_image_textChanged called with" << arg1; }
void MainWindow::on_Ext_video_textChanged(const QString &arg1) { qDebug() << "STUB: on_Ext_video_textChanged called with" << arg1; }
void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_model_vulkan_currentIndexChanged called with" << index; }
void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_ImageStyle_currentIndexChanged called with" << index; }
void MainWindow::on_pushButton_ResetVideoSettings_clicked() { qDebug() << "STUB: on_pushButton_ResetVideoSettings_clicked called"; }
void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &arg1) { qDebug() << "STUB: on_lineEdit_encoder_vid_textChanged called with" << arg1; }
void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &arg1) { qDebug() << "STUB: on_lineEdit_encoder_audio_textChanged called with" << arg1; }
void MainWindow::on_lineEdit_pixformat_textChanged(const QString &arg1) { qDebug() << "STUB: on_lineEdit_pixformat_textChanged called with" << arg1; }
void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_vcodec_copy_2mp4_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_acodec_copy_2mp4_stateChanged called with" << arg1; }
void MainWindow::on_pushButton_encodersList_clicked() { qDebug() << "STUB: on_pushButton_encodersList_clicked called"; }
void MainWindow::on_checkBox_DelOriginal_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_DelOriginal_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_FileList_Interactive_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_OutPath_isEnabled_stateChanged called with" << arg1; }
void MainWindow::on_pushButton_ForceRetry_clicked() { qDebug() << "STUB: on_pushButton_ForceRetry_clicked called"; }
void MainWindow::on_pushButton_PayPal_clicked() { qDebug() << "STUB: on_pushButton_PayPal_clicked called"; }
void MainWindow::on_checkBox_AudioDenoise_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_AudioDenoise_stateChanged called with" << arg1; }
void MainWindow::on_tabWidget_currentChanged(int index) { qDebug() << "STUB: on_tabWidget_currentChanged called with" << index; }
void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_ProcessVideoBySegment_stateChanged called with" << arg1; }
void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged called with" << index; }
void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_EnablePreProcessing_Anime4k_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_EnablePostProcessing_Anime4k_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_SpecifyGPU_Anime4k_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_GPUMode_Anime4K_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_ShowInterPro_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_ShowInterPro_stateChanged called with" << arg1; }
void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked() { qDebug() << "STUB: on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked called"; }
void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked() { qDebug() << "STUB: on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked called"; }
void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_ACNet_Anime4K_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_HDNMode_Anime4k_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_ReplaceOriginalFile_stateChanged called with" << arg1; }
void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_isCustFontEnable_stateChanged called with" << arg1; }
void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked() { qDebug() << "STUB: on_pushButton_ResizeFilesListSplitter_clicked called"; }
void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_GPGPUModel_A4k_currentIndexChanged called with" << index; }
void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_DisableGPU_converter_stateChanged called with" << arg1; }
void MainWindow::on_groupBox_video_settings_clicked() { qDebug() << "STUB: on_groupBox_video_settings_clicked called"; }
void MainWindow::on_pushButton_TurnOffScreen_clicked() { qDebug() << "STUB: on_pushButton_TurnOffScreen_clicked called"; }
void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked() { qDebug() << "STUB: on_pushButton_MultipleOfFPS_VFI_MIN_clicked called"; }
void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked() { qDebug() << "STUB: on_pushButton_MultipleOfFPS_VFI_ADD_clicked called"; }
void MainWindow::Add_progressBar_CompatibilityTest() { qDebug() << "STUB: Add_progressBar_CompatibilityTest called"; }
void MainWindow::SetEnable_pushButton_ForceRetry_self() { qDebug() << "STUB: SetEnable_pushButton_ForceRetry_self called"; }
void MainWindow::Waifu2x_Finished() { qDebug() << "STUB: Waifu2x_Finished called"; }
void MainWindow::Waifu2x_Finished_manual() { qDebug() << "STUB: Waifu2x_Finished_manual called"; }
void MainWindow::TimeSlot() { qDebug() << "STUB: TimeSlot called"; }
int MainWindow::Waifu2x_Compatibility_Test_finished() { qDebug() << "STUB: Waifu2x_Compatibility_Test_finished called"; return 0; }

// Stubs for functions causing undefined reference errors in linker stage

// From settings.cpp calls
void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_TargetProcessor_converter_currentIndexChanged called with index" << index; }
void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_AlwaysHideSettings_stateChanged called with" << arg1; }
void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_Engine_GIF_currentIndexChanged called with index" << index; }
void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_Engine_Image_currentIndexChanged called with index" << index; }
void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_Engine_Video_currentIndexChanged called with index" << index; }
void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_AspectRatio_custRes_currentIndexChanged called with index" << index; }

// From moc_mainwindow.cpp calls
void MainWindow::on_pushButton_Stop_clicked() { qDebug() << "STUB: on_pushButton_Stop_clicked called"; }
int MainWindow::on_pushButton_RemoveItem_clicked() { qDebug() << "STUB: on_pushButton_RemoveItem_clicked called"; return 0; }
void MainWindow::on_pushButton_Report_clicked() { qDebug() << "STUB: on_pushButton_Report_clicked called"; }
void MainWindow::on_pushButton_ReadMe_clicked() { qDebug() << "STUB: on_pushButton_ReadMe_clicked called"; }
void MainWindow::on_pushButton_clear_textbrowser_clicked() { qDebug() << "STUB: on_pushButton_clear_textbrowser_clicked called"; }
void MainWindow::on_pushButton_HideSettings_clicked() { qDebug() << "STUB: on_pushButton_HideSettings_clicked called"; }
void MainWindow::on_pushButton_ReadFileList_clicked() { qDebug() << "STUB: on_pushButton_ReadFileList_clicked called"; }
void MainWindow::on_Ext_image_editingFinished() { qDebug() << "STUB: on_Ext_image_editingFinished called"; }
void MainWindow::on_Ext_video_editingFinished() { qDebug() << "STUB: on_Ext_video_editingFinished called"; }
void MainWindow::on_checkBox_AutoSaveSettings_clicked() { qDebug() << "STUB: on_checkBox_AutoSaveSettings_clicked called"; }
/*! Browse for files and add them to the processing queue. */
void MainWindow::on_pushButton_BrowserFile_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (!dialog.exec()) {
        qWarning() << "File dialog failed or was cancelled";
        return;
    }

    const QStringList files = dialog.selectedFiles();
    if (files.isEmpty()) {
        qWarning() << "No files selected";
        return;
    }

    for (const QString &path : files) {
        if (ui->checkBox_ScanSubFolders->isChecked()) {
            Add_File_Folder_IncludeSubFolder(path);
        } else {
            Add_File_Folder(path);
        }
    }
}
void MainWindow::on_pushButton_wiki_clicked() { qDebug() << "STUB: on_pushButton_wiki_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked called"; }
void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked called"; }
void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_SRMD_CUDA_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_Converter_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Anime4k_CPU_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Anime4k_GPU_clicked called"; }
void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_FFmpeg_clicked called"; }
void MainWindow::on_checkBox_isCompatible_FFprobe_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_FFprobe_clicked called"; }
void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_ImageMagick_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Gifsicle_clicked called"; }
void MainWindow::on_checkBox_isCompatible_SoX_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_SoX_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked called"; }
void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked() { qDebug() << "STUB: on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked called"; }
void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_UpdateChannel_currentIndexChanged called with index" << index; }

void MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked()
{
    Realcugan_ncnn_vulkan_DetectGPU();
}
void MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_MultiGPU_RealCUGAN_stateChanged called with" << arg1; }
void MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked() { qDebug() << "STUB: on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked called"; }
void MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked() { qDebug() << "STUB: on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked called"; }
void MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked() { qDebug() << "STUB: on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked called"; }
void MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked() { qDebug() << "STUB: on_pushButton_TileSize_Add_RealCUGAN_clicked called"; }
void MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked() { qDebug() << "STUB: on_pushButton_TileSize_Minus_RealCUGAN_clicked called"; }
void MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_Model_RealCUGAN_currentIndexChanged called with index" << index; }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_finished(int exitCode, QProcess::ExitStatus exitStatus) { qDebug() << "STUB: Realcugan_NCNN_Vulkan_Iterative_finished called with exitCode" << exitCode << "exitStatus" << exitStatus; }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput() { qDebug() << "STUB: Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput called"; }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError() { qDebug() << "STUB: Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError called"; }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error) { qDebug() << "STUB: Realcugan_NCNN_Vulkan_Iterative_errorOccurred called with error" << error; }
void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()
{
    RealESRGAN_ncnn_vulkan_DetectGPU();
}
void MainWindow::on_comboBox_Model_RealsrNCNNVulkan_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_Model_RealsrNCNNVulkan_currentIndexChanged called with index" << index; }
void MainWindow::on_pushButton_Add_TileSize_RealsrNCNNVulkan_clicked() { qDebug() << "STUB: on_pushButton_Add_TileSize_RealsrNCNNVulkan_clicked called"; }
void MainWindow::on_pushButton_Minus_TileSize_RealsrNCNNVulkan_clicked() { qDebug() << "STUB: on_pushButton_Minus_TileSize_RealsrNCNNVulkan_clicked called"; }
void MainWindow::on_checkBox_MultiGPU_RealesrganNcnnVulkan_stateChanged(int arg1) { qDebug() << "STUB: on_checkBox_MultiGPU_RealesrganNcnnVulkan_stateChanged called with" << arg1; }
void MainWindow::on_comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan_currentIndexChanged(int index) { qDebug() << "STUB: on_comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan_currentIndexChanged called with index" << index; }
void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan_clicked() { qDebug() << "STUB: on_checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan_clicked called"; }
void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan_valueChanged(int arg1) { qDebug() << "STUB: on_spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan_valueChanged called with" << arg1; }
void MainWindow::on_pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan_clicked() { qDebug() << "STUB: on_pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan_clicked called"; }
void MainWindow::RealESRGAN_NCNN_Vulkan_finished(int exitCode, QProcess::ExitStatus exitStatus) { qDebug() << "STUB: RealESRGAN_NCNN_Vulkan_finished called with exitCode" << exitCode << "exitStatus" << exitStatus; }
void MainWindow::RealESRGAN_NCNN_Vulkan_errorOccurred(QProcess::ProcessError error) { qDebug() << "STUB: RealESRGAN_NCNN_Vulkan_errorOccurred called with error" << error; }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_finished(int exitCode, QProcess::ExitStatus exitStatus) { qDebug() << "STUB: RealESRGAN_NCNN_Vulkan_Iterative_finished called with exitCode" << exitCode << "exitStatus" << exitStatus; }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardOutput() { qDebug() << "STUB: RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardOutput called"; }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardError() { qDebug() << "STUB: RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardError called"; }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error) { qDebug() << "STUB: RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred called with error" << error; }
void MainWindow::onRealESRGANProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) { qDebug() << "STUB: onRealESRGANProcessFinished called with exitCode" << exitCode << "exitStatus" << exitStatus; }
void MainWindow::onRealESRGANProcessError(QProcess::ProcessError error) { qDebug() << "STUB: onRealESRGANProcessError called with error" << error; }
void MainWindow::onRealESRGANProcessStdOut() { qDebug() << "STUB: onRealESRGANProcessStdOut called"; }
void MainWindow::onRealESRGANProcessStdErr() { qDebug() << "STUB: onRealESRGANProcessStdErr called"; }

void MainWindow::ProcessDroppedFilesAsync(QList<QUrl> urls) { qDebug() << "STUB: ProcessDroppedFilesAsync called with" << urls.count() << "urls"; }
void MainWindow::ProcessDroppedFilesFinished() { qDebug() << "STUB: ProcessDroppedFilesFinished called"; }
void MainWindow::Add_File_Folder_MainThread(QString Full_Path) { qDebug() << "STUB: Add_File_Folder_MainThread called with path" << Full_Path; }
void MainWindow::Add_File_Folder_IncludeSubFolder_MainThread(QString Full_Path) { qDebug() << "STUB: Add_File_Folder_IncludeSubFolder_MainThread called with path" << Full_Path; }

// Stubs for slots listed in mainwindow.h but not found/causing errors previously
// These were in the error report section of mainwindow.h, so they are expected by MOC

// Private slots

// Helper function stubs (if any were missed and are private)
void MainWindow::LoadScaledImageToLabel(const QString &imagePath, QLabel *label) { Q_UNUSED(label); qDebug() << "STUB: LoadScaledImageToLabel called for" << imagePath; }
void MainWindow::UpdateTotalProcessedFilesCount() { qDebug() << "STUB: UpdateTotalProcessedFilesCount called"; }
void MainWindow::ProcessNextFile() { qDebug() << "STUB: ProcessNextFile called"; }
void MainWindow::CheckIfAllFinished() { qDebug() << "STUB: CheckIfAllFinished called"; }
void MainWindow::UpdateNumberOfActiveThreads() { qDebug() << "STUB: UpdateNumberOfActiveThreads called"; }
void MainWindow::UpdateProgressBar() { qDebug() << "STUB: UpdateProgressBar called"; }
