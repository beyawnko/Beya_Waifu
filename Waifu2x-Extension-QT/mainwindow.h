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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <QTime>
// BUILD REQUIREMENT: Ensure the Qt Multimedia module is included in the project file.
// e.g., in a .pro file: QT += multimedia
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
#include <cmath>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMetaType>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QThread>
#include <QThreadPool>
#include <QFileSystemWatcher>
#include <QListWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <atomic>
#include "topsupporterslist.h"
#include "FileManager.h"
#include "ProcessRunner.h"
#include "GpuManager.h"
#include "UiController.h"
#include "LiquidGlassWidget.h"
#include "anime4kprocessor.h"
#include "realesrganprocessor.h"


#ifndef Q_DECLARE_METATYPE
#define Q_DECLARE_METATYPE(Type)
#endif

typedef QList<QMap<QString, QString>> QList_QMap_QStrQStr;
Q_DECLARE_METATYPE(QList_QMap_QStrQStr)

// Struct for caching file metadata
struct FileMetadata {
  bool isValid = false;
  int width = 0;
  int height = 0;
  QString fps;
  double duration = 0.0;
  QString bitRate;
  qint64 frameCount = 0;
  bool isVFR = false;
  QString identifyOutput;
  bool isAnimated = false;
  QString fileFormat;
};
Q_DECLARE_METATYPE(FileMetadata)

// Struct for loading file info from list
struct FileLoadInfo {
  QString fileName;
  QString fullPath;
  QString status; // "Waiting", "Finished", "Error" from INI
  QString customResolutionWidth; // From INI
  QString customResolutionHeight; // From INI
};
Q_DECLARE_METATYPE(FileLoadInfo)

// Enum for processing status
// Enum for processing status - Renamed to avoid conflict
enum ProcessJobType { // Renamed from ProcessingType
  PROCESS_TYPE_NONE = 0,
  PROCESS_TYPE_IMAGE,
  PROCESS_TYPE_VIDEO,
  PROCESS_TYPE_GIF
};

enum class ProcessingState {
  Idle,
  Processing,
  Stopping,
  Error // Added for completeness, might not be used if errors are handled per-file
};

struct ProcessJob {
  int rowNum;
  ProcessJobType type; // Changed from ProcessingType
};

QT_BEGIN_NAMESPACE
namespace Ui
{
  class MainWindow;
}
QT_END_NAMESPACE

