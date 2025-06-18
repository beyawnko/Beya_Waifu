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
#include <QImageReader>
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QEventLoop> // Required for waiting on QMediaPlayer signals
#include <QDesktopServices> // For opening URLs
#include <QStandardItem> // For table view item manipulation
#include <algorithm>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
// QWidget and QTableView are pulled in by mainwindow.h or ui_mainwindow.h

MainWindow::MainWindow(int maxThreadsOverride, QWidget *parent)
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow) // Moved
    , isProcessing(false)     // Matches declaration order
    , currentProcess(nullptr) // Matches declaration order
    , Processing_Status(PROCESS_TYPE_NONE)    // Initialize here with enum
    , current_File_Row_Number(-1) // Initialize here
    , ui(new Ui::MainWindow) // Initialize ui later
{
    ui->setupUi(this);

    /*
     * Disabled block that previously re-parented the first image tab's table and
     * inserted a new layout with a title label. The application no longer
     * performs this dynamic re-layout and instead relies on the original UI
     * design from the Qt form.
     */

    // Initialize RealCUGAN pointers to ensure safe use when the
    // dedicated widgets are absent. Fallback to Frame Interpolation
    // widgets when available.
    comboBox_Model_RealCUGAN = nullptr;
    spinBox_Scale_RealCUGAN = nullptr;
    spinBox_DenoiseLevel_RealCUGAN = nullptr;
    spinBox_TileSize_RealCUGAN = nullptr;
    checkBox_TTA_RealCUGAN = nullptr;
    comboBox_GPUID_RealCUGAN = nullptr;
    pushButton_DetectGPU_RealCUGAN = nullptr;
    checkBox_MultiGPU_RealCUGAN = nullptr;
    groupBox_GPUSettings_MultiGPU_RealCUGAN = nullptr;
    comboBox_GPUIDs_MultiGPU_RealCUGAN = nullptr;
    listWidget_GPUList_MultiGPU_RealCUGAN = nullptr;
    pushButton_AddGPU_MultiGPU_RealCUGAN = nullptr;
    pushButton_RemoveGPU_MultiGPU_RealCUGAN = nullptr;
    pushButton_ClearGPU_MultiGPU_RealCUGAN = nullptr;
    pushButton_TileSize_Add_RealCUGAN = nullptr;
    pushButton_TileSize_Minus_RealCUGAN = nullptr;
    widget_RealCUGAN_Hidden = nullptr;

    if (ui->comboBox_Model_VFI)
        comboBox_Model_RealCUGAN = ui->comboBox_Model_VFI;
    if (ui->comboBox_GPUID_VFI)
        comboBox_GPUID_RealCUGAN = ui->comboBox_GPUID_VFI;
    if (ui->pushButton_DetectGPU_VFI)
        pushButton_DetectGPU_RealCUGAN = ui->pushButton_DetectGPU_VFI;
    if (ui->checkBox_MultiGPU_VFI)
        checkBox_MultiGPU_RealCUGAN = ui->checkBox_MultiGPU_VFI;
    if (ui->widget_RealCUGAN_Hidden)
        widget_RealCUGAN_Hidden = ui->widget_RealCUGAN_Hidden;
    // Current_Path is already initialized using qApp->applicationDirPath() in mainwindow.h or before constructor
    TempDir_Path = Current_Path + "/temp"; // Initialize here
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


void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow closing";
    QMainWindow::closeEvent(event);
    qApp->quit();
}

// Stubs for general utility functions that should be in mainwindow.cpp
// FileMetadataCache MainWindow::getOrFetchMetadata(const QString &filePath) { qDebug() << "STUB: MainWindow::getOrFetchMetadata called for" << filePath; return FileMetadataCache(); } // Already replaced
bool MainWindow::ReplaceOriginalFile(QString original_fullpath, QString output_fullpath)
{
    // Ensure output file exists
    QFileInfo outputFileInfo(output_fullpath);
    if (!outputFileInfo.exists() || !outputFileInfo.isFile()) {
        qWarning() << "ReplaceOriginalFile: Output file does not exist or is not a file:" << output_fullpath;
        return false;
    }

    // Ensure original file exists before trying to trash it
    QFileInfo originalFileInfo(original_fullpath);
    if (originalFileInfo.exists() && originalFileInfo.isFile()) {
        if (!fileManager.moveToTrash(original_fullpath)) {
            qWarning() << "ReplaceOriginalFile: Failed to move original file to trash:" << original_fullpath;
            // Depending on desired behavior, you might want to stop here or try to rename anyway.
            // For now, we'll log a warning and proceed with the rename attempt.
        } else {
            qDebug() << "ReplaceOriginalFile: Successfully moved original file to trash:" << original_fullpath;
        }
    } else {
        qDebug() << "ReplaceOriginalFile: Original file does not exist, no need to move to trash:" << original_fullpath;
    }

    // Rename output file to original file's path
    if (QFile::rename(output_fullpath, original_fullpath)) {
        qDebug() << "ReplaceOriginalFile: Successfully renamed" << output_fullpath << "to" << original_fullpath;
        return true;
    } else {
        qWarning() << "ReplaceOriginalFile: Failed to rename" << output_fullpath << "to" << original_fullpath << "Error:" << QFileDevice::NoError; // NoError seems wrong, but QFile doesn't have detailed error enums easily accessible here. Check QLastError or similar if specific errors are needed.
        // Attempt to log last error if possible, though QFile::error() is not static
        QFile f(output_fullpath); // Create a temporary QFile object to get error string
        qWarning() << "Rename error string:" << f.errorString(); // This might not be the exact error of the rename op but can give clues.
        return false;
    }
}
void MainWindow::MoveFileToOutputPath(QString OrginalPath, QString SourceFilePath)
{
    QFileInfo sourceFileInfo(SourceFilePath);
    if (!sourceFileInfo.exists() || !sourceFileInfo.isFile()) {
        qWarning() << "MoveFileToOutputPath: Source file does not exist or is not a file:" << SourceFilePath;
        return;
    }

    QFileInfo originalFileInfo(OrginalPath); // In this context, 'OrginalPath' is used to determine the target *directory*
    QString targetDirectoryPath;

    if (originalFileInfo.isDir()) { // If OrginalPath is already a directory
        targetDirectoryPath = originalFileInfo.absoluteFilePath();
    } else { // If OrginalPath is a file, get its directory
        targetDirectoryPath = originalFileInfo.absolutePath();
    }

    QDir targetDir(targetDirectoryPath);
    if (!targetDir.exists()) {
        if (!targetDir.mkpath(".")) { // "." means create the path stored in targetDir
            qWarning() << "MoveFileToOutputPath: Could not create target directory:" << targetDirectoryPath;
            return;
        }
        qDebug() << "MoveFileToOutputPath: Created target directory:" << targetDirectoryPath;
    }

    QString newFilePath = targetDir.filePath(sourceFileInfo.fileName());

    if (QFile::exists(newFilePath)) {
        qWarning() << "MoveFileToOutputPath: Target file already exists, attempting to remove:" << newFilePath;
        if (!QFile::remove(newFilePath)) {
            qWarning() << "MoveFileToOutputPath: Could not remove existing target file:" << newFilePath;
            // Optionally, append a number or UUID to the filename if overwriting is not desired
            // For now, if removal fails, the subsequent rename will likely also fail.
        }
    }

    if (QFile::rename(SourceFilePath, newFilePath)) {
        qDebug() << "MoveFileToOutputPath: Successfully moved" << SourceFilePath << "to" << newFilePath;
    } else {
        qWarning() << "MoveFileToOutputPath: Failed to move" << SourceFilePath << "to" << newFilePath;
        QFile f(SourceFilePath);
        qWarning() << "Move error string:" << f.errorString();
    }
}
QMap<QString, int> MainWindow::Image_Gif_Read_Resolution(QString SourceFileFullPath)
{
    QMap<QString, int> resolutionMap;
    resolutionMap.insert("width", 0);
    resolutionMap.insert("height", 0);

    QFileInfo fileInfo(SourceFileFullPath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        qWarning() << "Image_Gif_Read_Resolution: File does not exist or is not a file:" << SourceFileFullPath;
        return resolutionMap;
    }

    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "gif") {
        QMovie movie(SourceFileFullPath);
        if (movie.isValid()) {
            resolutionMap["width"] = movie.frameRect().width();
            resolutionMap["height"] = movie.frameRect().height();
            qDebug() << "Image_Gif_Read_Resolution: GIF resolution read for" << SourceFileFullPath << "W:" << resolutionMap["width"] << "H:" << resolutionMap["height"];
        } else {
            qWarning() << "Image_Gif_Read_Resolution: QMovie could not read GIF:" << SourceFileFullPath;
        }
    } else if (QStringList({"png", "jpg", "jpeg", "bmp", "webp", "tif", "tiff"})
                   .contains(suffix)) {
        QImageReader reader(SourceFileFullPath);
        if (reader.canRead()) {
            QSize dimensions = reader.size();
            resolutionMap["width"] = dimensions.width();
            resolutionMap["height"] = dimensions.height();
            qDebug() << "Image_Gif_Read_Resolution: Image resolution read for" << SourceFileFullPath << "W:" << resolutionMap["width"] << "H:" << resolutionMap["height"];
        } else {
            qWarning() << "Image_Gif_Read_Resolution: QImageReader could not read image:" << SourceFileFullPath << "Error:" << reader.errorString();
        }
    } else {
        qWarning() << "Image_Gif_Read_Resolution: Unsupported file type for resolution reading:" << SourceFileFullPath;
        // Optionally, could try getOrFetchMetadata here if it's desired to support video formats too,
        // but the function name suggests it's primarily for images and GIFs.
        // For now, it will return 0,0 for unsupported types.
    }

    return resolutionMap;
}
/**
 * @brief Pause execution for the specified number of seconds.
 */
void MainWindow::Delay_sec_sleep(int time)
{
    QThread::sleep(static_cast<unsigned long>(time));
}

/**
 * @brief Pause execution for the specified number of milliseconds.
 */
void MainWindow::Delay_msec_sleep(int time)
{
    QThread::msleep(static_cast<unsigned long>(time));
}
/**
 * @brief Execute a command string using the system shell.
 *
 * On Windows this launches cmd.exe, while on POSIX systems it uses
 * /bin/sh. The requestAdmin flag is currently ignored.
 */
void MainWindow::ExecuteCMD_batFile(QString cmd_str, bool requestAdmin)
{
    Q_UNUSED(requestAdmin);
#ifdef Q_OS_WIN
    QStringList args;
    args << "/c" << cmd_str;
    QProcess::startDetached(QStringLiteral("cmd.exe"), args);
#else
    QProcess::startDetached(QStringLiteral("/bin/sh"), {"-c", cmd_str});
#endif
}

// Stubs for _finished signals/slots and other functions expected in mainwindow.cpp (not causing multiple defs)
int MainWindow::Waifu2x_DetectGPU_finished()
{
    // This function is generic; it might be for Waifu2x-NCNN-Vulkan or Waifu2x-Converter.
    // Assuming it's for Waifu2x-NCNN-Vulkan based on Available_GPUID_Waifu2xNcnnVulkan.
    // If there's a specific button for Waifu2x NCNN Vulkan GPU detection:
    // ui->pushButton_DetectGPU_Waifu2xNCNN->setEnabled(true);
    // ui->pushButton_DetectGPU_Waifu2xNCNN->setText(tr("Detect GPU"));
    // ui->comboBox_GPUID_Waifu2xNCNN->clear();
    // for (const QString &gpu : Available_GPUID_Waifu2xNcnnVulkan) { // Or appropriate list
    //     ui->comboBox_GPUID_Waifu2xNCNN->addItem(gpu);
    // }

    // If it's for Waifu2x-Converter (which has "processors" not just GPUs)
    // This might be better handled by Waifu2x_DumpProcessorList_converter_finished()
    // For now, let's assume it's a general NCNN Vulkan engine if no specific UI exists.
    // If ui->comboBox_GPUID_Waifu2x exists:
    if(ui->comboBox_GPUID) { // Example if such a combo box exists
        ui->comboBox_GPUID->clear();
        ui->comboBox_GPUID->addItems(Available_GPUID_Waifu2xNcnnVulkan); // Use the correct list
        if(ui->pushButton_DetectGPU) { // Example button
             ui->pushButton_DetectGPU->setEnabled(true);
             ui->pushButton_DetectGPU->setText(tr("Detect GPU"));
        }
    }
    qDebug() << "Waifu2x_DetectGPU_finished completed. Populated relevant UI if elements exist.";
    return 0;
}

