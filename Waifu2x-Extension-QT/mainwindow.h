#pragma once
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

    My Github homepage: https://github.com/AaronFeng753
*/

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>
#include <QList>
#include <QMap>
#include <QtConcurrent>
#include <QProcess>
#include <QFutureWatcher>
#include <QImage>
#include <QImageWriter>
#include <QFile>
#include <QVariant>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <QTime>
#include <QMediaPlayer>
#include <QDesktopServices>
#include <QSize>
#include <QMovie>
#include <QTimer>
#include <QTextCursor>
#include <QMessageBox>
#include <QSettings>
#include <QTranslator>
#include <QScreen>
#include <QCloseEvent>
#include <QFileDialog>
#include <QtCore5Compat/QTextCodec>
#include <math.h>
#include <QMutex>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMetaType>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QThread>
#include <QThreadPool>
#include <QFileSystemWatcher>
#include <QListWidget> // Added for QListWidget
#include <QLabel>      // Added for QLabel
#include <QSpinBox>    // Added for QSpinBox
#include <QCheckBox>   // Added for QCheckBox
#include <QGroupBox>   // Added for QGroupBox
#include <QComboBox>   // Added for QComboBox
#include <QPushButton> // Added for QPushButton
#include <atomic> // Added for std::atomic
#include "topsupporterslist.h"
#include "FileManager.h"
#include "ProcessRunner.h"
#include "GpuManager.h"
#include "UiController.h"

#ifndef Q_DECLARE_METATYPE
#define Q_DECLARE_METATYPE(Type)
#endif

typedef QList<QMap<QString, QString>> QList_QMap_QStrQStr;
Q_DECLARE_METATYPE(QList_QMap_QStrQStr)

// Struct for caching file metadata
struct FileMetadataCache {
    bool isValid = false; // Flag to indicate if metadata is successfully populated
    int width = 0;
    int height = 0;
    QString fps; // e.g., "30/1" or "29.97"
    double duration = 0.0; // seconds
    QString bitRate; // e.g., "5000k" or just numeric string
    long long frameCount = 0;
    bool isVFR = false; // Variable Frame Rate
    QString identifyOutput; // For Image_Gif_Read_Resolution fallback
    bool isAnimated = false; // True for GIF/APNG with multiple frames
    QString fileFormat; // e.g., "png", "gif", "apng", "mp4" (typically the suffix)
};
Q_DECLARE_METATYPE(FileMetadataCache)


QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class RealCuganProcessor;
class VideoProcessor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int maxThreadsOverride = 0, QWidget *parent = nullptr);
    void changeEvent(QEvent *e);
    //=======================
    QString VERSION = "v3.41.02-beta"; // Software version
    bool isBetaVer = true;
    QString LastStableVer = "v3.31.13";
    QString LastBetaVer = "v3.41.02-beta";
    //=======================
    QTranslator * translator; // UI translator
    QIcon *MainIcon_QIcon = new QIcon(":/new/prefix1/icon/icon_main.png");
    TopSupportersList *TopSupportersList_widget;
    //=======
    QString Current_Path = qApp->applicationDirPath(); // Current path
    int globalMaxThreadCount = 0; // maximum threads for global pool
    //=======
    void Set_Font_fixed();
    bool SystemPrefersDark() const;
    void ApplyDarkStyle();
    //=================================  File handling =================================
    void dragEnterEvent(QDragEnterEvent *event); // Drag-and-drop event
    void dropEvent(QDropEvent *event);
    void Read_urls(QList<QUrl> urls);
    void Read_Input_paths_BrowserFile(QStringList Input_path_List);
    bool AddNew_gif=false;//whether new GIF files were added
    bool AddNew_image=false;//whether new images were added
    bool AddNew_video=false;//whether new videos were added
    void Add_File_Folder(QString Full_Path);//add file or folder depending on path type and update table/file list
    void Add_File_Folder_IncludeSubFolder(QString Full_Path);//add folder and scan subfolders
    QStringList getFileNames_IncludeSubFolder(QString path);//read file list including subfolders
    int FileList_Add(QString fileName, QString SourceFile_fullPath);//insert file directly to file list and table view
    //check whether a file already exists in the list
    bool Deduplicate_filelist(QString SourceFile_fullPath);
    bool file_isDirExist(QString SourceFile_fullPath);//check if folder exists
    void file_mkDir(QString SourceFile_fullPath);//create folder
    bool file_isDirEmpty(QString FolderPath);
    QStringList file_getFileNames_in_Folder_nofilter(QString path);//get file names in folder without filter
    bool file_DelDir(const QString &path);//delete folder regardless of empty state
    QString file_getBaseName(QString path);//get basename
    void file_MoveToTrash( QString file );//move file to recycle bin
    void MoveFileToOutputPath(QString Orginal,QString SourceFilePath);//move file to output path
    QString file_getFolderPath(QFileInfo fileInfo);//get folder path (without trailing slash)
    bool file_isDirWritable(QString DirPath);//check whether folder is writable
    //check whether the folder of the current row is writable
    bool file_isFilesFolderWritable_row_image(int rowNum);
    bool file_isFilesFolderWritable_row_video(int rowNum);
    bool file_isFilesFolderWritable_row_gif(int rowNum);
    bool file_OpenFolder(QString FolderPath);//open folder in file explorer
    bool file_OpenFilesFolder(QString FilePath);//open the folder containing the file
    bool file_OpenFile(QString FilePath);
    void file_MoveFiles_Folder_NcnnVulkanFolderProcess(QString Old_folder, QString New_folder, bool Delete_);
    bool file_generateMarkFile(QString FileFullPath,QString Msg);
    //=================================  Table =================================
    void ui_tableViews_setUpdatesEnabled(bool isEnabled);// enable/disable UI updates for file list tables
    void Init_Table();// initialize the three table views
    QStandardItemModel *Table_model_image = new QStandardItemModel();
    QStandardItemModel *Table_model_video = new QStandardItemModel();
    QStandardItemModel *Table_model_gif = new QStandardItemModel();
    // cancel custom resolution for a specific row
    void Table_image_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_gif_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_video_CustRes_Cancel_rowNumInt(int rowNum);
    void Table_ChangeAllStatusToWaiting();// change all rows status to "waiting"
    QMutex QMutex_Table_ChangeAllStatusToWaiting;
    void Table_Clear();// clear table view
    // get the next row number for inserting new data
    int Table_image_get_rowNum();
    int Table_gif_get_rowNum();
    int Table_video_get_rowNum();
    // read table and return map of full path to status
    QMap<QString, QString> Table_Read_status_fullpath(QStandardItemModel *Table_model);
    // currently selected row number
    int curRow_image = -1;
    int curRow_gif = -1;
    int curRow_video = -1;

    int Table_Save_Current_Table_Filelist(QString Table_FileList_ini);// save current file list including table

    int Table_Read_Saved_Table_Filelist(QString Table_FileList_ini);// read saved file list

    int Table_Save_Current_Table_Filelist_Watchdog(QString Table_FileList_ini);// watchdog thread to ensure list is saved

    bool Table_insert_finished=false;
    QMutex mutex_Table_insert_finished;// mutex guarding insert completion flag

    QMutex mutex_Table_insert;// mutex guarding table insert
    //================================= Waifu2x ====================================
    void ShowFileProcessSummary();// show processing summary report
    QString OutPutFolder_main="";// main output folder
    int Waifu2xMainThread();// main thread for waifu2x: read list and schedule scaling threads
    QStringList WaitForEngineIO(QStringList OutPutFilesFullPathList);
    QStringList WaitForEngineIO_NcnnVulkan(QString OutputFolderFullPath);
    void Restore_SplitFramesFolderPath(QString SplitFramesFolderPath, QStringList GPU_SplitFramesFolderPath_List);
    bool isForceRetryEnabled=true;
    //bool KILL_TASK_(QString TaskName,bool RequestAdmin);
    bool KILL_TASK_QStringList(QStringList TaskNameList,bool RequestAdmin);
    //==========================
    QString Waifu2x_ncnn_vulkan_FolderPath = "";
    QString Waifu2x_ncnn_vulkan_ProgramPath = "";
    //===
    int Waifu2x_NCNN_Vulkan_Image(int rowNum,bool ReProcess_MissingAlphaChannel);// Vulkan image upscaling thread
    // Vulkan GIF upscaling: main thread splits and assembles, sub threads upscale frames
    int Waifu2x_NCNN_Vulkan_GIF(int rowNum);
    // Vulkan video upscaling: main thread splits/assembles, sub threads upscale frames
    int Waifu2x_NCNN_Vulkan_Video(int rowNum);
    int Waifu2x_NCNN_Vulkan_Video_BySegment(int rowNum);
    QString Waifu2x_NCNN_Vulkan_ReadSettings();
    QString Waifu2x_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum);
    //===
    int Realsr_NCNN_Vulkan_Image(int rowNum,bool ReProcess_MissingAlphaChannel);// Realsr image upscaling thread
    // Realsr GIF upscaling: main thread splits/assembles, sub threads upscale frames
    int Realsr_NCNN_Vulkan_GIF(int rowNum);
    // Realsr video upscaling: main thread splits/assembles, sub threads upscale frames
    int Realsr_NCNN_Vulkan_Video(int rowNum);
    int Realsr_NCNN_Vulkan_Video_BySegment(int rowNum);
    QString Realsr_NCNN_Vulkan_ReadSettings();
    int Calculate_Temporary_ScaleRatio_RealsrNCNNVulkan(int ScaleRatio);
    QString Realsr_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum);
    //=========================
    int Anime4k_Image(int rowNum,bool ReProcess_MissingAlphaChannel);
    int Anime4k_GIF(int rowNum);
    int Anime4k_GIF_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    // Anime4k video upscaling: main thread splits/assembles, sub threads upscale frames
    int Anime4k_Video(int rowNum);
    int Anime4k_Video_BySegment(int rowNum);
    int Anime4k_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);
    QString Anime4k_ReadSettings(bool PreserveAlphaChannel);
    void DenoiseLevelSpinboxSetting_Anime4k();
    QString Anime4k_ProgramPath = Current_Path + "/Anime4K/Anime4K_waifu2xEX.exe";
    QString FFMPEG_EXE_PATH_Waifu2xEX;
    int Get_NumOfGPU_Anime4k();
    //=================================
    int Waifu2x_Converter_Image(int rowNum,bool ReProcess_MissingAlphaChannel);// Converter image upscaling thread
    // Converter GIF upscaling: main thread splits/assembles, sub threads upscale frames
    int Waifu2x_Converter_GIF(int rowNum);
    int Waifu2x_Converter_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    // Converter video upscaling: main thread splits/assembles, sub threads upscale frames
    int Waifu2x_Converter_Video(int rowNum);
    int Waifu2x_Converter_Video_BySegment(int rowNum);
    int Waifu2x_Converter_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);
    QString Waifu2xConverter_ReadSettings();
    //===================================
    int SRMD_NCNN_Vulkan_Image(int rowNum,bool ReProcess_MissingAlphaChannel);// SRMD image upscaling thread
    // SRMD GIF upscaling: main thread splits/assembles, sub threads upscale frames
    int SRMD_NCNN_Vulkan_GIF(int rowNum);
    // SRMD video upscaling: main thread splits/assembles, sub threads upscale frames
    int SRMD_NCNN_Vulkan_Video(int rowNum);
    int SRMD_NCNN_Vulkan_Video_BySegment(int rowNum);
    QString SrmdNcnnVulkan_ReadSettings();
    QMap<QString,int> Calculate_ScaleRatio_SrmdNcnnVulkan(int ScaleRatio);
    QString SrmdNcnnVulkan_ReadSettings_Video_GIF(int ThreadNum);
    //=================================
    int Waifu2x_Caffe_Image(int rowNum,bool ReProcess_MissingAlphaChannel);// Caffe image upscaling thread
    // Caffe GIF upscaling: main thread splits/assembles, sub threads upscale frames
    int Waifu2x_Caffe_GIF(int rowNum);
    int Waifu2x_Caffe_GIF_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    // Caffe video upscaling: main thread splits/assembles, sub threads upscale frames
    int Waifu2x_Caffe_Video(int rowNum);
    int Waifu2x_Caffe_Video_BySegment(int rowNum);
    int Waifu2x_Caffe_Video_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_video_ThreadNumRunning,bool *Frame_failed);
    QString Waifu2x_Caffe_ReadSettings();
    bool isWaifu2xCaffeEnabled();
    void DeleteErrorLog_Waifu2xCaffe();
    //====================================
    int SRMD_CUDA_Image(int rowNum,bool ReProcess_MissingAlphaChannel);// SRMD CUDA image upscaling thread
    // SRMD GIF upscaling (CUDA): main thread splits/assembles, sub threads upscale frames
    int SRMD_CUDA_GIF(int rowNum);
    // SRMD video upscaling (CUDA): main thread splits/assembles, sub threads upscale frames
    int SRMD_CUDA_Video(int rowNum);
    int SRMD_CUDA_Video_BySegment(int rowNum);
    //====================================

    void Wait_waifu2x_stop();// watchdog thread waiting for waifu2x threads to stop
    std::atomic<bool> waifu2x_STOP{false};// signal to request waifu2x stop
    std::atomic<bool> waifu2x_STOP_confirm{false};// confirmation from waifu2x stop watchdog
    std::atomic<bool> Stopping{false}; // Added for general process stopping

    int ThreadNumMax = 0;// maximum number of waifu2x threads
    std::atomic<int> ThreadNumRunning{0};// current running waifu2x thread count

    QMutex mutex_ThreadNumRunning;// mutex guarding total thread count
    QMutex mutex_SubThreadNumRunning;// mutex guarding internal thread count

    int Waifu2x_DetectGPU();// detect available GPUs (Vulkan)

    int Waifu2x_DumpProcessorList_converter();
    int Core_num = 0;
    QStringList Available_ProcessorList_converter;
    QString Processor_converter_STR="";

    int SRMD_DetectGPU();// detect available GPUs (SRMD)
    QStringList Available_GPUID_srmd;// available GPU ID list
    QString GPU_ID_STR_SRMD="";// gpuid argument for srmd command line, empty when auto

    int Realsr_ncnn_vulkan_DetectGPU();// detect available GPUs (RealSR)
    QStringList Available_GPUID_Realsr_ncnn_vulkan;// available GPU ID list

    void ListGPUs_Anime4k();// list available GPUs for Anime4k

    int FrameInterpolation_DetectGPU();// detect available GPUs (Vulkan)
    QStringList Available_GPUID_FrameInterpolation;// available GPU ID list
    QStringList Available_GPUID_Waifu2xNcnnVulkan; // Added for settings.cpp compatibility
    //======================== Image processing ================================
    bool Image_Gif_AutoSkip_CustRes(int rowNum,bool isGif);
    QMap<QString,int> Image_Gif_Read_Resolution(QString SourceFileFullPath);// obtain image/GIF resolution
    QString SaveImageAs_FormatAndQuality(QString OriginalSourceImage_fullPath,QString ScaledImage_fullPath,bool isDenoiseLevelEnabled,int DenoiseLevel);
    struct AlphaInfo {
        bool hasAlpha = false;
        bool is16Bit = false;
        QString rgbPath;
        QString alphaPath;
        QString tempDir;
    };
    AlphaInfo PrepareAlpha(const QString &inputImagePath);
    void RestoreAlpha(const AlphaInfo &info, const QString &processedRgbPath, const QString &finalOutputPath);
    //======================== Metadata Cache ================================
    FileMetadataCache getOrFetchMetadata(const QString &filePath);
    QMap<QString, FileMetadataCache> m_metadataCache;
    QMutex m_metadataCacheMutex;
    //================================================================
    int Waifu2x_Compatibility_Test();// engine compatibility check
    int Simple_Compatibility_Test(); // Public member function
    // initialize compatibility test progress bar
    void Init_progressBar_CompatibilityTest();
    // operations after compatibility test finished
    void Finish_progressBar_CompatibilityTest();
    // compatibility flags
    bool isCompatible_RealCUGAN_NCNN_Vulkan=false;
    bool isCompatible_RealESRGAN_NCNN_Vulkan=false;
    bool isCompatible_RifeNcnnVulkan = false;
    bool isCompatible_CainNcnnVulkan = false;
    bool isCompatible_DainNcnnVulkan = false;

    // Additional compatibility flags based on build errors
    bool isCompatible_Waifu2x_NCNN_Vulkan_NEW = false;
    bool isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P = false;
    bool isCompatible_Waifu2x_NCNN_Vulkan_OLD = false;
    bool isCompatible_SRMD_NCNN_Vulkan = false;
    bool isCompatible_SRMD_CUDA = false;
    bool isCompatible_Waifu2x_Converter = false;
    bool isCompatible_Anime4k_CPU = false;
    bool isCompatible_Anime4k_GPU = false;
    bool isCompatible_FFmpeg = false;
    bool isCompatible_FFprobe = false;
    bool isCompatible_ImageMagick = false;
    bool isCompatible_Gifsicle = false;
    bool isCompatible_SoX = false;
    bool isCompatible_Waifu2x_Caffe_CPU = false;
    bool isCompatible_Waifu2x_Caffe_GPU = false;
    bool isCompatible_Waifu2x_Caffe_cuDNN = false;
    bool isCompatible_Realsr_NCNN_Vulkan = false;

    //============================== Multi-GPU ======================================

    //RealsrNcnnVulkan
    int GPU_ID_RealsrNcnnVulkan_MultiGPU = 0;
    QMap<QString,QString> RealsrNcnnVulkan_MultiGPU();
    QMutex MultiGPU_QMutex_RealsrNcnnVulkan;
    QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_RealsrNcnnVulkan;
    QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan; // Added for settings.cpp compatibility
    QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_SrmdNcnnVulkan; // Added for settings.cpp compatibility
    void AddGPU_MultiGPU_RealsrNcnnVulkan(QString GPUID);

    //Waifu2xConverter
    int GPU_ID_Waifu2xConverter_MultiGPU = 0;
    QMap<QString,QString> Waifu2xConverter_MultiGPU();
    QMutex MultiGPU_QMutex_Waifu2xConverter;
    QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_Waifu2xConverter;
    void AddGPU_MultiGPU_Waifu2xConverter(QString GPUID);

    //RealCUGAN-ncnn-Vulkan
    QList<QProcess*> ProcList_RealCUGAN;
    QStringList Available_GPUID_RealCUGAN;
    QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_RealCUGAN;
    // RealCUGAN Member variables for settings
    QString m_realcugan_Model;
    int m_realcugan_DenoiseLevel;
    int m_realcugan_TileSize;
    bool m_realcugan_TTA;
    QString m_realcugan_GPUID; // Stores single GPU ID or potentially base for multi-GPU config
    QString m_realcugan_gpuJobConfig_temp; // Temporary storage for complex GPU job config string for batch processing
    RealCuganProcessor *realCuganProcessor;
    VideoProcessor *videoProcessor;
    FileManager fileManager;
    ProcessRunner processRunner;
    GpuManager gpuManager;
    UiController uiController;

    void Realcugan_NCNN_Vulkan_Image(int rowNum, bool experimental, bool ReProcess_MissingAlphaChannel);
    void Realcugan_NCNN_Vulkan_GIF(int rowNum);
    void Realcugan_NCNN_Vulkan_Video(int rowNum);
    void Realcugan_NCNN_Vulkan_Video_BySegment(int rowNum);
    void Realcugan_NCNN_Vulkan_ReadSettings();
    void Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum);
    bool APNG_RealcuganNCNNVulkan(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath); // Changed to bool
    void Realcugan_ncnn_vulkan_DetectGPU();
    QString RealcuganNcnnVulkan_MultiGPU(); // Modified to return QString for command arguments
    void AddGPU_MultiGPU_RealcuganNcnnVulkan(QString GPUID);
    void Realcugan_NCNN_Vulkan_PreLoad_Settings();
    QStringList Realcugan_NCNN_Vulkan_PrepareArguments(const QString &inputFile, const QString &outputFile, int currentPassScale, const QString &modelName, int denoiseLevel, int tileSize, const QString &gpuId, bool ttaEnabled, const QString &outputFormat, bool isMultiGPU, const QString &multiGpuJobArgs, bool experimental);
    void StartNextRealCUGANPass(QProcess *process);
    void Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString &tempPathBase, int maxPassIndex, bool keepFinal = false, const QString& finalFile = "");
    bool Realcugan_ProcessSingleFileIteratively(const QString &inputFile, const QString &outputFile, int targetScale, const QString &modelName, int denoiseLevel, int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob, bool ttaEnabled, const QString &outputFormat, bool experimental, int rowNumForStatusUpdate = -1);

    //RealCUGAN-ncnn-Vulkan UI Pointers
    QComboBox *comboBox_Model_RealCUGAN;
    QSpinBox *spinBox_Scale_RealCUGAN;
    QSpinBox *spinBox_DenoiseLevel_RealCUGAN;
    QSpinBox *spinBox_TileSize_RealCUGAN;
    QCheckBox *checkBox_TTA_RealCUGAN;
    QComboBox *comboBox_GPUID_RealCUGAN;
    QPushButton *pushButton_DetectGPU_RealCUGAN;
    QCheckBox *checkBox_MultiGPU_RealCUGAN;
    QGroupBox *groupBox_GPUSettings_MultiGPU_RealCUGAN;
    QComboBox *comboBox_GPUIDs_MultiGPU_RealCUGAN;
    QListWidget *listWidget_GPUList_MultiGPU_RealCUGAN; // For RealCuganProcessor usage
    QPushButton *pushButton_AddGPU_MultiGPU_RealCUGAN;
    QPushButton *pushButton_RemoveGPU_MultiGPU_RealCUGAN;
    QPushButton *pushButton_ClearGPU_MultiGPU_RealCUGAN;
    QPushButton *pushButton_TileSize_Add_RealCUGAN;    // For connect in constructor
    QPushButton *pushButton_TileSize_Minus_RealCUGAN;  // For connect in constructor

    //Realsr-ncnn-Vulkan UI Pointers
    QComboBox *comboBox_Model_RealsrNCNNVulkan;
    QComboBox *comboBox_GPUID_RealsrNCNNVulkan;
    QPushButton *pushButton_DetectGPU_RealsrNCNNVulkan;
    QSpinBox *spinBox_TileSize_RealsrNCNNVulkan;
    QPushButton *pushButton_Add_TileSize_RealsrNCNNVulkan;
    QPushButton *pushButton_Minus_TileSize_RealsrNCNNVulkan;
    QCheckBox *checkBox_TTA_RealsrNCNNVulkan;
    QCheckBox *checkBox_MultiGPU_RealsrNcnnVulkan;
    QGroupBox *groupBox_GPUSettings_MultiGPU_RealsrNcnnVulkan;
    QComboBox *comboBox_GPUIDs_MultiGPU_RealsrNcnnVulkan;
    QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_RealsrNcnnVulkan;
    QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan;
    QPushButton *pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan;
    // RealESRGAN Member variables for settings
    QString m_realesrgan_ModelName; // e.g., "realesrgan-x4plus"
    int m_realesrgan_ModelNativeScale; // e.g., 4
    int m_realesrgan_TileSize;
    bool m_realesrgan_TTA;
    QString m_realesrgan_GPUID;
    QString m_realesrgan_gpuJobConfig_temp; // For batch processing gpu string

    QList<QProcess*> ProcList_RealESRGAN;
    QStringList Available_GPUID_RealESRGAN_ncnn_vulkan;
    QString Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str; // Potentially for caching settings string
    int GPU_ID_RealesrganNcnnVulkan_MultiGPU_CycleCounter; // For cycling through GPUs in multi-GPU mode if needed
    QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_RealesrganNcnnVulkan;
    QMutex MultiGPU_QMutex_RealesrganNcnnVulkan;

    void RealESRGAN_NCNN_Vulkan_Image(int rowNum, bool ReProcess_MissingAlphaChannel);
    void RealESRGAN_NCNN_Vulkan_GIF(int rowNum);
    void RealESRGAN_NCNN_Vulkan_Video(int rowNum);
    void RealESRGAN_NCNN_Vulkan_Video_BySegment(int rowNum);
    void RealESRGAN_NCNN_Vulkan_ReadSettings(); // Main settings reader
    void RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum); // For batch (frames)
    bool APNG_RealESRGANNCNNVulkan(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath); // Changed to bool
    void RealESRGAN_ncnn_vulkan_DetectGPU();
    QString RealesrganNcnnVulkan_MultiGPU(); // Returns job string for multi-GPU
    void AddGPU_MultiGPU_RealesrganNcnnVulkan(QString GPUID);
    void RealESRGAN_NCNN_Vulkan_PreLoad_Settings();
    QStringList RealESRGAN_NCNN_Vulkan_PrepareArguments(const QString &inputFile, const QString &outputFile, int currentPassScale, const QString &modelName, int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob, bool ttaEnabled, const QString &outputFormat);
    bool RealESRGAN_ProcessSingleFileIteratively(const QString &inputFile, const QString &outputFile, int targetScale, int modelNativeScale, const QString &modelName, int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob, bool ttaEnabled, const QString &outputFormat, int rowNumForStatusUpdate = -1);
    QList<int> CalculateRealESRGANScaleSequence(int targetScale, int modelNativeScale);
    bool RealESRGAN_SetupTempDir(const QString &inputFile, const QString &outputFile, QDir &tempDir, QString &tempPathBase);
    void RealESRGAN_CleanupTempDir(const QDir &tempDir);


    //Anime4k
    int GPU_ID_Anime4k_GetGPUInfo = 0;
    QString Anime4k_GetGPUInfo();
    QMutex GetGPUInfo_QMutex_Anime4k;

    //Waifu2x-caffe
    int GPU_ID_Waifu2xCaffe_GetGPUInfo = 0;
    QString Waifu2xCaffe_GetGPUInfo();
    QMutex GetGPUInfo_QMutex_Waifu2xCaffe;
    int Get_NumOfGPU_W2xCaffe();
    //================================ progress bar =================================
    int Progressbar_MaxVal = 0;// progress bar maximum value
    int Progressbar_CurrentVal = 0;// progress bar current value
    void progressbar_clear();// clear progress bar
    void progressbar_SetToMax(int maxval);// set progress bar to maximum
    //=============================== textbrowser===============================
    void TextBrowser_StartMes();// output startup message
    //================================ gif ====================================
    void Gif_RemoveFromCustResList(int RowNumber);
    bool Gif_DoubleScaleRatioPrep(int RowNumber);
    int Gif_getDuration(QString gifPath);// get frame interval duration
    int Gif_getFrameDigits(QString gifPath);// get number of frame digits
    void Gif_splitGif(QString gifPath,QString SplitFramesFolderPath);// split gif
    void Gif_assembleGif(QString ResGifPath,QString ScaledFramesPath,int Duration,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled,QString SourceGifFullPath);// assemble gif
    QString Gif_compressGif(QString gifPath,QString gifPath_compressd);// compress gif
    void video_RemoveFromCustResList(int RowNumber);
    bool video_DoubleScaleRatioPrep(int RowNumber);
    QString isPreVFIDone_MarkFilePath(QString VideoPath);
    int CalNumDigits(int input_num);
    bool isSuccessiveFailuresDetected_VFI=false;
    int FrameInterpolation_Video_BySegment(int rowNum);
    int FrameInterpolation_Video(int rowNum);
    int Old_FrameInterpolation_Engine_Index=0;
    QString FrameInterpolation_ReadConfig(bool isUhdInput,int NumOfFrames);
    bool FrameInterpolation(QString SourcePath,QString OutputPath);
    bool Video_AutoSkip_CustRes(int rowNum);
    QMap<QString,int> video_get_Resolution(QString VideoFileFullPath);// get video frame rate
    QString video_get_fps(QString videoPath);// get video fps
    int video_get_frameNumDigits(QString videoPath);// get digits of frame count
    long long video_get_frameNum(QString videoPath); // Changed to long long
    int video_get_duration(QString videoPath);
    // check whether video is variable frame rate
    bool video_isVFR(QString videoPath);
    // split video
    void video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath);
    // assemble video
    int video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled);
    // read output video settings
    QString video_ReadSettings_OutputVid(QString AudioPath);
    // get video bitrate
    QString video_get_bitrate(QString videoPath,bool isReturnFullCMD,bool isVidOnly);
    // get video bitrate (calculated from resolution)
    QString video_get_bitrate_AccordingToRes_FrameFolder(QString ScaledFrameFolderPath,QString VideoPath);
    int video_UseRes2CalculateBitrate(QString VideoFileFullPath);// calculate recommended bitrate based on resolution
    // audio denoise
    QString video_AudioDenoise(QString OriginalAudioPath);
    // convert to mp4
    QString video_To_CFRMp4(QString VideoPath);
    // extract audio
    void video_get_audio(QString VideoPath,QString AudioPath);
    // split video by segment
    void video_video2images_ProcessBySegment(QString VideoPath,QString FrameFolderPath,int StartTime,int SegmentDuration);
    // assemble video from mp4 clips
    void video_AssembleVideoClips(QString VideoClipsFolderPath,QString VideoClipsFolderName,QString video_mp4_scaled_fullpath,QString AudioPath);
    // generate folder number for video segments
    QString video_getClipsFolderNo();
    QMutex MultiLine_ErrorOutput_QMutex;

    bool video_isNeedProcessBySegment(int rowNum);// determine whether video should be processed by segments based on duration
    void DelVfiDir(QString VideoPath);
    //============================   custom res  ====================================
    // custom resolution list
    QList<QMap<QString, QString>> Custom_resolution_list;//res_map["fullpath"],["height"],["width"]
    void CustRes_remove(QString fullpath);// remove entry from custom resolutions by path
    bool CustRes_isContained(QString fullpath);// check if path is in custom resolutions
    QMap<QString, QString> CustRes_getResMap(QString fullpath);// read custom resolution for file
    int CustRes_CalNewScaleRatio(QString fullpath,int Height_new,int width_new);// calculate new scale ratio
    int CustRes_SetCustRes();// set custom resolution
    int CustRes_CancelCustRes();// cancel custom resolution
    QMap<QString, QString> DoubleScaleRatio_Cal_NewScaleRatio_NewHW(QString fullpath,double ScaleRatio_double);

    Qt::AspectRatioMode CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;// aspect ratio strategy for custom resolution
    //======================== Settings ===========================================
    int Settings_Read_Apply();// read and apply settings
    bool Settings_isReseted = false;// reset settings flag
    QVariant Settings_Read_value(QString Key);
    bool isReadOldSettings = false;
    //================================ Other =======================================
    bool isCustomVideoSettingsClicked=true;

    void pushButton_Start_setEnabled_self(bool isEnabled);
    void pushButton_Stop_setEnabled_self(bool isEnabled);

    void TurnOffScreen();
    QFuture<void> TurnOffScreen_QF;// monitor repeated screen-off requests to avoid multiple nircmd launches

    bool FileProgressWatch_isEnabled = true;// enable progress monitoring for output folder
    QFileSystemWatcher *FileProgressWatcher = nullptr;// watch output folder
    QFileSystemWatcher *FileProgressWatcher_Text = nullptr;// watch output folder (text)
    QTimer *FileProgressStopTimer = nullptr;// timer for stopping monitor
    QTimer *FileProgressStopTimer_Text = nullptr;// timer for stopping text monitor

    int ForceRetryCount = 1;

    void OutputSettingsArea_setEnabled(bool isEnabled);

    bool isFirstTimeStart=false;

    bool isForceRetryClicked=false;
    QMutex isForceRetryClicked_QMutex;
    void DelTrash_ForceRetry_Anime4k(QString OutPut_Path);
    void isForceRetryClicked_SetTrue_Block_Anime4k();

    void AutoFinishAction_Message();
    int SystemShutDown_Countdown();// auto shutdown countdown
    int SystemShutDown_isAutoShutDown();// check if auto shutdown executed last run
    // blocking delay (safe)
    void Delay_sec_sleep(int time);
    void Delay_msec_sleep(int time);

    void Play_NFSound();// play notification sound

    QTimer *TimeCostTimer;// timer for measuring time cost
    long unsigned int TimeCost = 0;// elapsed time
    QString Seconds2hms(long unsigned int seconds);// seconds to HH:MM:SS
    long unsigned int TaskNumTotal=0;// total tasks (files to process)
    long unsigned int TaskNumFinished=0;// finished file count
    bool NewTaskFinished=false;// mark when a new task is finished
    long unsigned int ETA=0;// estimated remaining seconds

    int CheckUpadte_Auto();// auto check for updates

    int Donate_DownloadOnlineQRCode();

    bool isSettingsHide=false;// whether the main page settings group box is hidden

    bool isShowAnime4kWarning=true;

    void ConnectivityTest_RawGithubusercontentCom();// check if githubusercontent.com is reachable
    bool isConnectivityTest_RawGithubusercontentCom_Running=false;
    QMutex QMutex_ConnectivityTest_RawGithubusercontentCom;

    bool DownloadTo(QString OnlineLink,QString LocalPath);
    //=========== code executed when closing window ===============
    void closeEvent(QCloseEvent* event);// close event
    //void Close_self();// code executed when closing
    std::atomic<bool> QProcess_stop{false};// flag to stop all QProcess
    int Auto_Save_Settings_Watchdog(bool isWaitForSave);// watchdog to auto save settings
    QFuture<int> AutoUpdate;// monitor auto update thread
    QFuture<int> DownloadOnlineQRCode;// monitor online QR code download
    QFuture<int> Waifu2xMain;// monitor main waifu2x thread
    int Force_close();// forcibly close using cmd
    std::atomic<bool> isAlreadyClosed{false};

    // void ProcessDroppedFilesAsync(QList<QUrl> urls); // Public member declaration removed; slot version is authoritative.
    //================== current file progress =========================
    long unsigned int TimeCost_CurrentFile =0;
    long unsigned int TaskNumTotal_CurrentFile=0;
    long unsigned int TaskNumFinished_CurrentFile=0;
    bool NewTaskFinished_CurrentFile=false;

    // Utility to run a QProcess using signal based event loop
    bool runProcess(QProcess *process, const QString &cmd,
                    QByteArray *stdOut = nullptr,
                    QByteArray *stdErr = nullptr);
    long unsigned int ETA_CurrentFile=0;
    bool isStart_CurrentFile=false;
    //=============================================
    void Tip_FirstTimeStart();
    //================== system tray icon =================
    void Init_SystemTrayIcon();
    QSystemTrayIcon *systemTray = new QSystemTrayIcon(this);
    QMenu *pContextMenu = new QMenu(this);
    QAction *minimumAct_SystemTrayIcon = new QAction(this);
    QAction *restoreAct_SystemTrayIcon = new QAction(this);
    QAction *quitAct_SystemTrayIcon = new QAction(this);
    QAction *BackgroundModeAct_SystemTrayIcon = new QAction(this);
    QAction *SendFeedback_SystemTrayIcon = new QAction(this);
    QAction *About_SystemTrayIcon = new QAction(this);
    QAction *Donate_SystemTrayIcon = new QAction(this);
    QAction *Pause_SystemTrayIcon = new QAction(this);
    QAction *Start_SystemTrayIcon = new QAction(this);
    QAction *BecomePatron_SystemTrayIcon = new QAction(this);
    QAction *TopSupportersList_SystemTrayIcon = new QAction(this);
    //================= output path line edit context menu ==============
    void Init_ActionsMenu_lineEdit_outputPath();
    QAction *OpenFolder_lineEdit_outputPath = new QAction(this);
    //================= file list context menu ====================
    void Init_ActionsMenu_FilesList();
    QAction *OpenFile_QAction_FileList = new QAction(this);
    QAction *OpenFilesFolder_QAction_FileList = new QAction(this);
    QAction *RemoveFile_FilesList_QAction_FileList = new QAction(this);
    bool EnableApply2All_CustRes=true;
    QAction *Apply_CustRes_QAction_FileList = new QAction(this);
    QAction *Cancel_CustRes_QAction_FileList = new QAction(this);
    void OpenSelectedFile_FailedWarning_FilesList();
    //================ remove item button context menu =======================
    void Init_ActionsMenu_pushButton_RemoveItem();
    QAction *RemoveALL_image = new QAction(this);
    QAction *RemoveALL_gif = new QAction(this);
    QAction *RemoveALL_video = new QAction(this);
    //=================== generate bat file to execute cmd command ===========
    void ExecuteCMD_batFile(QString cmd_str,bool requestAdmin);
    QMutex ExecuteCMD_batFile_QMutex;
    void Del_TempBatFile();
    //=====================================================
    void comboBox_UpdateChannel_setCurrentIndex_self(int index);
    QMutex comboBox_UpdateChannel_setCurrentIndex_self_QMutex;
    bool isClicked_comboBox_UpdateChannel=true;
    //===================== directly replace original file =====================
    void checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    bool ReplaceOriginalFile(QString original_fullpath,QString output_fullpath);
    void Init_ActionsMenu_checkBox_ReplaceOriginalFile();// context menu for replacing original files
    QAction *QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile = new QAction(this);
    //===================== delete original file context menu ===========================
    void Init_ActionsMenu_checkBox_DelOriginal();// context menu for deleting original files
    QAction *QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal = new QAction(this);
    //===================== event filter =====================
    bool eventFilter(QObject *target, QEvent *event);
    //===================== tile size adjustment ======================
    int AddTileSize_NCNNVulkan_Converter(int OrginalTileSize);
    int MinusTileSize_NCNNVulkan_Converter(int OrginalTileSize);
    //==================== preload engine settings ==================
    void PreLoad_Engines_Settings();
    //===
    QString Waifu2x_NCNN_Vulkan_PreLoad_Settings();
    QString Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = "";
    //===
    QString SrmdNcnnVulkan_PreLoad_Settings();
    QString SrmdNcnnVulkan_PreLoad_Settings_Str = "";
    //===
    QString Realsr_NCNN_Vulkan_PreLoad_Settings();
    QString Realsr_NCNN_Vulkan_PreLoad_Settings_Str = "";
    //===
    QString Waifu2xCaffe_PreLoad_Settings();
    QString Waifu2xCaffe_PreLoad_Settings_Str = "";
    //===
    QString Waifu2xConverter_PreLoad_Settings();
    QString Waifu2xConverter_PreLoad_Settings_Str = "";
    //===
    QString Anime4k_PreLoad_Settings();
    QString Anime4k_PreLoad_Settings_Str = "";
    QString Anime4KCPP_PreLoad_Settings_Str = ""; // As seen in mainwindow.cpp errors
    QString Realcugan_NCNN_Vulkan_PreLoad_Settings_Str = "";
    QString Rife_NCNN_Vulkan_PreLoad_Settings_Str = "";
    QString SRMD_NCNN_Vulkan_PreLoad_Settings_Str = ""; // As seen in mainwindow.cpp errors
    QString Cain_NCNN_Vulkan_PreLoad_Settings_Str = "";
    QString Dain_NCNN_Vulkan_PreLoad_Settings_Str = "";
    QString SRMD_CUDA_PreLoad_Settings_Str = "";
    QString HDNDenoiseLevel_image = "";
    QString HDNDenoiseLevel_gif = "";
    QString HDNDenoiseLevel_video = "";
    //================== calculate temporary scale ratio ====================
    int Calculate_Temporary_ScaleRatio_W2xNCNNVulkan(int ScaleRatio);
    //================== multithreaded image resize =========================
    void ImagesResize_Folder_MultiThread(int New_width,int New_height,QString ImagesFolderPath);
    int TotalNumOfThreads_ImagesResize_Folder_MultiThread;
    int RunningNumOfThreads_ImagesResize_Folder_MultiThread;
    QMutex QMutex_ResizeImage_MultiThread;
    void ResizeImage_MultiThread(int New_width,int New_height,QString ImagesPath);
    //================== APNG processing =================
    void APNG_Main(int rowNum,bool isFromImageList);
    void APNG_Split2Frames(QString sourceFileFullPath,QString splitFramesFolder);
    void APNG_Frames2APNG(QString sourceFileFullPath,QString scaledFramesFolder,QString resultFileFullPath,bool isOverScaled);
    bool APNG_isAnimatedPNG(int rowNum);
    //Waifu2x-NCNN-Vulkan
    bool APNG_Waifu2xNCNNVulkan(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    //Waifu2x-Converter
    bool APNG_Waifu2xConverter(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    int Waifu2x_Converter_APNG_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //Srmd-NCNN-Vulkan
    bool APNG_SrmdNCNNVulkan(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    //Anime4k
    bool APNG_Anime4k(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    int Anime4k_APNG_scale(QMap<QString,QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //Srmd-CUDA
    bool APNG_SrmdCUDA(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    //waifu2x-caffe
    bool APNG_Waifu2xCaffe(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    int Waifu2x_Caffe_APNG_scale(QMap<QString, QString> Sub_Thread_info,int *Sub_gif_ThreadNumRunning,bool *Frame_failed);
    //Realsr-NCNN-Vulkan
    bool APNG_RealsrNCNNVulkan(QString splitFramesFolder,QString scaledFramesFolder,QString sourceFileFullPath,QStringList framesFileName_qStrList,QString resultFileFullPath);
    //=============
    ~MainWindow();

public slots:
    void Table_EnableSorting(bool EnableSorting);

    void Apply_CustRes_QAction_FileList_slot();
    void Cancel_CustRes_QAction_FileList_slot();

    void RemoveALL_image_slot();
    void RemoveALL_gif_slot();
    void RemoveALL_video_slot();

    void Add_progressBar_CompatibilityTest();// progress +1 - compatibility test progress bar

    void OpenSelectedFilesFolder_FilesList();
    void OpenSelectedFile_FilesList();

    void OpenOutputFolder();

    void Unable2Connect_RawGithubusercontentCom();

    void SetEnable_pushButton_ForceRetry_self();

    void SystemTray_hide_self();
    void SystemTray_showNormal_self();
    void SystemTray_showDonate();

    void SystemTray_NewMessage(QString message);
    void EnableBackgroundMode_SystemTray();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

    void progressbar_setRange_min_max(int min, int max);// set progress bar min and max
    void progressbar_Add();// progress bar +1

    // change row status based on row number
    void Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
    void Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
    void Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);

    void Waifu2x_Finished();// code executed when processing finishes automatically (manual handler will also run)
    void Waifu2x_Finished_manual();// code executed after manual stop

    void TextBrowser_NewMessage(QString message);// TextBrowser send new message "[time] message"

    void TimeSlot();// timer slot function

    int Waifu2x_Compatibility_Test_finished();// slot executed after compatibility test

    int Waifu2x_DetectGPU_finished();// slot executed after GPU detection

    int Realsr_ncnn_vulkan_DetectGPU_finished();// slot executed after RealSR GPU detection

    int FrameInterpolation_DetectGPU_finished();

    int CheckUpadte_NewUpdate(QString update_str,QString Change_log);// popup when update is detected

    void FinishedProcessing_DN();

    int Table_FileCount_reload();// reload file count below table

    // insert file name and full path into table
    void Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

    // insert custom resolution values into table
    void Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);

    // operations after reading or saving file list and table
    int Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini);
    int Table_Save_Current_Table_Filelist_Finished();

    void on_pushButton_ClearList_clicked();// clear list

    bool SystemShutDown();// shut down system

    int Waifu2x_DumpProcessorList_converter_finished();

    void Read_urls_finfished();

    void SRMD_DetectGPU_finished();

    void video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName,bool isProcessBySegment,QString VideoClipsFolderPath,QString VideoClipsFolderName,bool isVideoFrameInterpolationEnabled,int MultipleOfFPS);

    int Settings_Save();// save settings

    // save progress
    void video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath,int StartTime,bool isSplitComplete,bool isScaleComplete,int OLDSegmentDuration,int LastVideoClipNo);


    // Asynchronous file processing for drag and drop

    //================== current file processing progress =========================
    void CurrentFileProgress_Start(QString FileName,int FrameNum);
    void CurrentFileProgress_Stop();
    void CurrentFileProgress_progressbar_Add();
    void CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration);
    void CurrentFileProgress_progressbar_SetFinishedValue(int FinishedValue);
    void CurrentFileProgress_WatchFolderFileNum(QString FolderPath);
    void CurrentFileProgress_WatchFolderFileNum_Textbrower(QString SourceFile_fullPath,QString FolderPath,int TotalFileNum);

    void Donate_ReplaceQRCode(QString QRCodePath);

    void Set_checkBox_DisableResize_gif_Checked();

signals:
    void Send_Table_EnableSorting(bool EnableSorting);

    void Send_Add_progressBar_CompatibilityTest();// progress +1 - compatibility test progress bar

    void Send_Unable2Connect_RawGithubusercontentCom();

    void Send_SetEnable_pushButton_ForceRetry_self();

    void Send_SystemTray_NewMessage(QString message);

    void Send_PrograssBar_Range_min_max(int, int);
    void Send_progressbar_Add();

    void Send_Table_image_ChangeStatus_rowNumInt_statusQString(int, QString);
    void Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString);
    void Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString);

    void Send_Waifu2x_Finished();
    void Send_Waifu2x_Finished_manual();

    void Send_TextBrowser_NewMessage(QString);

    void Send_Waifu2x_Compatibility_Test_finished();

    void Send_Waifu2x_DetectGPU_finished();

    void Send_FinishedProcessing_DN();

    void Send_Realsr_ncnn_vulkan_DetectGPU_finished();
    void Send_FrameInterpolation_DetectGPU_finished();
    void Send_Realesrgan_ncnn_vulkan_DetectGPU_finished();

    void Send_CheckUpadte_NewUpdate(QString, QString);

    void Send_Table_FileCount_reload();

    void Send_Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Send_Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
    void Send_Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

    void Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
    void Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);

    void Send_Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini);
    void Send_Table_Save_Current_Table_Filelist_Finished();

    void Send_SystemShutDown();

    void Send_Waifu2x_DumpProcessorList_converter_finished();

    void Send_Read_urls_finfished();

    void Send_SRMD_DetectGPU_finished();

    // RealCUGAN slots for iterative processing and specific detection error
    void Realcugan_NCNN_Vulkan_Iterative_finished(int exitCode, QProcess::ExitStatus exitStatus); // Match QProcess signal
    void Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error);
    void Send_Realcugan_ncnn_vulkan_DetectGPU_finished(); // Renamed from Realcugan_ncnn_vulkan_DetectGPU_finished(int, QProcess::ExitStatus)
    void Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error); // Specific error slot for GPU detection

    // RealESRGAN slots
    void RealESRGAN_NCNN_Vulkan_finished(int exitCode, QProcess::ExitStatus exitStatus); // Match QProcess signal
    void RealESRGAN_NCNN_Vulkan_errorOccurred(QProcess::ProcessError error); // General error
    void RealESRGAN_NCNN_Vulkan_Iterative_finished(int exitCode, QProcess::ExitStatus exitStatus); // For iterative processing
    void RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error); // For iterative processing
    // void Send_Realesrgan_ncnn_vulkan_DetectGPU_finished(); // Already exists
    void RealESRGAN_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error); // Specific for GPU detection

    void Send_video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName,bool isProcessBySegment,QString VideoClipsFolderPath,QString VideoClipsFolderName,bool isVideoFrameInterpolationEnabled,int MultipleOfFPS);

    void Send_Settings_Save();

    void Send_video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath,int StartTime,bool isSplitComplete,bool isScaleComplete,int OLDSegmentDuration,int LastVideoClipNo);


    // Asynchronous file processing for drag and drop
    void ProcessDroppedFilesAsync(QList<QUrl> urls);

    //================== current file processing progress =========================
    void Send_CurrentFileProgress_Start(QString FileName,int FrameNum);
    void Send_CurrentFileProgress_Stop();
    void Send_CurrentFileProgress_progressbar_Add();
    void Send_CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration);
    void Send_CurrentFileProgress_progressbar_SetFinishedValue(int FinishedValue);

    void Send_Donate_ReplaceQRCode(QString QRCodePath);

    void Send_Set_checkBox_DisableResize_gif_Checked();


private slots:
    void ProcessDroppedFilesFinished(); // Handles post-processing for drag-and-dropped files.
    // Add other necessary private slots here if any. (on_pushButton_Patreon_clicked was moved to public)


private:
    // Helper function declarations for miscellaneous compilation errors
    void LoadScaledImageToLabel(const QString &imagePath, QLabel *label); // QLabel included
    void UpdateTotalProcessedFilesCount();
    void ProcessNextFile();
    void CheckIfAllFinished();
    void UpdateNumberOfActiveThreads();
    void UpdateProgressBar();
    void ShellMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon);

    Ui::MainWindow *ui;
    // Missing public member functions (if any, Waifu2x() and Check_PreLoad_Settings() were in error log)
public:
    int Waifu2x(); // Declaration based on mainwindow.cpp error
    bool Check_PreLoad_Settings(); // Declaration based on mainwindow.cpp error
};