class RealCuganProcessor;
class VideoProcessor;
class RealEsrganProcessor;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(int maxThreadsOverride = 0, QWidget *parent = nullptr);
  void changeEvent(QEvent *e);
  void resizeEvent(QResizeEvent *event) override;
  void startNextFileProcessing(); // Moved to public for QtConcurrent

  //======================= Version Info =======================
  QString VERSION = "v3.41.02-beta";
  bool isBetaVer = true;
  QString LastStableVer = "v3.31.13";
  QString LastBetaVer = "v3.41.02-beta";

  //======================= Core Components =======================
  RealCuganProcessor *realCuganProcessor;
  VideoProcessor *videoProcessor;
  RealEsrganProcessor *m_realEsrganProcessor;
  Anime4KProcessor *m_anime4kProcessor;
  FileManager fileManager;
  ProcessRunner processRunner;
  std::atomic<bool> waifu2x_STOP{false};
  GpuManager gpuManager;
  UiController uiController;

  //======================= Missing Member Variables (Stubs for realcugan_ncnn_vulkan.cpp) =======================
  QProcess* currentProcess; // Should be initialized (e.g., to nullptr) in constructor
  QStringList table_image_item_fullpath;
  QStringList table_image_item_fileName;
  QStringList table_video_item_fileName;
  QStringList table_gif_item_fileName;
  ProcessJobType Processing_Status; // Changed from ProcessingType to ProcessJobType
  int current_File_Row_Number;
  QString TempDir_Path; // Should be initialized

  //======================= UI & Styling =======================
  QTranslator * translator;
  QIcon *MainIcon_QIcon = new QIcon(":/new/prefix1/icon/icon_main.png");
  TopSupportersList *TopSupportersList_widget;
  void Set_Font_fixed();
  bool SystemPrefersDark() const;
  void ApplyDarkStyle();
  void setImageEngineIndex(int index);
  void setGifEngineIndex(int index);
  void setVideoEngineIndex(int index);

  //======================= Paths & Config =======================
  QString Current_Path = qApp->applicationDirPath();
  QString FFMPEG_EXE_PATH_Waifu2xEX; // Required
  int globalMaxThreadCount = 0;

  //======================= File Handling & Processing =======================
  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  void Read_urls(QList<QUrl> urls, const QSet<QString>& existingImagePaths_set, const QSet<QString>& existingGifPaths_set, const QSet<QString>& existingVideoPaths_set);
  void Read_Input_paths_BrowserFile(QStringList Input_path_List);
  bool AddNew_gif=false;
  bool AddNew_image=false;
  bool AddNew_video=false;
  void Add_File_Folder(QString Full_Path, QList<QPair<QString, QString>>& imagesToAdd, QList<QPair<QString, QString>>& gifsToAdd, QList<QPair<QString, QString>>& videosToAdd, bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo, const QSet<QString>& existingImagePaths_set, const QSet<QString>& existingGifPaths_set, const QSet<QString>& existingVideoPaths_set);
  void Add_File_Folder_IncludeSubFolder(QString Full_Path, QList<QPair<QString, QString>>& imagesToAdd, QList<QPair<QString, QString>>& gifsToAdd, QList<QPair<QString, QString>>& videosToAdd, bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo, const QSet<QString>& existingImagePaths_set, const QSet<QString>& existingGifPaths_set, const QSet<QString>& existingVideoPaths_set);
  QStringList getFileNames_IncludeSubFolder(QString path);
  int FileList_Add(QString fileName, QString SourceFile_fullPath, QList<QPair<QString, QString>>& imagesToAdd, QList<QPair<QString, QString>>& gifsToAdd, QList<QPair<QString, QString>>& videosToAdd, bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo, const QSet<QString>& existingImagePaths_set, const QSet<QString>& existingGifPaths_set, const QSet<QString>& existingVideoPaths_set);
  bool Deduplicate_filelist(QString SourceFile_fullPath); // This is the old main-thread one
  bool file_isDirExist(QString SourceFile_fullPath);
  void file_mkDir(QString SourceFile_fullPath);
  bool file_isDirEmpty(QString FolderPath);
  QStringList file_getFileNames_in_Folder_nofilter(QString path);
  bool file_DelDir(const QString &path);
  QString file_getBaseName(QString path);
  void file_MoveToTrash( QString file );
  void MoveFileToOutputPath(QString Orginal,QString SourceFilePath);
  QString file_getFolderPath(QFileInfo fileInfo);
  bool file_isDirWritable(QString DirPath);
  bool file_isFilesFolderWritable_row_image(int rowNum);
  bool file_isFilesFolderWritable_row_video(int rowNum);
  bool file_isFilesFolderWritable_row_gif(int rowNum);
  bool file_OpenFolder(QString FolderPath);
  bool file_OpenFilesFolder(QString FilePath);
  bool file_OpenFile(QString FilePath);
  void file_MoveFiles_Folder_NcnnVulkanFolderProcess(QString Old_folder, QString New_folder, bool Delete_);
  bool file_generateMarkFile(QString FileFullPath,QString Msg);

  //=================================  Table Management =================================
  void ui_tableViews_setUpdatesEnabled(bool isEnabled);
  void Init_Table();
  QStandardItemModel *Table_model_image;
  QStandardItemModel *Table_model_video;
  QStandardItemModel *Table_model_gif;
  void Table_image_CustRes_Cancel_rowNumInt(int rowNum);
  void Table_gif_CustRes_Cancel_rowNumInt(int rowNum);
  void Table_video_CustRes_Cancel_rowNumInt(int rowNum);
  void Table_ChangeAllStatusToWaiting();
  QMutex QMutex_Table_ChangeAllStatusToWaiting;
  void Table_Clear();
  int Table_image_get_rowNum();
  int Table_gif_get_rowNum();
  int Table_video_get_rowNum();
  QMap<QString, QString> Table_Read_status_fullpath(QStandardItemModel *Table_model);
  int curRow_image = -1;
  int curRow_gif = -1;
  int curRow_video = -1;
  int Table_Save_Current_Table_Filelist(QString Table_FileList_ini);
  int Table_Read_Saved_Table_Filelist(QString Table_FileList_ini);
  int Table_Save_Current_Table_Filelist_Watchdog(QString Table_FileList_ini);

  QStringList getImageFullPaths() const;

  //================================= Main Processing Logic (Waifu2x & Others) ====================================
  void ShowFileProcessSummary();
  QString OutPutFolder_main="";
  int Waifu2xMainThread();
  QStringList WaitForEngineIO(QStringList OutPutFilesFullPathList);
  QStringList WaitForEngineIO_NcnnVulkan(QString OutputFolderFullPath);
  void Restore_SplitFramesFolderPath(QString SplitFramesFolderPath, QStringList GPU_SplitFramesFolderPath_List);
  bool isForceRetryEnabled=true;
  bool KILL_TASK_QStringList(QStringList TaskNameList,bool RequestAdmin);

  QString Waifu2x_ncnn_vulkan_FolderPath = "";
  QString Waifu2x_ncnn_vulkan_ProgramPath = "";
  int Waifu2x_NCNN_Vulkan_Image(int rowNum,bool ReProcess_MissingAlphaChannel);
  int Waifu2x_NCNN_Vulkan_GIF(int rowNum);
  int Waifu2x_NCNN_Vulkan_Video(int rowNum);
  int Waifu2x_NCNN_Vulkan_Video_BySegment(int rowNum);
  QString Waifu2x_NCNN_Vulkan_ReadSettings();
  QString Waifu2x_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum);
  QString Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = "";
  QStringList Available_GPUID_Waifu2xNcnnVulkan;
  QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan;

  // Anime4K related members are handled by Anime4KProcessor

  int SRMD_CUDA_Image(int rowNum,bool ReProcess_MissingAlphaChannel);
  int SRMD_CUDA_GIF(int rowNum);
  int SRMD_CUDA_Video(int rowNum);
  int SRMD_CUDA_Video_BySegment(int rowNum);
  QString SRMD_CUDA_PreLoad_Settings_Str = "";

  QString m_realcugan_Model;
  int m_realcugan_Scale;
  int m_realcugan_DenoiseLevel;
  int m_realcugan_TileSize;
  bool m_realcugan_TTA;
  QString m_realcugan_GPUID;
  QList<QMap<QString, QString>> m_realcugan_gpuJobConfig_temp;
  QList<QProcess*> ProcList_RealCUGAN;
  QStringList Available_GPUID_RealCUGAN;
  QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_RealCUGAN;
  void Realcugan_NCNN_Vulkan_Image(int rowNum, bool experimental, bool ReProcess_MissingAlphaChannel);
  void Realcugan_NCNN_Vulkan_GIF(int rowNum);
  void Realcugan_NCNN_Vulkan_Video(int rowNum);
  void Realcugan_NCNN_Vulkan_ReadSettings();
  void Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum);
  bool APNG_RealcuganNCNNVulkan(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath);
  void Realcugan_ncnn_vulkan_DetectGPU();
  QString RealcuganNcnnVulkan_MultiGPU();
  void AddGPU_MultiGPU_RealcuganNcnnVulkan(const QString &gpuid, int threads = 1, int tile = 0);
  void RemoveGPU_MultiGPU_RealcuganNcnnVulkan(const QString &gpuid);
  void Realcugan_NCNN_Vulkan_PreLoad_Settings();
  QString Realcugan_NCNN_Vulkan_PreLoad_Settings_Str = "";
  QStringList Realcugan_NCNN_Vulkan_PrepareArguments(const QString &inputFile, const QString &outputFile, int currentPassScale, const QString &modelName, int denoiseLevel, int tileSize, const QString &gpuId, bool ttaEnabled, const QString &outputFormat, bool isMultiGPU, const QString &multiGpuJobArgs, bool experimental);
  void StartNextRealCUGANPass(QProcess *process);
  void Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString &tempPathBase, int maxPassIndex, bool keepFinal = false, const QString& finalFile = "");
  bool Realcugan_ProcessSingleFileIteratively(const QString &inputFile, const QString &outputFile, int targetScale, int originalWidth, int originalHeight, const QString &modelName, int denoiseLevel, int tileSize, const QString &gpuIdOrJobConfig, bool isMultiGPUJob, bool ttaEnabled, const QString &outputFormat, bool experimental, int rowNumForStatusUpdate = -1);

  QString Rife_NCNN_Vulkan_PreLoad_Settings_Str = "";
  QString Cain_NCNN_Vulkan_PreLoad_Settings_Str = "";
  QString Dain_NCNN_Vulkan_PreLoad_Settings_Str = "";
  int FrameInterpolation_DetectGPU();
  QStringList Available_GPUID_FrameInterpolation;
  int Old_FrameInterpolation_Engine_Index=0;
  QString FrameInterpolation_ReadConfig(bool isUhdInput,int NumOfFrames);
  bool FrameInterpolation(QString SourcePath,QString OutputPath);
  int FrameInterpolation_Video(int rowNum);
  int FrameInterpolation_Video_BySegment(int rowNum);

  void Wait_waifu2x_stop();
  int ThreadNumMax = 0;
  std::atomic<int> ThreadNumRunning{0};
  QMutex mutex_ThreadNumRunning;
  QMutex mutex_SubThreadNumRunning;

  int Waifu2x_DetectGPU();
  int Waifu2x_DumpProcessorList_converter();
  int Core_num = 0;
  QStringList Available_ProcessorList_converter;
  QString Processor_converter_STR="";
  int SRMD_DetectGPU();
  QStringList Available_GPUID_srmd;
  QString GPU_ID_STR_SRMD="";

  bool Image_Gif_AutoSkip_CustRes(int rowNum,bool isGif);
  QMap<QString,int> Image_Gif_Read_Resolution(QString SourceFileFullPath);
  QString SaveImageAs_FormatAndQuality(QString OriginalSourceImage_fullPath, QString ScaledImage_fullPath, bool isDenoiseLevelEnabled, int DenoiseLevel);
  struct AlphaInfo {
    bool hasAlpha = false;
    bool is16Bit = false;
    QString rgbPath;
    QString alphaPath;
    QString tempDir;
  };
  AlphaInfo PrepareAlpha(const QString &inputImagePath);
  void RestoreAlpha(const AlphaInfo &info, const QString &processedRgbPath, const QString &finalOutputPath);
  void Gif_RemoveFromCustResList(int RowNumber);
  bool Gif_DoubleScaleRatioPrep(int RowNumber);
  int Gif_getDuration(QString gifPath);
  int Gif_getFrameDigits(QString gifPath);
  void Gif_splitGif(QString gifPath,QString SplitFramesFolderPath);
  void Gif_assembleGif(QString ResGifPath, QString ScaledFramesPath, int Duration, bool CustRes_isEnabled, int CustRes_height, int CustRes_width, bool isOverScaled, QString SourceGifFullPath);
  QString Gif_compressGif(QString gifPath,QString gifPath_compressd);
  void video_RemoveFromCustResList(int RowNumber);
  bool video_DoubleScaleRatioPrep(int RowNumber);
  QString isPreVFIDone_MarkFilePath(QString VideoPath);
  int CalNumDigits(int input_num);
  bool isSuccessiveFailuresDetected_VFI=false;
  bool Video_AutoSkip_CustRes(int rowNum);
  QMap<QString,int> video_get_Resolution(QString VideoFileFullPath);
  QString video_get_fps(QString videoPath);
  int video_get_frameNumDigits(QString videoPath);
  long long video_get_frameNum(QString videoPath);
  int video_get_duration(QString videoPath);
  bool video_isVFR(QString videoPath);
  void video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath);
  int video_images2video(QString VideoPath, QString video_mp4_scaled_fullpath, QString ScaledFrameFolderPath, QString AudioPath, bool CustRes_isEnabled, int CustRes_height, int CustRes_width, bool isOverScaled);
  QString video_ReadSettings_OutputVid(QString AudioPath);
  QString video_get_bitrate(QString videoPath,bool isReturnFullCMD,bool isVidOnly);
  QString video_get_bitrate_AccordingToRes_FrameFolder(QString ScaledFrameFolderPath,QString VideoPath);
  int video_UseRes2CalculateBitrate(QString VideoFileFullPath);
  QString video_AudioDenoise(QString OriginalAudioPath);
  QString video_To_CFRMp4(QString VideoPath);
  void video_get_audio(QString VideoPath,QString AudioPath);
  void video_video2images_ProcessBySegment(QString VideoPath, QString FrameFolderPath, int StartTime, int SegmentDuration);
  void video_AssembleVideoClips(QString VideoClipsFolderPath, QString VideoClipsFolderName, QString video_mp4_scaled_fullpath, QString AudioPath);
  QString video_getClipsFolderNo();
  QMutex MultiLine_ErrorOutput_QMutex;
  bool video_isNeedProcessBySegment(int rowNum);
  void DelVfiDir(QString VideoPath);

  FileMetadata getOrFetchMetadata(const QString &filePath);
  QMap<QString, FileMetadata> m_metadataCache;
  QMutex m_metadataCacheMutex;

  int Waifu2x_Compatibility_Test();
  int Simple_Compatibility_Test();
  void waitForCompatibilityTest();
  void Finish_progressBar_CompatibilityTest();
  bool isCompatible_RealCUGAN_NCNN_Vulkan=false;
  bool isCompatible_RealESRGAN_NCNN_Vulkan=false;
  bool isCompatible_RifeNcnnVulkan = false;
  bool isCompatible_CainNcnnVulkan = false;
  bool isCompatible_DainNcnnVulkan = false;
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

  QList<QMap<QString, QString>> Custom_resolution_list;
  void CustRes_remove(QString fullpath);
  bool CustRes_isContained(QString fullpath);
  QMap<QString, QString> CustRes_getResMap(QString fullpath);
  int CustRes_CalNewScaleRatio(QString fullpath,int Height_new,int width_new);
  int CustRes_SetCustRes();
  int CustRes_CancelCustRes();
  QMap<QString, QString> DoubleScaleRatio_Cal_NewScaleRatio_NewHW(QString fullpath,double ScaleRatio_double);
  Qt::AspectRatioMode CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;

  int Settings_Read_Apply();
  bool Settings_isReseted = false;
  QVariant Settings_Read_value(QString Key);
  QVariant Settings_Read_value(const QString &key, const QVariant &defaultValue);
  bool isReadOldSettings = false;
  void PreLoad_Engines_Settings();
  int Calculate_Temporary_ScaleRatio_W2xNCNNVulkan(int ScaleRatio);

  QString Generate_Output_Path(const QString& original_filePath, const QString& suffix);
  void Set_Progress_Bar_Value(int val, int max_val);
  void SetCurrentFileProgressBarValue(int val, int max_val);

  void APNG_Main(int rowNum,bool isFromImageList);
  void APNG_Split2Frames(QString sourceFileFullPath,QString splitFramesFolder);
  void APNG_Frames2APNG(QString sourceFileFullPath, QString scaledFramesFolder, QString resultFileFullPath, bool isOverScaled);
  bool APNG_isAnimatedPNG(int rowNum);

  bool isCustomVideoSettingsClicked=true;
  void pushButton_Start_setEnabled_self(bool isEnabled);
  void pushButton_Stop_setEnabled_self(bool isEnabled);
  void TurnOffScreen();
  QFuture<void> TurnOffScreen_QF;
  bool FileProgressWatch_isEnabled = true;
  QFileSystemWatcher *FileProgressWatcher = nullptr;
  QFileSystemWatcher *FileProgressWatcher_Text = nullptr;
  QTimer *FileProgressStopTimer = nullptr;
  QTimer *FileProgressStopTimer_Text = nullptr;
  int ForceRetryCount = 1;
  void OutputSettingsArea_setEnabled(bool isEnabled);
  bool isFirstTimeStart=false;
  bool isForceRetryClicked=false;
  QMutex isForceRetryClicked_QMutex;
  void DelTrash_ForceRetry_Anime4k(QString OutPut_Path);
  void isForceRetryClicked_SetTrue_Block_Anime4k();
  void AutoFinishAction_Message();
  int SystemShutDown_Countdown();
  int SystemShutDown_isAutoShutDown();
  void Delay_sec_sleep(int time);
  void Delay_msec_sleep(int time);
  void Play_NFSound();
  QMediaPlayer *m_player = nullptr;
  QTimer *TimeCostTimer;
  long unsigned int TimeCost = 0;
  QString Seconds2hms(long unsigned int seconds);
  long unsigned int TaskNumTotal=0;
  long unsigned int TaskNumFinished=0;
  bool NewTaskFinished=false;
  long unsigned int ETA=0;
  long unsigned int Progressbar_MaxVal = 0;
  long unsigned int Progressbar_CurrentVal = 0;
  int CheckUpdate_Auto();
  int Donate_DownloadOnlineQRCode();
  void on_checkBox_BanGitee_clicked();
  bool isSettingsHide=false;
  bool isShowAnime4kWarning=true;
  void ConnectivityTest_RawGithubusercontentCom();
  bool isConnectivityTest_RawGithubusercontentCom_Running=false;
  QMutex QMutex_ConnectivityTest_RawGithubusercontentCom;
  bool DownloadTo(QString OnlineLink,QString LocalPath);
  void closeEvent(QCloseEvent* event) override;
  std::atomic<bool> QProcess_stop{false};
  int Auto_Save_Settings_Watchdog(bool isWaitForSave);
  QFuture<int> AutoUpdate;
  QFuture<int> DownloadOnlineQRCode;
  QFuture<int> Waifu2xMain;
  QFuture<void> compatibilityTestFuture;
  int Force_close();
  std::atomic<bool> isAlreadyClosed{false};
  long unsigned int TimeCost_CurrentFile =0;
  long unsigned int TaskNumTotal_CurrentFile=0;
  long unsigned int TaskNumFinished_CurrentFile=0;
  bool NewTaskFinished_CurrentFile=false;
  bool runProcess(QProcess *process, const QString &cmd, QByteArray *stdOut = nullptr, QByteArray *stdErr = nullptr);
  long unsigned int ETA_CurrentFile=0;
  bool isStart_CurrentFile=false;
  void Tip_FirstTimeStart();
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
  void Init_ActionsMenu_lineEdit_outputPath();
  QAction *OpenFolder_lineEdit_outputPath = new QAction(this);

  void Init_ActionsMenu_FilesList();
  QAction *OpenFile_QAction_FileList = new QAction(this);
  QAction *OpenFilesFolder_QAction_FileList = new QAction(this);
  QAction *RemoveFile_FilesList_QAction_FileList = new QAction(this);
  bool EnableApply2All_CustRes=true;
  QAction *Apply_CustRes_QAction_FileList = new QAction(this);
  QAction *Cancel_CustRes_QAction_FileList = new QAction(this);
  void OpenSelectedFile_FailedWarning_FilesList();
  void Init_ActionsMenu_pushButton_RemoveItem();
  QAction *RemoveALL_image = new QAction(this);
  QAction *RemoveALL_gif = new QAction(this);
  QAction *RemoveALL_video = new QAction(this);
  void ExecuteCMD_batFile(QString cmd_str,bool requestAdmin);
  QMutex ExecuteCMD_batFile_QMutex;
  void Del_TempBatFile();
  void checkBox_ReplaceOriginalFile_setEnabled_True_Self();
  bool ReplaceOriginalFile(QString original_fullpath,QString output_fullpath);
  void Init_ActionsMenu_checkBox_ReplaceOriginalFile();
  QAction *QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile = new QAction(this);
  void Init_ActionsMenu_checkBox_DelOriginal();
  QAction *QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal = new QAction(this);
  int AddTileSize_NCNNVulkan_Converter(int OrginalTileSize);
  int MinusTileSize_NCNNVulkan_Converter(int OrginalTileSize);
  void ImagesResize_Folder_MultiThread(int New_width,int New_height,QString ImagesFolderPath);
  int TotalNumOfThreads_ImagesResize_Folder_MultiThread;
  int RunningNumOfThreads_ImagesResize_Folder_MultiThread;
  QMutex QMutex_ResizeImage_MultiThread;
  void ResizeImage_MultiThread(int New_width,int New_height,QString ImagesPath);
  ~MainWindow();
  int Waifu2x();
  bool Check_PreLoad_Settings();

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
  QListWidget *listWidget_GPUList_MultiGPU_RealCUGAN;
  QPushButton *pushButton_AddGPU_MultiGPU_RealCUGAN;
  QPushButton *pushButton_RemoveGPU_MultiGPU_RealCUGAN;
  QPushButton *pushButton_ClearGPU_MultiGPU_RealCUGAN;
  QPushButton *pushButton_TileSize_Add_RealCUGAN;
  QPushButton *pushButton_TileSize_Minus_RealCUGAN;
  QWidget *widget_RealCUGAN_Hidden;

  QComboBox *comboBox_Model_RealsrNCNNVulkan;
  QComboBox *comboBox_GPUID_RealsrNCNNVulkan;
  QPushButton *pushButton_DetectGPU_RealsrNCNNVulkan;
  QSpinBox *spinBox_TileSize_RealsrNCNNVulkan;
  QPushButton *pushButton_Add_TileSize_RealsrNCNNVulkan;
  QPushButton *pushButton_Minus_TileSize_RealsrNCNNVulkan;
  QCheckBox *checkBox_TTA_RealsrNCNNVulkan;
  QCheckBox *checkBox_MultiGPU_RealesrganNcnnVulkan;
  QGroupBox *groupBox_GPUSettings_MultiGPU_RealesrganNcnnVulkan;
  QComboBox *comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan;
  QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan;
  QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan;
  QPushButton *pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan;