int MainWindow::Realsr_ncnn_vulkan_DetectGPU_finished()
{
    if (ui->pushButton_DetectGPU_RealsrNCNNVulkan) { // This is the Realsr/RealESRGAN detect button
        ui->pushButton_DetectGPU_RealsrNCNNVulkan->setEnabled(true);
        ui->pushButton_DetectGPU_RealsrNCNNVulkan->setText(tr("Detect GPU"));
    }
    if (ui->comboBox_GPUID_RealsrNCNNVulkan) { // This is the Realsr/RealESRGAN GPU combobox
        ui->comboBox_GPUID_RealsrNCNNVulkan->clear();
        // Available_GPUID_Realsr_ncnn_vulkan is for the old RealSR.
        // For RealESRGAN, it's Available_GPUID_RealESRGAN_ncnn_vulkan
        // The UI element name "RealsrNCNNVulkan" is ambiguous.
        // Assuming this function is for the specific "RealSR-NCNN-Vulkan" if it's distinct,
        // otherwise it might conflict with RealESRGAN_ncnn_vulkan_DetectGPU_finished()
        ui->comboBox_GPUID_RealsrNCNNVulkan->addItems(Available_GPUID_Realsr_ncnn_vulkan);
        qDebug() << "Realsr_ncnn_vulkan_DetectGPU_finished: Populated" << ui->comboBox_GPUID_RealsrNCNNVulkan->objectName() << "with" << Available_GPUID_Realsr_ncnn_vulkan.join(", ");
    } else {
        qWarning() << "Realsr_ncnn_vulkan_DetectGPU_finished: comboBox_GPUID_RealsrNCNNVulkan is null.";
    }
    return 0;
}
int MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_finished()
{
    // Assuming RealCUGAN UI elements were consolidated into VFI ones
    if (ui->pushButton_DetectGPU_VFI) {
        ui->pushButton_DetectGPU_VFI->setEnabled(true);
        ui->pushButton_DetectGPU_VFI->setText(tr("Detect GPU"));
    }
    if (ui->comboBox_GPUID_VFI) {
        ui->comboBox_GPUID_VFI->clear();
        for (const QString &gpu : Available_GPUID_RealCUGAN) // Still use RealCUGAN's available ID list
        {
            ui->comboBox_GPUID_VFI->addItem(gpu);
        }
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
void MainWindow::SRMD_DetectGPU_finished()
{
    if (ui->pushButton_DetectGPUID_srmd) { // Assuming a button like pushButton_DetectGPUID_srmd
        ui->pushButton_DetectGPUID_srmd->setEnabled(true);
        ui->pushButton_DetectGPUID_srmd->setText(tr("Detect GPU"));
    }
    if (ui->comboBox_GPUID_srmd) { // Assuming a combobox like comboBox_GPUID_srmd
        ui->comboBox_GPUID_srmd->clear();
        ui->comboBox_GPUID_srmd->addItems(Available_GPUID_srmd);
        qDebug() << "SRMD_DetectGPU_finished: Populated" << ui->comboBox_GPUID_srmd->objectName() << "with" << Available_GPUID_srmd.join(", ");
    } else {
         qWarning() << "SRMD_DetectGPU_finished: comboBox_GPUID_srmd is null.";
    }
}

int MainWindow::Waifu2x_DumpProcessorList_converter_finished()
{
    if (ui->comboBox_TargetProcessor_converter) {
        ui->comboBox_TargetProcessor_converter->clear();
        ui->comboBox_TargetProcessor_converter->addItems(Available_ProcessorList_converter);
        qDebug() << "Waifu2x_DumpProcessorList_converter_finished: Populated"
                 << ui->comboBox_TargetProcessor_converter->objectName()
                 << "with" << Available_ProcessorList_converter.join(", ");
    } else {
        qWarning() << "Waifu2x_DumpProcessorList_converter_finished: comboBox_TargetProcessor_converter is null.";
    }
    // This might also enable a "Detect Processor" button if one exists for Waifu2x Converter
    // if (ui->pushButton_DetectProcessor_Converter) {
    //     ui->pushButton_DetectProcessor_Converter->setEnabled(true);
    //     ui->pushButton_DetectProcessor_Converter->setText(tr("Detect Processor"));
    // }
    return 0;
}
/**
 * @brief Ensure the GIF resize disable checkbox is checked.
 */
void MainWindow::Set_checkBox_DisableResize_gif_Checked()
{
    if (ui->checkBox_DisableResize_gif && !ui->checkBox_DisableResize_gif->isChecked()) {
        ui->checkBox_DisableResize_gif->setChecked(true);
        Settings_Save();
    }
}

void MainWindow::Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    if (Table_model_image) {
        QList<QStandardItem *> rowItems;
        QStandardItem *fileNameItem = new QStandardItem(fileName);
        fileNameItem->setToolTip(SourceFile_fullPath); // Show full path on hover
        fileNameItem->setEditable(false);

        QStandardItem *pathItem = new QStandardItem(SourceFile_fullPath); // For internal use, can be hidden
        pathItem->setEditable(false);

        QStandardItem *statusItem = new QStandardItem(tr("Waiting"));
        statusItem->setEditable(false);
        statusItem->setTextAlignment(Qt::AlignCenter);


        // Column 0: Filename
        rowItems.append(fileNameItem);
        // Column 1: Full Path (Store it, but it might not be visible, or only partially)
        // Depending on table setup, you might just use tooltip or specific user role for full path
        // For simplicity, let's assume a column for it, which can be hidden by default by UI setup
        rowItems.append(pathItem);
        // Column 2: Status
        rowItems.append(statusItem);
        // Column 3: Resolution (e.g., "1920x1080") - initially empty or fetched async
        QStandardItem *resolutionItem = new QStandardItem("");
        resolutionItem->setEditable(false);
        resolutionItem->setTextAlignment(Qt::AlignCenter);
        rowItems.append(resolutionItem);
        // Column 4: Output Path - initially empty or based on settings
        QStandardItem *outputPathItem = new QStandardItem("");
        outputPathItem->setEditable(false);
        rowItems.append(outputPathItem);
        // Column 5: Engine Settings (e.g., "Model: realesrgan-x4plus, Scale: 2x") - initially based on current UI
        QStandardItem *engineSettingsItem = new QStandardItem(""); // Placeholder
        engineSettingsItem->setEditable(false);
        rowItems.append(engineSettingsItem);


        Table_model_image->appendRow(rowItems);

        // Store full path in user data for easy access if path column is hidden/managed differently
        // Table_model_image->item(Table_model_image->rowCount() -1, 0)->setData(SourceFile_fullPath, Qt::UserRole + 1);


        // These lists might be redundant if the model is the source of truth, but kept for compatibility if used elsewhere
        table_image_item_fullpath.append(SourceFile_fullPath);
        table_image_item_fileName.append(fileName);
        Table_FileCount_reload(); // Update file counts displayed in UI
    }
}

void MainWindow::Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    if (Table_model_gif) {
        QList<QStandardItem *> rowItems;
        QStandardItem *fileNameItem = new QStandardItem(fileName);
        fileNameItem->setToolTip(SourceFile_fullPath);
        fileNameItem->setEditable(false);

        QStandardItem *pathItem = new QStandardItem(SourceFile_fullPath);
        pathItem->setEditable(false);

        QStandardItem *statusItem = new QStandardItem(tr("Waiting"));
        statusItem->setEditable(false);
        statusItem->setTextAlignment(Qt::AlignCenter);

        rowItems.append(fileNameItem);
        rowItems.append(pathItem);
        rowItems.append(statusItem);
        // Add more columns similar to image table if needed (Resolution, Output Path, Engine Settings)
        QStandardItem *resolutionItem = new QStandardItem("");
        resolutionItem->setEditable(false);
        resolutionItem->setTextAlignment(Qt::AlignCenter);
        rowItems.append(resolutionItem);

        QStandardItem *outputPathItem = new QStandardItem("");
        outputPathItem->setEditable(false);
        rowItems.append(outputPathItem);

        QStandardItem *engineSettingsItem = new QStandardItem("");
        engineSettingsItem->setEditable(false);
        rowItems.append(engineSettingsItem);

        Table_model_gif->appendRow(rowItems);
        table_gif_item_fileName.append(fileName);
        // table_gif_item_fullpath.append(SourceFile_fullPath); // if this list exists
        Table_FileCount_reload();
    }
}

void MainWindow::Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    if (Table_model_video) {
        QList<QStandardItem *> rowItems;
        QStandardItem *fileNameItem = new QStandardItem(fileName);
        fileNameItem->setToolTip(SourceFile_fullPath);
        fileNameItem->setEditable(false);

        QStandardItem *pathItem = new QStandardItem(SourceFile_fullPath);
        pathItem->setEditable(false);

        QStandardItem *statusItem = new QStandardItem(tr("Waiting"));
        statusItem->setEditable(false);
        statusItem->setTextAlignment(Qt::AlignCenter);

        rowItems.append(fileNameItem);
        rowItems.append(pathItem);
        rowItems.append(statusItem);
        // Add more columns similar to image table if needed (Resolution, FPS, Duration, Output Path, Engine Settings)
        QStandardItem *resolutionItem = new QStandardItem(""); // e.g., "1920x1080"
        resolutionItem->setEditable(false);
        resolutionItem->setTextAlignment(Qt::AlignCenter);
        rowItems.append(resolutionItem);

        QStandardItem *fpsItem = new QStandardItem(""); // e.g., "29.97"
        fpsItem->setEditable(false);
        fpsItem->setTextAlignment(Qt::AlignCenter);
        rowItems.append(fpsItem);

        QStandardItem *durationItem = new QStandardItem(""); // e.g., "00:02:30"
        durationItem->setEditable(false);
        durationItem->setTextAlignment(Qt::AlignCenter);
        rowItems.append(durationItem);

        QStandardItem *outputPathItem = new QStandardItem("");
        outputPathItem->setEditable(false);
        rowItems.append(outputPathItem);

        QStandardItem *engineSettingsItem = new QStandardItem("");
        engineSettingsItem->setEditable(false);
        rowItems.append(engineSettingsItem);

        Table_model_video->appendRow(rowItems);
        table_video_item_fileName.append(fileName);
        // table_video_item_fullpath.append(SourceFile_fullPath); // if this list exists
        Table_FileCount_reload();
    }
}

