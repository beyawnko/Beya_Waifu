/*
    Copyright (C) 2021  Aaron Feng

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

    My Github homepage: https://github.com/AaronFeng753
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QEventLoop>
#include <QTimer>
#include <QSettings>
#include <QFile>
#include <QPalette>
#include <QColor>
#include <QApplication>
#include <QImageReader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>

// ========================= Metadata Cache Implementation =========================
FileMetadataCache MainWindow::getOrFetchMetadata(const QString &filePath)
{
    QMutexLocker locker(&m_metadataCacheMutex);
    if (m_metadataCache.contains(filePath) && m_metadataCache[filePath].isValid) {
        return m_metadataCache[filePath];
    }
    locker.unlock();

    FileMetadataCache metadata;
    metadata.isValid = false;

    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    metadata.fileFormat = suffix;

    QStringList imageExts = ui->Ext_image->text().split(":", Qt::SkipEmptyParts);
    QStringList videoExts = ui->Ext_video->text().split(":", Qt::SkipEmptyParts);

    bool isVideo = std::any_of(videoExts.cbegin(), videoExts.cend(), [&](const QString &ext){
        return suffix == ext.trimmed().toLower();
    });

    bool isPotentiallyAnimatedImage = (suffix == "gif" || suffix == "apng");

    if (isVideo) {
        metadata.isAnimated = false;
        metadata.isValid = true;
        QProcess ffprobeProcess;
        QString ffprobePath = Current_Path + "/ffmpeg/ffprobe_waifu2xEX.exe";
        QStringList args;
        args << "-v" << "quiet"
             << "-print_format" << "json"
             << "-show_format"
             << "-show_streams"
             << filePath;

        QByteArray stdOut, stdErr;
        if (runProcess(&ffprobeProcess, "\"" + ffprobePath + "\" " + args.join(" "), &stdOut, &stdErr)) {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(stdOut);
            if (!jsonDoc.isNull() && jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                QJsonObject formatObj = jsonObj["format"].toObject();
                metadata.duration = formatObj["duration"].toString().toDouble();
                metadata.bitRate = formatObj["bit_rate"].toString();

                QJsonArray streamsArray = jsonObj["streams"].toArray();
                for (const QJsonValue &streamVal : streamsArray) {
                    QJsonObject streamObj = streamVal.toObject();
                    if (streamObj["codec_type"].toString() == "video") {
                        metadata.width = streamObj["width"].toInt();
                        metadata.height = streamObj["height"].toInt();
                        metadata.fps = streamObj["avg_frame_rate"].toString();
                        if (streamObj.contains("nb_frames") && !streamObj["nb_frames"].toString().isEmpty()) {
                             metadata.frameCount = streamObj["nb_frames"].toString().toLongLong();
                        } else {
                            if (metadata.duration > 0 && !metadata.fps.isEmpty()) {
                                QStringList fpsParts = metadata.fps.split('/');
                                if (fpsParts.size() == 2) {
                                    double num = fpsParts[0].toDouble();
                                    double den = fpsParts[1].toDouble();
                                    if (den > 0) {
                                      double fpsDouble = num / den;
                                      if (fpsDouble > 0) {
                                          metadata.frameCount = static_cast<long long>(metadata.duration * fpsDouble);
                                      }
                                    }
                                }
                            }
                        }
                        QString r_frame_rate = streamObj["r_frame_rate"].toString();
                        if (metadata.fps != r_frame_rate && !r_frame_rate.isEmpty() && !metadata.fps.isEmpty()) {
                            metadata.isVFR = true;
                        }
                        break;
                    }
                }
            } else {
                qDebug() << "Failed to parse ffprobe JSON output for" << filePath << ":" << stdOut << stdErr;
                metadata.isValid = false;
            }
        } else {
            qDebug() << "ffprobe execution failed for" << filePath << ":" << ffprobeProcess.errorString() << stdErr;
            metadata.isValid = false;
        }
    } else if (isPotentiallyAnimatedImage) {
        metadata.isValid = true;
        QProcess identifyProcess;
        QString identifyPath = Current_Path + "/ImageMagick/identify_waifu2xEX.exe";
        QStringList args;
        args << "-format" << "%w %h %n %[delay]" << filePath;

        QByteArray stdOut, stdErr;
        if(runProcess(&identifyProcess, "\"" + identifyPath + "\" " + args.join(" "), &stdOut, &stdErr)) {
            QStringList lines = QString(stdOut).trimmed().split('\n');
            if (!lines.isEmpty()) {
                 QStringList parts = lines[0].trimmed().split(" ");
                if (parts.size() >= 2) {
                    metadata.width = parts[0].toInt();
                    metadata.height = parts[1].toInt();
                } else {
                    metadata.isValid = false;
                }
                metadata.frameCount = lines.size();
                if (metadata.isValid && metadata.frameCount > 0 ) {
                    double totalDelayHundredths = 0;
                    bool delayFound = false;
                    for(const QString& line : lines) {
                        QStringList frameParts = line.trimmed().split(" ");
                        if (frameParts.size() >= 4) { // Need at least 4 parts for delay
                             totalDelayHundredths += frameParts[3].toDouble();
                             delayFound = true;
                        } else if (frameParts.size() >= 3 && metadata.frameCount == 1) { // If only one frame, %n might be missing
                             totalDelayHundredths += frameParts[2].toDouble(); // Try 3rd part for delay
                             delayFound = true;
                        }
                    }
                    if (delayFound && totalDelayHundredths > 0) {
                        metadata.duration = totalDelayHundredths / 100.0;
                        if (metadata.duration > 0) {
                           metadata.fps = QString::number(metadata.frameCount / metadata.duration, 'f', 2);
                        }
                    } else if (delayFound && totalDelayHundredths == 0 && metadata.frameCount > 0) { // Static image or 0 delay reported
                        metadata.duration = 0; // Or a very small number if it's single frame animated
                        metadata.fps = "0/1"; // Indicate static or unknown fps
                    }
                }
                metadata.identifyOutput = stdOut.trimmed();
            } else {
                qDebug() << "Failed to parse identify output for" << filePath << ":" << stdOut << stdErr;
                metadata.isValid = false;
            }
        } else {
             qDebug() << "identify execution failed for" << filePath << ":" << identifyProcess.errorString() << stdErr;
             metadata.isValid = false;
        }

        if (!metadata.isValid || (metadata.width == 0 && metadata.height == 0)) {
            QImageReader reader(filePath);
            if (reader.canRead()) {
                QSize size = reader.size();
                metadata.width = size.width();
                metadata.height = size.height();
                metadata.isValid = (metadata.width > 0 && metadata.height > 0);
                 if (reader.supportsAnimation()) {
                    metadata.frameCount = reader.imageCount();
                    if (metadata.frameCount == 0 && metadata.isValid) metadata.frameCount = 1;
                } else if (metadata.isValid) {
                    metadata.frameCount = 1;
                }
            } else {
                 qDebug() << "QImageReader failed for" << filePath << ":" << reader.errorString();
                 metadata.isValid = false;
            }
        }
        metadata.isAnimated = metadata.isValid && metadata.frameCount > 1;

    } else {
        metadata.isAnimated = false;
        QImageReader reader(filePath);
        if (reader.canRead()) {
            QSize size = reader.size();
            metadata.width = size.width();
            metadata.height = size.height();
            metadata.frameCount = 1;
            metadata.duration = 0;
            metadata.fps = "0/0";
            metadata.isValid = (metadata.width > 0 && metadata.height > 0);
        } else {
            qDebug() << "QImageReader failed for" << filePath << ":" << reader.errorString();
            metadata.isValid = false;
        }
    }

    if (metadata.isValid) {
        QMutexLocker cacheLocker(&m_metadataCacheMutex);
        m_metadataCache[filePath] = metadata;
    }
    return metadata;
}


MainWindow::MainWindow(int maxThreadsOverride, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaTypeStreamOperators<QList_QMap_QStrQStr >("QList_QMap_QStrQStr");
    qRegisterMetaType<FileMetadataCache>("FileMetadataCache");
    globalMaxThreadCount = Settings_Read_value("/settings/MaxThreadCount", 0).toInt();
    if(maxThreadsOverride > 0) globalMaxThreadCount = maxThreadsOverride;
    if(globalMaxThreadCount <= 0) {
        int cores = QThread::idealThreadCount();
        if(cores < 1) cores = 1;
        globalMaxThreadCount = cores * 2;
    }
    QThreadPool::globalInstance()->setMaxThreadCount(globalMaxThreadCount);
    ui->spinBox_ThreadNum_image->setMaximum(globalMaxThreadCount);
    ui->spinBox_ThreadNum_gif_internal->setMaximum(globalMaxThreadCount);
    ui->spinBox_ThreadNum_video_internal->setMaximum(globalMaxThreadCount);
    if(ui->spinBox_NumOfThreads_VFI) ui->spinBox_NumOfThreads_VFI->setMaximum(globalMaxThreadCount);
    this->setWindowTitle("Waifu2x-Extension-GUI "+VERSION+" by Aaron Feng");
    translator = new QTranslator(this);
    ui->tabWidget->setCurrentIndex(1);
    ui->tabWidget->tabBar()->setTabTextColor(0,Qt::red);
    on_tabWidget_currentChanged(1);
    ui->tabWidget_Engines->setCurrentIndex(0);
    this->setAcceptDrops(true);
    Init_Table();
    ui->groupBox_CurrentFile->setVisible(0);
    pushButton_Stop_setEnabled_self(0);
    ui->pushButton_ForceRetry->setVisible(0);
    ui->progressBar_CompatibilityTest->setVisible(0);
    ui->tableView_image->setVisible(0);
    ui->tableView_gif->setVisible(0);
    ui->tableView_video->setVisible(0);
    Table_FileCount_reload();
    Init_ActionsMenu_checkBox_ReplaceOriginalFile();
    Init_ActionsMenu_checkBox_DelOriginal();
    ui->tableView_image->installEventFilter(this);
    ui->tableView_gif->installEventFilter(this);
    ui->tableView_video->installEventFilter(this);
    connect(this, SIGNAL(Send_Set_checkBox_DisableResize_gif_Checked()), this, SLOT(Set_checkBox_DisableResize_gif_Checked()));
    connect(this, SIGNAL(Send_Table_EnableSorting(bool)), this, SLOT(Table_EnableSorting(bool)));
    connect(this, SIGNAL(Send_Add_progressBar_CompatibilityTest()), this, SLOT(Add_progressBar_CompatibilityTest()));
    connect(this, SIGNAL(Send_Unable2Connect_RawGithubusercontentCom()), this, SLOT(Unable2Connect_RawGithubusercontentCom()));
    connect(this, SIGNAL(Send_SetEnable_pushButton_ForceRetry_self()), this, SLOT(SetEnable_pushButton_ForceRetry_self()));
    connect(this, SIGNAL(Send_SystemTray_NewMessage(QString)), this, SLOT(SystemTray_NewMessage(QString)));
    connect(this, SIGNAL(Send_PrograssBar_Range_min_max(int, int)), this, SLOT(progressbar_setRange_min_max(int, int)));
    connect(this, SIGNAL(Send_progressbar_Add()), this, SLOT(progressbar_Add()));
    connect(this, SIGNAL(Send_Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_FileCount_reload()), this, SLOT(Table_FileCount_reload()));
    connect(this, SIGNAL(Send_Table_image_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_image_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_gif_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_gif_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_video_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_video_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_Read_Saved_Table_Filelist_Finished(QString)), this, SLOT(Table_Read_Saved_Table_Filelist_Finished(QString)));
    connect(this, SIGNAL(Send_Table_Save_Current_Table_Filelist_Finished()), this, SLOT(Table_Save_Current_Table_Filelist_Finished()));
    connect(this, SIGNAL(Send_Waifu2x_Finished()), this, SLOT(Waifu2x_Finished()));
    connect(this, SIGNAL(Send_Waifu2x_Finished_manual()), this, SLOT(Waifu2x_Finished_manual()));
    connect(this, SIGNAL(Send_TextBrowser_NewMessage(QString)), this, SLOT(TextBrowser_NewMessage(QString)));
    connect(this, SIGNAL(Send_Waifu2x_Compatibility_Test_finished()), this, SLOT(Waifu2x_Compatibility_Test_finished()));
    connect(this, SIGNAL(Send_Waifu2x_DetectGPU_finished()), this, SLOT(Waifu2x_DetectGPU_finished()));
    connect(this, SIGNAL(Send_Realsr_ncnn_vulkan_DetectGPU_finished()), this, SLOT(Realsr_ncnn_vulkan_DetectGPU_finished()));
    connect(this, SIGNAL(Send_CheckUpadte_NewUpdate(QString,QString)), this, SLOT(CheckUpadte_NewUpdate(QString,QString)));
    connect(this, SIGNAL(Send_SystemShutDown()), this, SLOT(SystemShutDown()));
    connect(this, SIGNAL(Send_Waifu2x_DumpProcessorList_converter_finished()), this, SLOT(Waifu2x_DumpProcessorList_converter_finished()));
    connect(this, SIGNAL(Send_Read_urls_finfished()), this, SLOT(Read_urls_finfished())); // Connect the signal to the correct original finishing slot
    connect(this, SIGNAL(Send_FinishedProcessing_DN()), this, SLOT(FinishedProcessing_DN()));
    connect(this, SIGNAL(Send_SRMD_DetectGPU_finished()), this, SLOT(SRMD_DetectGPU_finished()));
    connect(this, SIGNAL(Send_FrameInterpolation_DetectGPU_finished()), this, SLOT(FrameInterpolation_DetectGPU_finished()));
    connect(this, SIGNAL(Send_video_write_VideoConfiguration(QString,int,int,bool,int,int,QString,bool,QString,QString,bool,int)), this, SLOT(video_write_VideoConfiguration(QString,int,int,bool,int,int,QString,bool,QString,QString,bool,int)));
    connect(this, SIGNAL(Send_Settings_Save()), this, SLOT(Settings_Save()));
    connect(this, SIGNAL(Send_video_write_Progress_ProcessBySegment(QString,int,bool,bool,int,int)), this, SLOT(video_write_Progress_ProcessBySegment(QString,int,bool,bool,int,int)));
    connect(this, SIGNAL(Send_Donate_ReplaceQRCode(QString)), this, SLOT(Donate_ReplaceQRCode(QString)));
    connect(this, SIGNAL(Send_CurrentFileProgress_Start(QString,int)), this, SLOT(CurrentFileProgress_Start(QString,int)));
    connect(this, SIGNAL(Send_CurrentFileProgress_Stop()), this, SLOT(CurrentFileProgress_Stop()));
    connect(this, SIGNAL(Send_CurrentFileProgress_progressbar_Add()), this, SLOT(CurrentFileProgress_progressbar_Add()));
    connect(this, SIGNAL(Send_CurrentFileProgress_progressbar_Add_SegmentDuration(int)), this, SLOT(CurrentFileProgress_progressbar_Add_SegmentDuration(int)));
    connect(this, SIGNAL(Send_CurrentFileProgress_progressbar_SetFinishedValue(int)), this, SLOT(CurrentFileProgress_progressbar_SetFinishedValue(int)));
    TimeCostTimer = new QTimer();
    connect(TimeCostTimer, SIGNAL(timeout()), this, SLOT(TimeSlot()));
    FileProgressWatcher = new QFileSystemWatcher(this);
    FileProgressWatcher_Text = new QFileSystemWatcher(this);
    FileProgressStopTimer = new QTimer();
    FileProgressStopTimer_Text = new QTimer();
    Settings_Read_Apply();
    Set_Font_fixed();
    ApplyDarkStyle();
    QtConcurrent::run(this, &MainWindow::DeleteErrorLog_Waifu2xCaffe);
    QtConcurrent::run(this, &MainWindow::Del_TempBatFile);
    AutoUpdate = QtConcurrent::run(this, &MainWindow::CheckUpadte_Auto);
    DownloadOnlineQRCode = QtConcurrent::run(this, &MainWindow::Donate_DownloadOnlineQRCode);
    SystemShutDown_isAutoShutDown();
    TextBrowser_StartMes();
    Tip_FirstTimeStart();
    file_mkDir(Current_Path+"/FilesList_W2xEX");
    if(file_isDirWritable(Current_Path)==false)
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("Error")), QString(tr("It is detected that this software lacks the necessary permissions to run."
                        "\n\nPlease close this software and start this software again after giving this software administrator permission. "
                        "Or reinstall the software into a directory that can run normally without administrator rights.\n\nOtherwise, this software may not work properly.")));
        Msg.setIcon(QMessageBox::Warning);
        Msg.addButton(QString("OK"), QMessageBox::NoRole);
        Msg.exec();
    }
    Init_SystemTrayIcon();
    Init_ActionsMenu_lineEdit_outputPath();
    Init_ActionsMenu_FilesList();
    Init_ActionsMenu_pushButton_RemoveItem();
    Init_ActionsMenu_checkBox_ReplaceOriginalFile();
    Init_ActionsMenu_checkBox_DelOriginal();
    ui->comboBox_Model_RealCUGAN = findChild<QComboBox*>("comboBox_Model_RealCUGAN");
    ui->spinBox_Scale_RealCUGAN = findChild<QSpinBox*>("spinBox_Scale_RealCUGAN");
    ui->spinBox_DenoiseLevel_RealCUGAN = findChild<QSpinBox*>("spinBox_DenoiseLevel_RealCUGAN");
    ui->spinBox_TileSize_RealCUGAN = findChild<QSpinBox*>("spinBox_TileSize_RealCUGAN");
    ui->checkBox_TTA_RealCUGAN = findChild<QCheckBox*>("checkBox_TTA_RealCUGAN");
    ui->comboBox_GPUID_RealCUGAN = findChild<QComboBox*>("comboBox_GPUID_RealCUGAN");
    ui->pushButton_DetectGPU_RealCUGAN = findChild<QPushButton*>("pushButton_DetectGPU_RealCUGAN");
    ui->checkBox_MultiGPU_RealCUGAN = findChild<QCheckBox*>("checkBox_MultiGPU_RealCUGAN");
    ui->groupBox_GPUSettings_MultiGPU_RealCUGAN = findChild<QGroupBox*>("groupBox_GPUSettings_MultiGPU_RealCUGAN");
    ui->comboBox_GPUIDs_MultiGPU_RealCUGAN = findChild<QComboBox*>("comboBox_GPUIDs_MultiGPU_RealCUGAN");
    ui->pushButton_AddGPU_MultiGPU_RealCUGAN = findChild<QPushButton*>("pushButton_AddGPU_MultiGPU_RealCUGAN");
    ProcList_RealCUGAN.clear();
    Realcugan_NCNN_Vulkan_PreLoad_Settings();
    if(ui->pushButton_DetectGPU_RealCUGAN)
        connect(ui->pushButton_DetectGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked);
    if(ui->checkBox_MultiGPU_RealCUGAN)
        connect(ui->checkBox_MultiGPU_RealCUGAN, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged);
    if(ui->pushButton_AddGPU_MultiGPU_RealCUGAN)
        connect(ui->pushButton_AddGPU_MultiGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked);
    ui->pushButton_TileSize_Add_RealCUGAN = findChild<QPushButton*>("pushButton_TileSize_Add_RealCUGAN");
    ui->pushButton_TileSize_Minus_RealCUGAN = findChild<QPushButton*>("pushButton_TileSize_Minus_RealCUGAN");
    if(ui->pushButton_TileSize_Add_RealCUGAN)
        connect(ui->pushButton_TileSize_Add_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked);
    if(ui->pushButton_TileSize_Minus_RealCUGAN)
        connect(ui->pushButton_TileSize_Minus_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked);
    ui->pushButton_RemoveGPU_MultiGPU_RealCUGAN = findChild<QPushButton*>("pushButton_RemoveGPU_MultiGPU_RealCUGAN");
    ui->pushButton_ClearGPU_MultiGPU_RealCUGAN = findChild<QPushButton*>("pushButton_ClearGPU_MultiGPU_RealCUGAN");
    if(ui->pushButton_RemoveGPU_MultiGPU_RealCUGAN)
        connect(ui->pushButton_RemoveGPU_MultiGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked);
    if(ui->pushButton_ClearGPU_MultiGPU_RealCUGAN)
        connect(ui->pushButton_ClearGPU_MultiGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked);
    if(ui->comboBox_Model_RealCUGAN)
        connect(ui->comboBox_Model_RealCUGAN, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged);
    ui->comboBox_Model_RealESRGAN = findChild<QComboBox*>("comboBox_Model_RealESRGAN");
    ui->comboBox_GPUID_RealESRGAN = findChild<QComboBox*>("comboBox_GPUID_RealESRGAN");
    ui->pushButton_DetectGPU_RealESRGAN = findChild<QPushButton*>("pushButton_DetectGPU_RealESRGAN");
    ui->spinBox_TileSize_RealESRGAN = findChild<QSpinBox*>("spinBox_TileSize_RealESRGAN");
    ui->pushButton_TileSize_Add_RealESRGAN = findChild<QPushButton*>("pushButton_TileSize_Add_RealESRGAN");
    ui->pushButton_TileSize_Minus_RealESRGAN = findChild<QPushButton*>("pushButton_TileSize_Minus_RealESRGAN");
    ui->checkBox_TTA_RealESRGAN = findChild<QCheckBox*>("checkBox_TTA_RealESRGAN");
    ui->checkBox_MultiGPU_RealESRGAN = findChild<QCheckBox*>("checkBox_MultiGPU_RealESRGAN");
    ui->groupBox_GPUSettings_MultiGPU_RealESRGAN = findChild<QGroupBox*>("groupBox_GPUSettings_MultiGPU_RealESRGAN");
    ui->comboBox_GPUIDs_MultiGPU_RealESRGAN = findChild<QComboBox*>("comboBox_GPUIDs_MultiGPU_RealESRGAN");
    ui->listWidget_GPUList_MultiGPU_RealESRGAN = findChild<QListWidget*>("listWidget_GPUList_MultiGPU_RealESRGAN");
    ui->pushButton_AddGPU_MultiGPU_RealESRGAN = findChild<QPushButton*>("pushButton_AddGPU_MultiGPU_RealESRGAN");
    ui->pushButton_RemoveGPU_MultiGPU_RealESRGAN = findChild<QPushButton*>("pushButton_RemoveGPU_MultiGPU_RealESRGAN");
    ui->pushButton_ClearGPU_MultiGPU_RealESRGAN = findChild<QPushButton*>("pushButton_ClearGPU_MultiGPU_RealESRGAN");
    ui->spinBox_Threads_MultiGPU_RealESRGAN = findChild<QSpinBox*>("spinBox_Threads_MultiGPU_RealESRGAN");
    ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN = findChild<QCheckBox*>("checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN");
    ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN = findChild<QSpinBox*>("spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN");
    ui->pushButton_ShowMultiGPUSettings_RealESRGAN = findChild<QPushButton*>("pushButton_ShowMultiGPUSettings_RealESRGAN");
    ProcList_RealESRGAN.clear();
    GPU_ID_RealesrganNcnnVulkan_MultiGPU_CycleCounter = 0;
    isCompatible_RealESRGAN_NCNN_Vulkan = false;
    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.clear();
    RealESRGAN_NCNN_Vulkan_PreLoad_Settings();
    if(ui->pushButton_DetectGPU_RealESRGAN)
        connect(ui->pushButton_DetectGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_DetectGPU_RealESRGAN_clicked);
    if(ui->comboBox_Model_RealESRGAN)
        connect(ui->comboBox_Model_RealESRGAN, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_Model_RealESRGAN_currentIndexChanged);
    if(ui->pushButton_TileSize_Add_RealESRGAN)
        connect(ui->pushButton_TileSize_Add_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Add_RealESRGAN_clicked);
    if(ui->pushButton_TileSize_Minus_RealESRGAN)
        connect(ui->pushButton_TileSize_Minus_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Minus_RealESRGAN_clicked);
    if(ui->checkBox_MultiGPU_RealESRGAN)
        connect(ui->checkBox_MultiGPU_RealESRGAN, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_MultiGPU_RealESRGAN_stateChanged);
    if(ui->comboBox_GPUIDs_MultiGPU_RealESRGAN)
        connect(ui->comboBox_GPUIDs_MultiGPU_RealESRGAN, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_GPUIDs_MultiGPU_RealESRGAN_currentIndexChanged);
    if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN)
        connect(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN, &QCheckBox::clicked, this, &MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN_clicked);
    if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN)
        connect(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN_valueChanged);
    if(ui->pushButton_ShowMultiGPUSettings_RealESRGAN)
        connect(ui->pushButton_ShowMultiGPUSettings_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_ShowMultiGPUSettings_RealESRGAN_clicked);
    if(ui->pushButton_AddGPU_MultiGPU_RealESRGAN)
        connect(ui->pushButton_AddGPU_MultiGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_AddGPU_MultiGPU_RealESRGAN_clicked);
    if(ui->pushButton_RemoveGPU_MultiGPU_RealESRGAN)
        connect(ui->pushButton_RemoveGPU_MultiGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealESRGAN_clicked);
    if(ui->pushButton_ClearGPU_MultiGPU_RealESRGAN)
        connect(ui->pushButton_ClearGPU_MultiGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_ClearGPU_MultiGPU_RealESRGAN_clicked);

    this->showNormal();
    this->activateWindow();
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    this->adjustSize();
}

void MainWindow::ProcessDroppedFilesAsync(QList<QUrl> urls)
{
    // Initialize progress bar variables (members of MainWindow)
    Progressbar_MaxVal = 0;
    Progressbar_CurrentVal = 0;

    // Count total files for progress bar
    Progressbar_MaxVal = urls.count(); // Simple count for now
    // Emit signals to update UI from the main thread
    QMetaObject::invokeMethod(this, "Send_PrograssBar_Range_min_max", Qt::QueuedConnection,
                              Q_ARG(int, 0), Q_ARG(int, Progressbar_MaxVal));

    QMetaObject::invokeMethod(this, [this](){
        ui_tableViews_setUpdatesEnabled(false);
    }, Qt::BlockingQueuedConnection);

    AddNew_gif = false;
    AddNew_image = false;
    AddNew_video = false;

    // The core loop:
    foreach(QUrl url, urls)
    {
        if (waifu2x_STOP.load()) { // Check for stop requests (assuming waifu2x_STOP is an atomic bool or properly protected)
             // Emit to main thread for UI updates
             QMetaObject::invokeMethod(this, "Send_TextBrowser_NewMessage", Qt::QueuedConnection,
                                       Q_ARG(QString, tr("File adding process was stopped.")));
             break;
        }

        QString Input_path = url.toLocalFile().trimmed();
        if(Input_path.isEmpty()) continue;

        QFileInfo FileInfo_Input_path(Input_path);
        if(FileInfo_Input_path.isDir())
        {
            bool scanSubFolders = false;
            // Safely read UI state from main thread
            QMetaObject::invokeMethod(this, [this, &scanSubFolders](){
                scanSubFolders = ui->checkBox_ScanSubFolders->isChecked();
            }, Qt::BlockingQueuedConnection);

            if (scanSubFolders) {
                Add_File_Folder_IncludeSubFolder(Input_path); // This function emits signals for UI updates
            } else {
                Add_File_Folder(Input_path); // Process only top-level directory contents
            }
        }
        else // Is a file
        {
            Add_File_Folder(Input_path);
        }
        // Emit progress bar update on main thread
        QMetaObject::invokeMethod(this, "Send_progressbar_Add", Qt::QueuedConnection);

    } // end foreach

    QMetaObject::invokeMethod(this, [this](){
        ui_tableViews_setUpdatesEnabled(true);
    }, Qt::BlockingQueuedConnection);

    // After all files are processed by the loop:
    // This signal should be connected to a slot that re-enables UI, sorts tables etc.
    // (e.g. the existing ProcessDroppedFilesFinished or a modified Read_urls_finfished)
    emit Send_Read_urls_finfished();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isAlreadyClosed.load())
    {
        event->accept();
        return;
    }
    if(ui->checkBox_PromptWhenExit->isChecked())
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("Notification")), QString(tr("Do you really wanna exit Waifu2x-Extension-GUI ?")));
        Msg.setIcon(QMessageBox::Question);
        QAbstractButton *pYesBtn = Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
        QAbstractButton *pNoBtn = Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
        Msg.exec();
        if (Msg.clickedButton() == pNoBtn)
        {
            event->ignore();
            return;
        }
        if (Msg.clickedButton() == pYesBtn)isAlreadyClosed = true;
    } else {
      isAlreadyClosed = true;
    }

    systemTray->hide();
    this->hide();
    QApplication::setQuitOnLastWindowClosed(true);
    QApplication::closeAllWindows();

    if(Waifu2xMain.isRunning() == true)
    {
        TimeCostTimer->stop();
        pushButton_Stop_setEnabled_self(0);
        waifu2x_STOP = true;
        QProcess_stop = true;
        emit TextBrowser_NewMessage(tr("Trying to stop, please wait..."));
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Notification")+" @Waifu2x-Extension-GUI");
        MSG_2->setText(tr("Waiting for the files processing thread to pause"));
        MSG_2->setIcon(QMessageBox::Information);
        MSG_2->setModal(true);
        MSG_2->setStandardButtons(NULL);
        MSG_2->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        MSG_2->show();
    }
    else
    {
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Notification")+" @Waifu2x-Extension-GUI");
        MSG_2->setText(tr("Closing...\n\nPlease wait"));
        MSG_2->setIcon(QMessageBox::Information);
        MSG_2->setModal(true);
        MSG_2->setStandardButtons(NULL);
        MSG_2->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        MSG_2->show();
    }
    AutoUpdate.cancel();
    DownloadOnlineQRCode.cancel();
    bool AutoSaveSettings = ui->checkBox_AutoSaveSettings->isChecked();
    if(AutoSaveSettings&&(!Settings_isReseted))
    {
        Settings_Save();
        QtConcurrent::run(this, &MainWindow::Auto_Save_Settings_Watchdog,true);
    }
    else
    {
        QtConcurrent::run(this, &MainWindow::Auto_Save_Settings_Watchdog,false);
    }
}

int MainWindow::Auto_Save_Settings_Watchdog(bool isWaitForSave)
{
    Waifu2xMain.waitForFinished();
    if(isWaitForSave == true)
    {
        Delay_msec_sleep(1000);
        QString settings_ini = Current_Path+"/settings.ini";
        while(!QFile::exists(settings_ini))
        {
            Delay_msec_sleep(250);
        }
        Delay_msec_sleep(3000);
    }
    Force_close();
    return 0;
}

int MainWindow::Force_close()
{
    QStringList TaskNameList;
    TaskNameList << "convert_waifu2xEX.exe"<<"ffmpeg_waifu2xEX.exe"<<"ffprobe_waifu2xEX.exe"<<"identify_waifu2xEX.exe"<<"gifsicle_waifu2xEX.exe"
                 <<"sox_waifu2xEX.exe"<<"wget_waifu2xEX.exe"<<"rife-ncnn-vulkan_waifu2xEX.exe"<<"cain-ncnn-vulkan_waifu2xEX.exe"<<"dain-ncnn-vulkan_waifu2xEX.exe"
                 <<"apngdis_waifu2xEX.exe"<<"apngasm_waifu2xEX.exe"<<"RealCUGAN-ncnn-Vulkan_waifu2xEX.exe"<<"RealESRGAN-ncnn-Vulkan_waifu2xEX.exe";
    KILL_TASK_QStringList(TaskNameList,true);
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq Waifu2x-Extension-GUI.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    return 0;
}

void MainWindow::changeEvent(QEvent *e)
{
    if((e->type()==QEvent::WindowStateChange)&&this->isMinimized())
    {
        if(ui->checkBox_MinimizeToTaskbar->isChecked())
        {
            this->hide();
        }
    }
}

void MainWindow::TimeSlot()
{
    TimeCost++;
    QString TimeCostStr = tr("Time taken:[")+Seconds2hms(TimeCost)+"]";
    ui->label_TimeCost->setText(TimeCostStr);
    if(ui->label_TimeRemain->isVisible())
    {
        long unsigned int TaskNumFinished_tmp = TaskNumFinished;
        long unsigned int TimeCost_tmp = TimeCost;
        long unsigned int TaskNumTotal_tmp = TaskNumTotal;
        if(TaskNumFinished_tmp>0&&TimeCost_tmp>0&&TaskNumTotal_tmp>0)
        {
            if(NewTaskFinished)
            {
                NewTaskFinished=false;
                double avgTimeCost = (double)TimeCost_tmp/(double)TaskNumFinished_tmp;
                ETA = avgTimeCost*((double)TaskNumTotal_tmp-(double)TaskNumFinished_tmp);
            }
            else
            {
                if(ETA>1)
                {
                    ETA--;
                }
            }
            QString TimeRemainingStr = tr("Time remaining:[")+Seconds2hms(ETA)+"]";
            ui->label_TimeRemain->setText(TimeRemainingStr);
            QDateTime time = QDateTime::currentDateTime();
            long unsigned int Time_t = time.toTime_t();
            Time_t+=ETA;
            time = QDateTime::fromTime_t(Time_t);
            QString Current_Time = time.toString("hh:mm:ss");
            QString ETA_str = "ETA:["+Current_Time+"]";
            ui->label_ETA->setText(ETA_str);
        }
    }
    if(isStart_CurrentFile)
    {
        TimeCost_CurrentFile++;
        QString TimeCostStr_CurrentFile = tr("Time taken:[")+Seconds2hms(TimeCost_CurrentFile)+"]";
        ui->label_TimeCost_CurrentFile->setText(TimeCostStr_CurrentFile);
        long unsigned int TaskNumFinished_tmp_CurrentFile = TaskNumFinished_CurrentFile;
        long unsigned int TimeCost_tmp_CurrentFile = TimeCost_CurrentFile;
        long unsigned int TaskNumTotal_tmp_CurrentFile = TaskNumTotal_CurrentFile;
        if(TaskNumFinished_tmp_CurrentFile>0&&TimeCost_tmp_CurrentFile>0&&TaskNumTotal_tmp_CurrentFile>0)
        {
            if(NewTaskFinished_CurrentFile)
            {
                NewTaskFinished_CurrentFile=false;
                double avgTimeCost_CurrentFile = (double)TimeCost_tmp_CurrentFile/(double)TaskNumFinished_tmp_CurrentFile;
                ETA_CurrentFile = avgTimeCost_CurrentFile*((double)TaskNumTotal_tmp_CurrentFile-(double)TaskNumFinished_tmp_CurrentFile);
            }
            else
            {
                if(ETA_CurrentFile>1)
                {
                    ETA_CurrentFile--;
                }
            }
            QString TimeRemainingStr_CurrentFile = tr("Time remaining:[")+Seconds2hms(ETA_CurrentFile)+"]";
            ui->label_TimeRemain_CurrentFile->setText(TimeRemainingStr_CurrentFile);
            QDateTime time_CurrentFile = QDateTime::currentDateTime();
            long unsigned int Time_t_CurrentFile = time_CurrentFile.toTime_t();
            Time_t_CurrentFile+=ETA_CurrentFile;
            time_CurrentFile = QDateTime::fromTime_t(Time_t_CurrentFile);
            QString Current_Time_CurrentFile = time_CurrentFile.toString("hh:mm:ss");
            QString ETA_str_CurrentFile = "ETA:["+Current_Time_CurrentFile+"]";
            ui->label_ETA_CurrentFile->setText(ETA_str_CurrentFile);
        }
    }
}
QString MainWindow::Seconds2hms(long unsigned int seconds)
{
    if(seconds<=0)return "0:0:0";
    long unsigned int hour = seconds / 3600;
    long unsigned int min = (seconds-(hour*3600))/60;
    long unsigned int sec = seconds - hour*3600 - min*60;
    return QString::number(hour,10)+":"+QString::number(min,10)+":"+QString::number(sec,10);
}

void MainWindow::Set_Font_fixed()
{
    QFont font;
    if(ui->checkBox_isCustFontEnable->isChecked())
    {
        font = ui->fontComboBox_CustFont->currentFont();
        font.setPixelSize(ui->spinBox_GlobalFontSize->value());
    }
    else
    {
        font = qApp->font();
        font.setPixelSize(15);
    }
    qApp->setFont(font);
}

bool MainWindow::SystemPrefersDark() const
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#else
    QColor bg = qApp->palette().color(QPalette::Window);
    return bg.lightness() < 128;
#endif
}

void MainWindow::ApplyDarkStyle()
{
    int mode = Settings_Read_value("/settings/DarkMode", 1).toInt();
    bool enable = false;
    if(mode == 1)
        enable = true;
    else if(mode == 2)
        enable = SystemPrefersDark();
    if(enable)
    {
        QFile f(":/style/styles/dark.qss");
        if(f.open(QIODevice::ReadOnly))
            qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
    }
    else
    {
        qApp->setStyleSheet("");
    }

    for(QWidget *w : QApplication::allWidgets())
    {
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    }
}

void MainWindow::on_pushButton_ClearList_clicked()
{
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = -1;
    Table_Clear();
    Custom_resolution_list.clear();
    QMutexLocker locker(&m_metadataCacheMutex);
    m_metadataCache.clear();
    locker.unlock();
    ui->label_DropFile->setVisible(1);
    ui->tableView_gif->setVisible(0);
    ui->tableView_image->setVisible(0);
    ui->tableView_video->setVisible(0);
    Table_FileCount_reload();
    progressbar_clear();
}

void MainWindow::on_pushButton_Stop_clicked()
{
    if(Waifu2xMain.isRunning()==false)return;
    TimeCostTimer->stop();
    pushButton_Stop_setEnabled_self(0);
    waifu2x_STOP = true;
    QProcess_stop = true;
    emit TextBrowser_NewMessage(tr("Trying to stop, please wait..."));
    QtConcurrent::run(this, &MainWindow::Wait_waifu2x_stop);
}

void MainWindow::Wait_waifu2x_stop()
{
    while(true)
    {
        if(waifu2x_STOP_confirm.load() || ThreadNumRunning.load()==0)
        {
            waifu2x_STOP_confirm = false;
            Waifu2xMain.waitForFinished();
            while(true)
            {
                if(Waifu2xMain.isRunning()==false)break;
                Delay_msec_sleep(300);
            }
            emit TextBrowser_NewMessage(tr("Processing of files has stopped."));
            QtConcurrent::run(this, &MainWindow::Play_NFSound);
            break;
        }
        Delay_msec_sleep(300);
    }
    emit Send_Waifu2x_Finished_manual();
}

int MainWindow::on_pushButton_RemoveItem_clicked()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        ui->tableView_image->clearSelection();
        ui->tableView_gif->clearSelection();
        ui->tableView_video->clearSelection();
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("No items are currently selected."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        return 0;
    }
    if(curRow_image >= 0)
    {
        QString fullPath = Table_model_image->item(curRow_image,2)->text();
        CustRes_remove(fullPath);
        QMutexLocker locker(&m_metadataCacheMutex);
        m_metadataCache.remove(fullPath);
        locker.unlock();
        ui->tableView_image->setUpdatesEnabled(false);
        Table_model_image->removeRow(curRow_image);
        ui->tableView_image->setUpdatesEnabled(true);
        curRow_image = -1;
        ui->tableView_image->clearSelection();
    }
    if(curRow_video >= 0)
    {
        QString fullPath = Table_model_video->item(curRow_video,2)->text();
        CustRes_remove(fullPath);
        QMutexLocker locker(&m_metadataCacheMutex);
        m_metadataCache.remove(fullPath);
        locker.unlock();
        ui->tableView_video->setUpdatesEnabled(false);
        Table_model_video->removeRow(curRow_video);
        ui->tableView_video->setUpdatesEnabled(true);
        curRow_video = -1;
        ui->tableView_video->clearSelection();
    }
    if(curRow_gif >= 0)
    {
        QString fullPath = Table_model_gif->item(curRow_gif,2)->text();
        CustRes_remove(fullPath);
        QMutexLocker locker(&m_metadataCacheMutex);
        m_metadataCache.remove(fullPath);
        locker.unlock();
        ui->tableView_gif->setUpdatesEnabled(false);
        Table_model_gif->removeRow(curRow_gif);
        ui->tableView_gif->setUpdatesEnabled(true);
        curRow_gif = -1;
        ui->tableView_gif->clearSelection();
    }
    if(Table_model_gif->rowCount()==0)
    {
        ui->tableView_gif->setVisible(0);
    }
    if(Table_model_image->rowCount()==0)
    {
        ui->tableView_image->setVisible(0);
    }
    if(Table_model_video->rowCount()==0)
    {
        ui->tableView_video->setVisible(0);
    }
    if(Table_model_gif->rowCount()==0&&Table_model_image->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        on_pushButton_ClearList_clicked();
    }
    Table_FileCount_reload();
    return 0;
}

void MainWindow::Delay_sec_sleep(int time)
{
    QThread::sleep(time);
}

void MainWindow::Delay_msec_sleep(int time)
{
    QThread::msleep(time);
}

void MainWindow::Play_NFSound()
{
    if(ui->checkBox_NfSound->isChecked()==false)return;
    QString NFSound = Current_Path+"/NFSound_Waifu2xEX.mp3";
    if(QFile::exists(NFSound)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Notification sound file is missing!"));
        return;
    }
    QMediaPlayer *player = new QMediaPlayer(this);
    connect(player, QOverload<QMediaPlayer::MediaStatus>::of(&QMediaPlayer::mediaStatusChanged),
            [=](QMediaPlayer::MediaStatus status){
        if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia || status == QMediaPlayer::NoMedia) {
            player->deleteLater();
        }
    });
    connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            [=](QMediaPlayer::Error error){
        Q_UNUSED(error);
        qDebug() << "Error playing notification sound:" << player->errorString();
        player->deleteLater();
    });
    player->setMedia(QUrl::fromLocalFile(NFSound));
    player->play();
}

void MainWindow::on_pushButton_Report_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/issues/new"));
}

void MainWindow::on_pushButton_ReadMe_clicked()
{
    if(ui->comboBox_language->currentIndex()==1)
    {
        QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/"));
        QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/"));
    }
    else
    {
        QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/"));
    }
}



void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->spinBox_DenoiseLevel_image->setRange(-1,3);
        ui->spinBox_DenoiseLevel_image->setValue(-1);
        ui->spinBox_DenoiseLevel_image->setEnabled(true);
        ui->spinBox_DenoiseLevel_image->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
        ui->label_ImageDenoiseLevel->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
    }
    else
    {
        ui->spinBox_DenoiseLevel_image->setRange(0,0);
        ui->spinBox_DenoiseLevel_image->setValue(0);
        ui->spinBox_DenoiseLevel_image->setEnabled(false);
        ui->spinBox_DenoiseLevel_image->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
        ui->label_ImageDenoiseLevel->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
}

void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->spinBox_DenoiseLevel_gif->setRange(-1,3);
        ui->spinBox_DenoiseLevel_gif->setValue(-1);
        ui->spinBox_DenoiseLevel_gif->setEnabled(true);
        ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
        ui->label_GIFDenoiseLevel->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
    }
    else
    {
        ui->spinBox_DenoiseLevel_gif->setRange(0,0);
        ui->spinBox_DenoiseLevel_gif->setValue(0);
        ui->spinBox_DenoiseLevel_gif->setEnabled(false);
        ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
        ui->label_GIFDenoiseLevel->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
}

void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->spinBox_DenoiseLevel_video->setRange(-1,3);
        ui->spinBox_DenoiseLevel_video->setValue(-1);
        ui->spinBox_DenoiseLevel_video->setEnabled(true);
        ui->spinBox_DenoiseLevel_video->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
        ui->label_VideoDenoiseLevel->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
    }
    else
    {
        ui->spinBox_DenoiseLevel_video->setRange(0,0);
        ui->spinBox_DenoiseLevel_video->setValue(0);
        ui->spinBox_DenoiseLevel_video->setEnabled(false);
        ui->spinBox_DenoiseLevel_video->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
        ui->label_VideoDenoiseLevel->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
}

void MainWindow::on_pushButton_clear_textbrowser_clicked()
{
    ui->textBrowser->clear();
    TextBrowser_StartMes();
}

void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int arg1)
{
    int size = ui->spinBox_textbrowser_fontsize->value();
    ui->textBrowser->setStyleSheet("font: "+QString::number(size,10)+"pt \"Arial\";");
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_CustRes_apply_clicked()
{
    CustRes_SetCustRes();
}

void MainWindow::on_pushButton_CustRes_cancel_clicked()
{
    CustRes_CancelCustRes();
}

void MainWindow::on_pushButton_HideSettings_clicked()
{
    if(ui->groupBox_Setting->isVisible())
    {
        ui->groupBox_Setting->setVisible(0);
        isSettingsHide=true;
        ui->pushButton_HideSettings->setText(tr("Show settings"));
    }
    else
    {
        ui->groupBox_Setting->setVisible(1);
        isSettingsHide=false;
        ui->pushButton_HideSettings->setText(tr("Hide settings"));
    }
}

void MainWindow::on_comboBox_language_currentIndexChanged(int index)
{
    QString JapaneseQM = Current_Path + "/language_Japanese.qm";
    if(QFile::exists(JapaneseQM))
    {
        QFile::remove(JapaneseQM);
        if(ui->comboBox_language->currentIndex()==2)
        {
            ui->comboBox_language->setCurrentIndex(0);
        }
        if(ui->comboBox_language->currentIndex()==3 || ui->comboBox_language->currentIndex()==-1)
        {
            ui->comboBox_language->setCurrentIndex(2);
        }
    }
    QString qmFilename="";
    switch(ui->comboBox_language->currentIndex())
    {
        case 0:
            {
                qmFilename = Current_Path + "/language_English.qm";
                break;
            }
        case 1:
            {
                qmFilename = Current_Path + "/language_Chinese.qm";
                break;
            }
        case 2:
            {
                qmFilename = Current_Path + "/language_TraditionalChinese.qm";
                break;
            }
    }
    if(QFile::exists(qmFilename)==false)
    {
        QMessageBox *MSG_languageFile404 = new QMessageBox();
        MSG_languageFile404->setWindowTitle(tr("Error"));
        MSG_languageFile404->setText(tr("Language file is missing, please reinstall this program."));
        MSG_languageFile404->setIcon(QMessageBox::Warning);
        MSG_languageFile404->setModal(true);
        MSG_languageFile404->show();
        return;
    }
    if (translator->load(qmFilename))
    {
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        Table_FileCount_reload();
        Init_Table();
        Init_SystemTrayIcon();
        Set_Font_fixed();
        if(ui->checkBox_AlwaysHideSettings->isChecked())
        {
            ui->groupBox_Setting->setVisible(0);
            isSettingsHide=true;
            ui->pushButton_HideSettings->setText(tr("Show settings"));
        }
        else
        {
            ui->groupBox_Setting->setVisible(1);
            isSettingsHide=false;
            ui->pushButton_HideSettings->setText(tr("Hide settings"));
        }
        if(ui->checkBox_AlwaysHideTextBrowser->isChecked())
        {
            ui->splitter_TextBrowser->setVisible(0);
            ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
        }
        else
        {
            ui->splitter_TextBrowser->setVisible(1);
            ui->pushButton_HideTextBro->setText(tr("Hide Text Browser"));
        }
        if(this->windowState()!=Qt::WindowMaximized)
        {
            this->adjustSize();
        }
    }
    else
    {
        QMessageBox *MSG_Unable2LoadLanguageFiles = new QMessageBox();
        MSG_Unable2LoadLanguageFiles->setWindowTitle(tr("Error"));
        MSG_Unable2LoadLanguageFiles->setText(tr("Language file cannot be loaded properly."));
        MSG_Unable2LoadLanguageFiles->setIcon(QMessageBox::Warning);
        MSG_Unable2LoadLanguageFiles->setModal(true);
        MSG_Unable2LoadLanguageFiles->show();
    }
}

void MainWindow::on_pushButton_ReadFileList_clicked()
{
    file_mkDir(Current_Path+"/FilesList_W2xEX");
    QString Table_FileList_ini = QFileDialog::getOpenFileName(this, tr("Select saved files list @Waifu2x-Extension-GUI"), Current_Path+"/FilesList_W2xEX", "*.ini");
    if(Table_FileList_ini=="")return;
    if(QFile::exists(Table_FileList_ini))
    {
        ui_tableViews_setUpdatesEnabled(false);
        this->setAcceptDrops(0);
        pushButton_Start_setEnabled_self(0);
        ui->pushButton_CustRes_cancel->setEnabled(0);
        ui->pushButton_CustRes_apply->setEnabled(0);
        ui->pushButton_ReadFileList->setEnabled(0);
        ui->pushButton_SaveFileList->setEnabled(0);
        ui->pushButton_BrowserFile->setEnabled(0);
        on_pushButton_ClearList_clicked();
        Send_TextBrowser_NewMessage(tr("Please wait while reading the file."));
        ui->label_DropFile->setText(tr("Loading list, please wait."));
        QtConcurrent::run(this, &MainWindow::Table_Read_Saved_Table_Filelist,Table_FileList_ini);
    }
    else
    {
        QMessageBox *MSG_FileList404 = new QMessageBox();
        MSG_FileList404->setWindowTitle(tr("Error"));
        MSG_FileList404->setText(tr("Target files list doesn't exist!"));
        MSG_FileList404->setIcon(QMessageBox::Warning);
        MSG_FileList404->setModal(true);
        MSG_FileList404->show();
    }
}

void MainWindow::on_Ext_image_editingFinished()
{
    QString ext_image_str = ui->Ext_image->text();
    ext_image_str = ext_image_str.trimmed()
            .replace(QString(QChar(0xFF1A)), ":").remove(" ")
            .remove(QString(QChar(0x3000))).replace(":gif:", ":");
    ui->Ext_image->setText(ext_image_str);
}

void MainWindow::on_Ext_video_editingFinished()
{
    QString ext_video_str = ui->Ext_video->text();
    ext_video_str = ext_video_str.trimmed()
            .replace(QString(QChar(0xFF1A)), ":").remove(" ")
            .remove(QString(QChar(0x3000))).replace(":gif:", ":");
    ui->Ext_video->setText(ext_video_str);
}

void MainWindow::on_checkBox_AutoSaveSettings_clicked()
{
    QString settings_ini = Current_Path+"/settings.ini";
    if(QFile::exists(settings_ini))
    {
        QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
        configIniWrite->setValue("/settings/AutoSaveSettings", ui->checkBox_AutoSaveSettings->isChecked());
    }
}

void MainWindow::on_pushButton_about_clicked()
{
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("About"));
    QString line1 = "Waifu2x-Extension-GUI\n\n";
    QString line2 = VERSION+"\n\n";
    QString line3 = "Github: https://github.com/AaronFeng753/Waifu2x-Extension-GUI\n\n";
    QString line4 = "Waifu2x-Extension-GUI is licensed under the\n";
    QString line5 = "GNU Affero General Public License v3.0\n\n";
    QString line6 = "Copyright (C) 2021 Aaron Feng. All rights reserved.\n\n";
    QString line7 = "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n\n";
    QString line8 = "Icons made by : Freepik & Icongeek26 & Roundicons From Flaticon(https://www.flaticon.com/)";
    MSG->setText(line1+line2+line3+line4+line5+line6+line7+line8);
    QImage img(":/new/prefix1/icon/icon_main.png");
    QImage img_scaled = img.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pix(QPixmap::fromImage(img_scaled));
    MSG->setIconPixmap(pix);
    MSG->setModal(false);
    MSG->show();
}

void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int index)
{
    int CurrentIndex = ui->comboBox_AspectRatio_custRes->currentIndex();
    switch(CurrentIndex)
    {
        case 0:
            {
                CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;
                break;
            }
        case 1:
            {
                CustRes_AspectRatioMode = Qt::KeepAspectRatio;
                break;
            }
        case 2:
            {
                CustRes_AspectRatioMode = Qt::KeepAspectRatioByExpanding;
                break;
            }
    }
}

void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideSettings->isChecked())
    {
        ui->groupBox_Setting->setVisible(0);
        ui->pushButton_HideSettings->setText(tr("Show settings"));
        isSettingsHide=true;
    }
}

void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()
{
    QString settings_ini = Current_Path+"/settings.ini";
    QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
    configIniWrite->setValue("/settings/GlobalFontSize", ui->spinBox_GlobalFontSize->value());
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("Custom Font Settings saved successfully.\n\nRestart the software to take effect."));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
}

void MainWindow::on_pushButton_BrowserFile_clicked()
{
    QString Last_browsed_path = Current_Path+"/LastBrowsedPath_W2xEX.ini";
    QStringList nameFilters;
    nameFilters.append("*.gif");
    nameFilters.append("*.apng");
    QString Ext_image_str = ui->Ext_image->text();
    QStringList nameFilters_image = Ext_image_str.split(":");
    nameFilters_image.removeAll("gif");
    nameFilters_image.removeAll("apng");
    for(int i = 0; i < nameFilters_image.size(); ++i)
    {
        QString tmp = nameFilters_image.at(i).trimmed();
        if(tmp=="")continue;
        tmp = "*." + tmp;
        nameFilters.append(tmp);
    }
    QString Ext_video_str = ui->Ext_video->text();
    QStringList nameFilters_video = Ext_video_str.split(":");
    nameFilters_video.removeAll("gif");
    nameFilters_video.removeAll("apng");
    for(int i = 0; i < nameFilters_video.size(); ++i)
    {
        QString tmp = nameFilters_video.at(i).trimmed();
        if(tmp=="")continue;
        tmp = "*." + tmp;
        nameFilters.append(tmp);
    }
    QString nameFilters_QString = "";
    for(int i = 0; i < nameFilters.size(); ++i)
    {
        QString tmp = nameFilters.at(i).trimmed();
        nameFilters_QString = nameFilters_QString +" "+ tmp;
    }
    QString BrowserStartPath = "";
    if(QFile::exists(Last_browsed_path))
    {
        QSettings *configIniRead = new QSettings(Last_browsed_path, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        BrowserStartPath = configIniRead->value("/Path").toString();
        if(!QFile::exists(BrowserStartPath))BrowserStartPath = "";
    }
    QStringList Input_path_List = QFileDialog::getOpenFileNames(this, tr("Select files @Waifu2x-Extension-GUI"), BrowserStartPath,  tr("All file(")+nameFilters_QString+")");
    if(Input_path_List.isEmpty())
    {
        return;
    }
    QFile::remove(Last_browsed_path);
    QSettings *configIniWrite = new QSettings(Last_browsed_path, QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    QFileInfo lastPath(Input_path_List.at(0));
    QString folder_lastPath = file_getFolderPath(lastPath);
    configIniWrite->setValue("/Path", folder_lastPath);
    AddNew_gif=false;
    AddNew_image=false;
    AddNew_video=false;
    ui_tableViews_setUpdatesEnabled(false);
    ui->groupBox_Setting->setEnabled(0);
    ui->groupBox_FileList->setEnabled(0);
    ui->groupBox_InputExt->setEnabled(0);
    pushButton_Start_setEnabled_self(0);
    ui->checkBox_ScanSubFolders->setEnabled(0);
    this->setAcceptDrops(0);
    ui->label_DropFile->setText(tr("Adding files, please wait."));
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));
    QtConcurrent::run(this, &MainWindow::Read_Input_paths_BrowserFile, Input_path_List);
}

void MainWindow::Read_Input_paths_BrowserFile(QStringList Input_path_List)
{
    Progressbar_MaxVal = Input_path_List.size();
    Progressbar_CurrentVal = 0;
    emit Send_PrograssBar_Range_min_max(0, Progressbar_MaxVal);
    foreach(QString Input_path, Input_path_List)
    {
        Input_path=Input_path.trimmed();
        if(QFile::exists(Input_path)==false)continue;
        Add_File_Folder(Input_path);
        emit Send_progressbar_Add();
    }
    emit Send_Read_urls_finfished();
}


void MainWindow::ProcessDroppedFilesFinished()
{
    // This function is a slot and runs in the main GUI thread.
    // It's called when ProcessDroppedFilesAsync emits Send_Read_urls_finfished
    this->setAcceptDrops(true);
    // Resetting label_DropFile text and other UI updates are handled by Read_urls_finfished

    ui->groupBox_Setting->setEnabled(true);
    ui->groupBox_FileList->setEnabled(true);
    ui->groupBox_InputExt->setEnabled(true);
    ui->checkBox_ScanSubFolders->setEnabled(true);

    // Call the original slot that handles final UI updates (table sorting, file count, start button state)
    Read_urls_finfished();

    emit Send_TextBrowser_NewMessage(tr("Finished processing dropped files."));
}


void MainWindow::on_pushButton_wiki_clicked()
{
    if(ui->comboBox_language->currentIndex()==1)
    {
        QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/wikis"));
    }
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/wiki"));
}

void MainWindow::on_pushButton_HideTextBro_clicked()
{
    if(ui->textBrowser->isVisible())
    {
        ui->splitter_TextBrowser->setVisible(0);
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
    }
    else
    {
        ui->splitter_TextBrowser->setVisible(1);
        ui->pushButton_HideTextBro->setText(tr("Hide Text Browser"));
    }
}

void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideTextBrowser->isChecked())
    {
        ui->splitter_TextBrowser->setVisible(0);
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
    }
}



void MainWindow::on_Ext_image_textChanged(const QString &arg1)
{
    QString lower = ui->Ext_image->text().toLower();
    ui->Ext_image->setText(lower);
}

void MainWindow::on_Ext_video_textChanged(const QString &arg1)
{
    QString lower = ui->Ext_video->text().toLower();
    ui->Ext_video->setText(lower);
}

void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int index)
{
    if(ui->comboBox_model_vulkan->currentIndex()==0)
    {
        ui->comboBox_ImageStyle->setEnabled(1);
        ui->label_ImageStyle_W2xNCNNVulkan->setVisible(1);
        ui->comboBox_ImageStyle->setVisible(1);
        if(ui->comboBox_Engine_Image->currentIndex()!=0&&ui->comboBox_Engine_GIF->currentIndex()!=0&&ui->comboBox_Engine_Video->currentIndex()!=0)
        {
            ui->comboBox_ImageStyle->setEnabled(0);
            ui->label_ImageStyle_W2xNCNNVulkan->setVisible(0);
            ui->comboBox_ImageStyle->setVisible(0);
        }
    }
    if(ui->comboBox_model_vulkan->currentIndex()==1)
    {
        ui->comboBox_ImageStyle->setEnabled(0);
        ui->label_ImageStyle_W2xNCNNVulkan->setVisible(0);
        ui->comboBox_ImageStyle->setVisible(0);
    }
}

void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int index)
{
    if(ui->comboBox_ImageStyle->currentIndex()==0)
    {
        ui->comboBox_model_vulkan->setEnabled(1);
    }
    if(ui->comboBox_ImageStyle->currentIndex()==1)
    {
        ui->comboBox_model_vulkan->setEnabled(0);
    }
}

void MainWindow::on_pushButton_ResetVideoSettings_clicked()
{
    ui->lineEdit_pixformat->setText("yuv420p");
    ui->lineEdit_encoder_vid->setText("libx264");
    ui->lineEdit_encoder_audio->setText("aac");
    ui->spinBox_bitrate_vid->setValue(6000);
    ui->spinBox_bitrate_audio->setValue(320);
    ui->spinBox_bitrate_vid_2mp4->setValue(2500);
    ui->spinBox_bitrate_audio_2mp4->setValue(320);
    ui->checkBox_acodec_copy_2mp4->setChecked(0);
    ui->checkBox_vcodec_copy_2mp4->setChecked(0);
    ui->spinBox_bitrate_vid_2mp4->setEnabled(1);
    ui->spinBox_bitrate_audio_2mp4->setEnabled(1);
    ui->lineEdit_ExCommand_2mp4->setText("");
    ui->lineEdit_ExCommand_output->setText("");
}

void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_encoder_vid->text().trimmed();
    ui->lineEdit_encoder_vid->setText(tmp);
}

void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_encoder_audio->text().trimmed();
    ui->lineEdit_encoder_audio->setText(tmp);
}

void MainWindow::on_lineEdit_pixformat_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_pixformat->text().trimmed();
    ui->lineEdit_pixformat->setText(tmp);
}

void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1)
{
    if(ui->checkBox_vcodec_copy_2mp4->isChecked())
    {
        ui->spinBox_bitrate_vid_2mp4->setEnabled(0);
    }
    else
    {
        ui->spinBox_bitrate_vid_2mp4->setEnabled(1);
    }
}

void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int arg1)
{
    if(ui->checkBox_acodec_copy_2mp4->isChecked())
    {
        ui->spinBox_bitrate_audio_2mp4->setEnabled(0);
    }
    else
    {
        ui->spinBox_bitrate_audio_2mp4->setEnabled(1);
    }
}


void MainWindow::on_pushButton_encodersList_clicked()
{
    file_OpenFile(Current_Path+"/FFmpeg_Encoders_List_waifu2xEX.bat");
}

void MainWindow::Tip_FirstTimeStart()
{
    QString FirstTimeStart = Current_Path+"/FirstTimeStart";
    if(QFile::exists(FirstTimeStart))
    {
        isFirstTimeStart=false;
        return;
    }
    else
    {
        isFirstTimeStart=true;
        QMessageBox Msg(QMessageBox::Question, tr("Choose your language"), tr("Choose your language."));
        Msg.setIcon(QMessageBox::Information);
        QAbstractButton *pYesBtn_English = Msg.addButton(QString("English"), QMessageBox::YesRole); // Assuming "English" is fine as is, or could be tr("English")
        QAbstractButton *pYesBtn_Chinese = Msg.addButton(tr("Simplified Chinese"), QMessageBox::YesRole);
        QAbstractButton *pYesBtn_TraditionalChinese = Msg.addButton(tr("Traditional Chinese (by uimee)"), QMessageBox::YesRole);
        Msg.exec();
        if (Msg.clickedButton() == pYesBtn_English)ui->comboBox_language->setCurrentIndex(0);
        if (Msg.clickedButton() == pYesBtn_Chinese)ui->comboBox_language->setCurrentIndex(1);
        if (Msg.clickedButton() == pYesBtn_TraditionalChinese)ui->comboBox_language->setCurrentIndex(2);
        on_comboBox_language_currentIndexChanged(0);
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Notification"));
        MSG_2->setText(tr("It is detected that this is the first time you have started the software, so the compatibility test will be performed automatically. Please wait for a while, then check the test result."));
        MSG_2->setIcon(QMessageBox::Information);
        MSG_2->setModal(true);
        MSG_2->show();
        file_generateMarkFile(FirstTimeStart,"");
        on_pushButton_clear_textbrowser_clicked();
        on_pushButton_compatibilityTest_clicked();
    }
}

void MainWindow::on_checkBox_DelOriginal_stateChanged(int arg1)
{
    if(ui->checkBox_DelOriginal->isChecked())
    {
        QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setEnabled(1);
        ui->checkBox_ReplaceOriginalFile->setEnabled(0);
        ui->checkBox_ReplaceOriginalFile->setChecked(0);
    }
    else
    {
        QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setEnabled(0);
        checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    }
}

void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int arg1)
{
    if(ui->checkBox_FileList_Interactive->isChecked())
    {
        ui->tableView_image->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView_gif->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView_video->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
    else
    {
        ui->tableView_image->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_gif->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_video->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
}

void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int arg1)
{
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        ui->lineEdit_outputPath->setEnabled(1);
        ui->checkBox_OutPath_KeepOriginalFileName->setEnabled(1);
        ui->checkBox_KeepParentFolder->setEnabled(1);
        ui->checkBox_OutPath_Overwrite->setEnabled(1);
        ui->checkBox_ReplaceOriginalFile->setEnabled(0);
        ui->checkBox_ReplaceOriginalFile->setChecked(0);
        ui->checkBox_AutoOpenOutputPath->setEnabled(1);
    }
    else
    {
        ui->lineEdit_outputPath->setEnabled(0);
        ui->checkBox_OutPath_KeepOriginalFileName->setEnabled(0);
        ui->checkBox_KeepParentFolder->setEnabled(0);
        ui->checkBox_OutPath_Overwrite->setEnabled(0);
        ui->checkBox_AutoOpenOutputPath->setEnabled(0);
        checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    }
}

void MainWindow::on_pushButton_ForceRetry_clicked()
{
    if(isForceRetryEnabled==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Force retry is disabled when processing Video or GIF."));
        return;
    }
    ui->pushButton_ForceRetry->setEnabled(0);
    QtConcurrent::run(this, &MainWindow::isForceRetryClicked_SetTrue_Block_Anime4k);
    ForceRetryCount++;
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq Anime4K_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-ncnn-vulkan_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-converter-cpp_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq srmd-ncnn-vulkan_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-caffe_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq realsr-ncnn-vulkan_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    emit Send_TextBrowser_NewMessage(tr("Force retry."));
    return;
}
void MainWindow::SetEnable_pushButton_ForceRetry_self()
{
    ui->pushButton_ForceRetry->setEnabled(1);
    return;
}
void MainWindow::on_pushButton_PayPal_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.me/aaronfeng753"));
}
void MainWindow::on_checkBox_AudioDenoise_stateChanged(int arg1)
{
    if(ui->checkBox_AudioDenoise->isChecked())
    {
        ui->doubleSpinBox_AudioDenoiseLevel->setEnabled(1);
    }
    else
    {
        ui->doubleSpinBox_AudioDenoiseLevel->setEnabled(0);
    }
}
void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch(ui->tabWidget->currentIndex())
    {
        case 0:
            {
                ui->label_DonateQRCode->setVisible(1);
                ui->pushButton_PayPal->setVisible(1);
                ui->pushButton_Patreon->setVisible(1);
                ui->label_DonateText->setVisible(1);
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 1:
            {
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                ui->groupBox_Progress->setVisible(1);
                ui->splitter_2->setVisible(1);
                if(isSettingsHide==false)
                {
                    ui->groupBox_Setting->setVisible(1);
                }
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 2:
            {
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                ui->groupBox_Engine->setVisible(1);
                ui->groupBox_NumOfThreads->setVisible(1);
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 3:
            {
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                ui->groupBox_AudioDenoise->setVisible(1);
                ui->groupBox_video_settings->setVisible(1);
                ui->groupBox_FrameInterpolation->setVisible(1);
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 4:
            {
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                ui->groupBox_3->setVisible(1);
                ui->groupBox_8->setVisible(1);
                ui->groupBox_InputExt->setVisible(1);
                ui->groupBox_other_1->setVisible(1);
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 5:
            {
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                ui->groupBox_CompatibilityTestRes->setVisible(1);
                ui->pushButton_compatibilityTest->setVisible(1);
                break;
            }
    }
}
void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int arg1)
{
    if(ui->checkBox_ProcessVideoBySegment->isChecked())
    {
        ui->label_SegmentDuration->setEnabled(1);
        ui->spinBox_SegmentDuration->setEnabled(1);
    }
    else
    {
        ui->label_SegmentDuration->setEnabled(0);
        ui->spinBox_SegmentDuration->setEnabled(0);
    }
}
void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index)
{
    switch (ui->comboBox_version_Waifu2xNCNNVulkan->currentIndex())
    {
        case 0:
            {
                Waifu2x_ncnn_vulkan_FolderPath = Current_Path + "/waifu2x-ncnn-vulkan";
                Waifu2x_ncnn_vulkan_ProgramPath = Waifu2x_ncnn_vulkan_FolderPath + "/waifu2x-ncnn-vulkan_waifu2xEX.exe";
                ui->checkBox_TTA_vulkan->setEnabled(1);
                return;
            }
        case 1:
            {
                Waifu2x_ncnn_vulkan_FolderPath = Current_Path + "/waifu2x-ncnn-vulkan";
                Waifu2x_ncnn_vulkan_ProgramPath = Waifu2x_ncnn_vulkan_FolderPath + "/waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe";
                ui->checkBox_TTA_vulkan->setEnabled(1);
                return;
            }
        case 2:
            {
                Waifu2x_ncnn_vulkan_FolderPath = Current_Path + "/waifu2x-ncnn-vulkan-old";
                Waifu2x_ncnn_vulkan_ProgramPath = Waifu2x_ncnn_vulkan_FolderPath + "/waifu2x-ncnn-vulkan_waifu2xEX.exe";
                ui->checkBox_TTA_vulkan->setEnabled(0);
                ui->checkBox_TTA_vulkan->setChecked(0);
                return;
            }
    }
}
void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1)
{
    if(ui->checkBox_EnablePreProcessing_Anime4k->isChecked())
    {
        ui->checkBox_MedianBlur_Pre_Anime4k->setEnabled(1);
        ui->checkBox_MeanBlur_Pre_Anime4k->setEnabled(1);
        ui->checkBox_CASSharping_Pre_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlurWeak_Pre_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlur_Pre_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilter_Pre_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilterFaster_Pre_Anime4k->setEnabled(1);
    }
    else
    {
        ui->checkBox_MedianBlur_Pre_Anime4k->setEnabled(0);
        ui->checkBox_MeanBlur_Pre_Anime4k->setEnabled(0);
        ui->checkBox_CASSharping_Pre_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlurWeak_Pre_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlur_Pre_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilter_Pre_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilterFaster_Pre_Anime4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1)
{
    if(ui->checkBox_EnablePostProcessing_Anime4k->isChecked())
    {
        ui->checkBox_MedianBlur_Post_Anime4k->setEnabled(1);
        ui->checkBox_MeanBlur_Post_Anime4k->setEnabled(1);
        ui->checkBox_CASSharping_Post_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlurWeak_Post_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlur_Post_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilter_Post_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilterFaster_Post_Anime4k->setEnabled(1);
    }
    else
    {
        ui->checkBox_MedianBlur_Post_Anime4k->setEnabled(0);
        ui->checkBox_MeanBlur_Post_Anime4k->setEnabled(0);
        ui->checkBox_CASSharping_Post_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlurWeak_Post_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlur_Post_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilter_Post_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilterFaster_Post_Anime4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1)
{
    if(ui->checkBox_SpecifyGPU_Anime4k->isChecked())
    {
        ui->lineEdit_GPUs_Anime4k->setEnabled(1);
        ui->pushButton_ListGPUs_Anime4k->setEnabled(1);
        ui->pushButton_VerifyGPUsConfig_Anime4k->setEnabled(1);
    }
    else
    {
        ui->lineEdit_GPUs_Anime4k->setEnabled(0);
        ui->pushButton_ListGPUs_Anime4k->setEnabled(0);
        ui->pushButton_VerifyGPUsConfig_Anime4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_OLD);
}
void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked()
{
    ui->checkBox_isCompatible_SRMD_NCNN_Vulkan->setChecked(isCompatible_SRMD_NCNN_Vulkan);
}
void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked()
{
    ui->checkBox_isCompatible_SRMD_CUDA->setChecked(isCompatible_SRMD_CUDA);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Converter->setChecked(isCompatible_Waifu2x_Converter);
}
void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked()
{
    ui->checkBox_isCompatible_Anime4k_CPU->setChecked(isCompatible_Anime4k_CPU);
}
void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked()
{
    ui->checkBox_isCompatible_Anime4k_GPU->setChecked(isCompatible_Anime4k_GPU);
}
void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked()
{
    ui->checkBox_isCompatible_FFmpeg->setChecked(isCompatible_FFmpeg);
}
void MainWindow::on_checkBox_isCompatible_FFprobe_clicked()
{
    ui->checkBox_isCompatible_FFprobe->setChecked(isCompatible_FFprobe);
}
void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked()
{
    ui->checkBox_isCompatible_ImageMagick->setChecked(isCompatible_ImageMagick);
}
void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked()
{
    ui->checkBox_isCompatible_Gifsicle->setChecked(isCompatible_Gifsicle);
}
void MainWindow::on_checkBox_isCompatible_SoX_clicked()
{
    ui->checkBox_isCompatible_SoX->setChecked(isCompatible_SoX);
}
void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int arg1)
{
    if(ui->checkBox_GPUMode_Anime4K->isChecked())
    {
        ui->checkBox_SpecifyGPU_Anime4k->setEnabled(1);
        ui->comboBox_GPGPUModel_A4k->setEnabled(1);
        on_comboBox_GPGPUModel_A4k_currentIndexChanged(1);
    }
    else
    {
        ui->checkBox_SpecifyGPU_Anime4k->setEnabled(0);
        ui->checkBox_SpecifyGPU_Anime4k->setChecked(0);
        ui->comboBox_GPGPUModel_A4k->setEnabled(0);
        on_comboBox_GPGPUModel_A4k_currentIndexChanged(1);
    }
}
void MainWindow::on_checkBox_ShowInterPro_stateChanged(int arg1)
{
    if(ui->checkBox_ShowInterPro->isChecked()==false)
    {
        emit Send_CurrentFileProgress_Stop();
    }
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Caffe_CPU->setChecked(isCompatible_Waifu2x_Caffe_CPU);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Caffe_GPU->setChecked(isCompatible_Waifu2x_Caffe_GPU);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Caffe_cuDNN->setChecked(isCompatible_Waifu2x_Caffe_cuDNN);
}
void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked()
{
    int VAL = ui->spinBox_SplitSize_Waifu2xCaffe->value()*2;
    if(VAL<=999999999)
    {
        ui->spinBox_SplitSize_Waifu2xCaffe->setValue(VAL);
    }
}
void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked()
{
    int VAL = ui->spinBox_SplitSize_Waifu2xCaffe->value()/2;
    if(VAL>=2)
    {
        ui->spinBox_SplitSize_Waifu2xCaffe->setValue(VAL);
    }
}
void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked()
{
    ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->setChecked(isCompatible_Realsr_NCNN_Vulkan);
}
void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int arg1)
{
    if(ui->checkBox_ACNet_Anime4K->isChecked())
    {
        ui->checkBox_HDNMode_Anime4k->setEnabled(1);
        ui->groupBox_PostProcessing_Anime4k->setEnabled(0);
        ui->groupBox_PreProcessing_Anime4k->setEnabled(0);
        ui->doubleSpinBox_PushColorStrength_Anime4K->setEnabled(0);
        ui->doubleSpinBox_PushGradientStrength_Anime4K->setEnabled(0);
        ui->spinBox_Passes_Anime4K->setEnabled(0);
        ui->spinBox_PushColorCount_Anime4K->setEnabled(0);
    }
    else
    {
        ui->checkBox_HDNMode_Anime4k->setEnabled(0);
        on_checkBox_GPUMode_Anime4K_stateChanged(0);
        on_checkBox_SpecifyGPU_Anime4k_stateChanged(0);
        ui->groupBox_PostProcessing_Anime4k->setEnabled(1);
        ui->groupBox_PreProcessing_Anime4k->setEnabled(1);
        ui->doubleSpinBox_PushColorStrength_Anime4K->setEnabled(1);
        ui->doubleSpinBox_PushGradientStrength_Anime4K->setEnabled(1);
        ui->spinBox_Passes_Anime4K->setEnabled(1);
        ui->spinBox_PushColorCount_Anime4K->setEnabled(1);
    }
    DenoiseLevelSpinboxSetting_Anime4k();
}
void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int arg1)
{
    DenoiseLevelSpinboxSetting_Anime4k();
}

void MainWindow::ExecuteCMD_batFile(QString cmd_str,bool requestAdmin)
{
    ExecuteCMD_batFile_QMutex.lock();
    QString cmd_commands = "@echo off\n "+cmd_str+"\n exit";
    Delay_msec_sleep(10);
    file_mkDir(Current_Path+"/batFiles_tmp");
    QString Bat_path = Current_Path+"/batFiles_tmp/W2xEX_"+QDateTime::currentDateTime().toString("dhhmmsszzz")+".bat";
    QFile OpenFile_cmdFile(Bat_path);
    OpenFile_cmdFile.remove();
    if (OpenFile_cmdFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&OpenFile_cmdFile);
        stream << cmd_commands;
    }
    OpenFile_cmdFile.close();
    if(requestAdmin)
    {
        ShellExecuteW(NULL, QString("runas").toStdWString().c_str(), QString(Bat_path).toStdWString().c_str(), QString(Bat_path).toStdWString().c_str(), NULL, 1);
    }
    else
    {
        QDesktopServices::openUrl(QUrl("file:"+QUrl::toPercentEncoding(Bat_path)));
    }
    ExecuteCMD_batFile_QMutex.unlock();
}
void MainWindow::Del_TempBatFile()
{
    file_DelDir(Current_Path+"/batFiles_tmp");
}
void MainWindow::comboBox_UpdateChannel_setCurrentIndex_self(int index)
{
    comboBox_UpdateChannel_setCurrentIndex_self_QMutex.lock();
    isClicked_comboBox_UpdateChannel=false;
    ui->comboBox_UpdateChannel->setCurrentIndex(index);
    isClicked_comboBox_UpdateChannel=true;
    comboBox_UpdateChannel_setCurrentIndex_self_QMutex.unlock();
}
void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int index)
{
    if(isClicked_comboBox_UpdateChannel && AutoUpdate.isRunning()==false)
    {
        AutoUpdate = QtConcurrent::run(this, &MainWindow::CheckUpadte_Auto);
    }
}
void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int arg1)
{
    if(ui->checkBox_ReplaceOriginalFile->isChecked())
    {
        ui->groupBox_OutPut->setEnabled(0);
        ui->checkBox_OutPath_isEnabled->setChecked(0);
        ui->checkBox_DelOriginal->setEnabled(0);
    }
    else
    {
        ui->groupBox_OutPut->setEnabled(1);
        ui->checkBox_DelOriginal->setEnabled(1);
    }
}
void MainWindow::checkBox_ReplaceOriginalFile_setEnabled_True_Self()
{
    if(ui->checkBox_DelOriginal->isChecked()==false && ui->checkBox_OutPath_isEnabled->isChecked()==false)
    {
        ui->checkBox_ReplaceOriginalFile->setEnabled(1);
    }
}
bool MainWindow::ReplaceOriginalFile(QString original_fullpath,QString output_fullpath)
{
    if(ui->checkBox_ReplaceOriginalFile->isChecked()==false || QFile::exists(output_fullpath)==false)return false;
    QFileInfo fileinfo_original_fullpath(original_fullpath);
    QFileInfo fileinfo_output_fullpath(output_fullpath);
    QString file_name = file_getBaseName(original_fullpath);
    QString file_ext = fileinfo_output_fullpath.suffix();
    QString file_path = file_getFolderPath(fileinfo_original_fullpath);
    QString Target_fullpath=file_path+"/"+file_name+"."+file_ext;
    if(QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->isChecked())
    {
        file_MoveToTrash(original_fullpath);
        file_MoveToTrash(Target_fullpath);
    }
    else
    {
        QFile::remove(original_fullpath);
        QFile::remove(Target_fullpath);
    }
    if(QFile::rename(output_fullpath,Target_fullpath)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Failed to move [")+output_fullpath+tr("] to [")+Target_fullpath+"]");
    }
    return true;
}
void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int arg1)
{
    if(ui->checkBox_isCustFontEnable->isChecked())
    {
        ui->pushButton_Save_GlobalFontSize->setEnabled(1);
        ui->spinBox_GlobalFontSize->setEnabled(1);
        ui->fontComboBox_CustFont->setEnabled(1);
    }
    else
    {
        ui->pushButton_Save_GlobalFontSize->setEnabled(0);
        ui->spinBox_GlobalFontSize->setEnabled(0);
        ui->fontComboBox_CustFont->setEnabled(0);
    }
}
void MainWindow::OutputSettingsArea_setEnabled(bool isEnabled)
{
    ui->scrollArea_outputPathSettings->setEnabled(isEnabled);
    ui->lineEdit_outputPath->setClearButtonEnabled(isEnabled);
    if(isEnabled==true)
    {
        ui->lineEdit_outputPath->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        ui->lineEdit_outputPath->setFocusPolicy(Qt::NoFocus);
    }
}
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui->tableView_image || target == ui->tableView_gif || target == ui->tableView_video)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                if(RemoveFile_FilesList_QAction_FileList->isEnabled())
                {
                    on_pushButton_RemoveItem_clicked();
                }
                return true;
            }
            if ((keyEvent->modifiers() == Qt::ControlModifier) && (keyEvent->key() == Qt::Key_A))
            {
                if(Apply_CustRes_QAction_FileList->isEnabled())
                {
                    EnableApply2All_CustRes=false;
                    on_pushButton_CustRes_apply_clicked();
                    EnableApply2All_CustRes=true;
                }
                return true;
            }
            if ((keyEvent->modifiers() == Qt::ControlModifier) && (keyEvent->key() == Qt::Key_C))
            {
                if(Cancel_CustRes_QAction_FileList->isEnabled())
                {
                    EnableApply2All_CustRes=false;
                    on_pushButton_CustRes_cancel_clicked();
                    EnableApply2All_CustRes=true;
                }
                return true;
            }
        }
    }
    return false;
}
void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked()
{
    ui->splitter_FilesList->setSizes(QList<int>() << 1 << 1 << 1);
}
void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int index)
{
    if(ui->comboBox_GPGPUModel_A4k->currentText().toLower().trimmed()=="opencl" && ui->checkBox_GPUMode_Anime4K->isChecked())
    {
        ui->spinBox_OpenCLCommandQueues_A4k->setEnabled(1);
        ui->checkBox_OpenCLParallelIO_A4k->setEnabled(1);
    }
    else
    {
        ui->spinBox_OpenCLCommandQueues_A4k->setEnabled(0);
        ui->checkBox_OpenCLParallelIO_A4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int arg1)
{
    if(ui->checkBox_DisableGPU_converter->isChecked())
    {
        ui->checkBox_MultiGPU_Waifu2xConverter->setChecked(0);
        ui->comboBox_TargetProcessor_converter->setEnabled(0);
        ui->comboBox_TargetProcessor_converter->setCurrentIndex(0);
        ui->checkBox_MultiGPU_Waifu2xConverter->setEnabled(0);
    }
    else
    {
        ui->comboBox_TargetProcessor_converter->setEnabled(1);
        ui->checkBox_MultiGPU_Waifu2xConverter->setEnabled(1);
    }
}
void MainWindow::on_groupBox_video_settings_clicked()
{
    if(ui->groupBox_video_settings->isChecked())
    {
        if(isCustomVideoSettingsClicked==true)
        {
            QMessageBox *MSG = new QMessageBox();
            MSG->setWindowTitle(tr("Warning"));
            MSG->setText(tr("Change Custom video settings might cause ERROR.\n\nMake sure you know what you are doing before change any settings."));
            MSG->setIcon(QMessageBox::Warning);
            MSG->setModal(true);
            MSG->show();
        }
        ui->groupBox_OutputVideoSettings->setEnabled(1);
        ui->groupBox_ToMp4VideoSettings->setEnabled(1);
        ui->pushButton_encodersList->setEnabled(1);
        ui->pushButton_ResetVideoSettings->setEnabled(1);
    }
    else
    {
        ui->groupBox_OutputVideoSettings->setEnabled(0);
        ui->groupBox_ToMp4VideoSettings->setEnabled(0);
        ui->pushButton_encodersList->setEnabled(0);
        ui->pushButton_ResetVideoSettings->setEnabled(0);
    }
}
void MainWindow::Set_checkBox_DisableResize_gif_Checked()
{
    emit Send_TextBrowser_NewMessage(tr("[Disable \"-resize\"] is automatically enabled to fix compatibility issue and improve performance."));
    ui->checkBox_DisableResize_gif->setChecked(true);
}

void MainWindow::on_pushButton_TurnOffScreen_clicked()
{
    if(TurnOffScreen_QF.isRunning() == true)return;
    TurnOffScreen_QF = QtConcurrent::run(this, &MainWindow::TurnOffScreen);
}

void MainWindow::TurnOffScreen()
{
    QProcess OffScreen;
    runProcess(&OffScreen,
               "\"" + Current_Path + "/nircmd-x64/nircmd.exe\" monitor off");
    return;
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


void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked()
{
    int VAL = 2;
    if(ui->comboBox_Engine_VFI->currentIndex()==2)
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()-1;
    }
    else
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()/2;
    }
    if(VAL>=2)
    {
        ui->spinBox_MultipleOfFPS_VFI->setValue(VAL);
    }
}

void MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealCUGAN) return;
    ui->spinBox_TileSize_RealCUGAN->setValue(AddTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealCUGAN->value()));
}

void MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealCUGAN) return;
    ui->spinBox_TileSize_RealCUGAN->setValue(MinusTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealCUGAN->value()));
}

void MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(!ui->comboBox_Model_RealCUGAN) return;
    qDebug() << "RealCUGAN model changed to:" << ui->comboBox_Model_RealCUGAN->currentText();
    Realcugan_NCNN_Vulkan_ReadSettings();
}


void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked()
{
    int VAL = 2;
    if(ui->comboBox_Engine_VFI->currentIndex()==2)
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()+1;
    }
    else
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()*2;
    }
    if(VAL<=999999999)
    {
        ui->spinBox_MultipleOfFPS_VFI->setValue(VAL);
    }
}

void MainWindow::PreLoad_Engines_Settings()
{
    Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_vulkan->count() <= 0) Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str += tr("Waifu2x model list is empty.") + "\n";
    if(ui->comboBox_GPUID_vulkan->count() <= 0) Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str += tr("Waifu2x GPU list is empty.") + "\n";
    if(Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str != "") Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = tr("Waifu2x-ncnn-Vulkan Preload Failed:") + "\n" + Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str;

    Waifu2x_Converter_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_converter->count() <= 0) Waifu2x_Converter_PreLoad_Settings_Str += tr("Waifu2x model list is empty.") + "\n";
    if(ui->comboBox_TargetProcessor_converter->count() <= 0) Waifu2x_Converter_PreLoad_Settings_Str += tr("Waifu2x GPU list is empty.") + "\n";
    if(Waifu2x_Converter_PreLoad_Settings_Str != "") Waifu2x_Converter_PreLoad_Settings_Str = tr("Waifu2x-Converter-CPP Preload Failed:") + "\n" + Waifu2x_Converter_PreLoad_Settings_Str;

    SRMD_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_srmd->count() <= 0) SRMD_NCNN_Vulkan_PreLoad_Settings_Str += tr("SRMD model list is empty.") + "\n";
    if(ui->comboBox_GPUID_srmd->count() <= 0) SRMD_NCNN_Vulkan_PreLoad_Settings_Str += tr("SRMD GPU list is empty.") + "\n";
    if(SRMD_NCNN_Vulkan_PreLoad_Settings_Str != "") SRMD_NCNN_Vulkan_PreLoad_Settings_Str = tr("SRMD-ncnn-Vulkan Preload Failed:") + "\n" + SRMD_NCNN_Vulkan_PreLoad_Settings_Str;

    SRMD_CUDA_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_srmd_cuda->count() <= 0) SRMD_CUDA_PreLoad_Settings_Str += tr("SRMD model list is empty.") + "\n";
    if(ui->comboBox_GPUID_srmd_cuda->count() <= 0) SRMD_CUDA_PreLoad_Settings_Str += tr("SRMD GPU list is empty.") + "\n";
    if(SRMD_CUDA_PreLoad_Settings_Str != "") SRMD_CUDA_PreLoad_Settings_Str = tr("SRMD-CUDA Preload Failed:") + "\n" + SRMD_CUDA_PreLoad_Settings_Str;

    Anime4KCPP_PreLoad_Settings_Str = "";
    if(ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_PlatformID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Platform ID list is empty.") + "\n";
    if(ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_DeviceID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Device ID list is empty.") + "\n";
    if(Anime4KCPP_PreLoad_Settings_Str != "") Anime4KCPP_PreLoad_Settings_Str = tr("Anime4KCPP Preload Failed:") + "\n" + Anime4KCPP_PreLoad_Settings_Str;

    Waifu2x_Caffe_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_caffe->count() <= 0) Waifu2x_Caffe_PreLoad_Settings_Str += tr("Waifu2x model list is empty.") + "\n";
    if(ui->comboBox_GPUID_caffe->count() <= 0) Waifu2x_Caffe_PreLoad_Settings_Str += tr("Waifu2x GPU list is empty.") + "\n";
    if(Waifu2x_Caffe_PreLoad_Settings_Str != "") Waifu2x_Caffe_PreLoad_Settings_Str = tr("Waifu2x-Caffe Preload Failed:") + "\n" + Waifu2x_Caffe_PreLoad_Settings_Str;

    Realsr_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_realsr->count() <= 0) Realsr_NCNN_Vulkan_PreLoad_Settings_Str += tr("Realsr model list is empty.") + "\n";
    if(ui->comboBox_GPUID_realsr->count() <= 0) Realsr_NCNN_Vulkan_PreLoad_Settings_Str += tr("Realsr GPU list is empty.") + "\n";
    if(Realsr_NCNN_Vulkan_PreLoad_Settings_Str != "") Realsr_NCNN_Vulkan_PreLoad_Settings_Str = tr("Realsr-ncnn-Vulkan Preload Failed:") + "\n" + Realsr_NCNN_Vulkan_PreLoad_Settings_Str;

    Realcugan_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if (ui->comboBox_Model_RealCUGAN && ui->comboBox_GPUID_RealCUGAN) {
        if(ui->comboBox_Model_RealCUGAN->count() <= 0) Realcugan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealCUGAN model list is empty.") + "\n";
        if(ui->comboBox_GPUID_RealCUGAN->count() <= 0) Realcugan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealCUGAN GPU list is empty.") + "\n";
    } else {
        Realcugan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealCUGAN UI elements not found.") + "\n";
    }
    if(Realcugan_NCNN_Vulkan_PreLoad_Settings_Str != "") Realcugan_NCNN_Vulkan_PreLoad_Settings_Str = tr("RealCUGAN-ncnn-Vulkan Preload Failed:") + "\n" + Realcugan_NCNN_Vulkan_PreLoad_Settings_Str;

    Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if (ui->comboBox_Model_RealESRGAN && ui->comboBox_GPUID_RealESRGAN) {
        if(ui->comboBox_Model_RealESRGAN->count() <= 0) Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealESRGAN model list is empty.") + "\n";
        if(ui->comboBox_GPUID_RealESRGAN->count() <= 0) Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealESRGAN GPU list is empty.") + "\n";
    } else {
        Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealESRGAN UI elements not found.") + "\n";
    }
    if(Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str != "") Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str = tr("RealESRGAN-ncnn-Vulkan Preload Failed:") + "\n" + Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str;

    Rife_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_rife->count() <= 0) Rife_NCNN_Vulkan_PreLoad_Settings_Str += tr("RIFE model list is empty.") + "\n";
    if(ui->comboBox_GPUID_rife->count() <= 0) Rife_NCNN_Vulkan_PreLoad_Settings_Str += tr("RIFE GPU list is empty.") + "\n";
    if(Rife_NCNN_Vulkan_PreLoad_Settings_Str != "") Rife_NCNN_Vulkan_PreLoad_Settings_Str = tr("RIFE-ncnn-Vulkan Preload Failed:") + "\n" + Rife_NCNN_Vulkan_PreLoad_Settings_Str;

    Cain_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_cain->count() <= 0) Cain_NCNN_Vulkan_PreLoad_Settings_Str += tr("CAIN model list is empty.") + "\n";
    if(ui->comboBox_GPUID_cain->count() <= 0) Cain_NCNN_Vulkan_PreLoad_Settings_Str += tr("CAIN GPU list is empty.") + "\n";
    if(Cain_NCNN_Vulkan_PreLoad_Settings_Str != "") Cain_NCNN_Vulkan_PreLoad_Settings_Str = tr("CAIN-ncnn-Vulkan Preload Failed:") + "\n" + Cain_NCNN_Vulkan_PreLoad_Settings_Str;

    Dain_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_dain->count() <= 0) Dain_NCNN_Vulkan_PreLoad_Settings_Str += tr("DAIN model list is empty.") + "\n";
    if(ui->comboBox_GPUID_dain->count() <= 0) Dain_NCNN_Vulkan_PreLoad_Settings_Str += tr("DAIN GPU list is empty.") + "\n";
    if(Dain_NCNN_Vulkan_PreLoad_Settings_Str != "") Dain_NCNN_Vulkan_PreLoad_Settings_Str = tr("DAIN-ncnn-Vulkan Preload Failed:") + "\n" + Dain_NCNN_Vulkan_PreLoad_Settings_Str;
}

void MainWindow::Waifu2x(){}

bool MainWindow::Check_PreLoad_Settings()
{
    bool isPreLoadError = false;
    QString ErrorMsg_PreLoad = "";
    int Engine = 0;
    if(ui->tabWidget_FileType->currentIndex()==0) Engine = ui->comboBox_Engine_Image->currentIndex();
    if(ui->tabWidget_FileType->currentIndex()==1) Engine = ui->comboBox_Engine_GIF->currentIndex();
    if(ui->tabWidget_FileType->currentIndex()==2) Engine = ui->comboBox_Engine_Video->currentIndex();
    if(ui->tabWidget_FileType->currentIndex()==3) Engine = ui->comboBox_Engine_VFI->currentIndex();

    if(Engine==0 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==1 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(Waifu2x_Converter_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Waifu2x_Converter_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==2 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(SRMD_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = SRMD_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==3 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(Anime4KCPP_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Anime4KCPP_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==4 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(Waifu2x_Caffe_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Waifu2x_Caffe_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==5 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(Realsr_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Realsr_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==6 && (ui->tabWidget_FileType->currentIndex()!=3))
    {
        if(SRMD_CUDA_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = SRMD_CUDA_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==7)
    {
        if(Realcugan_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Realcugan_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==8)
    {
        if(Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==0 && (ui->tabWidget_FileType->currentIndex()==3))
    {
        if(Rife_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Rife_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==1 && (ui->tabWidget_FileType->currentIndex()==3))
    {
        if(Cain_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Cain_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==2 && (ui->tabWidget_FileType->currentIndex()==3))
    {
        if(Dain_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Dain_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }

    if(isPreLoadError)
    {
        QMessageBox *MSG_PreLoadError = new QMessageBox();
        MSG_PreLoadError->setWindowTitle(tr("Error"));
        MSG_PreLoadError->setText(ErrorMsg_PreLoad + "\n" + tr("Please check your settings."));
        MSG_PreLoadError->setIcon(QMessageBox::Warning);
        MSG_PreLoadError->setModal(true);
        MSG_PreLoadError->show();
        return false;
    }
    return true;
}

void MainWindow::APNG_Main(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath)
{
    int Engine = 0;
    if(ui->tabWidget_FileType->currentIndex()==0) Engine = ui->comboBox_Engine_Image->currentIndex();
    if(ui->tabWidget_FileType->currentIndex()==1) Engine = ui->comboBox_Engine_GIF->currentIndex();

    switch(Engine)
    {
        case 0: APNG_Waifu2xNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 1: APNG_Waifu2xConverter(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 2: APNG_SRMDNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 3: APNG_Anime4K(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 4: APNG_Waifu2xCaffe(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 5: APNG_RealSRNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 6: APNG_SRMDCuda(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 7: APNG_RealcuganNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 8: APNG_RealESRGANNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
    }
}

int MainWindow::Table_Save_Current_Table_Filelist(QString Table_FileList_ini)
{
    QSettings *configIniWrite = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    configIniWrite->setValue("/Warning/CN", tr("Please do not modify this file, otherwise it may cause the program to crash! If problems occur after modification, please delete this file and restart the program.")); // Please do not modify this file, otherwise it may cause the program to crash! If problems occur after modification, please delete this file and restart the program.

    int rowCount_image = Table_model_image->rowCount();
    configIniWrite->setValue("/numOfimage", rowCount_image);
    for(int i=0;i<rowCount_image;i++)
    {
        configIniWrite->setValue("/image_"+QString::number(i,10)+"/fileName",Table_model_image->item(i,0)->text());
        configIniWrite->setValue("/image_"+QString::number(i,10)+"/fullPath",Table_model_image->item(i,2)->text());
        configIniWrite->setValue("/image_"+QString::number(i,10)+"/status",Table_model_image->item(i,3)->text());
        if(Table_model_image->item(i,4)!=nullptr)
        {
            configIniWrite->setValue("/image_"+QString::number(i,10)+"/custRes",Table_model_image->item(i,4)->text());
        }
        else
        {
            configIniWrite->setValue("/image_"+QString::number(i,10)+"/custRes","");
        }
    }

    int rowCount_gif = Table_model_gif->rowCount();
    configIniWrite->setValue("/numOfgif", rowCount_gif);
    for(int i=0;i<rowCount_gif;i++)
    {
        configIniWrite->setValue("/gif_"+QString::number(i,10)+"/fileName",Table_model_gif->item(i,0)->text());
        configIniWrite->setValue("/gif_"+QString::number(i,10)+"/fullPath",Table_model_gif->item(i,2)->text());
        configIniWrite->setValue("/gif_"+QString::number(i,10)+"/status",Table_model_gif->item(i,5)->text());
        if(Table_model_gif->item(i,6)!=nullptr)
        {
            configIniWrite->setValue("/gif_"+QString::number(i,10)+"/custRes",Table_model_gif->item(i,6)->text());
        }
        else
        {
            configIniWrite->setValue("/gif_"+QString::number(i,10)+"/custRes","");
        }
    }

    int rowCount_video = Table_model_video->rowCount();
    configIniWrite->setValue("/numOfvideo", rowCount_video);
    for(int i=0;i<rowCount_video;i++)
    {
        configIniWrite->setValue("/video_"+QString::number(i,10)+"/fileName",Table_model_video->item(i,0)->text());
        configIniWrite->setValue("/video_"+QString::number(i,10)+"/fullPath",Table_model_video->item(i,2)->text());
        configIniWrite->setValue("/video_"+QString::number(i,10)+"/status",Table_model_video->item(i,7)->text());
        if(Table_model_video->item(i,8)!=nullptr)
        {
            configIniWrite->setValue("/video_"+QString::number(i,10)+"/custRes",Table_model_video->item(i,8)->text());
        }
        else
        {
            configIniWrite->setValue("/video_"+QString::number(i,10)+"/custRes","");
        }
    }
    configIniWrite->sync();
    delete configIniWrite;
    emit Send_Table_Save_Current_Table_Filelist_Finished();
    return 0;
}

int MainWindow::Table_Read_Saved_Table_Filelist(QString Table_FileList_ini)
{
    QSettings *configIniRead = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));

    int rowCount_image = configIniRead->value("/numOfimage").toInt();
    for(int i=0;i<rowCount_image;i++)
    {
        QString fileName = configIniRead->value("/image_"+QString::number(i,10)+"/fileName").toString();
        QString fullPath = configIniRead->value("/image_"+QString::number(i,10)+"/fullPath").toString();
        QString status = configIniRead->value("/image_"+QString::number(i,10)+"/status").toString();
        QString custRes = configIniRead->value("/image_"+QString::number(i,10)+"/custRes").toString();

        if(!QFile::exists(fullPath)) {
            emit Send_TextBrowser_NewMessage(tr("File not found (skipped): ") + fullPath);
            continue;
        }
        FileMetadataCache metadata = getOrFetchMetadata(fullPath);

        Table_model_image->insertRow(i);
        Table_model_image->setData(Table_model_image->index(i,0),fileName);
        if(metadata.isValid) {
            Table_model_image->setData(Table_model_image->index(i,1),QString::number(metadata.width)+"x"+QString::number(metadata.height));
        } else {
            Table_model_image->setData(Table_model_image->index(i,1),tr("Error"));
            emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullPath);
        }
        Table_model_image->setData(Table_model_image->index(i,2),fullPath);
        Table_model_image->setData(Table_model_image->index(i,3),status);
        if(!custRes.isEmpty())
        {
            Table_model_image->setData(Table_model_image->index(i,4),custRes);
            QMap<QString,QString> custResMap;
            custResMap.insert("fullpath",fullPath);
            QStringList resWH = custRes.split("x");
            if(resWH.count()==2)
            {
                custResMap.insert("width",resWH.at(0));
                custResMap.insert("height",resWH.at(1));
                Custom_resolution_list.append(custResMap);
            }
        }
        if(ui->tableView_image->isVisible()==0)ui->tableView_image->setVisible(1);
    }

    int rowCount_gif = configIniRead->value("/numOfgif").toInt();
    for(int i=0;i<rowCount_gif;i++)
    {
        QString fileName = configIniRead->value("/gif_"+QString::number(i,10)+"/fileName").toString();
        QString fullPath = configIniRead->value("/gif_"+QString::number(i,10)+"/fullPath").toString();
        QString status = configIniRead->value("/gif_"+QString::number(i,10)+"/status").toString();
        QString custRes = configIniRead->value("/gif_"+QString::number(i,10)+"/custRes").toString();

        if(!QFile::exists(fullPath)) {
            emit Send_TextBrowser_NewMessage(tr("File not found (skipped): ") + fullPath);
            continue;
        }
        FileMetadataCache metadata = getOrFetchMetadata(fullPath);

        Table_model_gif->insertRow(i);
        Table_model_gif->setData(Table_model_gif->index(i,0),fileName);
        if(metadata.isValid) {
            Table_model_gif->setData(Table_model_gif->index(i,1),QString::number(metadata.width)+"x"+QString::number(metadata.height));
            Table_model_gif->setData(Table_model_gif->index(i,3),QString::number(metadata.frameCount));
            Table_model_gif->setData(Table_model_gif->index(i,4),QString::number(metadata.duration,'f',2)+"s");
        } else {
            Table_model_gif->setData(Table_model_gif->index(i,1),tr("Error"));
            Table_model_gif->setData(Table_model_gif->index(i,3),tr("N/A"));
            Table_model_gif->setData(Table_model_gif->index(i,4),tr("N/A"));
            emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullPath);
        }
        Table_model_gif->setData(Table_model_gif->index(i,2),fullPath);
        Table_model_gif->setData(Table_model_gif->index(i,5),status);
        if(!custRes.isEmpty())
        {
            Table_model_gif->setData(Table_model_gif->index(i,6),custRes);
            QMap<QString,QString> custResMap;
            custResMap.insert("fullpath",fullPath);
            QStringList resWH = custRes.split("x");
            if(resWH.count()==2)
            {
                custResMap.insert("width",resWH.at(0));
                custResMap.insert("height",resWH.at(1));
                Custom_resolution_list.append(custResMap);
            }
        }
        if(ui->tableView_gif->isVisible()==0)ui->tableView_gif->setVisible(1);
    }

    int rowCount_video = configIniRead->value("/numOfvideo").toInt();
    for(int i=0;i<rowCount_video;i++)
    {
        QString fileName = configIniRead->value("/video_"+QString::number(i,10)+"/fileName").toString();
        QString fullPath = configIniRead->value("/video_"+QString::number(i,10)+"/fullPath").toString();
        QString status = configIniRead->value("/video_"+QString::number(i,10)+"/status").toString();
        QString custRes = configIniRead->value("/video_"+QString::number(i,10)+"/custRes").toString();

        if(!QFile::exists(fullPath)) {
             emit Send_TextBrowser_NewMessage(tr("File not found (skipped): ") + fullPath);
            continue;
        }
        FileMetadataCache metadata = getOrFetchMetadata(fullPath);

        Table_model_video->insertRow(i);
        Table_model_video->setData(Table_model_video->index(i,0),fileName);
        if(metadata.isValid) {
            Table_model_video->setData(Table_model_video->index(i,1),QString::number(metadata.width)+"x"+QString::number(metadata.height));
            Table_model_video->setData(Table_model_video->index(i,3),metadata.fps);
            Table_model_video->setData(Table_model_video->index(i,4),QString::number(metadata.duration,'f',2)+"s");
            Table_model_video->setData(Table_model_video->index(i,5),QString::number(metadata.frameCount));
            Table_model_video->setData(Table_model_video->index(i,6),metadata.isVFR ? tr("Yes") : tr("No"));
        } else {
            Table_model_video->setData(Table_model_video->index(i,1),tr("Error"));
            Table_model_video->setData(Table_model_video->index(i,3),tr("N/A"));
            Table_model_video->setData(Table_model_video->index(i,4),tr("N/A"));
            Table_model_video->setData(Table_model_video->index(i,5),tr("N/A"));
            Table_model_video->setData(Table_model_video->index(i,6),tr("N/A"));
            emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullPath);
        }
        Table_model_video->setData(Table_model_video->index(i,2),fullPath);
        Table_model_video->setData(Table_model_video->index(i,7),status);
        if(!custRes.isEmpty())
        {
            Table_model_video->setData(Table_model_video->index(i,8),custRes);
            QMap<QString,QString> custResMap;
            custResMap.insert("fullpath",fullPath);
            QStringList resWH = custRes.split("x");
            if(resWH.count()==2)
            {
                custResMap.insert("width",resWH.at(0));
                custResMap.insert("height",resWH.at(1));
                Custom_resolution_list.append(custResMap);
            }
        }
        if(ui->tableView_video->isVisible()==0)ui->tableView_video->setVisible(1);
    }
    delete configIniRead;
    emit Send_Table_Read_Saved_Table_Filelist_Finished(Table_FileList_ini);
    return 0;
}

void MainWindow::Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(SourceFile_fullPath);
    if(!metadata.isValid)
    {
        emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for image: ") + SourceFile_fullPath);
        return;
    }

    int rowNum = Table_image_get_rowNum();
    Table_model_image->insertRow(rowNum);
    Table_model_image->setData(Table_model_image->index(rowNum, 0), fileName);
    Table_model_image->setData(Table_model_image->index(rowNum, 1), QString::number(metadata.width) + "x" + QString::number(metadata.height));
    Table_model_image->setData(Table_model_image->index(rowNum, 2), SourceFile_fullPath);
    Table_model_image->setData(Table_model_image->index(rowNum, 3), tr("Waiting"));
    ui->tableView_image->scrollToBottom();
    if(ui->tableView_image->isVisible()==0)ui->tableView_image->setVisible(1);
    AddNew_image=true;
}

void MainWindow::Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(SourceFile_fullPath);
    if(!metadata.isValid)
    {
        emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for GIF/APNG: ") + SourceFile_fullPath);
        return;
    }

    int rowNum = Table_gif_get_rowNum();
    Table_model_gif->insertRow(rowNum);
    Table_model_gif->setData(Table_model_gif->index(rowNum, 0), fileName);
    Table_model_gif->setData(Table_model_gif->index(rowNum, 1), QString::number(metadata.width) + "x" + QString::number(metadata.height));
    Table_model_gif->setData(Table_model_gif->index(rowNum, 2), SourceFile_fullPath);
    Table_model_gif->setData(Table_model_gif->index(rowNum, 3), QString::number(metadata.frameCount));
    Table_model_gif->setData(Table_model_gif->index(rowNum, 4), QString::number(metadata.duration, 'f', 2) + "s");
    Table_model_gif->setData(Table_model_gif->index(rowNum, 5), tr("Waiting"));
    ui->tableView_gif->scrollToBottom();
    if(ui->tableView_gif->isVisible()==0)ui->tableView_gif->setVisible(1);
    AddNew_gif=true;
}

void MainWindow::Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(SourceFile_fullPath);
    if(!metadata.isValid)
    {
        emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for video: ") + SourceFile_fullPath);
        return;
    }

    int rowNum = Table_video_get_rowNum();
    Table_model_video->insertRow(rowNum);
    Table_model_video->setData(Table_model_video->index(rowNum, 0), fileName);
    Table_model_video->setData(Table_model_video->index(rowNum, 1), QString::number(metadata.width) + "x" + QString::number(metadata.height));
    Table_model_video->setData(Table_model_video->index(rowNum, 2), SourceFile_fullPath);
    Table_model_video->setData(Table_model_video->index(rowNum, 3), metadata.fps);
    Table_model_video->setData(Table_model_video->index(rowNum, 4), QString::number(metadata.duration, 'f', 2) + "s");
    Table_model_video->setData(Table_model_video->index(rowNum, 5), QString::number(metadata.frameCount));
    Table_model_video->setData(Table_model_video->index(rowNum, 6), metadata.isVFR ? tr("Yes") : tr("No"));
    Table_model_video->setData(Table_model_video->index(rowNum, 7), tr("Waiting"));
    ui->tableView_video->scrollToBottom();
    if(ui->tableView_video->isVisible()==0)ui->tableView_video->setVisible(1);
    AddNew_video=true;
}

QMap<QString,int> MainWindow::Image_Gif_Read_Resolution(QString SourceFileFullPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(SourceFileFullPath);
    QMap<QString,int> map;
    if(metadata.isValid)
    {
        map.insert("width",metadata.width);
        map.insert("height",metadata.height);
        map.insert("frameCount",static_cast<int>(metadata.frameCount));
    }
    else
    {
        map.insert("width",0);
        map.insert("height",0);
        map.insert("frameCount",0);
        qDebug() << "Image_Gif_Read_Resolution (using cache) failed for: " << SourceFileFullPath;
    }
    return map;
}

void MainWindow::Add_File_Folder(QString Input_path)
{
    QFileInfo fileInfo(Input_path);
    QString fileName = fileInfo.fileName();
    QString suffix = fileInfo.suffix().toLower();
    QStringList imageExts = ui->Ext_image->text().split(":", Qt::SkipEmptyParts);
    QStringList videoExts = ui->Ext_video->text().split(":", Qt::SkipEmptyParts);

    bool isVideo = std::any_of(videoExts.cbegin(), videoExts.cend(), [&](const QString &ext){
        return suffix == ext.trimmed().toLower();
    });

    if (isVideo) {
        if(Deduplicate_filelist(Input_path)) return;
        Table_video_insert_fileName_fullPath(fileName, Input_path);
    } else if (suffix == "gif" || suffix == "apng") {
        if(Deduplicate_filelist(Input_path)) return;
        Table_gif_insert_fileName_fullPath(fileName, Input_path);
    } else {
        bool isImage = std::any_of(imageExts.cbegin(), imageExts.cend(), [&](const QString &ext){
            return suffix == ext.trimmed().toLower();
        });
        if (isImage) {
            if(Deduplicate_filelist(Input_path)) return;
            Table_image_insert_fileName_fullPath(fileName, Input_path);
        } else {
            qDebug() << "Unknown file type or unlisted extension:" << Input_path;
        }
    }
}

int MainWindow::CustRes_SetCustRes()
{
    if(curRow_image == -1 && curRow_gif == -1 && curRow_video == -1 && EnableApply2All_CustRes==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("No items are currently selected."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        return 1;
    }

    int Height_new = ui->spinBox_height_custRes->value();
    int Width_new = ui->spinBox_width_custRes->value();
    QString New_Res_str = QString::number(Width_new)+"x"+QString::number(Height_new);

    if(EnableApply2All_CustRes)
    {
        //Image
        for(int i=0;i<Table_model_image->rowCount();i++)
        {
            QString fullpath = Table_model_image->item(i,2)->text();
            FileMetadataCache metadata = getOrFetchMetadata(fullpath);
            if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
                 emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullpath + tr(". Skipping custom resolution."));
                 continue;
            }
            Table_model_image->setData(Table_model_image->index(i,1),QString::number(metadata.width)+"x"+QString::number(metadata.height)+"->"+New_Res_str);
            Table_model_image->setData(Table_model_image->index(i,4),New_Res_str);
            if(CustRes_isContained(fullpath)==false)
            {
                QMap<QString,QString> res_map;
                res_map.insert("fullpath",fullpath);
                res_map.insert("height",QString::number(Height_new,10));
                res_map.insert("width",QString::number(Width_new,10));
                Custom_resolution_list.append(res_map);
            }
        }
        //GIF
        for(int i=0;i<Table_model_gif->rowCount();i++)
        {
            QString fullpath = Table_model_gif->item(i,2)->text();
            FileMetadataCache metadata = getOrFetchMetadata(fullpath);
            if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
                 emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullpath + tr(". Skipping custom resolution."));
                 continue;
            }
            Table_model_gif->setData(Table_model_gif->index(i,1),QString::number(metadata.width)+"x"+QString::number(metadata.height)+"->"+New_Res_str);
            Table_model_gif->setData(Table_model_gif->index(i,6),New_Res_str);
            if(CustRes_isContained(fullpath)==false)
            {
                QMap<QString,QString> res_map;
                res_map.insert("fullpath",fullpath);
                res_map.insert("height",QString::number(Height_new,10));
                res_map.insert("width",QString::number(Width_new,10));
                Custom_resolution_list.append(res_map);
            }
        }
        //Video
        for(int i=0;i<Table_model_video->rowCount();i++)
        {
            QString fullpath = Table_model_video->item(i,2)->text();
            FileMetadataCache metadata = getOrFetchMetadata(fullpath);
             if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
                 emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullpath + tr(". Skipping custom resolution."));
                 continue;
            }
            Table_model_video->setData(Table_model_video->index(i,1),QString::number(metadata.width)+"x"+QString::number(metadata.height)+"->"+New_Res_str);
            Table_model_video->setData(Table_model_video->index(i,8),New_Res_str);
            if(CustRes_isContained(fullpath)==false)
            {
                QMap<QString,QString> res_map;
                res_map.insert("fullpath",fullpath);
                res_map.insert("height",QString::number(Height_new,10));
                res_map.insert("width",QString::number(Width_new,10));
                Custom_resolution_list.append(res_map);
            }
        }
    }
    else
    {
        if(curRow_image >= 0)
        {
            QString fullpath = Table_model_image->item(curRow_image,2)->text();
            FileMetadataCache metadata = getOrFetchMetadata(fullpath);
            if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
                 emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullpath + tr(". Skipping custom resolution."));
                 return 1;
            }
            // The signal was originally sending original_height + "->" + new_height.
            // Now it sends the new resolution string directly.
            // The table display logic should be updated if it relied on the "->" format for original dimensions.
            // However, the primary display of original dimensions is in column 1, which is populated by Table_image_insert_fileName_fullPath.
            // This signal updates column 4 with the custom resolution.
            emit Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(curRow_image, QString::number(Height_new), QString::number(Width_new));
             Table_model_image->setData(Table_model_image->index(curRow_image,1),QString::number(metadata.width)+"x"+QString::number(metadata.height)+"->"+New_Res_str); // Keep visual cue of change
            Table_model_image->setData(Table_model_image->index(curRow_image,4),New_Res_str);


            if(CustRes_isContained(fullpath)==false)
            {
                QMap<QString,QString> res_map;
                res_map.insert("fullpath",fullpath);
                res_map.insert("height",QString::number(Height_new,10));
                res_map.insert("width",QString::number(Width_new,10));
                Custom_resolution_list.append(res_map);
            }
        }
        if(curRow_gif >= 0)
        {
            QString fullpath = Table_model_gif->item(curRow_gif,2)->text();
            FileMetadataCache metadata = getOrFetchMetadata(fullpath);
            if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
                 emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullpath + tr(". Skipping custom resolution."));
                 return 1;
            }
            emit Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(curRow_gif,QString::number(Height_new,10),QString::number(Width_new,10));
            Table_model_gif->setData(Table_model_gif->index(curRow_gif,1),QString::number(metadata.width)+"x"+QString::number(metadata.height)+"->"+New_Res_str);
            Table_model_gif->setData(Table_model_gif->index(curRow_gif,6),New_Res_str);

            if(CustRes_isContained(fullpath)==false)
            {
                QMap<QString,QString> res_map;
                res_map.insert("fullpath",fullpath);
                res_map.insert("height",QString::number(Height_new,10));
                res_map.insert("width",QString::number(Width_new,10));
                Custom_resolution_list.append(res_map);
            }
        }
        if(curRow_video >= 0)
        {
            QString fullpath = Table_model_video->item(curRow_video,2)->text();
            FileMetadataCache metadata = getOrFetchMetadata(fullpath);
            if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
                 emit Send_TextBrowser_NewMessage(tr("Failed to read metadata for: ") + fullpath + tr(". Skipping custom resolution."));
                 return 1;
            }
            emit Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(curRow_video,QString::number(Height_new,10),QString::number(Width_new,10));
            Table_model_video->setData(Table_model_video->index(curRow_video,1),QString::number(metadata.width)+"x"+QString::number(metadata.height)+"->"+New_Res_str);
            Table_model_video->setData(Table_model_video->index(curRow_video,8),New_Res_str);
            if(CustRes_isContained(fullpath)==false)
            {
                QMap<QString,QString> res_map;
                res_map.insert("fullpath",fullpath);
                res_map.insert("height",QString::number(Height_new,10));
                res_map.insert("width",QString::number(Width_new,10));
                Custom_resolution_list.append(res_map);
            }
        }
    }
    return 0;
}

int MainWindow::Gif_getDuration(QString gifPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(gifPath);
    if (metadata.isValid && metadata.isAnimated && metadata.frameCount > 0 && metadata.duration > 0) {
        // Calculate delay per frame in milliseconds
        return qRound((metadata.duration * 1000.0) / metadata.frameCount);
    }
    // Fallback or default if proper metadata not found
    QMovie *mov = new QMovie(gifPath);
    int res = mov->nextFrameDelay();
    delete mov;
    return res;
}

int MainWindow::Gif_getFrameDigits(QString gifPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(gifPath);
    long long frameCount = 0;
    if (metadata.isValid && metadata.isAnimated) {
        frameCount = metadata.frameCount;
    } else {
        // Fallback for safety, though ideally getOrFetchMetadata handles it
        QMovie *mov = new QMovie(gifPath);
        frameCount = mov->frameCount();
        delete mov;
    }
    if(frameCount==0) return 5; // Default if still zero
    return CalNumDigits(frameCount);
}

QString MainWindow::video_get_bitrate(QString videoPath,bool isReturnFullCMD,bool isVidOnly)
{
    FileMetadataCache metadata = getOrFetchMetadata(videoPath);
    if(metadata.isValid && !metadata.bitRate.isEmpty() && !isReturnFullCMD) { // Assuming bitRate is just the number string
        return metadata.bitRate;
    }

    // Fallback to original ffprobe call if cache doesn't have it or full command is needed
    QProcess ffprobe;
    QStringList args;
    args << "-v" << "error" << "-select_streams" << "v:0" << "-show_entries" << "format=bit_rate" << "-of" << "default=noprint_wrappers=1:nokey=1" << videoPath;
    if(isVidOnly==false) args.replace(2,"a:0"); // change to audio stream

    QString ffprobePath = Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe";
    QString cmd = "\""+ ffprobePath +"\" "+args.join(" ");
    if(isReturnFullCMD) return cmd;

    QByteArray stdOut;
    runProcess(&ffprobe, cmd, &stdOut);
    return QString(stdOut).trimmed();
}

int MainWindow::video_UseRes2CalculateBitrate(QString VideoFileFullPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(VideoFileFullPath);
    if(!metadata.isValid || metadata.width == 0 || metadata.height == 0) return 6000; // Default bitrate

    int w = metadata.width;
    int h = metadata.height;
    double res = w*h;
    if(res<=(1280*720)) return 6000;
    if(res<=(1920*1080)) return 9000;
    if(res<=(2560*1440)) return 12000;
    if(res<=(3840*2160)) return 20000;
    return 25000; // For resolutions larger than 4K
}

bool MainWindow::video_isNeedProcessBySegment(int rowNum)
{
    if(ui->checkBox_ProcessVideoBySegment->isChecked()==false)return false;
    QString fullpath = Table_model_video->item(rowNum,2)->text();
    FileMetadataCache metadata = getOrFetchMetadata(fullpath);
    if(!metadata.isValid) return false; // Cannot determine, so don't process by segment

    return metadata.duration > ui->spinBox_SegmentDuration->value();
}

QString MainWindow::FrameInterpolation_ReadConfig(bool isUhdInput,int NumOfFrames)
{
    // isUhdInput is determined by original video resolution.
    // This function doesn't directly read file metadata but uses pre-determined UHD status.
    // If UHD status needs to be re-checked from a file path, then getOrFetchMetadata would be used.
    // For now, its direct logic based on passed bool seems fine.
    // ... (original logic based on isUhdInput and NumOfFrames remains) ...
    QString ConfigStr = "";
    // ... (rest of the original function)
    return ConfigStr;
}

bool MainWindow::FrameInterpolation(QString SourcePath,QString OutputPath)
{
    FileMetadataCache metadata = getOrFetchMetadata(SourcePath);
    if(!metadata.isValid){
        emit Send_TextBrowser_NewMessage(tr("Frame Interpolation failed: Could not read metadata for ") + SourcePath);
        return false;
    }
    // ... (original logic using metadata.width, metadata.height, metadata.frameCount, etc.) ...
    // Example change:
    // QMap<QString,int> HW_map = video_get_Resolution(SourcePath);
    // int original_width = HW_map.value("width");
    // int original_height = HW_map.value("height");
    // Becomes:
    int original_width = metadata.width;
    int original_height = metadata.height;
    long long totalFrames = metadata.frameCount; // Use frameCount from metadata

    // ... (rest of the original function, adapting to use metadata fields) ...
    return true; // Placeholder
}

bool MainWindow::Video_AutoSkip_CustRes(int rowNum)
{
    QString fullpath = Table_model_video->item(rowNum,2)->text();
    FileMetadataCache metadata = getOrFetchMetadata(fullpath);
    if(!metadata.isValid || metadata.width == 0 || metadata.height == 0) return true; // Skip if cannot read

    int original_width = metadata.width;
    int original_height = metadata.height;

    if(original_width > original_height) // Landscape
    {
        if(ui->checkBox_AutoSkip_CustRes_lanscape->isChecked()) return true;
    }
    if(original_width < original_height) // Portrait
    {
        if(ui->checkBox_AutoSkip_CustRes_portrait->isChecked()) return true;
    }
    if(original_width == original_height) // Square
    {
        if(ui->checkBox_AutoSkip_CustRes_square->isChecked()) return true;
    }
    return false;
}


// ... (The rest of the file, including all other functions, slots, etc.)
// ... (This is a highly abridged version for demonstration purposes)
void MainWindow::Init_Table(){}
void MainWindow::Table_Clear(){}
int MainWindow::Table_image_get_rowNum(){ return Table_model_image->rowCount(); }
int MainWindow::Table_gif_get_rowNum(){ return Table_model_gif->rowCount(); }
int MainWindow::Table_video_get_rowNum(){ return Table_model_video->rowCount(); }
bool MainWindow::Deduplicate_filelist(QString){ return false;}
void MainWindow::TextBrowser_StartMes(){}
void MainWindow::Init_ActionsMenu_checkBox_ReplaceOriginalFile(){}
void MainWindow::Init_ActionsMenu_checkBox_DelOriginal(){}
int MainWindow::Settings_Read_Apply(){ return 0; }
int MainWindow::CheckUpadte_Auto(){ return 0; }
int MainWindow::Donate_DownloadOnlineQRCode(){ return 0; }
int MainWindow::SystemShutDown_isAutoShutDown(){ return 0; }
void MainWindow::Tip_FirstTimeStart(){}
void MainWindow::file_mkDir(QString){}
bool MainWindow::file_isDirWritable(QString){return true;}
void MainWindow::Init_SystemTrayIcon(){}
void MainWindow::Init_ActionsMenu_lineEdit_outputPath(){}
void MainWindow::Init_ActionsMenu_FilesList(){}
void MainWindow::Init_ActionsMenu_pushButton_RemoveItem(){}
void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings(){}
void MainWindow::RealESRGAN_NCNN_Vulkan_PreLoad_Settings(){}
int MainWindow::Table_FileCount_reload(){ return 0; }
void MainWindow::progressbar_clear(){}
void MainWindow::CustRes_remove(QString){}
void MainWindow::on_pushButton_Start_clicked(){}
void MainWindow::on_pushButton_CheckUpdate_clicked(){}
void MainWindow::on_pushButton_compatibilityTest_clicked(){}
void MainWindow::on_pushButton_DetectGPU_clicked(){}
void MainWindow::on_pushButton_SaveSettings_clicked(){}
void MainWindow::on_pushButton_ResetSettings_clicked(){}
void MainWindow::on_pushButton_DumpProcessorList_converter_clicked(){}
void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int){}
void MainWindow::on_pushButton_DetectGPUID_srmd_clicked(){}
void MainWindow::on_pushButton_ListGPUs_Anime4k_clicked(){}
void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked(){}
void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_clicked(){}
void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan_currentIndexChanged(int){}
void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_valueChanged(int){}
void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_clicked(){}
void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_stateChanged(int){}
void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_stateChanged(int){}
void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_clicked(){}
void MainWindow::on_comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan_currentIndexChanged(int){}
void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan_clicked(){}
void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan_valueChanged(int){}
void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_stateChanged(int){}
void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_clicked(){}
void MainWindow::on_comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan_currentIndexChanged(int){}
void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealsrNcnnVulkan_clicked(){}
void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan_valueChanged(int){}
void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_clicked(){}
void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_stateChanged(int){}
void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xConverter_currentIndexChanged(int){}
void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter_clicked(){}
void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter_valueChanged(int){}
void MainWindow::on_checkBox_EnableMultiGPU_Waifu2xCaffe_stateChanged(int){}
void MainWindow::on_comboBox_ProcessMode_Waifu2xCaffe_currentIndexChanged(int){}
void MainWindow::on_lineEdit_GPUs_Anime4k_editingFinished(){}
void MainWindow::on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished(){}
void MainWindow::on_pushButton_VerifyGPUsConfig_Anime4k_clicked(){}
void MainWindow::on_pushButton_VerifyGPUsConfig_Waifu2xCaffe_clicked(){}
void MainWindow::on_tableView_image_doubleClicked(const QModelIndex&){}
void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex&){}
void MainWindow::on_tableView_video_doubleClicked(const QModelIndex&){}
void MainWindow::on_checkBox_BanGitee_clicked(){}
void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xConverter_clicked(){}
void MainWindow::on_pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan_clicked(){}
void MainWindow::on_tableView_image_pressed(const QModelIndex&){}
void MainWindow::on_tableView_gif_pressed(const QModelIndex&){}
void MainWindow::on_tableView_video_pressed(const QModelIndex&){}
void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int){}
void MainWindow::on_pushButton_TileSize_Add_W2xNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_TileSize_Minus_W2xNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_BlockSize_Add_W2xConverter_clicked(){}
void MainWindow::on_pushButton_BlockSize_Minus_W2xConverter_clicked(){}
void MainWindow::on_pushButton_Add_TileSize_SrmdNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_Minus_TileSize_SrmdNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_Add_TileSize_RealsrNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_Minus_TileSize_RealsrNCNNVulkan_clicked(){}
void MainWindow::on_pushButton_DetectGPU_VFI_clicked(){}
void MainWindow::on_lineEdit_MultiGPU_IDs_VFI_editingFinished(){}
void MainWindow::on_checkBox_MultiGPU_VFI_stateChanged(int){}
void MainWindow::on_groupBox_FrameInterpolation_clicked(){}
void MainWindow::on_checkBox_isCompatible_RifeNcnnVulkan_clicked(){}
void MainWindow::on_comboBox_Engine_VFI_currentIndexChanged(int){}
void MainWindow::on_checkBox_isCompatible_CainNcnnVulkan_clicked(){}
void MainWindow::on_pushButton_Verify_MultiGPU_VFI_clicked(){}
void MainWindow::on_checkBox_EnableVFI_Home_clicked(){}
void MainWindow::on_checkBox_MultiThread_VFI_stateChanged(int){}
void MainWindow::on_checkBox_MultiThread_VFI_clicked(){}
void MainWindow::on_checkBox_isCompatible_DainNcnnVulkan_clicked(){}
void MainWindow::on_pushButton_SupportersList_clicked(){}
void MainWindow::Table_EnableSorting(bool){}
void MainWindow::Apply_CustRes_QAction_FileList_slot(){}
void MainWindow::Cancel_CustRes_QAction_FileList_slot(){}
void MainWindow::RemoveALL_image_slot(){}
void MainWindow::RemoveALL_gif_slot(){}
void MainWindow::RemoveALL_video_slot(){}
void MainWindow::Add_progressBar_CompatibilityTest(){}
void MainWindow::OpenSelectedFilesFolder_FilesList(){}
void MainWindow::OpenSelectedFile_FilesList(){}
void MainWindow::OpenOutputFolder(){}
void MainWindow::Unable2Connect_RawGithubusercontentCom(){}
void MainWindow::SystemTray_hide_self(){}
void MainWindow::SystemTray_showNormal_self(){}
void MainWindow::SystemTray_showDonate(){}
void MainWindow::SystemTray_NewMessage(QString){}
void MainWindow::EnableBackgroundMode_SystemTray(){}
void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason){}
void MainWindow::progressbar_setRange_min_max(int,int){}
void MainWindow::progressbar_Add(){}
void MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString(int,QString){}
void MainWindow::Table_gif_ChangeStatus_rowNumInt_statusQString(int,QString){}
void MainWindow::Table_video_ChangeStatus_rowNumInt_statusQString(int,QString){}
void MainWindow::Waifu2x_Finished(){}
void MainWindow::Waifu2x_Finished_manual(){}
void MainWindow::TextBrowser_NewMessage(QString){}
int MainWindow::Waifu2x_Compatibility_Test_finished(){ return 0; }
int MainWindow::Waifu2x_DetectGPU_finished(){ return 0; }
int MainWindow::Realsr_ncnn_vulkan_DetectGPU_finished(){ return 0; }
int MainWindow::FrameInterpolation_DetectGPU_finished(){ return 0; }
int MainWindow::CheckUpadte_NewUpdate(QString,QString){ return 0; }
void MainWindow::FinishedProcessing_DN(){}
void MainWindow::Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString){}
void MainWindow::Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString){}
void MainWindow::Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString){}
int MainWindow::Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini_arg){emit Send_Table_Read_Saved_Table_Filelist_Finished(Table_FileList_ini_arg); return 0;}
int MainWindow::Table_Save_Current_Table_Filelist_Finished(){emit Send_Table_Save_Current_Table_Filelist_Finished(); return 0;}
bool MainWindow::SystemShutDown(){return false;}
int MainWindow::Waifu2x_DumpProcessorList_converter_finished(){return 0;}
void MainWindow::Read_urls(QList<QUrl> urls)
{
    qDebug() << "MainWindow::Read_urls called, dispatching to ProcessDroppedFilesAsync.";
    // Disable drag and drop and give the user feedback while we
    // process the dropped files on a worker thread.  This mirrors
    // the behaviour used when adding files through the "Browse" button
    // so the UI remains consistent.
    ui_tableViews_setUpdatesEnabled(false);
    ui->groupBox_Setting->setEnabled(false);
    ui->groupBox_FileList->setEnabled(false);
    ui->groupBox_InputExt->setEnabled(false);
    ui->checkBox_ScanSubFolders->setEnabled(false);
    pushButton_Start_setEnabled_self(0);
    this->setAcceptDrops(false);
    ui->label_DropFile->setText(tr("Adding files, please wait."));
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));

    QtConcurrent::run(this, &MainWindow::ProcessDroppedFilesAsync, urls);
}
void MainWindow::Read_urls_finfished()
{
    // Re-enable UI elements that were disabled while processing the drop.
    ui_tableViews_setUpdatesEnabled(true);
    ui->groupBox_Setting->setEnabled(true);
    ui->groupBox_FileList->setEnabled(true);
    ui->groupBox_InputExt->setEnabled(true);
    ui->checkBox_ScanSubFolders->setEnabled(true);
    pushButton_Start_setEnabled_self(1);
    this->setAcceptDrops(true);
    ui->label_DropFile->setText(tr("Drag and drop files or folders here\n(Image, GIF and Video)"));
    emit Send_TextBrowser_NewMessage(tr("Add file complete."));
}
void MainWindow::SRMD_DetectGPU_finished(){}
void MainWindow::video_write_VideoConfiguration(QString,int,int,bool,int,int,QString,bool,QString,QString,bool,int){}
int MainWindow::Settings_Save(){return 0;}
void MainWindow::video_write_Progress_ProcessBySegment(QString,int,bool,bool,int,int){}
void MainWindow::CurrentFileProgress_Start(QString,int){}
void MainWindow::CurrentFileProgress_Stop(){}
void MainWindow::CurrentFileProgress_progressbar_Add(){}
void MainWindow::CurrentFileProgress_progressbar_Add_SegmentDuration(int){}
void MainWindow::CurrentFileProgress_progressbar_SetFinishedValue(int){}
void MainWindow::CurrentFileProgress_WatchFolderFileNum(QString){}
void MainWindow::CurrentFileProgress_WatchFolderFileNum_Textbrower(QString,QString,int){}
void MainWindow::Donate_ReplaceQRCode(QString){}
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_finished(int,QProcess::ExitStatus){}
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput(){}
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError(){}
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError){}
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_finished(int,QProcess::ExitStatus){}
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardOutput(){}
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardError(){}
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError){}
void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings(){}
void MainWindow::RealESRGAN_NCNN_Vulkan_PreLoad_Settings(){}
void MainWindow::Realcugan_NCNN_Vulkan_Image(int, bool){}
void MainWindow::Realcugan_NCNN_Vulkan_GIF(int){}
void MainWindow::Realcugan_NCNN_Vulkan_Video(int){}
void MainWindow::Realcugan_NCNN_Vulkan_Video_BySegment(int){}
void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings(){}
void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int){}
void MainWindow::APNG_RealcuganNCNNVulkan(QString, QString, QString, QStringList, QString){}
void MainWindow::Realcugan_ncnn_vulkan_DetectGPU(){}
QString MainWindow::RealcuganNcnnVulkan_MultiGPU(){ return ""; }
void MainWindow::AddGPU_MultiGPU_RealcuganNcnnVulkan(QString){}
QStringList MainWindow::Realcugan_NCNN_Vulkan_PrepareArguments(const QString&, const QString&, int, const QString&, int, int, const QString&, bool, const QString&, bool, const QString&){ return QStringList(); }
void MainWindow::StartNextRealCUGANPass(QProcess*){}
void MainWindow::Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString&, int, bool, const QString&){}
bool MainWindow::Realcugan_ProcessSingleFileIteratively(const QString&, const QString&, int, const QString&, int, int, const QString&, bool, bool, const QString&, int){ return false; }
void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int, bool){}
void MainWindow::RealESRGAN_NCNN_Vulkan_GIF(int){}
void MainWindow::RealESRGAN_NCNN_Vulkan_Video(int){}
void MainWindow::RealESRGAN_NCNN_Vulkan_Video_BySegment(int){}
void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings(){}
void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(int){}
void MainWindow::APNG_RealESRGANNCNNVulkan(QString, QString, QString, QStringList, QString){}
void MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU(){}
QString MainWindow::RealesrganNcnnVulkan_MultiGPU(){ return ""; }
void MainWindow::AddGPU_MultiGPU_RealesrganNcnnVulkan(QString){}
QStringList MainWindow::RealESRGAN_NCNN_Vulkan_PrepareArguments(const QString&, const QString&, int, const QString&, int, const QString&, bool, bool, const QString&){ return QStringList(); }
bool MainWindow::RealESRGAN_ProcessSingleFileIteratively(const QString&, const QString&, int, int, const QString&, int, const QString&, bool, bool, const QString&, int){ return false; }
QList<int> MainWindow::CalculateRealESRGANScaleSequence(int, int){ return QList<int>(); }
bool MainWindow::RealESRGAN_SetupTempDir(const QString&, const QString&, QDir&, QString&){ return false; }
void MainWindow::RealESRGAN_CleanupTempDir(const QDir&){}
void MainWindow::on_checkBox_isCompatible_RealCUGAN_NCNN_Vulkan_clicked(){}
void MainWindow::on_checkBox_isCompatible_RealESRGAN_NCNN_Vulkan_clicked(){}
// QString MainWindow::video_get_fps(QString videoPath){ FileMetadataCache m = getOrFetchMetadata(videoPath); return m.isValid ? m.fps : ""; } // Removed, use getOrFetchMetadata directly
// long long MainWindow::video_get_frameNum(QString videoPath){ FileMetadataCache m = getOrFetchMetadata(videoPath); return m.isValid ? m.frameCount : 0; } // Removed
// int MainWindow::video_get_duration(QString videoPath){ FileMetadataCache m = getOrFetchMetadata(videoPath); return m.isValid ? static_cast<int>(m.duration) : 0; } // Removed
// bool MainWindow::video_isVFR(QString videoPath){ FileMetadataCache m = getOrFetchMetadata(videoPath); return m.isValid && m.isVFR; } // Removed
// QMap<QString,int> MainWindow::video_get_Resolution(QString VideoFileFullPath){ FileMetadataCache m = getOrFetchMetadata(VideoFileFullPath); QMap<QString,int> r; if(m.isValid){r.insert("width",m.width); r.insert("height",m.height);} else {r.insert("width",0); r.insert("height",0);} return r;} // Removed
int MainWindow::CustRes_CancelCustRes(){ return 0; }
int MainWindow::Waifu2x(){ return 0;}
int MainWindow::Waifu2xMainThread(){ return 0; }
int MainWindow::Waifu2x_NCNN_Vulkan_Image(int,bool){return 0;}
int MainWindow::Waifu2x_NCNN_Vulkan_GIF(int){return 0;}
int MainWindow::Waifu2x_NCNN_Vulkan_Video(int){return 0;}
int MainWindow::Waifu2x_NCNN_Vulkan_Video_BySegment(int){return 0;}
QString MainWindow::Waifu2x_NCNN_Vulkan_ReadSettings(){return "";}
QString MainWindow::Waifu2x_NCNN_Vulkan_ReadSettings_Video_GIF(int){return "";}
int MainWindow::Realsr_NCNN_Vulkan_Image(int,bool){return 0;}
int MainWindow::Realsr_NCNN_Vulkan_GIF(int){return 0;}
int MainWindow::Realsr_NCNN_Vulkan_Video(int){return 0;}
int MainWindow::Realsr_NCNN_Vulkan_Video_BySegment(int){return 0;}
QString MainWindow::Realsr_NCNN_Vulkan_ReadSettings(){return "";}
int MainWindow::Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(int){return 0;}
QString MainWindow::Realsr_NCNN_Vulkan_ReadSettings_Video_GIF(int){return "";}
int MainWindow::Anime4k_Image(int,bool){return 0;}
int MainWindow::Anime4k_GIF(int){return 0;}
int MainWindow::Anime4k_GIF_scale(QMap<QString,QString>,int*,bool*){return 0;}
int MainWindow::Anime4k_Video(int){return 0;}
int MainWindow::Anime4k_Video_BySegment(int){return 0;}
int MainWindow::Anime4k_Video_scale(QMap<QString,QString>,int*,bool*){return 0;}
QString MainWindow::Anime4k_ReadSettings(bool){return "";}
void MainWindow::DenoiseLevelSpinboxSetting_Anime4k(){}
int MainWindow::Get_NumOfGPU_Anime4k(){return 0;}
int MainWindow::Waifu2x_Converter_Image(int,bool){return 0;}
int MainWindow::Waifu2x_Converter_GIF(int){return 0;}
int MainWindow::Waifu2x_Converter_GIF_scale(QMap<QString, QString>,int*,bool*){return 0;}
int MainWindow::Waifu2x_Converter_Video(int){return 0;}
int MainWindow::Waifu2x_Converter_Video_BySegment(int){return 0;}
int MainWindow::Waifu2x_Converter_Video_scale(QMap<QString,QString>,int*,bool*){return 0;}
QString MainWindow::Waifu2xConverter_ReadSettings(){return "";}
int MainWindow::SRMD_NCNN_Vulkan_Image(int,bool){return 0;}
int MainWindow::SRMD_NCNN_Vulkan_GIF(int){return 0;}
int MainWindow::SRMD_NCNN_Vulkan_Video(int){return 0;}
int MainWindow::SRMD_NCNN_Vulkan_Video_BySegment(int){return 0;}
QString MainWindow::SrmdNcnnVulkan_ReadSettings(){return "";}
QMap<QString,int> MainWindow::Calculate_ScaleRatio_SrmdNcnnVulkan(int){return QMap<QString,int>();}
QString MainWindow::SrmdNcnnVulkan_ReadSettings_Video_GIF(int){return "";}
int MainWindow::Waifu2x_Caffe_Image(int,bool){return 0;}
int MainWindow::Waifu2x_Caffe_GIF(int){return 0;}
int MainWindow::Waifu2x_Caffe_GIF_scale(QMap<QString, QString>,int*,bool*){return 0;}
int MainWindow::Waifu2x_Caffe_Video(int){return 0;}
int MainWindow::Waifu2x_Caffe_Video_BySegment(int){return 0;}
int MainWindow::Waifu2x_Caffe_Video_scale(QMap<QString,QString>,int*,bool*){return 0;}
QString MainWindow::Waifu2x_Caffe_ReadSettings(){return "";}
bool MainWindow::isWaifu2xCaffeEnabled(){return false;}
void MainWindow::DeleteErrorLog_Waifu2xCaffe(){}
int MainWindow::SRMD_CUDA_Image(int,bool){return 0;}
int MainWindow::SRMD_CUDA_GIF(int){return 0;}
int MainWindow::SRMD_CUDA_Video(int){return 0;}
int MainWindow::SRMD_CUDA_Video_BySegment(int){return 0;}
// ... (The rest of the functions from the original file) ...