public slots:
  void Batch_Table_Update_slots(const QList<FileLoadInfo>& imageFiles,
                                const QList<FileLoadInfo>& gifFiles,
                                const QList<FileLoadInfo>& videoFiles,
                                bool addNewImage, bool addNewGif, bool addNewVideo);
  void Table_EnableSorting(bool EnableSorting);
  void Apply_CustRes_QAction_FileList_slot();
  void Cancel_CustRes_QAction_FileList_slot();
  void RemoveALL_image_slot();
  void RemoveALL_gif_slot();
  void RemoveALL_video_slot();
  void Add_progressBar_CompatibilityTest();
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
  void progressbar_setRange_min_max_slots(int min, int max_val);
  void progressbar_Add_slots();
  void Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
  void Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
  void Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
  void Waifu2x_Finished();
  void Waifu2x_Finished_manual();
  void TextBrowser_NewMessage(QString message);
  void TimeSlot();
  int Waifu2x_Compatibility_Test_finished();
  int Waifu2x_DetectGPU_finished();
  int FrameInterpolation_DetectGPU_finished();
  int Realcugan_NCNN_Vulkan_DetectGPU_finished();
  int CheckUpdate_NewUpdate(QString update_str, QString Change_log);
  void FinishedProcessing_DN();
  int Table_FileCount_reload();
  void Table_image_insert_fileName_fullPath(const FileLoadInfo& fileInfo);
  void Table_gif_insert_fileName_fullPath(const FileLoadInfo& fileInfo);
  void Table_video_insert_fileName_fullPath(const FileLoadInfo& fileInfo);
  void Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
  void Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
  void Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
  int Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini);
  int Table_Save_Current_Table_Filelist_Finished();
  void on_pushButton_ClearList_clicked();
  bool SystemShutDown();
  int Waifu2x_DumpProcessorList_converter_finished();
  void Read_urls_finfished();
  void SRMD_DetectGPU_finished();
  void video_write_VideoConfiguration(QString VideoConfiguration_fullPath, int ScaleRatio, int DenoiseLevel, bool CustRes_isEnabled, int CustRes_height, int CustRes_width, QString EngineName, bool isProcessBySegment, QString VideoClipsFolderPath, QString VideoClipsFolderName, bool isVideoFrameInterpolationEnabled, int MultipleOfFPS);
  int Settings_Save();
  void video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath, int StartTime, bool isSplitComplete, bool isScaleComplete, int OLDSegmentDuration, int LastVideoClipNo);
  void CurrentFileProgress_Start(QString FileName,int FrameNum);
  void CurrentFileProgress_Stop();
  void CurrentFileProgress_progressbar_Add();
  void CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration);
  void CurrentFileProgress_progressbar_SetFinishedValue(int FinishedValue);
  void CurrentFileProgress_WatchFolderFileNum(QString FolderPath);
  void CurrentFileProgress_WatchFolderFileNum_Textbrower(QString SourceFile_fullPath, QString FolderPath, int TotalFileNum);
  void Donate_ReplaceQRCode(QString QRCodePath);
  void Set_checkBox_DisableResize_gif_Checked();
  void on_pushButton_Patreon_clicked();
  void on_pushButton_SupportersList_clicked();
  void on_comboBox_ImageSaveFormat_currentIndexChanged(int index);
  void on_pushButton_Stop_clicked();
  int on_pushButton_RemoveItem_clicked();
  void on_pushButton_Report_clicked();
  void on_pushButton_ReadMe_clicked();
  void on_comboBox_Engine_Image_currentIndexChanged(int index);
  void on_comboBox_Engine_GIF_currentIndexChanged(int index);
  void on_comboBox_Engine_Video_currentIndexChanged(int index);
  void on_pushButton_clear_textbrowser_clicked();
  void on_spinBox_textbrowser_fontsize_valueChanged(int arg1);
  void on_pushButton_HideSettings_clicked();
  void on_pushButton_ReadFileList_clicked();
  void on_Ext_image_editingFinished();
  void on_Ext_video_editingFinished();
  void on_checkBox_AutoSaveSettings_clicked();
  void on_pushButton_about_clicked();
  void on_comboBox_AspectRatio_custRes_currentIndexChanged(int index);
  void on_checkBox_AlwaysHideSettings_stateChanged(int arg1);
  void on_pushButton_Save_GlobalFontSize_clicked();
  void on_pushButton_BrowserFile_clicked();
  void on_pushButton_CheckUpdate_clicked();
  void on_pushButton_compatibilityTest_clicked();
  void on_pushButton_wiki_clicked();
  void on_pushButton_HideTextBro_clicked();
  void on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1);
  void on_Ext_image_textChanged(const QString &arg1);
  void on_Ext_video_textChanged(const QString &arg1);
  void on_comboBox_model_vulkan_currentIndexChanged(int index);
  void on_comboBox_ImageStyle_currentIndexChanged(int index);
  void on_pushButton_ResetVideoSettings_clicked();
  void on_lineEdit_encoder_vid_textChanged(const QString &arg1);
  void on_lineEdit_encoder_audio_textChanged(const QString &arg1);
  void on_lineEdit_pixformat_textChanged(const QString &arg1);
  void on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1);
  void on_checkBox_acodec_copy_2mp4_stateChanged(int arg1);
  void on_pushButton_encodersList_clicked();
  void on_checkBox_DelOriginal_stateChanged(int arg1);
  void on_checkBox_FileList_Interactive_stateChanged(int arg1);
  void on_checkBox_OutPath_isEnabled_stateChanged(int arg1);
  void on_pushButton_ForceRetry_clicked();
  void on_pushButton_PayPal_clicked();
  void on_checkBox_AudioDenoise_stateChanged(int arg1);
  void on_tabWidget_currentChanged(int index);
  void on_checkBox_ProcessVideoBySegment_stateChanged(int arg1);
  void on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index);
  void on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1);
  void on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1);
  void on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1);
  void on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked();
  void on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked();
  void on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked();
  void on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked();
  void on_checkBox_isCompatible_SRMD_CUDA_clicked();
  void on_checkBox_isCompatible_Waifu2x_Converter_clicked();
  void on_checkBox_isCompatible_Anime4k_CPU_clicked();
  void on_checkBox_isCompatible_Anime4k_GPU_clicked();
  void on_checkBox_isCompatible_FFmpeg_clicked();
  void on_checkBox_isCompatible_FFprobe_clicked();
  void on_checkBox_isCompatible_ImageMagick_clicked();
  void on_checkBox_isCompatible_Gifsicle_clicked();
  void on_checkBox_isCompatible_SoX_clicked();
  void on_checkBox_GPUMode_Anime4K_stateChanged(int arg1);
  void on_checkBox_ShowInterPro_stateChanged(int arg1);
  void on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked();
  void on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked();
  void on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked();
  void on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked();
  void on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked();
  void on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked();
  void on_checkBox_ACNet_Anime4K_stateChanged(int arg1);
  void on_checkBox_HDNMode_Anime4k_stateChanged(int arg1);
  void on_checkBox_ReplaceOriginalFile_stateChanged(int arg1);
  void on_checkBox_isCustFontEnable_stateChanged(int arg1);
  void on_comboBox_GPGPUModel_A4k_currentIndexChanged(int index);
  void on_checkBox_DisableGPU_converter_stateChanged(int arg1);
  void on_groupBox_video_settings_clicked();
  void on_pushButton_TurnOffScreen_clicked();
  void on_pushButton_MultipleOfFPS_VFI_MIN_clicked();
  void on_pushButton_MultipleOfFPS_VFI_ADD_clicked();
  void on_comboBox_TargetProcessor_converter_currentIndexChanged(int index);
  void on_pushButton_CustRes_cancel_clicked();
  void on_pushButton_CustRes_apply_clicked();
  void on_pushButton_DetectGPU_VFI_clicked();
  void on_lineEdit_MultiGPU_IDs_VFI_editingFinished();
  void on_checkBox_MultiGPU_VFI_stateChanged(int arg1);
  void on_groupBox_FrameInterpolation_clicked();
  void on_checkBox_EnableVFI_Home_clicked();
  void on_checkBox_isCompatible_RifeNcnnVulkan_clicked();
  void on_checkBox_isCompatible_CainNcnnVulkan_clicked();
  void on_checkBox_isCompatible_DainNcnnVulkan_clicked();
  void on_comboBox_Engine_VFI_currentIndexChanged(int index);
  void on_pushButton_Verify_MultiGPU_VFI_clicked();
  void on_checkBox_MultiThread_VFI_stateChanged(int arg1);
  void on_checkBox_MultiThread_VFI_clicked();
  void on_pushButton_DetectGPU_RealCUGAN_clicked();
  void on_checkBox_MultiGPU_RealCUGAN_stateChanged(int arg1);
  void on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked();
  void on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked();
  void on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked();
  void on_pushButton_TileSize_Add_RealCUGAN_clicked();
  void on_pushButton_TileSize_Minus_RealCUGAN_clicked();
  void on_comboBox_Model_RealCUGAN_currentIndexChanged(int index);
  void Realcugan_NCNN_Vulkan_Iterative_finished(int exitCode, QProcess::ExitStatus exitStatus);
  void Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput();
  void Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError();
  void Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error);
  void Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error);

  void ProgressBarClear();
  void ProgressBarSetToMax(int maxval);
  void on_pushButton_SaveFileList_clicked();
  void on_tableView_image_doubleClicked(const QModelIndex &index);
  void on_tableView_gif_doubleClicked(const QModelIndex &index);
  void on_tableView_video_doubleClicked(const QModelIndex &index);
  void on_tableView_image_pressed(const QModelIndex &index);
  void on_tableView_gif_pressed(const QModelIndex &index);
  void on_tableView_video_pressed(const QModelIndex &index);
  void on_pushButton_SaveSettings_clicked();
  void on_pushButton_ResetSettings_clicked();

  // Slots for VFI synchronization
  void on_groupBox_FrameInterpolation_toggled(bool checked);

  // Slots for generic processor signals
  void onFileProgress(int rowNum, int percent);
  void onProcessingFinished(int rowNum, bool success);

  // Compatibility Test Slots
  void StartFullCompatibilityTest();