// Stubs for remaining undefined references from linker errors
void MainWindow::on_pushButton_CustRes_apply_clicked()
{
    // This function is responsible for applying custom resolution settings.
    // It needs to interact with UI elements where the user inputs custom resolution,
    // then store these settings, possibly updating a list (Custom_resolution_list)
    // and applying them to selected items in the table views.

    // Example: Reading from hypothetical UI elements
    // int newWidth = ui->spinBox_CustomWidth->value();
    // int newHeight = ui->spinBox_CustomHeight->value();
    // QString selectedFilePath = ui->label_SelectedFileForCustRes->text(); // Assuming a label shows this

    // if (selectedFilePath.isEmpty() || (newWidth == 0 && newHeight == 0)) {
    //     QMessageBox::warning(this, tr("Custom Resolution"), tr("No file selected or resolution is zero."));
    //     return;
    // }

    // CustRes_remove(selectedFilePath); // Remove old entry if exists
    // QMap<QString, QString> resMap;
    // resMap.insert("fullpath", selectedFilePath);
    // resMap.insert("width", QString::number(newWidth));
    // resMap.insert("height", QString::number(newHeight));
    // Custom_resolution_list.append(resMap);

    // Apply to current selection or globally based on UI design
    // This part is highly dependent on how custom resolution is applied (per file, per type, etc.)
    // For instance, if applying to a selected item in image table:
    // if (curRow_image != -1) {
    //     Table_image_CustRes_rowNumInt_HeightQString_WidthQString(curRow_image, QString::number(newHeight), QString::number(newWidth));
    // }
    // Similar logic for GIF and Video tables.

    // Settings_Save(); // Save settings if custom resolutions are persisted this way
    TextBrowser_NewMessage(tr("Custom resolution applied (actual implementation depends on UI specifics)."));
}

void MainWindow::on_pushButton_CustRes_cancel_clicked()
{
    // This function cancels the custom resolution operation.
    // It might clear UI fields or reset temporary states.

    // Example: Clearing hypothetical UI elements
    // ui->spinBox_CustomWidth->setValue(0);
    // ui->spinBox_CustomHeight->setValue(0);
    // ui->label_SelectedFileForCustRes->clear();

    // If there's a specific item whose custom resolution is being cancelled:
    // int currentRow = -1; // Determine current row from active table
    // if (ui->tabWidget_Input->currentIndex() == 0 && curRow_image != -1) { // Image tab
    //     Table_image_CustRes_Cancel_rowNumInt(curRow_image);
    // } else if (ui->tabWidget_Input->currentIndex() == 1 && curRow_gif != -1) { // GIF tab
    //     Table_gif_CustRes_Cancel_rowNumInt(curRow_gif);
    // } else if (ui->tabWidget_Input->currentIndex() == 2 && curRow_video != -1) { // Video tab
    //     Table_video_CustRes_Cancel_rowNumInt(curRow_video);
    // }

    TextBrowser_NewMessage(tr("Custom resolution cancelled (actual implementation depends on UI specifics)."));
}

/**
 * @brief Safely update the update channel combo box.
 */
void MainWindow::comboBox_UpdateChannel_setCurrentIndex_self(int index)
{
    QMutexLocker locker(&comboBox_UpdateChannel_setCurrentIndex_self_QMutex);
    if (ui->comboBox_UpdateChannel) {
        if (index >= 0 && index < ui->comboBox_UpdateChannel->count()) {
            ui->comboBox_UpdateChannel->setCurrentIndex(index);
        }
    }
}

void MainWindow::on_comboBox_language_currentIndexChanged(int index)
{
    QString lang = ui->comboBox_language->itemData(index).toString(); // Assuming itemData stores "en", "zh_CN" etc.
    if (translator->isEmpty()) {
        qApp->installTranslator(translator);
    } else {
        qApp->removeTranslator(translator); // Remove previous translator
        delete translator; // Delete the old translator object
        translator = new QTranslator(this); // Create a new one
        qApp->installTranslator(translator);
    }

    if (lang == "en") {
        if (!translator->load(":/language/language_English.qm")) {
            qWarning() << "Failed to load translation file: :/language/language_English.qm";
        }
    } else if (lang == "zh_CN") {
        if (!translator->load(":/language/language_ChineseSimplified.qm")) { // Example path
            qWarning() << "Failed to load translation file: :/language/language_ChineseSimplified.qm";
        }
    } else if (lang == "zh_TW") {
        if (!translator->load(":/language/language_ChineseTraditional.qm")) { // Example path
            qWarning() << "Failed to load translation file: :/language/language_ChineseTraditional.qm";
        }
    }
    // Add other languages here
    // else if (lang == "ja") {
    //     translator->load(":/language/language_Japanese.qm");
    // }

    Settings_Save(); // Save the new language preference
    ui->retranslateUi(this); // Retranslate UI (though Qt does much of this automatically on changeEvent)
    TextBrowser_NewMessage(tr("Language changed to %1. Restart recommended if not all elements updated.").arg(ui->comboBox_language->currentText()));
}

void MainWindow::on_pushButton_about_clicked()
{
    QMessageBox::about(this, tr("About Beya Waifu"),
                       tr("<h3>Beya Waifu %1</h3>"
                          "<p>Upscaling software for images, GIFs, and videos using various AI models.</p>"
                          "<p>Copyright (C) 2025 beyawnko</p>"
                          "<p>This program comes with ABSOLUTELY NO WARRANTY.</p>"
                          "<p>This is free software, and you are welcome to redistribute it "
                          "under certain conditions; see the GNU AGPLv3+ license for details.</p>"
                          "<p>GitHub: <a href='https://github.com/beyawnko/Beya_Waifu'>https://github.com/beyawnko/Beya_Waifu</a></p>")
                       .arg(VERSION));
}

void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int arg1)
{
    QFont font = ui->textBrowser->font();
    font.setPointSize(arg1);
    ui->textBrowser->setFont(font);
}

void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()
{
    // The font size is likely already applied by on_spinBox_textbrowser_fontsize_valueChanged.
    // This button's purpose is to persist this value.
    Settings_Save(); // This should save the value from ui->spinBox_GlobalFontSize (or spinBox_textbrowser_fontsize)
                     // Ensure spinBox_GlobalFontSize value is actually saved by Settings_Save under a specific key.
    TextBrowser_NewMessage(tr("Global font size saved."));
}

void MainWindow::on_pushButton_HideTextBro_clicked()
{
    bool isHidden = ui->textBrowser->isHidden();
    ui->textBrowser->setHidden(!isHidden);
    ui->pushButton_HideTextBro->setText(!isHidden ? tr("Show Text Browser") : tr("Hide Text Browser"));
}

void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1)
{
    bool shouldHide = (arg1 == Qt::Checked);
    if (shouldHide) {
        ui->textBrowser->hide();
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
        ui->pushButton_HideTextBro->setEnabled(false); // Disable manual toggle if always hidden
    } else {
        // Don't automatically show, respect current state by pushButton_HideTextBro_clicked
        ui->pushButton_HideTextBro->setEnabled(true);
    }
    Settings_Save(); // Save this preference
}

void MainWindow::on_Ext_image_textChanged(const QString &arg1)
{
    // This function is called when the text in the image extension lineEdit changes.
    // It should save the new preferred image output extension.
    // Example: ui->lineEdit_Ext_image->text() is arg1
    Settings_Save(); // Ensure Settings_Save reads from ui->lineEdit_Ext_image and saves it
    qDebug() << "Image output extension changed to:" << arg1;
}

void MainWindow::on_Ext_video_textChanged(const QString &arg1)
{
    // This function is called when the text in the video extension lineEdit changes.
    // It should save the new preferred video output extension.
    // Example: ui->lineEdit_Ext_video->text() is arg1
    Settings_Save(); // Ensure Settings_Save reads from ui->lineEdit_Ext_video and saves it
    qDebug() << "Video output extension changed to:" << arg1;
}

void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int index)
{
    // This handles changes in the Waifu2x-NCNN-Vulkan model selection (if this is the correct combo box)
    // The name "comboBox_model_vulkan" is generic. Assuming it's for Waifu2x NCNN Vulkan.
    // If it's for a different engine, the logic would target that engine's settings.
    Q_UNUSED(index);
    Settings_Save(); // Persist the selected model index or name for Waifu2x NCNN Vulkan
    // May need to trigger a settings reload for the specific processor if it caches them:
    // realCuganProcessor->readSettings(); // Or whichever processor this combo box affects
    qDebug() << "Vulkan model selection changed. Index:" << index << "Text:" << ui->comboBox_model_vulkan->currentText();
}

void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int index)
{
    // This handles changes in the image processing style (e.g., artwork, photo).
    // This usually corresponds to different models or pre/post-processing steps.
    Q_UNUSED(index);
    Settings_Save(); // Persist the selected image style
    qDebug() << "Image style changed. Index:" << index << "Text:" << ui->comboBox_ImageStyle->currentText();
}

void MainWindow::on_pushButton_ResetVideoSettings_clicked()
{
    // Reset video settings to default values.
    // This involves setting UI elements for video processing to their defaults
    // and then saving these default settings.

    // Example for hypothetical video settings UI elements:
    // ui->comboBox_VideoEncoder->setCurrentIndex(0); // Assuming 0 is default
    // ui->spinBox_VideoBitrate->setValue(5000); // Default bitrate
    // ui->checkBox_UseHardwareEncoding_Video->setChecked(false);
    // ui->lineEdit_encoder_vid->setText("libx264"); // Default video encoder
    // ui->lineEdit_encoder_audio->setText("aac");   // Default audio encoder
    // ui->lineEdit_pixformat->setText("yuv420p"); // Default pixel format
    // ui->checkBox_vcodec_copy_2mp4->setChecked(false);
    // ui->checkBox_acodec_copy_2mp4->setChecked(false);
    // ... reset other video related UI elements ...

    Settings_Save(); // Persist the reset settings
    TextBrowser_NewMessage(tr("Video settings have been reset to defaults."));
}

void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save custom video encoder string
    qDebug() << "Video encoder text changed to:" << arg1;
}

void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save custom audio encoder string
    qDebug() << "Audio encoder text changed to:" << arg1;
}

void MainWindow::on_lineEdit_pixformat_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save custom pixel format string
    qDebug() << "Pixel format text changed to:" << arg1;
}

void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save video codec copy state
    qDebug() << "Video codec copy state changed:" << arg1;
}

void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save audio codec copy state
    qDebug() << "Audio codec copy state changed:" << arg1;
}

void MainWindow::on_pushButton_encodersList_clicked()
{
    // Display a list of available FFmpeg encoders.
    // This typically involves running "ffmpeg -encoders" and showing the output.
    QString ffmpegPath = FFMPEG_EXE_PATH_Waifu2xEX; // Assuming this member holds the path to ffmpeg
    QProcess ffmpegProcess;
    QStringList arguments;
    arguments << "-encoders";
    ffmpegProcess.start(ffmpegPath, arguments);
    if (!ffmpegProcess.waitForStarted()) {
        QMessageBox::warning(this, tr("FFmpeg Error"), tr("Failed to start FFmpeg process."));
        return;
    }
    if (!ffmpegProcess.waitForFinished()) {
        QMessageBox::warning(this, tr("FFmpeg Error"), tr("FFmpeg process timed out or failed to finish."));
        return;
    }
    QString output = QString::fromLocal8Bit(ffmpegProcess.readAllStandardOutput());
    QString errorOutput = QString::fromLocal8Bit(ffmpegProcess.readAllStandardError()); // Encoders list might be on stderr

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Available FFmpeg Encoders"));
    msgBox.setTextFormat(Qt::PlainText);
    if (!output.trimmed().isEmpty()) {
         msgBox.setText(output);
    } else if (!errorOutput.trimmed().isEmpty()) {
         msgBox.setText(errorOutput); // Often, info like this goes to stderr
    } else {
        msgBox.setText(tr("No encoders found or FFmpeg error."));
    }
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::on_checkBox_DelOriginal_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save "Delete Original File" preference
    qDebug() << "Delete original file state changed:" << arg1;
    if (ui->checkBox_ReplaceOriginalFile && ui->checkBox_DelOriginal->isChecked()) {
        ui->checkBox_ReplaceOriginalFile->setChecked(false); // Cannot replace and delete
        TextBrowser_NewMessage(tr("'Replace Original File' has been unchecked because 'Delete Original File' is selected."));
    }
}

void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save "File List Interactive" preference
    // This might change how table views behave, e.g., single click vs double click to open/select
    bool isInteractive = (arg1 == Qt::Checked);
    if(isInteractive){
        ui->tableView_image->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView_gif->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView_video->setEditTriggers(QAbstractItemView::NoEditTriggers);
    } else {
        ui->tableView_image->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        ui->tableView_gif->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        ui->tableView_video->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    }
    qDebug() << "File list interactive state changed:" << arg1;
}

void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int arg1)
{
    bool isEnabled = (arg1 == Qt::Checked);
    ui->lineEdit_outputPath->setEnabled(isEnabled);
    // ui->pushButton_output_path_browse->setEnabled(isEnabled);
    if (!isEnabled) {
        ui->lineEdit_outputPath->clear(); // Optionally clear path if disabled
    }
    Settings_Save(); // Save "Output Path Enabled" preference
    qDebug() << "Output path enabled state changed:" << arg1;
}

void MainWindow::on_pushButton_ForceRetry_clicked()
{
    // This function should re-queue files that have failed.
    // It needs to iterate through the table models, find items with a "Failed" status,
    // change their status to "Waiting" (or similar), and potentially re-trigger the processing queue.
    int retriedCount = 0;
    for (int i = 0; i < Table_model_image->rowCount(); ++i) {
        if (Table_model_image->item(i, 2)->text() == tr("Failed")) { // Assuming status is in column 2
            Table_model_image->item(i, 2)->setText(tr("Waiting"));
            // Add logic to re-add this item to the processing queue if necessary
            retriedCount++;
        }
    }
    for (int i = 0; i < Table_model_gif->rowCount(); ++i) {
        if (Table_model_gif->item(i, 2)->text() == tr("Failed")) {
            Table_model_gif->item(i, 2)->setText(tr("Waiting"));
            retriedCount++;
        }
    }
    for (int i = 0; i < Table_model_video->rowCount(); ++i) {
        if (Table_model_video->item(i, 2)->text() == tr("Failed")) {
            Table_model_video->item(i, 2)->setText(tr("Waiting"));
            retriedCount++;
        }
    }

    if (retriedCount > 0) {
        TextBrowser_NewMessage(tr("%n file(s) have been re-queued for processing.", "", retriedCount));
        // Potentially call Waifu2xMainThread() or similar to restart processing if it's stopped.
        // if (!isProcessing) Waifu2xMainThread();
    } else {
        TextBrowser_NewMessage(tr("No failed files found to retry."));
    }
    isForceRetryClicked = true; // Set flag if needed by other parts of the application
}

void MainWindow::on_pushButton_PayPal_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.me/Beyawnko"));
}

void MainWindow::on_checkBox_AudioDenoise_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save "Audio Denoise" preference
    qDebug() << "Audio denoise state changed:" << arg1;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    // This function is called when the active tab in the main tabWidget changes.
    // It can be used to update UI elements or settings specific to the new active tab.
    qDebug() << "Main tab widget changed to tab index:" << index;
    // Example: if settings visibility depends on the tab:
    // if (index == 0) { // Image tab
    //     ui->groupBox_ImageSpecificSettings->setVisible(true);
    //     ui->groupBox_VideoSpecificSettings->setVisible(false);
    // } else if (index == 2) { // Video tab
    //     ui->groupBox_ImageSpecificSettings->setVisible(false);
    //     ui->groupBox_VideoSpecificSettings->setVisible(true);
    // }
    // Settings_Read_Apply(); // Could re-apply settings if they are tab-dependent.
}

void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save "Process Video by Segment" preference
    bool isEnabled = (arg1 == Qt::Checked);
    ui->spinBox_VideoSplitDuration->setEnabled(isEnabled); // Assuming such a spinbox exists
    qDebug() << "Process video by segment state changed:" << arg1;
}

void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save(); // Save selected Waifu2x NCNN Vulkan version
    // This might also trigger an update of the executable path for this engine if different versions are different files.
    // e.g., Waifu2x_ncnn_vulkan_ProgramPath = Current_Path + "/waifu2x-ncnn-vulkan-" + ui->comboBox_version_Waifu2xNCNNVulkan->currentText();
    qDebug() << "Waifu2x NCNN Vulkan version changed. Index:" << index << "Version:" << ui->comboBox_version_Waifu2xNCNNVulkan->currentText();
}

void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save();
    qDebug() << "Anime4K Enable PreProcessing state changed:" << arg1;
}

void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save();
    qDebug() << "Anime4K Enable PostProcessing state changed:" << arg1;
}

void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save();
    ui->lineEdit_GPUs_Anime4k->setEnabled(arg1 == Qt::Checked);
    qDebug() << "Anime4K Specify GPU state changed:" << arg1;
}

void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save();
    // This might enable/disable other UI elements related to GPU selection for Anime4K
    // For example, if there's a dropdown for specific GPU models that only applies in GPU mode.
    // ui->comboBox_Anime4K_GPGPU_Model->setEnabled(arg1 == Qt::Checked);
    qDebug() << "Anime4K GPU Mode state changed:" << arg1;
}

void MainWindow::on_checkBox_ShowInterPro_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save preference for showing intermediate processing steps/info
    qDebug() << "Show Intermediate Processing state changed:" << arg1;
}

void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked()
{
    int currentSize = ui->spinBox_SplitSize_Waifu2xCaffe->value();
    ui->spinBox_SplitSize_Waifu2xCaffe->setValue(currentSize + 128); // Example increment
    Settings_Save();
}

void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked()
{
    int currentSize = ui->spinBox_SplitSize_Waifu2xCaffe->value();
    if (currentSize >= 128) { // Example minimum
        ui->spinBox_SplitSize_Waifu2xCaffe->setValue(currentSize - 128);
    } else {
        ui->spinBox_SplitSize_Waifu2xCaffe->setValue(0); // Or a specific minimum like 0 or 64
    }
    Settings_Save();
}

void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save ACNet usage preference for Anime4K
    // This might also affect other UI elements if ACNet has specific parameters
    bool isChecked = (arg1 == Qt::Checked);
    if(isChecked && ui->checkBox_HDNMode_Anime4k){ // If HDN mode exists
        ui->checkBox_HDNMode_Anime4k->setChecked(false); // ACNet and HDN might be mutually exclusive
    }
    qDebug() << "Anime4K ACNet state changed:" << arg1;
}

void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save HDN Mode preference for Anime4K
    bool isChecked = (arg1 == Qt::Checked);
    if(isChecked && ui->checkBox_ACNet_Anime4K){ // If ACNet checkbox exists
        ui->checkBox_ACNet_Anime4K->setChecked(false); // ACNet and HDN might be mutually exclusive
    }
    // Enable/disable HDN level spinbox
    // if(ui->spinBox_HDNLevel_Anime4k) ui->spinBox_HDNLevel_Anime4k->setEnabled(isChecked);
    // TODO: spinBox_HDNLevel_Anime4k not found in UI. Related control might be ui->spinBox_Passes_Anime4K or was removed.
    qDebug() << "Anime4K HDN Mode state changed:" << arg1;
}

void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save "Replace Original File" preference
    if (ui->checkBox_DelOriginal && ui->checkBox_ReplaceOriginalFile->isChecked()) {
        ui->checkBox_DelOriginal->setChecked(false); // Cannot replace and delete
        TextBrowser_NewMessage(tr("'Delete Original File' has been unchecked because 'Replace Original File' is selected."));
    }
    qDebug() << "Replace original file state changed:" << arg1;
}

void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Set_Font_fixed(); // Apply the font change
    Settings_Save();  // Save the preference
    qDebug() << "Custom font enabled state changed:" << arg1;
}

void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked()
{
    // This button likely toggles the visibility or size of a splitter section in the UI
    // For example, if ui->splitter_FilesAndInfo is a QSplitter:
    // QList<int> sizes = ui->splitter_FilesAndInfo->sizes();
    // if (sizes.count() == 2) {
    //     if (sizes[1] > 0) { // If info panel is visible, hide it
    //         ui->splitter_FilesAndInfo->setSizes({sizes[0] + sizes[1], 0});
    //         ui->pushButton_ResizeFilesListSplitter->setText(">"); // Or some icon
    //     } else { // If info panel is hidden, show it
    //         // Calculate a reasonable default size, e.g., 70% files, 30% info
    //         int totalWidth = ui->splitter_FilesAndInfo->width();
    //         ui->splitter_FilesAndInfo->setSizes({(int)(totalWidth * 0.7), (int)(totalWidth * 0.3)});
    //         ui->pushButton_ResizeFilesListSplitter->setText("<"); // Or some icon
    //     }
    // }
    qDebug() << "Resize files list splitter clicked (specific implementation depends on UI structure).";
}

void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save(); // Save selected GPGPU model for Anime4K
    qDebug() << "Anime4K GPGPU Model changed. Index:" << index << "Model:" << ui->comboBox_GPGPUModel_A4k->currentText();
}

void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save(); // Save preference for disabling GPU in Waifu2x Converter
    ui->comboBox_TargetProcessor_converter->setEnabled(arg1 != Qt::Checked); // Enable processor choice only if GPU is not disabled
    qDebug() << "Waifu2x Converter Disable GPU state changed:" << arg1;
}

void MainWindow::on_groupBox_video_settings_clicked()
{
    // This might be connected if the group box itself is checkable and toggles its content visibility
    // Or it's just a general click handler for some other purpose.
    // If it's checkable:
    // if (ui->groupBox_video_settings->isCheckable()) {
    //     bool isChecked = ui->groupBox_video_settings->isChecked();
    //     // Enable/disable child widgets or change layout based on isChecked
    //     qDebug() << "Video settings group box clicked, isChecked:" << isChecked;
    // }
    qDebug() << "Video settings group box clicked (specific action depends on setup).";
}

void MainWindow::on_pushButton_TurnOffScreen_clicked()
{
    TurnOffScreen(); // Call the actual function that turns off the screen
}

void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked()
{
    int currentValue = ui->spinBox_MultipleOfFPS_VFI->value();
    if (currentValue > ui->spinBox_MultipleOfFPS_VFI->minimum()) { // Ensure not going below min
        ui->spinBox_MultipleOfFPS_VFI->setValue(currentValue - 1); // Or some other step
    }
    Settings_Save();
}

void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked()
{
    int currentValue = ui->spinBox_MultipleOfFPS_VFI->value();
    if (currentValue < ui->spinBox_MultipleOfFPS_VFI->maximum()) { // Ensure not going above max
        ui->spinBox_MultipleOfFPS_VFI->setValue(currentValue + 1); // Or some other step
    }
    Settings_Save();
}

void MainWindow::Add_progressBar_CompatibilityTest()
{
    if (ui->progressBar_CompatibilityTest->value() < ui->progressBar_CompatibilityTest->maximum()) {
        ui->progressBar_CompatibilityTest->setValue(ui->progressBar_CompatibilityTest->value() + 1);
    }
}

void MainWindow::SetEnable_pushButton_ForceRetry_self()
{
    ui->pushButton_ForceRetry->setEnabled(true); // Or based on some logic
}

void MainWindow::Waifu2x_Finished()
{
    // This function is called when all processing is complete.
    // Update UI elements, play sound, execute post-finish actions etc.
    isProcessing = false;
    pushButton_Start_setEnabled_self(true);
    pushButton_Stop_setEnabled_self(false);
    TextBrowser_NewMessage(tr("All tasks finished."));
    Play_NFSound(); // Play notification sound

    // Auto finish actions
    if (ui->comboBox_FinishAction->currentIndex() == 1) { // Shutdown
        SystemShutDown();
    } else if (ui->comboBox_FinishAction->currentIndex() == 2) { // Sleep
        // SystemSleep(); // Requires platform specific implementation
        TextBrowser_NewMessage(tr("Sleep action is platform dependent and might not be implemented."));
    } else if (ui->comboBox_FinishAction->currentIndex() == 3) { // Hibernate
        // SystemHibernate(); // Requires platform specific implementation
        TextBrowser_NewMessage(tr("Hibernate action is platform dependent and might not be implemented."));
    }

    // Reset progress bars
    ui->progressBar->setValue(0);
    ui->progressBar_CurrentFile->setValue(0);
    // Update file counts, ETA etc.
    TaskNumFinished = 0;
    TaskNumTotal = 0;
    ETA = 0;
    // TimeCost = 0; // Might want to keep total time cost until cleared
    if (ui->progressBar) ui->progressBar->setFormat(tr("Waiting for tasks..."));
    if (ui->label_progressBar_filenum) ui->label_progressBar_filenum->setText("0/0");
    if (ui->label_ETA) ui->label_ETA->setText(tr("ETA:N/A"));
    if (ui->label_TimeCost) ui->label_TimeCost->setText(tr("Time taken:N/A"));
}