private slots:
  void TextBrowser_StartMes();
  void toggleLiquidGlass(bool enabled);
  void on_pushButton_Start_clicked();

private:
  void RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay();
  void Anime4k_Image(int rowNum, bool useTTA);
  void RealESRGAN_NCNN_Vulkan_Image(int rowNum, bool useTTA);
  void RealESRGAN_NCNN_Vulkan_Video(int rowNum);

  int Waifu2x_Caffe_Image(int r, bool flag);
  int Waifu2x_Converter_Image(int r, bool flag);
  int Realsr_ncnn_vulkan_DetectGPU_finished();
  int RealESRGAN_ncnn_vulkan_DetectGPU_finished();

  void tryStartNextFile();
  void CheckIfAllFinished();
  void UpdateNumberOfActiveThreads();
  void UpdateProgressBar();
  void ShellMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon);
  bool Deduplicate_filelist_worker(const QString& SourceFile_fullPath, const QSet<QString>& existingImagePaths_set, const QSet<QString>& existingGifPaths_set, const QSet<QString>& existingVideoPaths_set);
  void ProcessFileListWorker(QString file_list_Path, const QSet<QString>& existingImagePaths, const QSet<QString>& existingGifPaths, const QSet<QString>& existingVideoPaths);

  // Compatibility Test methods
  void ExecuteCompatibilityTests();
  bool TestEngineCommand(const QString& engineName, const QString& executablePath, const QStringList& arguments, QCheckBox* checkBox);

  bool m_isVFISyncing = false;
  LiquidGlassWidget *glassWidget {nullptr};
  bool glassEnabled {false};

  ProcessingState m_currentState;
  QQueue<ProcessJob> m_jobQueue;
  bool isProcessing = false;
  bool waifu2x_STOP_confirm = false;
  int m_NumProc = 0;
  int m_ErrorProc = 0;
  int m_TotalNumProc = 0;
  int m_StartedProc = 0;
  int m_FinishedProc = 0;

  Ui::MainWindow *ui;