void MainWindow::Waifu2x_Finished_manual()
{
    // Similar to Waifu2x_Finished but for manual stop
    isProcessing = false;
    waifu2x_STOP_confirm = true; // Confirm stop
    pushButton_Start_setEnabled_self(true);
    pushButton_Stop_setEnabled_self(false);
    TextBrowser_NewMessage(tr("Processing manually stopped."));
    ui->progressBar->setValue(0);
    ui->progressBar_CurrentFile->setValue(0);
    if (ui->progressBar) ui->progressBar->setFormat(tr("Stopped."));
}

void MainWindow::TimeSlot()
{
    // This is a timer slot, likely updating elapsed time, ETA, etc.
    if (isProcessing && TaskNumTotal > 0) {
        TimeCost++;
        ui->label_TimeCost->setText(Seconds2hms(TimeCost));

        if (TaskNumFinished > 0 && NewTaskFinished) { // Calculate ETA based on average time per task
            long unsigned int avgTimePerTask = TimeCost / TaskNumFinished;
            ETA = avgTimePerTask * (TaskNumTotal - TaskNumFinished);
            NewTaskFinished = false; // Reset flag
        }
        if (ETA > 0) {
            ui->label_ETA->setText(Seconds2hms(ETA));
        } else {
            ui->label_ETA->setText(tr("Calculating..."));
        }
        if(TaskNumTotal > 0) {
             ui->progressBar->setValue((int)((double)TaskNumFinished / TaskNumTotal * 100));
        }
    }
}

int MainWindow::Waifu2x_Compatibility_Test_finished()
{
    // This function is called when the compatibility test finishes.
    // Update UI based on test results.
    ui->pushButton_compatibilityTest->setEnabled(true);
    ui->pushButton_compatibilityTest->setText(tr("Compatibility Test"));
    Finish_progressBar_CompatibilityTest();

    // Update checkboxes based on isCompatible_ flags
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW);
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P);
    // ... and so on for all other compatibility flags and their checkboxes

    if (widget_RealCUGAN_Hidden)
        widget_RealCUGAN_Hidden->setVisible(isCompatible_RealCUGAN_NCNN_Vulkan);

    TextBrowser_NewMessage(tr("Compatibility test finished. Check results in settings."));
    return 0;
}

// Stubs for functions causing undefined reference errors in linker stage

// From settings.cpp calls
void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save();
    qDebug() << "Waifu2x Converter Target Processor changed. Index:" << index << "Processor:" << ui->comboBox_TargetProcessor_converter->currentText();
}

void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    Settings_Save();
    if(ui->checkBox_AlwaysHideSettings->isChecked()){
        ui->groupBox_Setting->setHidden(true);
        ui->pushButton_HideSettings->setText(tr("Show Settings"));
        ui->pushButton_HideSettings->setEnabled(false);
    } else {
        ui->pushButton_HideSettings->setEnabled(true);
    }
    qDebug() << "Always Hide Settings state changed:" << arg1;
}

void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save();
    // Logic to show/hide engine-specific settings for GIF tab
    uiController.updateEngineSettingsVisibility(this->ui, ui->comboBox_Engine_GIF->currentText());
    qDebug() << "GIF Engine changed. Index:" << index << "Engine:" << ui->comboBox_Engine_GIF->currentText();
}

void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save();
    // Logic to show/hide engine-specific settings for Image tab
    uiController.updateEngineSettingsVisibility(this->ui, ui->comboBox_Engine_Image->currentText());
    qDebug() << "Image Engine changed. Index:" << index << "Engine:" << ui->comboBox_Engine_Image->currentText();
}

void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save();
    // Logic to show/hide engine-specific settings for Video tab
    uiController.updateEngineSettingsVisibility(this->ui, ui->comboBox_Engine_Video->currentText());
    qDebug() << "Video Engine changed. Index:" << index << "Engine:" << ui->comboBox_Engine_Video->currentText();
}

void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // Set CustRes_AspectRatioMode based on index
    if (index == 0) CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;
    else if (index == 1) CustRes_AspectRatioMode = Qt::KeepAspectRatio;
    else if (index == 2) CustRes_AspectRatioMode = Qt::KeepAspectRatioByExpanding;
    Settings_Save();
    qDebug() << "Custom Resolution Aspect Ratio Mode changed. Index:" << index;
}


// From moc_mainwindow.cpp calls
void MainWindow::on_pushButton_Stop_clicked()
{
    if (isProcessing) {
        waifu2x_STOP = true; // Signal processing thread to stop
        TextBrowser_NewMessage(tr("Stopping process... Please wait."));
        pushButton_Stop_setEnabled_self(false); // Disable stop button once clicked
        // The actual stop confirmation and UI update will be in Waifu2x_Finished_manual or similar
    }
}

int MainWindow::on_pushButton_RemoveItem_clicked()
{
    // Remove selected item(s) from the currently active table view
    QAbstractItemView *currentTableView = nullptr;
    QStandardItemModel *currentModel = nullptr;
    int currentTabIndex = ui->tabWidget->currentIndex();

    if (currentTabIndex == 0) { // Image
        currentTableView = ui->tableView_image;
        currentModel = Table_model_image;
    } else if (currentTabIndex == 1) { // GIF
        currentTableView = ui->tableView_gif;
        currentModel = Table_model_gif;
    } else if (currentTabIndex == 2) { // Video
        currentTableView = ui->tableView_video;
        currentModel = Table_model_video;
    }

    if (currentTableView && currentModel) {
        QModelIndexList selectedRows = currentTableView->selectionModel()->selectedRows();
        // Sort rows in descending order to avoid issues with index changes upon removal
        std::sort(selectedRows.begin(), selectedRows.end(),
                  [](const QModelIndex &a, const QModelIndex &b) {
                      return a.row() > b.row();
                  });
        for (const QModelIndex &index : selectedRows) {
            // Also remove from internal lists like table_image_item_fullpath if they are synced
            // Example for image tab:
            // if (currentModel == Table_model_image && index.row() < table_image_item_fullpath.size()) {
            //     table_image_item_fullpath.removeAt(index.row());
            //     table_image_item_fileName.removeAt(index.row());
            // }
            currentModel->removeRow(index.row());
        }
        Table_FileCount_reload(); // Update file counts displayed in UI
        TextBrowser_NewMessage(tr("%n item(s) removed.", "", selectedRows.count()));
        return selectedRows.count();
    }
    return 0;
}

void MainWindow::on_pushButton_Report_clicked()
{
    // Open a link to report issues, likely GitHub issues page
    QDesktopServices::openUrl(QUrl("https://github.com/beyawnko/Beya_Waifu/issues"));
}

void MainWindow::on_pushButton_ReadMe_clicked()
{
    // Open the project's README file or a wiki page
    QDesktopServices::openUrl(QUrl("https://github.com/beyawnko/Beya_Waifu/README.md"));
}

void MainWindow::on_pushButton_clear_textbrowser_clicked()
{
    ui->textBrowser->clear();
}

void MainWindow::on_pushButton_HideSettings_clicked()
{
    bool isHidden = ui->groupBox_Setting->isHidden();
    ui->groupBox_Setting->setHidden(!isHidden);
    ui->pushButton_HideSettings->setText(!isHidden ? tr("Show Settings") : tr("Hide Settings"));
}

void MainWindow::on_pushButton_ReadFileList_clicked()
{
    // Load a list of files from a text file (e.g., .txt, .ini)
    QString fileListPath = QFileDialog::getOpenFileName(this, tr("Open File List"), "", tr("Text files (*.txt);;INI files (*.ini);;All files (*.*)"));
    if (!fileListPath.isEmpty()) {
        Table_Read_Saved_Table_Filelist(fileListPath);
    }
}

void MainWindow::on_Ext_image_editingFinished()
{
    // This is called when editing of image extension lineEdit is finished (e.g., focus lost)
    // The actual saving might be better handled by on_Ext_image_textChanged for immediate feedback,
    // but this can serve as a final confirmation or trigger for persistence.
    Settings_Save();
    qDebug() << "Image output extension editing finished. Value:" << ui->Ext_image->text();
}

void MainWindow::on_Ext_video_editingFinished()
{
    Settings_Save();
    qDebug() << "Video output extension editing finished. Value:" << ui->Ext_video->text();
}

void MainWindow::on_checkBox_AutoSaveSettings_clicked()
{
    Settings_Save(); // Save the state of this checkbox itself
    bool isAutoSave = ui->checkBox_AutoSaveSettings->isChecked();
    if (isAutoSave) {
        TextBrowser_NewMessage(tr("Settings will now be saved automatically on change (where implemented)."));
    } else {
        TextBrowser_NewMessage(tr("Automatic settings saving disabled. Remember to save manually."));
    }
}
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

    QList<QUrl> urls;
    for (const QString &path : files) {
        urls << QUrl::fromLocalFile(path);
    }

    // Mirror drag-and-drop behavior to keep UI responsive
    AddNew_gif = false;
    AddNew_image = false;
    AddNew_video = false;
    ui_tableViews_setUpdatesEnabled(false);
    ui->groupBox_Setting->setEnabled(0);
    ui->groupBox_FileList->setEnabled(0);
    ui->groupBox_InputExt->setEnabled(0);
    pushButton_Start_setEnabled_self(0);
    ui->checkBox_ScanSubFolders->setEnabled(0);
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));

    (void)QtConcurrent::run([this, urls]() { this->Read_urls(urls); });
}
/**
 * @brief Open the project's online wiki.
 */
void MainWindow::on_pushButton_wiki_clicked()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/beyawnko/Beya_Waifu")));
}
void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    Settings_Save(); // Save the selected update channel
    // Potentially trigger an immediate check for updates or change how updates are fetched
    qDebug() << "Update channel changed. Index:" << index << "Channel:" << ui->comboBox_UpdateChannel->currentText();
}