signals:
  void Send_Table_EnableSorting(bool EnableSorting);
  void Send_Add_progressBar_CompatibilityTest();
  void Send_Unable2Connect_RawGithubusercontentCom();
  void Send_SetEnable_pushButton_ForceRetry_self();
  void Send_SystemTray_NewMessage(QString message);
  void Send_PrograssBar_Range_min_max_slots(int min, int max_val);
  void Send_progressbar_Add_slots();
  void Send_progressbar_Add();
  void Send_Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status);
  void Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString);
  void Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString);
  void Send_Waifu2x_Finished();
  void Send_Waifu2x_Finished_manual();
  void Send_TextBrowser_NewMessage(QString);
  void Send_Waifu2x_Compatibility_Test_finished();
  void Send_Waifu2x_DetectGPU_finished();
  void Send_FinishedProcessing_DN();
  void Send_FrameInterpolation_DetectGPU_finished();
  void Send_CheckUpdate_NewUpdate(QString, QString);
  void Send_Table_FileCount_reload();
  void Send_Table_image_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
  void Send_Table_gif_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);
  void Send_Table_video_insert_fileName_fullPath(QString fileName, QString SourceFile_fullPath);

  void Send_Batch_Table_Update(const QList<FileLoadInfo>& imageFiles,
                               const QList<FileLoadInfo>& gifFiles,
                               const QList<FileLoadInfo>& videoFiles,
                               bool addNewImage, bool addNewGif, bool addNewVideo);

  void Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
  void Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
  void Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width);
  void Send_Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini);
  void Send_Table_Save_Current_Table_Filelist_Finished();
  void Send_SystemShutDown();
  void Send_Waifu2x_DumpProcessorList_converter_finished();
  void Send_Read_urls_finfished();
  void Send_SRMD_DetectGPU_finished();
  void Send_video_write_VideoConfiguration(QString VideoConfiguration_fullPath, int ScaleRatio, int DenoiseLevel, bool CustRes_isEnabled, int CustRes_height, int CustRes_width, QString EngineName, bool isProcessBySegment, QString VideoClipsFolderPath, QString VideoClipsFolderName, bool isVideoFrameInterpolationEnabled, int MultipleOfFPS);
  void Send_Settings_Save();
  void Send_video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath, int StartTime, bool isSplitComplete, bool isScaleComplete, int OLDSegmentDuration, int LastVideoClipNo);
  void Send_CurrentFileProgress_Start(QString FileName,int FrameNum);
  void Send_CurrentFileProgress_Stop();
  void Send_CurrentFileProgress_progressbar_Add();
  void Send_CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration);
  void Send_CurrentFileProgress_progressbar_SetFinishedValue(int FinishedValue);
  void Send_Donate_ReplaceQRCode(QString QRCodePath);
  void Send_Set_checkBox_DisableResize_gif_Checked();
  void Send_Realcugan_ncnn_vulkan_DetectGPU_finished();

};
#endif // MAINWINDOW_H