// Compatibility Checkbox Implementations
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked()
{
    isCompatible_Waifu2x_NCNN_Vulkan_NEW = ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked()
{
    isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P = ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked()
{
    isCompatible_Waifu2x_NCNN_Vulkan_OLD = ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked()
{
    isCompatible_SRMD_NCNN_Vulkan = ui->checkBox_isCompatible_SRMD_NCNN_Vulkan->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked()
{
    isCompatible_SRMD_CUDA = ui->checkBox_isCompatible_SRMD_CUDA->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked()
{
    isCompatible_Waifu2x_Converter = ui->checkBox_isCompatible_Waifu2x_Converter->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked()
{
    isCompatible_Anime4k_CPU = ui->checkBox_isCompatible_Anime4k_CPU->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked()
{
    isCompatible_Anime4k_GPU = ui->checkBox_isCompatible_Anime4k_GPU->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked()
{
    isCompatible_FFmpeg = ui->checkBox_isCompatible_FFmpeg->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_FFprobe_clicked()
{
    isCompatible_FFprobe = ui->checkBox_isCompatible_FFprobe->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked()
{
    isCompatible_ImageMagick = ui->checkBox_isCompatible_ImageMagick->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked()
{
    isCompatible_Gifsicle = ui->checkBox_isCompatible_Gifsicle->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_SoX_clicked()
{
    isCompatible_SoX = ui->checkBox_isCompatible_SoX->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked()
{
    isCompatible_Waifu2x_Caffe_CPU = ui->checkBox_isCompatible_Waifu2x_Caffe_CPU->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked()
{
    isCompatible_Waifu2x_Caffe_GPU = ui->checkBox_isCompatible_Waifu2x_Caffe_GPU->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked()
{
    isCompatible_Waifu2x_Caffe_cuDNN = ui->checkBox_isCompatible_Waifu2x_Caffe_cuDNN->isChecked();
    Settings_Save();
}
void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked()
{
    isCompatible_Realsr_NCNN_Vulkan = ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->isChecked();
    Settings_Save();
}


void MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked() // This slot name might need to change if the button was renamed in UI
{
    // If pushButton_DetectGPU_RealCUGAN was renamed to pushButton_DetectGPU_VFI,
    // this slot might not be connected anymore or should be renamed too.
    // For now, just call the detection logic.
    Realcugan_ncnn_vulkan_DetectGPU();
}
/** Toggle visibility of the RealCUGAN multi GPU group. */
void MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged(int arg1)
{
    if (groupBox_GPUSettings_MultiGPU_RealCUGAN)
        groupBox_GPUSettings_MultiGPU_RealCUGAN->setVisible(arg1 == Qt::Checked);
    // Re-read settings so processor picks up the change
    if (realCuganProcessor)
        realCuganProcessor->readSettings();
    // TODO: update running jobs when multi GPU state changes
}

/** Add the currently selected GPU to the RealCUGAN GPU list. */
void MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked()
{
    if (!comboBox_GPUIDs_MultiGPU_RealCUGAN || !listWidget_GPUList_MultiGPU_RealCUGAN)
        return;

    const QString id = comboBox_GPUIDs_MultiGPU_RealCUGAN->currentText();
    if (id.isEmpty())
        return;

    for (const auto &gpu : GPUIDs_List_MultiGPU_RealCUGAN) {
        if (gpu.value("id") == id)
            return; // already added
    }

    QMap<QString, QString> map;
    map["id"] = id;
    map["threads"] = "1";
    map["tilesize"] = spinBox_TileSize_RealCUGAN
            ? QString::number(spinBox_TileSize_RealCUGAN->value())
            : QStringLiteral("0");
    map["enabled"] = "true";
    GPUIDs_List_MultiGPU_RealCUGAN.append(map);

    QListWidgetItem *item = new QListWidgetItem(id, listWidget_GPUList_MultiGPU_RealCUGAN);
    item->setData(Qt::UserRole, id);
    listWidget_GPUList_MultiGPU_RealCUGAN->addItem(item);
    // TODO: allow editing threads and tile size per GPU
}

/** Remove the selected GPU from the RealCUGAN GPU list. */
void MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked()
{
    if (!listWidget_GPUList_MultiGPU_RealCUGAN)
        return;

    int row = listWidget_GPUList_MultiGPU_RealCUGAN->currentRow();
    if (row < 0 || row >= listWidget_GPUList_MultiGPU_RealCUGAN->count())
        return;

    QString id = listWidget_GPUList_MultiGPU_RealCUGAN->currentItem()->data(Qt::UserRole).toString();
    listWidget_GPUList_MultiGPU_RealCUGAN->takeItem(row);

    for (int i = 0; i < GPUIDs_List_MultiGPU_RealCUGAN.size(); ++i) {
        if (GPUIDs_List_MultiGPU_RealCUGAN.at(i).value("id") == id) {
            GPUIDs_List_MultiGPU_RealCUGAN.removeAt(i);
            break;
        }
    }
}

/** Clear all GPUs from the RealCUGAN GPU list. */
void MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked()
{
    GPUIDs_List_MultiGPU_RealCUGAN.clear();
    if (listWidget_GPUList_MultiGPU_RealCUGAN)
        listWidget_GPUList_MultiGPU_RealCUGAN->clear();
}

/**
 * @brief Build the job string for multi GPU execution.
 *
 * Uses RealcuganJobManager to convert the stored list to the
 * formatted -g/-j options required by the executable.
 */
QString MainWindow::RealcuganNcnnVulkan_MultiGPU()
{
    RealcuganJobManager mgr;
    QString fallback = m_realcugan_GPUID.split(":").first();
    bool enabled = checkBox_MultiGPU_RealCUGAN && checkBox_MultiGPU_RealCUGAN->isChecked();
    return mgr.buildGpuJobString(enabled,
                                 GPUIDs_List_MultiGPU_RealCUGAN,
                                 fallback);
}

/** Add a GPU ID with thread count to the internal list. */
void MainWindow::AddGPU_MultiGPU_RealcuganNcnnVulkan(const QString &gpuid, int threads)
{
    for (const auto &gpu : GPUIDs_List_MultiGPU_RealCUGAN) {
        if (gpu.value("ID") == gpuid)
            return;
    }
    QMap<QString, QString> map;
    map["ID"] = gpuid;
    map["Threads"] = QString::number(threads);
    GPUIDs_List_MultiGPU_RealCUGAN.append(map);
}

/** Remove a GPU ID from the internal list. */
void MainWindow::RemoveGPU_MultiGPU_RealcuganNcnnVulkan(const QString &gpuid)
{
    for (int i = 0; i < GPUIDs_List_MultiGPU_RealCUGAN.size(); ++i) {
        if (GPUIDs_List_MultiGPU_RealCUGAN.at(i).value("ID") == gpuid) {
            GPUIDs_List_MultiGPU_RealCUGAN.removeAt(i);
            break;
        }
    }
}

/** Increase the RealCUGAN tile size by the spin box step. */
void MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked()
{
    if (spinBox_TileSize_RealCUGAN)
        spinBox_TileSize_RealCUGAN->setValue(spinBox_TileSize_RealCUGAN->value() +
                                             spinBox_TileSize_RealCUGAN->singleStep());
}

/** Decrease the RealCUGAN tile size by the spin box step. */
void MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked()
{
    if (!spinBox_TileSize_RealCUGAN)
        return;
    int step = spinBox_TileSize_RealCUGAN->singleStep();
    int newVal = spinBox_TileSize_RealCUGAN->value() - step;
    if (newVal < spinBox_TileSize_RealCUGAN->minimum())
        newVal = spinBox_TileSize_RealCUGAN->minimum();
    spinBox_TileSize_RealCUGAN->setValue(newVal);
}

/** Store the currently selected RealCUGAN model. */
void MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged(int index)
{
    if (comboBox_Model_RealCUGAN)
        m_realcugan_Model = comboBox_Model_RealCUGAN->itemText(index);
    // TODO: refresh processor settings when model changes
}

/**
 * @brief Handle completion of an iterative RealCUGAN process.
 *
 * Refresh settings when the process exits normally and notify the user on
 * failure. Afterwards the overall status is updated.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_finished(int exitCode,
                                                          QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        if (realCuganProcessor)
            realCuganProcessor->readSettings();
        m_FinishedProc++;
    } else {
        m_ErrorProc++;
        ShellMessageBox(tr("RealCUGAN Error"),
                        tr("RealCUGAN exited with code %1").arg(exitCode),
                        QMessageBox::Critical);
    }
    CheckIfAllFinished();
}

/**
 * @brief Read stdout from iterative RealCUGAN execution and update progress.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput()
{
    if (!currentProcess)
        return;
    QString output = QString::fromLocal8Bit(currentProcess->readAllStandardOutput());
    QRegularExpression re(QStringLiteral("(\\d+)%")); // This line should already be correct from the previous subtask
    QRegularExpressionMatch m = re.match(output);
    if (m.hasMatch()) {
        int percent = m.captured(1).toInt();
        Set_Current_File_Progress_Bar_Value(percent, 100);
    }
}

/**
 * @brief Read stderr from iterative RealCUGAN execution.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError()
{
    if (!currentProcess)
        return;
    qWarning().noquote() << currentProcess->readAllStandardError();
}

/**
 * @brief Handle errors from iterative RealCUGAN execution.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error)
{
    Q_UNUSED(error); // Add this line
    m_ErrorProc++;
    ShellMessageBox(tr("RealCUGAN Error"), currentProcess ? currentProcess->errorString() : QString(),
                    QMessageBox::Critical);
    CheckIfAllFinished();
}
void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()
{
    RealESRGAN_ncnn_vulkan_DetectGPU();
}
void MainWindow::on_comboBox_Model_RealsrNCNNVulkan_currentIndexChanged(int index) { Q_UNUSED(index); Settings_Save(); qDebug() << "RealESRGAN Model changed:" << ui->comboBox_Model_RealsrNCNNVulkan->currentText(); }
void MainWindow::on_pushButton_Add_TileSize_RealsrNCNNVulkan_clicked() { ui->spinBox_TileSize_RealsrNCNNVulkan->setValue(ui->spinBox_TileSize_RealsrNCNNVulkan->value() + 64); Settings_Save(); }
void MainWindow::on_pushButton_Minus_TileSize_RealsrNCNNVulkan_clicked() { int newVal = ui->spinBox_TileSize_RealsrNCNNVulkan->value() - 64; if (newVal < 0) newVal = 0; ui->spinBox_TileSize_RealsrNCNNVulkan->setValue(newVal); Settings_Save(); }
void MainWindow::on_checkBox_MultiGPU_RealesrganNcnnVulkan_stateChanged(int arg1) { Q_UNUSED(arg1); ui->groupBox_GPUSettings_MultiGPU_RealesrganNcnnVulkan->setVisible(arg1 == Qt::Checked); Settings_Save(); }
void MainWindow::on_comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan_currentIndexChanged(int index) { Q_UNUSED(index); RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay(); Settings_Save(); }
void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan_clicked() { Settings_Save(); } // Logic might be in RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay or when building args
void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan_valueChanged(int arg1) { Q_UNUSED(arg1); Settings_Save(); }
void MainWindow::on_pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan_clicked() { /* This might toggle a more detailed dialog or section, not implemented here */ qDebug() << "Show MultiGPU Settings for RealESRGAN clicked."; }

void MainWindow::RealESRGAN_NCNN_Vulkan_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_finished: exitCode" << exitCode << "exitStatus" << exitStatus;
    QString statusMessage;
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);

    if (success) {
        m_FinishedProc++;
        statusMessage = tr("Finished");
        // Potentially update table row with new file size, output path, etc.
    } else {
        m_ErrorProc++;
        statusMessage = tr("Error");
        ShellMessageBox(tr("RealESRGAN Error"),
                        tr("RealESRGAN process failed. Exit code: %1, Status: %2")
                        .arg(exitCode).arg(exitStatus == QProcess::NormalExit ? "Normal Exit" : "Crash Exit"),
                        QMessageBox::Critical);
    }

    // Update table status for the current file (assuming current_File_Row_Number is set)
    // This needs to know which table (image, gif, video) is active.
    // For simplicity, assuming a generic way or it's handled by a more abstract system.
    if (current_File_Row_Number != -1) {
        // Example: Table_image_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, statusMessage);
        // This needs to be adapted based on which table view/model is actually being processed by RealESRGAN
        qDebug() << "Updating table status for row (if applicable):" << current_File_Row_Number << "to" << statusMessage;
    }

    UpdateProgressBar();
    CheckIfAllFinished(); // Check if all files in batch are done
    ProcessNextFile();    // Attempt to process next file in queue
}

void MainWindow::RealESRGAN_NCNN_Vulkan_errorOccurred(QProcess::ProcessError error)
{
    qWarning() << "RealESRGAN_NCNN_Vulkan_errorOccurred: error" << error;
    if (error == QProcess::FailedToStart || error == QProcess::Crashed || error == QProcess::Timedout) {
        m_ErrorProc++;
        ShellMessageBox(tr("RealESRGAN Process Error"),
                        currentProcess ? currentProcess->errorString() : tr("Unknown process error."),
                        QMessageBox::Critical);
        // Update table status
        if (current_File_Row_Number != -1) {
             // Example: Table_image_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, tr("Error"));
             qDebug() << "Updating table status for row (if applicable):" << current_File_Row_Number << "to Error";
        }
        UpdateProgressBar();
        CheckIfAllFinished();
        ProcessNextFile();
    }
    // Other errors like ReadError might not mean the process itself failed yet.
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_Iterative_finished: exitCode" << exitCode << "exitStatus" << exitStatus;
    // Similar to RealESRGAN_NCNN_Vulkan_finished, but specifically for iterative/multi-pass
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    QString statusMessage = success ? tr("Finished (Iterative)") : tr("Error (Iterative)");

    if (success) {
        m_FinishedProc++;
    } else {
        m_ErrorProc++;
        ShellMessageBox(tr("RealESRGAN Iterative Error"),
                        tr("RealESRGAN iterative process failed. Exit code: %1, Status: %2")
                        .arg(exitCode).arg(exitStatus == QProcess::NormalExit ? "Normal Exit" : "Crash Exit"),
                        QMessageBox::Critical);
    }
    // Update table status
    if (current_File_Row_Number != -1) {
        // Example: Table_image_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, statusMessage);
        qDebug() << "Updating table status for iterative process, row:" << current_File_Row_Number << "to" << statusMessage;
    }
    UpdateProgressBar();
    CheckIfAllFinished();
    ProcessNextFile();
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardOutput()
{
    if (!currentProcess) return;
    QString output = QString::fromLocal8Bit(currentProcess->readAllStandardOutput());
    qDebug() << "RealESRGAN Iterative StdOut:" << output;
    // Assuming progress is reported as "X%"
    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatchIterator i = re.globalMatch(output);
    QString lastProgress;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        lastProgress = match.captured(1);
    }
    if (!lastProgress.isEmpty()) {
        int percent = lastProgress.toInt();
        Set_Current_File_Progress_Bar_Value(percent, 100);
    }
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardError()
{
    if (!currentProcess) return;
    QString errorOutput = QString::fromLocal8Bit(currentProcess->readAllStandardError());
    qWarning() << "RealESRGAN Iterative StdErr:" << errorOutput;
    // Can also parse for specific error messages if needed
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error)
{
    qWarning() << "RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred: error" << error;
    if (error == QProcess::FailedToStart || error == QProcess::Crashed || error == QProcess::Timedout) {
        m_ErrorProc++;
        ShellMessageBox(tr("RealESRGAN Iterative Process Error"),
                        currentProcess ? currentProcess->errorString() : tr("Unknown process error."),
                        QMessageBox::Critical);
        if (current_File_Row_Number != -1) {
            // Example: Table_image_ChangeStatus_rowNumInt_statusQString(current_File_Row_Number, tr("Error (Iterative)"));
             qDebug() << "Updating table status for iterative process error, row:" << current_File_Row_Number << "to Error";
        }
        UpdateProgressBar();
        CheckIfAllFinished();
        ProcessNextFile();
    }
}

// These might be connected directly from a QProcess object if used outside the main iterative logic
void MainWindow::onRealESRGANProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "onRealESRGANProcessFinished (wrapper) called.";
    RealESRGAN_NCNN_Vulkan_finished(exitCode, exitStatus); // Delegate to the main handler
}

void MainWindow::onRealESRGANProcessError(QProcess::ProcessError error)
{
    qDebug() << "onRealESRGANProcessError (wrapper) called.";
    RealESRGAN_NCNN_Vulkan_errorOccurred(error); // Delegate to the main handler
}

void MainWindow::onRealESRGANProcessStdOut()
{
    qDebug() << "onRealESRGANProcessStdOut (wrapper) called.";
    RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardOutput(); // Delegate
}

void MainWindow::onRealESRGANProcessStdErr()
{
    qDebug() << "onRealESRGANProcessStdErr (wrapper) called.";
    RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardError(); // Delegate
}



// Stubs for slots listed in mainwindow.h but not found/causing errors previously
// These were in the error report section of mainwindow.h, so they are expected by MOC

// Private slots

// Helper function stubs (if any were missed and are private)
void MainWindow::LoadScaledImageToLabel(const QString &imagePath, QLabel *label)
{
    if (!label) return;
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        label->setText(tr("Preview not available"));
        return;
    }
    label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::UpdateTotalProcessedFilesCount()
{
    // Assuming m_TotalNumProc holds the total number of files initially queued.
    // m_FinishedProc and m_ErrorProc are incremented as files complete.
    int processedCount = m_FinishedProc + m_ErrorProc;
    if (ui->label_progressBar_filenum) { // Check if the UI element exists
        ui->label_progressBar_filenum->setText(QString("%1/%2").arg(processedCount).arg(m_TotalNumProc));
    }
    UpdateProgressBar(); // Also update progress bar as it's related
}

void MainWindow::ProcessNextFile()
{
    // This is a simplified conceptual implementation. The actual logic
    // for queuing and starting next file depends heavily on how Waifu2xMainThread()
    // and related functions manage the queue and threading.

    qDebug() << "ProcessNextFile called. Active threads:" << ThreadNumRunning << "Max threads:" << ThreadNumMax;

    if (ThreadNumRunning < ThreadNumMax) { // Check if a thread is available
        // This is where you'd typically fetch the next file from your queue
        // (e.g., from table_image_item_fullpath or a dedicated QList/Queue)
        // and then call the appropriate processing function (like Waifu2x_NCNN_Vulkan_Image, etc.)
        // For this stub, we'll just log.

        // Example:
        // if (!fileQueue.isEmpty()) {
        //     QString nextFile = fileQueue.dequeue();
        //     int nextFileRow = findRowForFile(nextFile); // Find its row in the table
        //     current_File_Row_Number = nextFileRow; // Set current file context
        //     Processing_Status = determineProcessingType(nextFile); // Image, GIF, Video
        //
        //     ThreadNumRunning++;
        //     UpdateNumberOfActiveThreads();
        //
        //     if (Processing_Status == PROCESS_TYPE_IMAGE) {
        //         Waifu2x_NCNN_Vulkan_Image(nextFileRow, false); // Or appropriate engine call
        //     } // ... else if for GIF, Video ...
        // } else {
        //     qDebug() << "ProcessNextFile: No more files in queue or queue not managed here.";
        //     CheckIfAllFinished(); // No more files, check if everything is done
        // }
    } else {
        qDebug() << "ProcessNextFile: No available threads to process next file.";
    }
    // If ProcessNextFile is called after each file completion, and no explicit queue management outside
    // Waifu2xMainThread, then Waifu2xMainThread itself might be the loop that calls ProcessNextFile.
    // Or, Waifu2xMainThread starts N threads, and each thread, upon finishing, calls ProcessNextFile.
}

void MainWindow::CheckIfAllFinished()
{
    qDebug() << "CheckIfAllFinished: Started:" << m_StartedProc << "Finished:" << m_FinishedProc << "Errors:" << m_ErrorProc << "Total in UI:" << m_TotalNumProc;
    // m_TotalNumProc should be set when files are initially added.
    // m_StartedProc might not be the right variable if processing happens in parallel batches.
    // A more robust check might be if (m_FinishedProc + m_ErrorProc >= m_TotalNumProc)
    // and ThreadNumRunning == 0 (if threads are managed globally)

    if ((m_FinishedProc + m_ErrorProc) >= m_TotalNumProc && m_TotalNumProc > 0) { // Ensure m_TotalNumProc is set
        qDebug() << "All files processed or attempted.";
        if (isProcessing) { // Check if it was processing something
             isProcessing = false; // Mark processing as globally finished
             // This will call the Send_Waifu2x_Finished signal if connected as in constructor
             // emit Send_Waifu2x_Finished(); // Or call Waifu2x_Finished directly
             Waifu2x_Finished(); // Call directly for immediate UI updates
        }
        // Reset counters for a potential new batch, or do this when Start is clicked again.
        // m_StartedProc = 0;
        // m_FinishedProc = 0;
        // m_ErrorProc = 0;
        // m_TotalNumProc = 0; // Reset this carefully, perhaps on "Clear List" or new "Start"
    } else {
        qDebug() << "CheckIfAllFinished: Still files pending or m_TotalNumProc not correctly set.";
    }
}

void MainWindow::UpdateNumberOfActiveThreads()
{
    // if (ui->label_NumThreadsActive) { // Check if the UI element exists
    //     ui->label_NumThreadsActive->setText(QString::number(ThreadNumRunning.load())); // ThreadNumRunning is std::atomic
    // }
    // TODO: label_NumThreadsActive not found in UI. This info might need a new display element.
}

void MainWindow::UpdateProgressBar()
{
    if (m_TotalNumProc > 0) {
        int processedCount = m_FinishedProc + m_ErrorProc;
        int percentage = (int)(((double)processedCount / m_TotalNumProc) * 100.0);
        if (ui->progressBar) { // Check if the UI element exists
            ui->progressBar->setValue(percentage);
            ui->progressBar->setFormat(tr("Overall Progress: %p% (%1/%2)")
                                     .arg(processedCount)
                                     .arg(m_TotalNumProc));
        }
    } else {
        if (ui->progressBar) {
            ui->progressBar->setValue(0);
            ui->progressBar->setFormat(tr("Waiting for tasks..."));
        }
    }
}

// Definitions for functions declared in mainwindow.h, previously causing linker errors
QString MainWindow::Generate_Output_Path(const QString& original_fileName_WithExt, const QString& suffix_NoPeriod)
{
    QFileInfo originalFileInfo(original_fileName_WithExt);
    QString baseName = originalFileInfo.completeBaseName(); // Filename without extension
    QString originalExt = originalFileInfo.suffix(); // Original extension

    QString outputDirToUse = OutPutFolder_main; // User-defined output folder
    if (outputDirToUse.isEmpty() || !QDir(outputDirToUse).exists()) {
        // Fallback to a default if not set or invalid: e.g., subdirectory in app path or source file path
        outputDirToUse = originalFileInfo.absolutePath() + "/output_waifu2x"; // Example: in a subfolder next to original
        QDir().mkpath(outputDirToUse); // Ensure it exists
    }

    QString newBaseName = baseName;
    if (!suffix_NoPeriod.isEmpty()) {
        newBaseName += "_" + suffix_NoPeriod;
    }

    // Determine output extension - could be from UI settings (e.g. ui->comboBox_ImageSaveFormat)
    // For now, let's assume we try to keep original extension or a default like PNG for images.
    QString outputExt = originalExt;
    // Example: if (Processing_Status == PROCESS_TYPE_IMAGE && ui->comboBox_ImageSaveFormat->currentText() != "Keep Original") {
    //    outputExt = ui->comboBox_ImageSaveFormat->currentText().toLower();
    // }

    return QDir(outputDirToUse).filePath(newBaseName + "." + outputExt);
}

/**
 * @brief Update the global progress bar.
 */
void MainWindow::Set_Progress_Bar_Value(int val, int max_val)
{
    if (ui->progressBar) {
        ui->progressBar->setRange(0, max_val);
        ui->progressBar->setValue(val);
    }
}

/**
 * @brief Update the progress bar for the currently processed file.
 */
void MainWindow::Set_Current_File_Progress_Bar_Value(int val, int max_val)
{
    if (ui->progressBar_CurrentFile) {
        ui->progressBar_CurrentFile->setRange(0, max_val);
        ui->progressBar_CurrentFile->setValue(val);
    }
}

//======================== Metadata Cache Implementation ================================
FileMetadataCache MainWindow::getOrFetchMetadata(const QString &filePath)
{
    // Attempt to read from cache first (thread-safe due to mutex)
    {
        QMutexLocker locker(&m_metadataCacheMutex);
        if (m_metadataCache.contains(filePath)) {
            return m_metadataCache.value(filePath);
        }
    } // Mutex is released here

    FileMetadataCache metadata;
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        qWarning() << "getOrFetchMetadata: File does not exist or is not a file:" << filePath;
        return metadata; // Return default (invalid) metadata
    }

    metadata.fileFormat = fileInfo.suffix().toLower();

    // Handle GIFs
    if (metadata.fileFormat == "gif") {
        QMovie movie(filePath);
        if (movie.isValid()) {
            metadata.width = movie.frameRect().width();
            metadata.height = movie.frameRect().height();
            metadata.frameCount = movie.frameCount();
            metadata.isAnimated = movie.frameCount() > 1;
            metadata.isValid = true;
            qDebug() << "getOrFetchMetadata: GIF metadata read for" << filePath << "W:" << metadata.width << "H:" << metadata.height << "Frames:" << metadata.frameCount;
        } else {
            qWarning() << "getOrFetchMetadata: QMovie could not read GIF:" << filePath;
        }
    }
    // Handle common image formats
    else if (QStringList({"png", "jpg", "jpeg", "bmp", "webp", "tif", "tiff"})
                   .contains(metadata.fileFormat)) {
        QImageReader reader(filePath);
        if (reader.canRead()) {
            QSize dimensions = reader.size();
            metadata.width = dimensions.width();
            metadata.height = dimensions.height();
            metadata.isValid = true;
            metadata.frameCount = 1; // Static images have 1 frame
            metadata.isAnimated = false;
            qDebug() << "getOrFetchMetadata: Image metadata read for" << filePath << "W:" << metadata.width << "H:" << metadata.height;
        } else {
            qWarning() << "getOrFetchMetadata: QImageReader could not read image:" << filePath << "Error:" << reader.errorString();
        }
    }
    // Handle video formats (basic metadata using QMediaPlayer)
    else {
        QMediaPlayer player;
        QEventLoop loop;
        bool metadataSuccessfullyLoaded = false; // Flag to ensure loop quits correctly

        // Connect signals to handle metadata changes or errors
        connect(&player, &QMediaPlayer::mediaStatusChanged, [&](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia) {
                QMediaMetaData md = player.metaData();
                if (!md.isEmpty()) {
                    QVariant resolutionVariant = md.value(QMediaMetaData::Resolution);
                    if (resolutionVariant.isValid()) {
                        QSize res = resolutionVariant.toSize();
                        metadata.width = res.width();
                        metadata.height = res.height();
                    }
                    metadata.duration = player.duration(); // in milliseconds
                    QVariant frameRateVariant = md.value(QMediaMetaData::VideoFrameRate);
                    if (frameRateVariant.isValid()) {
                        metadata.fps = QString::number(frameRateVariant.toDouble(), 'f', 2); // Format to 2 decimal places
                    }

                    // Basic validation: width, height, and duration must be positive
                    if (metadata.width > 0 && metadata.height > 0 && metadata.duration > 0) {
                        metadata.isValid = true;
                        qDebug() << "getOrFetchMetadata: Video metadata read for" << filePath
                                 << "W:" << metadata.width << "H:" << metadata.height
                                 << "Duration:" << metadata.duration << "FPS:" << metadata.fps;
                    } else {
                        qWarning() << "getOrFetchMetadata: Video metadata incomplete for" << filePath
                                   << "W:" << metadata.width << "H:" << metadata.height
                                   << "Duration:" << metadata.duration;
                    }
                    metadataSuccessfullyLoaded = true;
                    loop.quit();
                } else {
                     qWarning() << "getOrFetchMetadata: LoadedMedia but no metadata available for" << filePath;
                     metadataSuccessfullyLoaded = true; // Still quit loop
                     loop.quit();
                }
            } else if (status == QMediaPlayer::InvalidMedia || status == QMediaPlayer::NoMedia ||
                       status == QMediaPlayer::EndOfMedia /* EndOfMedia might be too early */) {
                qWarning() << "getOrFetchMetadata: Media status issue for" << filePath << "Status:" << status;
                metadataSuccessfullyLoaded = true; // Ensure loop quits on error or invalid status
                loop.quit();
            }
        });

        connect(&player, &QMediaPlayer::errorOccurred,
            [&](QMediaPlayer::Error error, const QString &errorString) {
            qWarning() << "getOrFetchMetadata: QMediaPlayer error for" << filePath << "Error:" << error << errorString;
            metadataSuccessfullyLoaded = true; // Ensure loop quits on error
            loop.quit();
        });

        player.setSource(QUrl::fromLocalFile(filePath));

        // Timeout for metadata loading to prevent indefinite blocking
        QTimer::singleShot(10000, &loop, [&]() { // Increased timeout to 10s
            if (!metadataSuccessfullyLoaded) { // Check if not already quit by success/error
                qWarning() << "getOrFetchMetadata: Timeout waiting for metadata for" << filePath;
                loop.quit();
            }
        });
        loop.exec(); // Start event loop

        // One final check if metadata became available right when timeout happened or loop exited for other reasons
        QMediaMetaData md = player.metaData();
        if (!metadata.isValid && !md.isEmpty()) {
            QVariant resolutionVariant = md.value(QMediaMetaData::Resolution);
            if (resolutionVariant.isValid()) {
                QSize res = resolutionVariant.toSize();
                metadata.width = res.width();
                metadata.height = res.height();
            }
            metadata.duration = player.duration();
            QVariant frameRateVariant = md.value(QMediaMetaData::VideoFrameRate);
            if (frameRateVariant.isValid()) {
                metadata.fps = QString::number(frameRateVariant.toDouble(), 'f', 2);
            }
            if (metadata.width > 0 && metadata.height > 0 && metadata.duration > 0) {
                metadata.isValid = true;
                qDebug() << "getOrFetchMetadata: Video metadata (final check) read for" << filePath;
            }
        }
    }

    // Cache the retrieved metadata if it's valid
    if (metadata.isValid) {
        QMutexLocker locker(&m_metadataCacheMutex); // Lock again to write to cache
        m_metadataCache.insert(filePath, metadata);
    } else {
        qWarning() << "getOrFetchMetadata: Failed to retrieve valid metadata for" << filePath << ". Format:" << metadata.fileFormat;
    }

    return metadata;
}


bool MainWindow::Realcugan_ProcessSingleFileIteratively(const QString &inputFile,
                                                        const QString &outputFile,
                                                        int targetScale,
                                                        int /*originalWidth*/,
                                                        int /*originalHeight*/,
                                                        const QString &modelName,
                                                        int denoiseLevel,
                                                        int tileSize,
                                                        const QString &gpuIdOrJobConfig,
                                                        bool isMultiGPUJob,
                                                        bool ttaEnabled,
                                                        const QString &outputFormat,
                                                        bool experimental,
                                                        int rowNumForStatusUpdate) // Keep rowNumForStatusUpdate if used by status updates
{
    Q_UNUSED(rowNumForStatusUpdate); // Add this line
    // Ensure RealCUGAN settings are up-to-date for the current context
    // This call might be redundant if settings are guaranteed to be fresh before starting iterative processing.
    // However, it's safer to ensure they reflect the latest UI/state if this function can be called
    // in different contexts or after potential UI changes without a full settings reload.
    if (realCuganProcessor) { // Check if processor exists
        realCuganProcessor->readSettings(); // This will update m_realcugan_Model, m_realcugan_DenoiseLevel etc. from UI/QSettings
    } else {
        qWarning() << "RealCugan_ProcessSingleFileIteratively: realCuganProcessor is null!";
        return false;
    }

    // Determine jobs, syncgap, verbose for this specific call.
    // These could be specific to the type of content if known (image, video frame, gif frame)
    // For generic iterative processing, using generic keys or fallback to current m_realcugan_ members.
    // Using general settings keys for now.
    QString jobsStr = Settings_Read_value("/settings/RealCUGANJobsGeneric",
                                         QString("1:1:1")).toString();
    QString syncGapStr = Settings_Read_value("/settings/RealCUGANSyncGapGeneric",
                                          QString("3")).toString();
    // verboseLog could also be a member like m_realcugan_verboseLog if it's a global setting for RealCUGAN
    bool verboseLog = Settings_Read_value("/settings/RealCUGANVerboseLog",
                                         QVariant(false)).toBool();


    if (!realCuganProcessor)
    {
        qWarning() << "RealCUGAN processor not available";
        return false;
    }

    QString exePath = realCuganProcessor->executablePath(experimental);
    QStringList args = realCuganProcessor->prepareArguments(inputFile,
                                                           outputFile,
                                                           targetScale,
                                                           modelName,
                                                           denoiseLevel,
                                                           tileSize,
                                                           gpuIdOrJobConfig, // This is actual GPU ID string or multiGPU job string
                                                           ttaEnabled,
                                                           outputFormat,
                                                           isMultiGPUJob,    // Indicates if gpuIdOrJobConfig is a multi-GPU setup
                                                           gpuIdOrJobConfig, // Passed as multiGpuJobArgs
                                                           experimental,
                                                           jobsStr,
                                                           syncGapStr,
                                                           verboseLog);

    QProcess proc;
    connect(&proc,
            &QProcess::readyReadStandardOutput,
            this,
            &MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput);
    connect(&proc,
            &QProcess::readyReadStandardError,
            this,
            &MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError);

    bool ok = runProcess(&proc, QStringLiteral("\"%1\" %2").arg(exePath, args.join(' ')));
    if (!ok)
    {
        qWarning() << "RealCUGAN failed for" << inputFile;
    }
    return ok;
}

void MainWindow::Play_NFSound()
{
    qDebug() << "STUB: MainWindow::Play_NFSound() called.";
    // TODO: Implement actual sound playback logic here
    // Example using QMediaPlayer (ensure QMediaPlayer is included and a member `m_player` exists and is initialized):
    // if (Settings_Read_value("/settings/EnableSoundNotification", true).toBool()) {
    //     if (!m_player) {
    //         m_player = new QMediaPlayer(this);
    //     }
    //     m_player->setSource(QUrl("qrc:/sounds/NFSound_Waifu2xEX.mp3")); // Ensure this path is correct in your .qrc
    //     m_player->play();
    // }
}

QString MainWindow::Seconds2hms(long unsigned int seconds)
{
    long unsigned int h = seconds / 3600;
    long unsigned int m = (seconds % 3600) / 60;
    long unsigned int s = seconds % 60;
    return QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}

void MainWindow::Finish_progressBar_CompatibilityTest()
{
    qDebug() << "STUB: MainWindow::Finish_progressBar_CompatibilityTest() called.";
    if (ui->progressBar_CompatibilityTest) { // Check if UI element exists
        ui->progressBar_CompatibilityTest->setValue(ui->progressBar_CompatibilityTest->maximum());
        ui->progressBar_CompatibilityTest->setFormat(tr("Compatibility Test Finished"));
    }
    // Potentially enable the test button again
    // if (ui->pushButton_compatibilityTest) {
    //     ui->pushButton_compatibilityTest->setEnabled(true);
    //     ui->pushButton_compatibilityTest->setText(tr("Compatibility Test"));
    // }
}

void MainWindow::TurnOffScreen()
{
    qDebug() << "STUB: MainWindow::TurnOffScreen() called.";
#ifdef Q_OS_WIN
    // Simulate turning off the screen by sending SC_MONITORPOWER command
    // HWND_BROADCAST can be problematic. Prefer getting the top-level window handle.
    // SendMessage(GetConsoleWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2); // 2 = off, 1 = low power, -1 = on
    // For a GUI app, you might need to find the main window's handle or use a different approach.
    // This is a placeholder, proper implementation for screen off is platform-specific and complex.
    qDebug() << "Screen off functionality is platform-specific and not fully implemented in this stub.";
#else
    qDebug() << "Screen off functionality not implemented for this OS in this stub.";
#endif
}
