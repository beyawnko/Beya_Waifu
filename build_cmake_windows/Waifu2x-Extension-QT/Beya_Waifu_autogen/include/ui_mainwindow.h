/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_40;
    QSplitter *splitter_4;
    QTabWidget *tabWidget;
    QWidget *tab_Donate;
    QGridLayout *gridLayout_22;
    QLabel *label_DonateText;
    QHBoxLayout *horizontalLayout_60;
    QPushButton *pushButton_PayPal;
    QPushButton *pushButton_Patreon;
    QLabel *label_DonateQRCode;
    QWidget *tab_Home;
    QHBoxLayout *homeTabLayout;
    QSplitter *homeMainSplitter;
    QWidget *leftPaneWidget;
    QVBoxLayout *leftPaneLayout;
    QGroupBox *groupBox_FileList;
    QVBoxLayout *verticalLayout_FileList;
    QTabWidget *fileListTabWidget;
    QWidget *tab_Images;
    QVBoxLayout *verticalLayout_ImagesTab;
    QTableView *tableView_image;
    QWidget *tab_Gifs;
    QVBoxLayout *verticalLayout_GifsTab;
    QTableView *tableView_gif;
    QWidget *tab_Videos;
    QVBoxLayout *verticalLayout_VideosTab;
    QTableView *tableView_video;
    QHBoxLayout *horizontalLayout_FileListControls;
    QLabel *label_FileCount;
    QSpacerItem *horizontalSpacer_file_controls;
    QPushButton *pushButton_ClearList;
    QPushButton *pushButton_RemoveItem;
    QPushButton *pushButton_SaveFileList;
    QPushButton *pushButton_ReadFileList;
    QPushButton *pushButton_BrowserFile;
    QGroupBox *groupBox_CurrentFile;
    QVBoxLayout *verticalLayout_CurrentFile;
    QProgressBar *progressBar_CurrentFile;
    QHBoxLayout *horizontalLayout_CurrentFileLabels;
    QLabel *label_FrameProgress_CurrentFile;
    QLabel *label_TimeCost_CurrentFile;
    QLabel *label_TimeRemain_CurrentFile;
    QLabel *label_ETA_CurrentFile;
    QGroupBox *groupBox_Progress;
    QHBoxLayout *horizontalLayout_Progress;
    QLabel *label_progressBar_filenum;
    QProgressBar *progressBar;
    QLabel *label_TimeCost;
    QLabel *label_TimeRemain;
    QLabel *label_ETA;
    QSpacerItem *horizontalSpacer_ProgressControls;
    QPushButton *pushButton_Start;
    QPushButton *pushButton_Stop;
    QPushButton *pushButton_ForceRetry;
    QGroupBox *groupBox_Setting;
    QVBoxLayout *verticalLayout_RightPane;
    QHBoxLayout *horizontalLayout_HideButtons;
    QPushButton *pushButton_HideSettings;
    QPushButton *pushButton_HideTextBro;
    QSpacerItem *horizontalSpacer_HideButtons;
    QGroupBox *groupBox_CustRes;
    QGridLayout *gridLayout_CustRes_New;
    QLabel *label_18;
    QSpinBox *spinBox_CustRes_width;
    QLabel *label_15;
    QSpinBox *spinBox_CustRes_height;
    QLabel *label_44;
    QComboBox *comboBox_AspectRatio_custRes;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_CustRes_apply;
    QPushButton *pushButton_CustRes_cancel;
    QHBoxLayout *horizontalLayout_CustRes_Checkboxes;
    QCheckBox *checkBox_custres_isAll;
    QCheckBox *checkBox_AutoSkip_CustomRes;
    QGroupBox *groupBox_ImageOutputSettings;
    QGridLayout *gridLayout_26;
    QFrame *frame_ImageQuality;
    QGridLayout *gridLayout_45;
    QLabel *label_63;
    QSpinBox *spinBox_ImageQualityLevel;
    QLabel *label_ImageStyle_W2xNCNNVulkan;
    QLabel *label_ImageStyle_W2xCaffe;
    QFrame *frame_ImageFormat;
    QGridLayout *gridLayout_42;
    QComboBox *comboBox_ImageSaveFormat;
    QLabel *label_20;
    QComboBox *comboBox_ImageStyle;
    QComboBox *comboBox_ImageStyle_Waifu2xCaffe;
    QFrame *frame_13;
    QGridLayout *gridLayout_66;
    QCheckBox *checkBox_DelOriginal;
    QCheckBox *checkBox_ReplaceOriginalFile;
    QCheckBox *checkBox_OptGIF;
    QComboBox *comboBox_FinishAction;
    QCheckBox *checkBox_ReProcFinFiles;
    QGroupBox *groupBox_OutPut;
    QGridLayout *gridLayout_23;
    QLineEdit *lineEdit_outputPath;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBox_OutPath_isEnabled;
    QCheckBox *checkBox_AutoOpenOutputPath;
    QCheckBox *checkBox_OutPath_Overwrite;
    QGroupBox *groupBox_ScaleRaton_DenoiseLevel;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_51;
    QLabel *label_57;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox_ScaleRatio_image;
    QLabel *label_16;
    QDoubleSpinBox *doubleSpinBox_ScaleRatio_gif;
    QLabel *label_17;
    QDoubleSpinBox *doubleSpinBox_ScaleRatio_video;
    QHBoxLayout *horizontalLayout_52;
    QLabel *label_66;
    QLabel *label_ImageDenoiseLevel;
    QSpinBox *spinBox_DenoiseLevel_image;
    QLabel *label_GIFDenoiseLevel;
    QSpinBox *spinBox_DenoiseLevel_gif;
    QLabel *label_VideoDenoiseLevel;
    QSpinBox *spinBox_DenoiseLevel_video;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_28;
    QHBoxLayout *horizontalLayout_50;
    QPushButton *pushButton_MultipleOfFPS_VFI_MIN;
    QSpinBox *spinBox_MultipleOfFPS_VFI;
    QPushButton *pushButton_MultipleOfFPS_VFI_ADD;
    QSpacerItem *verticalSpacer_RightPane;
    QWidget *tab_EngineSettings;
    QGridLayout *gridLayout_29;
    QGroupBox *groupBox_NumOfThreads;
    QGridLayout *gridLayout_8;
    QSpacerItem *horizontalSpacer_6;
    QFrame *frame_16;
    QGridLayout *gridLayout_57;
    QLabel *label_11;
    QFrame *line;
    QLabel *label_13;
    QFrame *line_2;
    QSpinBox *spinBox_ThreadNum_gif_internal;
    QSpinBox *spinBox_ThreadNum_video_internal;
    QLabel *label_12;
    QSpinBox *spinBox_ThreadNum_image;
    QSpacerItem *horizontalSpacer_29;
    QSpacerItem *horizontalSpacer_49;
    QLabel *label_NumThreadsActive;
    QSpacerItem *horizontalSpacer_50;
    QGroupBox *groupBox_Engine;
    QGridLayout *gridLayout_6;
    QFrame *line_6;
    QFrame *frame_18;
    QGridLayout *gridLayout_59;
    QFrame *frame_17;
    QGridLayout *gridLayout_58;
    QLabel *label_7;
    QFrame *line_12;
    QComboBox *comboBox_Engine_GIF;
    QSpacerItem *horizontalSpacer_30;
    QLabel *label_6;
    QLabel *label_8;
    QFrame *line_11;
    QComboBox *comboBox_Engine_Video;
    QComboBox *comboBox_Engine_Image;
    QSpacerItem *horizontalSpacer_8;
    QTabWidget *tabWidget_Engines;
    QWidget *tab_W2xNcnnVulkan;
    QGridLayout *gridLayout_53;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_10;
    QComboBox *comboBox_GPUID;
    QPushButton *pushButton_DetectGPU;
    QCheckBox *checkBox_MultiGPU_Waifu2xNCNNVulkan;
    QSpacerItem *horizontalSpacer_28;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_9;
    QFrame *frame_TileSize_W2xNcnnVulkan;
    QGridLayout *gridLayout_31;
    QPushButton *pushButton_TileSize_Minus_W2xNCNNVulkan;
    QSpinBox *spinBox_TileSize;
    QPushButton *pushButton_TileSize_Add_W2xNCNNVulkan;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_24;
    QGroupBox *groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan;
    QGridLayout *gridLayout_39;
    QComboBox *comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan;
    QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan;
    QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan;
    QPushButton *pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan;
    QLabel *label_64;
    QLabel *label_65;
    QFrame *line_21;
    QSpacerItem *horizontalSpacer_31;
    QFrame *line_31;
    QFrame *line_30;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_47;
    QComboBox *comboBox_version_Waifu2xNCNNVulkan;
    QSpacerItem *horizontalSpacer_18;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_26;
    QComboBox *comboBox_model_vulkan;
    QCheckBox *checkBox_TTA_vulkan;
    QSpacerItem *horizontalSpacer_22;
    QFrame *line_32;
    QWidget *tab_W2xConverter;
    QGridLayout *gridLayout_54;
    QFrame *line_34;
    QHBoxLayout *horizontalLayout_17;
    QCheckBox *checkBox_ForceOpenCL_converter;
    QCheckBox *checkBox_DisableGPU_converter;
    QCheckBox *checkBox_TTA_converter;
    QSpacerItem *horizontalSpacer_20;
    QFrame *line_33;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_53;
    QComboBox *comboBox_TargetProcessor_converter;
    QPushButton *pushButton_DumpProcessorList_converter;
    QCheckBox *checkBox_MultiGPU_Waifu2xConverter;
    QSpacerItem *horizontalSpacer_21;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_54;
    QFrame *frame_BlockSize_W2xConverter;
    QGridLayout *gridLayout_35;
    QPushButton *pushButton_BlockSize_Minus_W2xConverter;
    QSpinBox *spinBox_BlockSize_converter;
    QPushButton *pushButton_BlockSize_Add_W2xConverter;
    QSpacerItem *horizontalSpacer_19;
    QHBoxLayout *horizontalLayout_25;
    QGroupBox *groupBox_GPUSettings_MultiGPU_Waifu2xConverter;
    QGridLayout *gridLayout_44;
    QPushButton *pushButton_ShowMultiGPUSettings_Waifu2xConverter;
    QLabel *label_75;
    QLabel *label_74;
    QComboBox *comboBox_GPUIDs_MultiGPU_Waifu2xConverter;
    QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter;
    QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter;
    QFrame *line_22;
    QSpacerItem *horizontalSpacer_32;
    QWidget *tab_A4k;
    QGridLayout *gridLayout_62;
    QFrame *frame_21;
    QGridLayout *gridLayout_61;
    QCheckBox *checkBox_OpenCLParallelIO_A4k;
    QCheckBox *checkBox_ACNet_Anime4K;
    QCheckBox *checkBox_HDNMode_Anime4k;
    QCheckBox *checkBox_FastMode_Anime4K;
    QLabel *label_23;
    QSpinBox *spinBox_OpenCLCommandQueues_A4k;
    QFrame *line_26;
    QSpacerItem *horizontalSpacer_46;
    QFrame *line_5;
    QFrame *frame_20;
    QGridLayout *gridLayout_5;
    QLineEdit *lineEdit_GPUs_Anime4k;
    QPushButton *pushButton_ListGPUs_Anime4k;
    QComboBox *comboBox_GPGPUModel_A4k;
    QFrame *line_13;
    QCheckBox *checkBox_GPUMode_Anime4K;
    QPushButton *pushButton_VerifyGPUsConfig_Anime4k;
    QCheckBox *checkBox_SpecifyGPU_Anime4k;
    QSpacerItem *horizontalSpacer_45;
    QFrame *line_14;
    QWidget *widget;
    QGridLayout *gridLayout_38;
    QLabel *label_50;
    QDoubleSpinBox *doubleSpinBox_PushGradientStrength_Anime4K;
    QSpinBox *spinBox_PushColorCount_Anime4K;
    QDoubleSpinBox *doubleSpinBox_PushColorStrength_Anime4K;
    QLabel *label_48;
    QSpinBox *spinBox_Passes_Anime4K;
    QLabel *label_51;
    QLabel *label_49;
    QSpacerItem *horizontalSpacer_47;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_PreProcessing_Anime4k;
    QGridLayout *gridLayout_25;
    QFrame *line_8;
    QCheckBox *checkBox_MeanBlur_Pre_Anime4k;
    QCheckBox *checkBox_MedianBlur_Pre_Anime4k;
    QCheckBox *checkBox_GaussianBlurWeak_Pre_Anime4k;
    QCheckBox *checkBox_CASSharping_Pre_Anime4k;
    QCheckBox *checkBox_GaussianBlur_Pre_Anime4k;
    QCheckBox *checkBox_BilateralFilter_Pre_Anime4k;
    QCheckBox *checkBox_BilateralFilterFaster_Pre_Anime4k;
    QCheckBox *checkBox_EnablePreProcessing_Anime4k;
    QGroupBox *groupBox_PostProcessing_Anime4k;
    QGridLayout *gridLayout_24;
    QCheckBox *checkBox_EnablePostProcessing_Anime4k;
    QFrame *line_9;
    QCheckBox *checkBox_MedianBlur_Post_Anime4k;
    QCheckBox *checkBox_MeanBlur_Post_Anime4k;
    QCheckBox *checkBox_CASSharping_Post_Anime4k;
    QCheckBox *checkBox_GaussianBlur_Post_Anime4k;
    QCheckBox *checkBox_BilateralFilter_Post_Anime4k;
    QCheckBox *checkBox_GaussianBlurWeak_Post_Anime4k;
    QCheckBox *checkBox_BilateralFilterFaster_Post_Anime4k;
    QWidget *tab_SrmdNcnnVulkan;
    QGridLayout *gridLayout_55;
    QHBoxLayout *horizontalLayout_26;
    QGroupBox *groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan;
    QGridLayout *gridLayout_41;
    QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan;
    QLabel *label_68;
    QComboBox *comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan;
    QPushButton *pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan;
    QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan;
    QLabel *label_69;
    QFrame *line_23;
    QSpacerItem *horizontalSpacer_23;
    QHBoxLayout *horizontalLayout_19;
    QLabel *label_41;
    QComboBox *comboBox_GPUID_srmd;
    QPushButton *pushButton_DetectGPUID_srmd;
    QCheckBox *checkBox_MultiGPU_SrmdNCNNVulkan;
    QSpacerItem *horizontalSpacer_34;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_40;
    QFrame *frame_TileSize_SrmdNcnnVulkan;
    QGridLayout *gridLayout_36;
    QPushButton *pushButton_Add_TileSize_SrmdNCNNVulkan;
    QPushButton *pushButton_Minus_TileSize_SrmdNCNNVulkan;
    QSpinBox *spinBox_TileSize_srmd;
    QCheckBox *checkBox_TTA_srmd;
    QSpacerItem *horizontalSpacer_33;
    QFrame *line_37;
    QWidget *tab_W2xCaffe;
    QGridLayout *gridLayout_30;
    QHBoxLayout *horizontalLayout_29;
    QLabel *label_56;
    QComboBox *comboBox_Model_3D_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_38;
    QHBoxLayout *horizontalLayout_31;
    QLabel *label_43;
    QSpinBox *spinBox_BatchSize_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_36;
    QFrame *line_41;
    QHBoxLayout *horizontalLayout_27;
    QLabel *label_14;
    QComboBox *comboBox_ProcessMode_Waifu2xCaffe;
    QCheckBox *checkBox_TTA_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_40;
    QHBoxLayout *horizontalLayout_33;
    QCheckBox *checkBox_EnableMultiGPU_Waifu2xCaffe;
    QLineEdit *lineEdit_MultiGPUInfo_Waifu2xCaffe;
    QPushButton *pushButton_VerifyGPUsConfig_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_35;
    QFrame *line_40;
    QFrame *line_42;
    QFrame *line_39;
    QHBoxLayout *horizontalLayout_28;
    QLabel *label_25;
    QComboBox *comboBox_Model_2D_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_39;
    QHBoxLayout *horizontalLayout_30;
    QLabel *label_38;
    QSpinBox *spinBox_GPUID_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_37;
    QFrame *line_38;
    QHBoxLayout *horizontalLayout_32;
    QLabel *label_52;
    QPushButton *pushButton_SplitSize_Minus_Waifu2xCaffe;
    QSpinBox *spinBox_SplitSize_Waifu2xCaffe;
    QPushButton *pushButton_SplitSize_Add_Waifu2xCaffe;
    QSpacerItem *horizontalSpacer_24;
    QFrame *line_43;
    QWidget *tab_RealSRNcnnVulkan;
    QGridLayout *gridLayout_34;
    QFrame *line_36;
    QFrame *line_35;
    QHBoxLayout *horizontalLayout_23;
    QLabel *label_60;
    QComboBox *comboBox_Model_RealsrNCNNVulkan;
    QCheckBox *checkBox_TTA_RealsrNCNNVulkan;
    QSpacerItem *horizontalSpacer_41;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_62;
    QComboBox *comboBox_GPUID_RealsrNCNNVulkan;
    QPushButton *pushButton_DetectGPU_RealsrNCNNVulkan;
    QCheckBox *checkBox_MultiGPU_RealSRNcnnVulkan;
    QSpacerItem *horizontalSpacer_42;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label_61;
    QFrame *frame_TileSize_RealsrNcnnVulkan;
    QGridLayout *gridLayout_56;
    QPushButton *pushButton_Minus_TileSize_RealsrNCNNVulkan;
    QSpinBox *spinBox_TileSize_RealsrNCNNVulkan;
    QPushButton *pushButton_Add_TileSize_RealsrNCNNVulkan;
    QSpacerItem *horizontalSpacer_26;
    QHBoxLayout *horizontalLayout_34;
    QGroupBox *groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan;
    QGridLayout *gridLayout_43;
    QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan;
    QLabel *label_73;
    QLabel *label_72;
    QPushButton *pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan;
    QComboBox *comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan;
    QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan;
    QFrame *line_24;
    QSpacerItem *horizontalSpacer_25;
    QWidget *tab_RealESRGAN;
    QGridLayout *gridLayout_RealESRGAN;
    QHBoxLayout *horizontalLayout_RealESRGAN_Model;
    QLabel *label_Model_RealESRGAN;
    QComboBox *comboBox_Model_RealESRGAN;
    QCheckBox *checkBox_TTA_RealESRGAN;
    QSpacerItem *horizontalSpacer_RealESRGAN_Model;
    QHBoxLayout *horizontalLayout_RealESRGAN_TileSize;
    QLabel *label_TileSize_RealESRGAN;
    QFrame *frame_TileSize_RealESRGAN;
    QGridLayout *gridLayout_TileSize_RealESRGAN;
    QPushButton *pushButton_Minus_TileSize_RealESRGAN;
    QSpinBox *spinBox_TileSize_RealESRGAN;
    QPushButton *pushButton_Add_TileSize_RealESRGAN;
    QSpacerItem *horizontalSpacer_TileSize_RealESRGAN;
    QHBoxLayout *horizontalLayout_GPU_RealESRGAN;
    QLabel *label_GPUID_RealESRGAN;
    QComboBox *comboBox_GPUID_RealESRGAN;
    QPushButton *pushButton_DetectGPU_RealESRGAN;
    QCheckBox *checkBox_MultiGPU_RealESRGANEngine;
    QSpacerItem *horizontalSpacer_GPU_RealESRGAN;
    QHBoxLayout *horizontalLayout_MultiGPUConfig_RealESRGAN;
    QGroupBox *groupBox_GPUSettings_MultiGPU_RealESRGAN;
    QGridLayout *gridLayout_MultiGPUConfigBox_RealESRGAN;
    QCheckBox *checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN;
    QLabel *label_MultiGPUID_RealESRGAN;
    QLabel *label_MultiGPUTile_RealESRGAN;
    QPushButton *pushButton_ShowMultiGPUSettings_RealESRGAN;
    QComboBox *comboBox_GPUIDs_MultiGPU_RealESRGAN;
    QSpinBox *spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN;
    QFrame *line_MultiGPUSeparator_RealESRGAN;
    QSpacerItem *horizontalSpacer_MultiGPUConfig_RealESRGAN;
    QFrame *line_RealESRGAN_Sep1;
    QFrame *line_RealESRGAN_Sep2;
    QWidget *tab_RealCUGAN;
    QGridLayout *gridLayout_RealCUGAN;
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
    QWidget *tab_VideoSettings;
    QVBoxLayout *verticalLayout_VideoSettingsTab;
    QHBoxLayout *horizontalLayout_top_video;
    QGroupBox *groupBox_AudioDenoise;
    QGridLayout *gridLayout_16;
    QDoubleSpinBox *doubleSpinBox_AudioDenoiseLevel;
    QLabel *label_46;
    QCheckBox *checkBox_AudioDenoise;
    QFrame *line_7;
    QGroupBox *groupBox_ProcessVideoBySegment;
    QGridLayout *gridLayout_69;
    QLabel *label_SegmentDuration;
    QSpinBox *spinBox_VideoSplitDuration;
    QCheckBox *checkBox_ProcessVideoBySegment;
    QFrame *line_46;
    QSpacerItem *horizontalSpacer_top_video_stretch;
    QGroupBox *groupBox_FrameInterpolation;
    QGridLayout *gridLayout_68;
    QFrame *frame_15;
    QGridLayout *gridLayout_67;
    QCheckBox *checkBox_UHD_VFI;
    QSpacerItem *horizontalSpacer_12;
    QFrame *line_28;
    QFrame *frame_14;
    QGridLayout *gridLayout_65;
    QLabel *label_37;
    QComboBox *comboBox_Engine_VFI;
    QCheckBox *checkBox_TTA_VFI;
    QCheckBox *checkBox_VfiAfterScale_VFI;
    QFrame *line_27;
    QFrame *line_29;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_30;
    QComboBox *comboBox_GPUID_VFI;
    QPushButton *pushButton_DetectGPU_VFI;
    QHBoxLayout *horizontalLayout_6;
    QCheckBox *checkBox_MultiGPU_VFI;
    QLineEdit *lineEdit_MultiGPU_IDs_VFI;
    QPushButton *pushButton_Verify_MultiGPU_VFI;
    QHBoxLayout *horizontalLayout_49;
    QLabel *label_19;
    QSpinBox *spinBox_TileSize_VFI;
    QSpacerItem *horizontalSpacer_48;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_3;
    QComboBox *comboBox_Model_VFI;
    QHBoxLayout *horizontalLayout_9;
    QCheckBox *checkBox_MultiThread_VFI;
    QSpinBox *spinBox_NumOfThreads_VFI;
    QCheckBox *checkBox_AutoAdjustNumOfThreads_VFI;
    QGroupBox *groupBox_video_settings;
    QHBoxLayout *horizontalLayout_video_settings_group;
    QGroupBox *groupBox_OutputVideoSettings;
    QFormLayout *formLayout_OutputVideoSettings;
    QLabel *label_31;
    QLineEdit *lineEdit_encoder_vid;
    QLabel *label_27;
    QSpinBox *spinBox_bitrate_vid;
    QLabel *label_32;
    QLineEdit *lineEdit_encoder_audio;
    QLabel *label_29;
    QSpinBox *spinBox_bitrate_audio;
    QLabel *label_33;
    QLineEdit *lineEdit_pixformat;
    QFrame *line_17;
    QLabel *label_42;
    QLineEdit *lineEdit_ExCommand_output;
    QGroupBox *groupBox_ToMp4VideoSettings;
    QFormLayout *formLayout_ToMp4VideoSettings;
    QLabel *label_34;
    QSpinBox *spinBox_bitrate_vid_2mp4;
    QLabel *label_35;
    QSpinBox *spinBox_bitrate_audio_2mp4;
    QHBoxLayout *horizontalLayout_39_form;
    QCheckBox *checkBox_acodec_copy_2mp4;
    QCheckBox *checkBox_vcodec_copy_2mp4;
    QCheckBox *checkBox_IgnoreFrameRateMode;
    QFrame *line_18;
    QLabel *label_45;
    QLineEdit *lineEdit_ExCommand_2mp4;
    QHBoxLayout *horizontalLayout_bottom_buttons_video;
    QSpacerItem *horizontalSpacer_bottom_video_spacer;
    QPushButton *pushButton_ResetVideoSettings;
    QPushButton *pushButton_encodersList;
    QSpacerItem *verticalSpacer_VideoSettingsTab;
    QWidget *tab_AdditionalSettings;
    QHBoxLayout *horizontalLayout_AdditionalSettingsTab;
    QVBoxLayout *verticalLayout_col1_additional;
    QGroupBox *groupBox_InputExt;
    QGridLayout *gridLayout_10;
    QHBoxLayout *horizontalLayout_47;
    QLabel *label_4;
    QLineEdit *Ext_image;
    QHBoxLayout *horizontalLayout_48;
    QLabel *label_5;
    QLineEdit *Ext_video;
    QHBoxLayout *horizontalLayout_35;
    QCheckBox *checkBox_PreProcessImage;
    QCheckBox *checkBox_AutoDetectAlphaChannel;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *checkBox_AlwaysPreProcessAlphaPNG;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_17;
    QHBoxLayout *horizontalLayout_37;
    QLabel *label_24;
    QFontComboBox *fontComboBox_CustFont;
    QHBoxLayout *horizontalLayout_38;
    QPushButton *pushButton_Save_GlobalFontSize;
    QCheckBox *checkBox_isCustFontEnable;
    QHBoxLayout *horizontalLayout_36;
    QLabel *label_22;
    QSpinBox *spinBox_GlobalFontSize;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_other_1;
    QGridLayout *gridLayout_7;
    QHBoxLayout *horizontalLayout_57;
    QPushButton *pushButton_SaveSettings;
    QPushButton *pushButton_ResetSettings;
    QPushButton *pushButton_CheckUpdate;
    QHBoxLayout *horizontalLayout_58;
    QPushButton *pushButton_Report;
    QPushButton *pushButton_ReadMe;
    QPushButton *pushButton_wiki;
    QHBoxLayout *horizontalLayout_59;
    QPushButton *pushButton_about;
    QPushButton *pushButton_SupportersList;
    QVBoxLayout *verticalLayout_col2_additional;
    QCheckBox *checkBox_UpdatePopup;
    QCheckBox *checkBox_MinimizeToTaskbar;
    QCheckBox *checkBox_AutoSaveSettings;
    QCheckBox *checkBox_AlwaysHideTextBrowser;
    QCheckBox *checkBox_ScanSubFolders;
    QCheckBox *checkBox_PromptWhenExit;
    QCheckBox *checkBox_DisableResize_gif;
    QCheckBox *checkBox_BanGitee;
    QSpacerItem *verticalSpacer_col2_additional;
    QVBoxLayout *verticalLayout_col3_additional;
    QCheckBox *checkBox_FileListAutoSlide;
    QCheckBox *checkBox_ShowInterPro;
    QCheckBox *checkBox_AlwaysHideSettings;
    QCheckBox *checkBox_FileList_Interactive;
    QCheckBox *checkBox_KeepVideoCache;
    QCheckBox *checkBox_SummaryPopup;
    QCheckBox *checkBox_NfSound;
    QSpacerItem *verticalSpacer_col3_additional;
    QWidget *tab_CompatibilityTest;
    QVBoxLayout *verticalLayout_CompatTestTab;
    QGroupBox *groupBox_CompatibilityTestRes;
    QGridLayout *gridLayout_CompatTestResInternal;
    QLabel *label_58;
    QCheckBox *checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW;
    QCheckBox *checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P;
    QCheckBox *checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD;
    QCheckBox *checkBox_isCompatible_SRMD_NCNN_Vulkan;
    QCheckBox *checkBox_isCompatible_Waifu2x_Converter;
    QCheckBox *checkBox_isCompatible_Anime4k_CPU;
    QCheckBox *checkBox_isCompatible_Anime4k_GPU;
    QCheckBox *checkBox_isCompatible_Waifu2x_Caffe_CPU;
    QCheckBox *checkBox_isCompatible_Waifu2x_Caffe_GPU;
    QCheckBox *checkBox_isCompatible_Waifu2x_Caffe_cuDNN;
    QCheckBox *checkBox_isCompatible_Realsr_NCNN_Vulkan;
    QCheckBox *checkBox_isCompatible_SRMD_CUDA;
    QFrame *line_16;
    QLabel *label_59;
    QCheckBox *checkBox_isCompatible_SoX;
    QCheckBox *checkBox_isCompatible_ImageMagick;
    QCheckBox *checkBox_isCompatible_Gifsicle;
    QCheckBox *checkBox_isCompatible_FFprobe;
    QCheckBox *checkBox_isCompatible_FFmpeg;
    QFrame *line_19;
    QLabel *label_36;
    QCheckBox *checkBox_isCompatible_RifeNcnnVulkan;
    QCheckBox *checkBox_isCompatible_CainNcnnVulkan;
    QCheckBox *checkBox_isCompatible_DainNcnnVulkan;
    QLabel *label_2;
    QProgressBar *progressBar_CompatibilityTest;
    QPushButton *pushButton_compatibilityTest;
    QSplitter *splitter_TextBrowser;
    QTextBrowser *textBrowser;
    QGroupBox *groupBox_textBrowserSettings;
    QGridLayout *gridLayout_9;
    QPushButton *pushButton_clear_textbrowser;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_TextBroFontSize;
    QSpinBox *spinBox_textbrowser_fontsize;
    QCheckBox *checkBox_OutPath_KeepOriginalFileName;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1096, 794);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        MainWindow->setMaximumSize(QSize(16777215, 16777215));
        MainWindow->setFocusPolicy(Qt::StrongFocus);
        MainWindow->setWindowTitle(QString::fromUtf8("Waifu2x-Extension-GUI by Aaron Feng"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/icon/icon_main.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        gridLayout_40 = new QGridLayout(centralwidget);
        gridLayout_40->setObjectName("gridLayout_40");
        gridLayout_40->setContentsMargins(6, 6, 6, 6);
        splitter_4 = new QSplitter(centralwidget);
        splitter_4->setObjectName("splitter_4");
        splitter_4->setOrientation(Qt::Vertical);
        splitter_4->setHandleWidth(5);
        tabWidget = new QTabWidget(splitter_4);
        tabWidget->setObjectName("tabWidget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        tabWidget->setMinimumSize(QSize(0, 0));
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setIconSize(QSize(23, 23));
        tabWidget->setElideMode(Qt::ElideNone);
        tabWidget->setUsesScrollButtons(false);
        tabWidget->setDocumentMode(false);
        tabWidget->setTabsClosable(false);
        tabWidget->setMovable(false);
        tab_Donate = new QWidget();
        tab_Donate->setObjectName("tab_Donate");
        tab_Donate->setEnabled(true);
        gridLayout_22 = new QGridLayout(tab_Donate);
        gridLayout_22->setObjectName("gridLayout_22");
        label_DonateText = new QLabel(tab_Donate);
        label_DonateText->setObjectName("label_DonateText");
        label_DonateText->setMaximumSize(QSize(16777215, 50));
        QFont font;
        font.setPointSize(10);
        font.setBold(false);
        font.setUnderline(false);
        font.setStrikeOut(false);
        label_DonateText->setFont(font);
        label_DonateText->setAlignment(Qt::AlignCenter);

        gridLayout_22->addWidget(label_DonateText, 0, 0, 1, 1);

        horizontalLayout_60 = new QHBoxLayout();
        horizontalLayout_60->setObjectName("horizontalLayout_60");
        pushButton_PayPal = new QPushButton(tab_Donate);
        pushButton_PayPal->setObjectName("pushButton_PayPal");
        sizePolicy.setHeightForWidth(pushButton_PayPal->sizePolicy().hasHeightForWidth());
        pushButton_PayPal->setSizePolicy(sizePolicy);
        pushButton_PayPal->setMinimumSize(QSize(240, 60));
        QFont font1;
        font1.setPointSize(20);
        font1.setBold(true);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setStrikeOut(false);
        pushButton_PayPal->setFont(font1);
        pushButton_PayPal->setText(QString::fromUtf8(" PayPal"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/new/prefix1/icon/paypal.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_PayPal->setIcon(icon1);
        pushButton_PayPal->setIconSize(QSize(38, 38));

        horizontalLayout_60->addWidget(pushButton_PayPal);

        pushButton_Patreon = new QPushButton(tab_Donate);
        pushButton_Patreon->setObjectName("pushButton_Patreon");
        pushButton_Patreon->setMinimumSize(QSize(0, 60));
        QFont font2;
        font2.setPointSize(20);
        font2.setBold(true);
        pushButton_Patreon->setFont(font2);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/new/prefix1/icon/patreon.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_Patreon->setIcon(icon2);
        pushButton_Patreon->setIconSize(QSize(38, 38));

        horizontalLayout_60->addWidget(pushButton_Patreon);

        horizontalLayout_60->setStretch(0, 10);
        horizontalLayout_60->setStretch(1, 33);

        gridLayout_22->addLayout(horizontalLayout_60, 1, 0, 1, 1);

        label_DonateQRCode = new QLabel(tab_Donate);
        label_DonateQRCode->setObjectName("label_DonateQRCode");
        sizePolicy1.setHeightForWidth(label_DonateQRCode->sizePolicy().hasHeightForWidth());
        label_DonateQRCode->setSizePolicy(sizePolicy1);
        label_DonateQRCode->setMinimumSize(QSize(750, 360));
        label_DonateQRCode->setSizeIncrement(QSize(2, 1));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("Arial")});
        font3.setPointSize(13);
        font3.setBold(true);
        label_DonateQRCode->setFont(font3);
        label_DonateQRCode->setFrameShape(QFrame::NoFrame);
        label_DonateQRCode->setScaledContents(true);
        label_DonateQRCode->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout_22->addWidget(label_DonateQRCode, 2, 0, 1, 1);

        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/new/prefix1/icon/donateTabIcon_1.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_Donate, icon3, QString());
        tab_Home = new QWidget();
        tab_Home->setObjectName("tab_Home");
        QFont font4;
        font4.setPointSize(9);
        tab_Home->setFont(font4);
        homeTabLayout = new QHBoxLayout(tab_Home);
        homeTabLayout->setObjectName("homeTabLayout");
        homeTabLayout->setContentsMargins(6, 6, 6, 6);
        homeMainSplitter = new QSplitter(tab_Home);
        homeMainSplitter->setObjectName("homeMainSplitter");
        homeMainSplitter->setOrientation(Qt::Horizontal);
        homeMainSplitter->setHandleWidth(5);
        leftPaneWidget = new QWidget(homeMainSplitter);
        leftPaneWidget->setObjectName("leftPaneWidget");
        leftPaneLayout = new QVBoxLayout(leftPaneWidget);
        leftPaneLayout->setObjectName("leftPaneLayout");
        leftPaneLayout->setContentsMargins(0, 0, 0, 0);
        groupBox_FileList = new QGroupBox(leftPaneWidget);
        groupBox_FileList->setObjectName("groupBox_FileList");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(1);
        sizePolicy2.setHeightForWidth(groupBox_FileList->sizePolicy().hasHeightForWidth());
        groupBox_FileList->setSizePolicy(sizePolicy2);
        verticalLayout_FileList = new QVBoxLayout(groupBox_FileList);
        verticalLayout_FileList->setObjectName("verticalLayout_FileList");
        fileListTabWidget = new QTabWidget(groupBox_FileList);
        fileListTabWidget->setObjectName("fileListTabWidget");
        tab_Images = new QWidget();
        tab_Images->setObjectName("tab_Images");
        verticalLayout_ImagesTab = new QVBoxLayout(tab_Images);
        verticalLayout_ImagesTab->setObjectName("verticalLayout_ImagesTab");
        verticalLayout_ImagesTab->setContentsMargins(0, 0, 0, 0);
        tableView_image = new QTableView(tab_Images);
        tableView_image->setObjectName("tableView_image");
        tableView_image->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableView_image->setAutoScroll(false);
        tableView_image->setAutoScrollMargin(0);
        tableView_image->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView_image->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView_image->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView_image->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableView_image->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableView_image->setSortingEnabled(false);
        tableView_image->setCornerButtonEnabled(false);
        tableView_image->setShowGrid(false);
        tableView_image->horizontalHeader()->setVisible(true);
        tableView_image->horizontalHeader()->setCascadingSectionResizes(false);
        tableView_image->horizontalHeader()->setMinimumSectionSize(20);
        tableView_image->horizontalHeader()->setDefaultSectionSize(200);
        tableView_image->horizontalHeader()->setStretchLastSection(true);
        tableView_image->verticalHeader()->setVisible(false);
        tableView_image->verticalHeader()->setCascadingSectionResizes(false);
        tableView_image->verticalHeader()->setDefaultSectionSize(24);
        tableView_image->verticalHeader()->setStretchLastSection(false);

        verticalLayout_ImagesTab->addWidget(tableView_image);

        fileListTabWidget->addTab(tab_Images, QString());
        tab_Gifs = new QWidget();
        tab_Gifs->setObjectName("tab_Gifs");
        verticalLayout_GifsTab = new QVBoxLayout(tab_Gifs);
        verticalLayout_GifsTab->setObjectName("verticalLayout_GifsTab");
        verticalLayout_GifsTab->setContentsMargins(0, 0, 0, 0);
        tableView_gif = new QTableView(tab_Gifs);
        tableView_gif->setObjectName("tableView_gif");
        tableView_gif->setEnabled(true);
        tableView_gif->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableView_gif->setAutoScroll(false);
        tableView_gif->setAutoScrollMargin(0);
        tableView_gif->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView_gif->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView_gif->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView_gif->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableView_gif->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableView_gif->setSortingEnabled(false);
        tableView_gif->setCornerButtonEnabled(false);
        tableView_gif->setShowGrid(false);
        tableView_gif->horizontalHeader()->setVisible(true);
        tableView_gif->horizontalHeader()->setCascadingSectionResizes(false);
        tableView_gif->horizontalHeader()->setMinimumSectionSize(20);
        tableView_gif->horizontalHeader()->setDefaultSectionSize(200);
        tableView_gif->horizontalHeader()->setStretchLastSection(true);
        tableView_gif->verticalHeader()->setVisible(false);
        tableView_gif->verticalHeader()->setCascadingSectionResizes(false);
        tableView_gif->verticalHeader()->setDefaultSectionSize(24);

        verticalLayout_GifsTab->addWidget(tableView_gif);

        fileListTabWidget->addTab(tab_Gifs, QString());
        tab_Videos = new QWidget();
        tab_Videos->setObjectName("tab_Videos");
        verticalLayout_VideosTab = new QVBoxLayout(tab_Videos);
        verticalLayout_VideosTab->setObjectName("verticalLayout_VideosTab");
        verticalLayout_VideosTab->setContentsMargins(0, 0, 0, 0);
        tableView_video = new QTableView(tab_Videos);
        tableView_video->setObjectName("tableView_video");
        tableView_video->setContextMenuPolicy(Qt::ActionsContextMenu);
        tableView_video->setAutoScroll(false);
        tableView_video->setAutoScrollMargin(0);
        tableView_video->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView_video->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView_video->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView_video->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableView_video->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        tableView_video->setSortingEnabled(false);
        tableView_video->setCornerButtonEnabled(false);
        tableView_video->setShowGrid(false);
        tableView_video->horizontalHeader()->setVisible(true);
        tableView_video->horizontalHeader()->setMinimumSectionSize(20);
        tableView_video->horizontalHeader()->setDefaultSectionSize(200);
        tableView_video->horizontalHeader()->setStretchLastSection(true);
        tableView_video->verticalHeader()->setVisible(false);
        tableView_video->verticalHeader()->setDefaultSectionSize(24);

        verticalLayout_VideosTab->addWidget(tableView_video);

        fileListTabWidget->addTab(tab_Videos, QString());

        verticalLayout_FileList->addWidget(fileListTabWidget);

        horizontalLayout_FileListControls = new QHBoxLayout();
        horizontalLayout_FileListControls->setObjectName("horizontalLayout_FileListControls");
        label_FileCount = new QLabel(groupBox_FileList);
        label_FileCount->setObjectName("label_FileCount");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_FileCount->sizePolicy().hasHeightForWidth());
        label_FileCount->setSizePolicy(sizePolicy3);
        label_FileCount->setFrameShape(QFrame::Box);
        label_FileCount->setText(QString::fromUtf8("File count: 0"));
        label_FileCount->setAlignment(Qt::AlignCenter);
        label_FileCount->setMargin(5);

        horizontalLayout_FileListControls->addWidget(label_FileCount);

        horizontalSpacer_file_controls = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_FileListControls->addItem(horizontalSpacer_file_controls);

        pushButton_ClearList = new QPushButton(groupBox_FileList);
        pushButton_ClearList->setObjectName("pushButton_ClearList");
        pushButton_ClearList->setMinimumSize(QSize(35, 35));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/new/prefix1/icon/ClearList.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_ClearList->setIcon(icon4);

        horizontalLayout_FileListControls->addWidget(pushButton_ClearList);

        pushButton_RemoveItem = new QPushButton(groupBox_FileList);
        pushButton_RemoveItem->setObjectName("pushButton_RemoveItem");
        pushButton_RemoveItem->setMinimumSize(QSize(35, 35));
        pushButton_RemoveItem->setContextMenuPolicy(Qt::ActionsContextMenu);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/new/prefix1/icon/RemoveItem.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_RemoveItem->setIcon(icon5);

        horizontalLayout_FileListControls->addWidget(pushButton_RemoveItem);

        pushButton_SaveFileList = new QPushButton(groupBox_FileList);
        pushButton_SaveFileList->setObjectName("pushButton_SaveFileList");
        pushButton_SaveFileList->setMinimumSize(QSize(35, 35));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/new/prefix1/icon/Save_FileList.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_SaveFileList->setIcon(icon6);

        horizontalLayout_FileListControls->addWidget(pushButton_SaveFileList);

        pushButton_ReadFileList = new QPushButton(groupBox_FileList);
        pushButton_ReadFileList->setObjectName("pushButton_ReadFileList");
        pushButton_ReadFileList->setMinimumSize(QSize(35, 35));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/new/prefix1/icon/Read_FileList.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_ReadFileList->setIcon(icon7);

        horizontalLayout_FileListControls->addWidget(pushButton_ReadFileList);

        pushButton_BrowserFile = new QPushButton(groupBox_FileList);
        pushButton_BrowserFile->setObjectName("pushButton_BrowserFile");
        pushButton_BrowserFile->setMinimumSize(QSize(35, 35));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/new/prefix1/icon/opne_folder.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_BrowserFile->setIcon(icon8);

        horizontalLayout_FileListControls->addWidget(pushButton_BrowserFile);


        verticalLayout_FileList->addLayout(horizontalLayout_FileListControls);


        leftPaneLayout->addWidget(groupBox_FileList);

        groupBox_CurrentFile = new QGroupBox(leftPaneWidget);
        groupBox_CurrentFile->setObjectName("groupBox_CurrentFile");
        sizePolicy.setHeightForWidth(groupBox_CurrentFile->sizePolicy().hasHeightForWidth());
        groupBox_CurrentFile->setSizePolicy(sizePolicy);
        verticalLayout_CurrentFile = new QVBoxLayout(groupBox_CurrentFile);
        verticalLayout_CurrentFile->setObjectName("verticalLayout_CurrentFile");
        progressBar_CurrentFile = new QProgressBar(groupBox_CurrentFile);
        progressBar_CurrentFile->setObjectName("progressBar_CurrentFile");
        progressBar_CurrentFile->setValue(0);

        verticalLayout_CurrentFile->addWidget(progressBar_CurrentFile);

        horizontalLayout_CurrentFileLabels = new QHBoxLayout();
        horizontalLayout_CurrentFileLabels->setObjectName("horizontalLayout_CurrentFileLabels");
        label_FrameProgress_CurrentFile = new QLabel(groupBox_CurrentFile);
        label_FrameProgress_CurrentFile->setObjectName("label_FrameProgress_CurrentFile");
        label_FrameProgress_CurrentFile->setFrameShape(QFrame::Box);
        label_FrameProgress_CurrentFile->setText(QString::fromUtf8("0/0"));

        horizontalLayout_CurrentFileLabels->addWidget(label_FrameProgress_CurrentFile);

        label_TimeCost_CurrentFile = new QLabel(groupBox_CurrentFile);
        label_TimeCost_CurrentFile->setObjectName("label_TimeCost_CurrentFile");
        label_TimeCost_CurrentFile->setFrameShape(QFrame::Box);

        horizontalLayout_CurrentFileLabels->addWidget(label_TimeCost_CurrentFile);

        label_TimeRemain_CurrentFile = new QLabel(groupBox_CurrentFile);
        label_TimeRemain_CurrentFile->setObjectName("label_TimeRemain_CurrentFile");
        label_TimeRemain_CurrentFile->setFrameShape(QFrame::Box);

        horizontalLayout_CurrentFileLabels->addWidget(label_TimeRemain_CurrentFile);

        label_ETA_CurrentFile = new QLabel(groupBox_CurrentFile);
        label_ETA_CurrentFile->setObjectName("label_ETA_CurrentFile");
        label_ETA_CurrentFile->setFrameShape(QFrame::Box);

        horizontalLayout_CurrentFileLabels->addWidget(label_ETA_CurrentFile);


        verticalLayout_CurrentFile->addLayout(horizontalLayout_CurrentFileLabels);


        leftPaneLayout->addWidget(groupBox_CurrentFile);

        groupBox_Progress = new QGroupBox(leftPaneWidget);
        groupBox_Progress->setObjectName("groupBox_Progress");
        horizontalLayout_Progress = new QHBoxLayout(groupBox_Progress);
        horizontalLayout_Progress->setObjectName("horizontalLayout_Progress");
        label_progressBar_filenum = new QLabel(groupBox_Progress);
        label_progressBar_filenum->setObjectName("label_progressBar_filenum");
        label_progressBar_filenum->setFrameShape(QFrame::Box);
        label_progressBar_filenum->setText(QString::fromUtf8("0/0"));

        horizontalLayout_Progress->addWidget(label_progressBar_filenum);

        progressBar = new QProgressBar(groupBox_Progress);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(0);
        progressBar->setFormat(QString::fromUtf8("%p%"));

        horizontalLayout_Progress->addWidget(progressBar);

        label_TimeCost = new QLabel(groupBox_Progress);
        label_TimeCost->setObjectName("label_TimeCost");
        label_TimeCost->setFrameShape(QFrame::Box);

        horizontalLayout_Progress->addWidget(label_TimeCost);

        label_TimeRemain = new QLabel(groupBox_Progress);
        label_TimeRemain->setObjectName("label_TimeRemain");
        label_TimeRemain->setFrameShape(QFrame::Box);

        horizontalLayout_Progress->addWidget(label_TimeRemain);

        label_ETA = new QLabel(groupBox_Progress);
        label_ETA->setObjectName("label_ETA");
        label_ETA->setFrameShape(QFrame::Box);

        horizontalLayout_Progress->addWidget(label_ETA);

        horizontalSpacer_ProgressControls = new QSpacerItem(10, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_Progress->addItem(horizontalSpacer_ProgressControls);

        pushButton_Start = new QPushButton(groupBox_Progress);
        pushButton_Start->setObjectName("pushButton_Start");
        pushButton_Start->setIcon(icon);

        horizontalLayout_Progress->addWidget(pushButton_Start);

        pushButton_Stop = new QPushButton(groupBox_Progress);
        pushButton_Stop->setObjectName("pushButton_Stop");
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/new/prefix1/icon/pause-button.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_Stop->setIcon(icon9);

        horizontalLayout_Progress->addWidget(pushButton_Stop);

        pushButton_ForceRetry = new QPushButton(groupBox_Progress);
        pushButton_ForceRetry->setObjectName("pushButton_ForceRetry");
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/new/prefix1/icon/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_ForceRetry->setIcon(icon10);

        horizontalLayout_Progress->addWidget(pushButton_ForceRetry);


        leftPaneLayout->addWidget(groupBox_Progress);

        homeMainSplitter->addWidget(leftPaneWidget);
        groupBox_Setting = new QGroupBox(homeMainSplitter);
        groupBox_Setting->setObjectName("groupBox_Setting");
        sizePolicy1.setHeightForWidth(groupBox_Setting->sizePolicy().hasHeightForWidth());
        groupBox_Setting->setSizePolicy(sizePolicy1);
        groupBox_Setting->setMinimumSize(QSize(0, 0));
        groupBox_Setting->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout_RightPane = new QVBoxLayout(groupBox_Setting);
        verticalLayout_RightPane->setObjectName("verticalLayout_RightPane");
        horizontalLayout_HideButtons = new QHBoxLayout();
        horizontalLayout_HideButtons->setObjectName("horizontalLayout_HideButtons");
        pushButton_HideSettings = new QPushButton(groupBox_Setting);
        pushButton_HideSettings->setObjectName("pushButton_HideSettings");
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/new/prefix1/icon/AddSetting.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_HideSettings->setIcon(icon11);

        horizontalLayout_HideButtons->addWidget(pushButton_HideSettings);

        pushButton_HideTextBro = new QPushButton(groupBox_Setting);
        pushButton_HideTextBro->setObjectName("pushButton_HideTextBro");
        pushButton_HideTextBro->setIcon(icon11);

        horizontalLayout_HideButtons->addWidget(pushButton_HideTextBro);

        horizontalSpacer_HideButtons = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_HideButtons->addItem(horizontalSpacer_HideButtons);


        verticalLayout_RightPane->addLayout(horizontalLayout_HideButtons);

        groupBox_CustRes = new QGroupBox(groupBox_Setting);
        groupBox_CustRes->setObjectName("groupBox_CustRes");
        gridLayout_CustRes_New = new QGridLayout(groupBox_CustRes);
        gridLayout_CustRes_New->setObjectName("gridLayout_CustRes_New");
        label_18 = new QLabel(groupBox_CustRes);
        label_18->setObjectName("label_18");

        gridLayout_CustRes_New->addWidget(label_18, 0, 0, 1, 1);

        spinBox_CustRes_width = new QSpinBox(groupBox_CustRes);
        spinBox_CustRes_width->setObjectName("spinBox_CustRes_width");
        spinBox_CustRes_width->setMaximum(999999999);
        spinBox_CustRes_width->setValue(1920);

        gridLayout_CustRes_New->addWidget(spinBox_CustRes_width, 0, 1, 1, 1);

        label_15 = new QLabel(groupBox_CustRes);
        label_15->setObjectName("label_15");

        gridLayout_CustRes_New->addWidget(label_15, 1, 0, 1, 1);

        spinBox_CustRes_height = new QSpinBox(groupBox_CustRes);
        spinBox_CustRes_height->setObjectName("spinBox_CustRes_height");
        spinBox_CustRes_height->setMaximum(999999999);
        spinBox_CustRes_height->setValue(1080);

        gridLayout_CustRes_New->addWidget(spinBox_CustRes_height, 1, 1, 1, 1);

        label_44 = new QLabel(groupBox_CustRes);
        label_44->setObjectName("label_44");

        gridLayout_CustRes_New->addWidget(label_44, 2, 0, 1, 2);

        comboBox_AspectRatio_custRes = new QComboBox(groupBox_CustRes);
        comboBox_AspectRatio_custRes->addItem(QString());
        comboBox_AspectRatio_custRes->addItem(QString());
        comboBox_AspectRatio_custRes->addItem(QString());
        comboBox_AspectRatio_custRes->setObjectName("comboBox_AspectRatio_custRes");

        gridLayout_CustRes_New->addWidget(comboBox_AspectRatio_custRes, 3, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton_CustRes_apply = new QPushButton(groupBox_CustRes);
        pushButton_CustRes_apply->setObjectName("pushButton_CustRes_apply");
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/new/prefix1/icon/Apply.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_CustRes_apply->setIcon(icon12);

        horizontalLayout->addWidget(pushButton_CustRes_apply);

        pushButton_CustRes_cancel = new QPushButton(groupBox_CustRes);
        pushButton_CustRes_cancel->setObjectName("pushButton_CustRes_cancel");
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/new/prefix1/icon/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_CustRes_cancel->setIcon(icon13);

        horizontalLayout->addWidget(pushButton_CustRes_cancel);


        gridLayout_CustRes_New->addLayout(horizontalLayout, 4, 0, 1, 2);

        horizontalLayout_CustRes_Checkboxes = new QHBoxLayout();
        horizontalLayout_CustRes_Checkboxes->setObjectName("horizontalLayout_CustRes_Checkboxes");
        checkBox_custres_isAll = new QCheckBox(groupBox_CustRes);
        checkBox_custres_isAll->setObjectName("checkBox_custres_isAll");

        horizontalLayout_CustRes_Checkboxes->addWidget(checkBox_custres_isAll);

        checkBox_AutoSkip_CustomRes = new QCheckBox(groupBox_CustRes);
        checkBox_AutoSkip_CustomRes->setObjectName("checkBox_AutoSkip_CustomRes");

        horizontalLayout_CustRes_Checkboxes->addWidget(checkBox_AutoSkip_CustomRes);


        gridLayout_CustRes_New->addLayout(horizontalLayout_CustRes_Checkboxes, 5, 0, 1, 2);


        verticalLayout_RightPane->addWidget(groupBox_CustRes);

        groupBox_ImageOutputSettings = new QGroupBox(groupBox_Setting);
        groupBox_ImageOutputSettings->setObjectName("groupBox_ImageOutputSettings");
        gridLayout_26 = new QGridLayout(groupBox_ImageOutputSettings);
        gridLayout_26->setObjectName("gridLayout_26");
        frame_ImageQuality = new QFrame(groupBox_ImageOutputSettings);
        frame_ImageQuality->setObjectName("frame_ImageQuality");
        gridLayout_45 = new QGridLayout(frame_ImageQuality);
        gridLayout_45->setObjectName("gridLayout_45");
        label_63 = new QLabel(frame_ImageQuality);
        label_63->setObjectName("label_63");

        gridLayout_45->addWidget(label_63, 1, 0, 1, 1);

        spinBox_ImageQualityLevel = new QSpinBox(frame_ImageQuality);
        spinBox_ImageQualityLevel->setObjectName("spinBox_ImageQualityLevel");
        spinBox_ImageQualityLevel->setMaximum(100);
        spinBox_ImageQualityLevel->setValue(100);

        gridLayout_45->addWidget(spinBox_ImageQualityLevel, 2, 0, 1, 1);


        gridLayout_26->addWidget(frame_ImageQuality, 1, 1, 1, 1);

        label_ImageStyle_W2xNCNNVulkan = new QLabel(groupBox_ImageOutputSettings);
        label_ImageStyle_W2xNCNNVulkan->setObjectName("label_ImageStyle_W2xNCNNVulkan");

        gridLayout_26->addWidget(label_ImageStyle_W2xNCNNVulkan, 7, 0, 1, 1);

        label_ImageStyle_W2xCaffe = new QLabel(groupBox_ImageOutputSettings);
        label_ImageStyle_W2xCaffe->setObjectName("label_ImageStyle_W2xCaffe");

        gridLayout_26->addWidget(label_ImageStyle_W2xCaffe, 8, 0, 1, 1);

        frame_ImageFormat = new QFrame(groupBox_ImageOutputSettings);
        frame_ImageFormat->setObjectName("frame_ImageFormat");
        gridLayout_42 = new QGridLayout(frame_ImageFormat);
        gridLayout_42->setObjectName("gridLayout_42");
        comboBox_ImageSaveFormat = new QComboBox(frame_ImageFormat);
        comboBox_ImageSaveFormat->addItem(QString::fromUtf8("png"));
        comboBox_ImageSaveFormat->addItem(QString::fromUtf8("jpg"));
        comboBox_ImageSaveFormat->addItem(QString::fromUtf8("webp"));
        comboBox_ImageSaveFormat->setObjectName("comboBox_ImageSaveFormat");

        gridLayout_42->addWidget(comboBox_ImageSaveFormat, 1, 0, 1, 1);

        label_20 = new QLabel(frame_ImageFormat);
        label_20->setObjectName("label_20");

        gridLayout_42->addWidget(label_20, 0, 0, 1, 1);


        gridLayout_26->addWidget(frame_ImageFormat, 1, 0, 1, 1);

        comboBox_ImageStyle = new QComboBox(groupBox_ImageOutputSettings);
        comboBox_ImageStyle->addItem(QString());
        comboBox_ImageStyle->addItem(QString());
        comboBox_ImageStyle->setObjectName("comboBox_ImageStyle");
        comboBox_ImageStyle->setCurrentText(QString::fromUtf8("2D Anime"));

        gridLayout_26->addWidget(comboBox_ImageStyle, 7, 1, 1, 1);

        comboBox_ImageStyle_Waifu2xCaffe = new QComboBox(groupBox_ImageOutputSettings);
        comboBox_ImageStyle_Waifu2xCaffe->addItem(QString());
        comboBox_ImageStyle_Waifu2xCaffe->addItem(QString());
        comboBox_ImageStyle_Waifu2xCaffe->setObjectName("comboBox_ImageStyle_Waifu2xCaffe");

        gridLayout_26->addWidget(comboBox_ImageStyle_Waifu2xCaffe, 8, 1, 1, 1);

        frame_13 = new QFrame(groupBox_ImageOutputSettings);
        frame_13->setObjectName("frame_13");
        gridLayout_66 = new QGridLayout(frame_13);
        gridLayout_66->setObjectName("gridLayout_66");
        checkBox_DelOriginal = new QCheckBox(frame_13);
        checkBox_DelOriginal->setObjectName("checkBox_DelOriginal");

        gridLayout_66->addWidget(checkBox_DelOriginal, 0, 1, 1, 1);

        checkBox_ReplaceOriginalFile = new QCheckBox(frame_13);
        checkBox_ReplaceOriginalFile->setObjectName("checkBox_ReplaceOriginalFile");

        gridLayout_66->addWidget(checkBox_ReplaceOriginalFile, 0, 2, 1, 1);

        checkBox_OptGIF = new QCheckBox(frame_13);
        checkBox_OptGIF->setObjectName("checkBox_OptGIF");

        gridLayout_66->addWidget(checkBox_OptGIF, 0, 0, 1, 1);


        gridLayout_26->addWidget(frame_13, 4, 0, 1, 2);

        comboBox_FinishAction = new QComboBox(groupBox_ImageOutputSettings);
        comboBox_FinishAction->addItem(QString());
        comboBox_FinishAction->setObjectName("comboBox_FinishAction");

        gridLayout_26->addWidget(comboBox_FinishAction, 5, 0, 1, 1);

        checkBox_ReProcFinFiles = new QCheckBox(groupBox_ImageOutputSettings);
        checkBox_ReProcFinFiles->setObjectName("checkBox_ReProcFinFiles");

        gridLayout_26->addWidget(checkBox_ReProcFinFiles, 5, 1, 1, 1);


        verticalLayout_RightPane->addWidget(groupBox_ImageOutputSettings);

        groupBox_OutPut = new QGroupBox(groupBox_Setting);
        groupBox_OutPut->setObjectName("groupBox_OutPut");
        gridLayout_23 = new QGridLayout(groupBox_OutPut);
        gridLayout_23->setObjectName("gridLayout_23");
        lineEdit_outputPath = new QLineEdit(groupBox_OutPut);
        lineEdit_outputPath->setObjectName("lineEdit_outputPath");

        gridLayout_23->addWidget(lineEdit_outputPath, 0, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        checkBox_OutPath_isEnabled = new QCheckBox(groupBox_OutPut);
        checkBox_OutPath_isEnabled->setObjectName("checkBox_OutPath_isEnabled");

        horizontalLayout_5->addWidget(checkBox_OutPath_isEnabled);

        checkBox_AutoOpenOutputPath = new QCheckBox(groupBox_OutPut);
        checkBox_AutoOpenOutputPath->setObjectName("checkBox_AutoOpenOutputPath");

        horizontalLayout_5->addWidget(checkBox_AutoOpenOutputPath);

        checkBox_OutPath_Overwrite = new QCheckBox(groupBox_OutPut);
        checkBox_OutPath_Overwrite->setObjectName("checkBox_OutPath_Overwrite");

        horizontalLayout_5->addWidget(checkBox_OutPath_Overwrite);


        gridLayout_23->addLayout(horizontalLayout_5, 1, 0, 1, 1);


        verticalLayout_RightPane->addWidget(groupBox_OutPut);

        groupBox_ScaleRaton_DenoiseLevel = new QGroupBox(groupBox_Setting);
        groupBox_ScaleRaton_DenoiseLevel->setObjectName("groupBox_ScaleRaton_DenoiseLevel");
        groupBox_ScaleRaton_DenoiseLevel->setTitle(QString::fromUtf8(""));
        gridLayout = new QGridLayout(groupBox_ScaleRaton_DenoiseLevel);
        gridLayout->setObjectName("gridLayout");
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_51 = new QHBoxLayout();
        horizontalLayout_51->setObjectName("horizontalLayout_51");
        label_57 = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_57->setObjectName("label_57");

        horizontalLayout_51->addWidget(label_57);

        label = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label->setObjectName("label");

        horizontalLayout_51->addWidget(label);

        doubleSpinBox_ScaleRatio_image = new QDoubleSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        doubleSpinBox_ScaleRatio_image->setObjectName("doubleSpinBox_ScaleRatio_image");
        doubleSpinBox_ScaleRatio_image->setValue(2.000000000000000);

        horizontalLayout_51->addWidget(doubleSpinBox_ScaleRatio_image);

        label_16 = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_16->setObjectName("label_16");

        horizontalLayout_51->addWidget(label_16);

        doubleSpinBox_ScaleRatio_gif = new QDoubleSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        doubleSpinBox_ScaleRatio_gif->setObjectName("doubleSpinBox_ScaleRatio_gif");
        doubleSpinBox_ScaleRatio_gif->setValue(2.000000000000000);

        horizontalLayout_51->addWidget(doubleSpinBox_ScaleRatio_gif);

        label_17 = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_17->setObjectName("label_17");

        horizontalLayout_51->addWidget(label_17);

        doubleSpinBox_ScaleRatio_video = new QDoubleSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        doubleSpinBox_ScaleRatio_video->setObjectName("doubleSpinBox_ScaleRatio_video");
        doubleSpinBox_ScaleRatio_video->setValue(2.000000000000000);

        horizontalLayout_51->addWidget(doubleSpinBox_ScaleRatio_video);


        verticalLayout_3->addLayout(horizontalLayout_51);

        horizontalLayout_52 = new QHBoxLayout();
        horizontalLayout_52->setObjectName("horizontalLayout_52");
        label_66 = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_66->setObjectName("label_66");

        horizontalLayout_52->addWidget(label_66);

        label_ImageDenoiseLevel = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_ImageDenoiseLevel->setObjectName("label_ImageDenoiseLevel");

        horizontalLayout_52->addWidget(label_ImageDenoiseLevel);

        spinBox_DenoiseLevel_image = new QSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        spinBox_DenoiseLevel_image->setObjectName("spinBox_DenoiseLevel_image");
        spinBox_DenoiseLevel_image->setValue(2);

        horizontalLayout_52->addWidget(spinBox_DenoiseLevel_image);

        label_GIFDenoiseLevel = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_GIFDenoiseLevel->setObjectName("label_GIFDenoiseLevel");

        horizontalLayout_52->addWidget(label_GIFDenoiseLevel);

        spinBox_DenoiseLevel_gif = new QSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        spinBox_DenoiseLevel_gif->setObjectName("spinBox_DenoiseLevel_gif");
        spinBox_DenoiseLevel_gif->setValue(2);

        horizontalLayout_52->addWidget(spinBox_DenoiseLevel_gif);

        label_VideoDenoiseLevel = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_VideoDenoiseLevel->setObjectName("label_VideoDenoiseLevel");

        horizontalLayout_52->addWidget(label_VideoDenoiseLevel);

        spinBox_DenoiseLevel_video = new QSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        spinBox_DenoiseLevel_video->setObjectName("spinBox_DenoiseLevel_video");
        spinBox_DenoiseLevel_video->setValue(2);

        horizontalLayout_52->addWidget(spinBox_DenoiseLevel_video);


        verticalLayout_3->addLayout(horizontalLayout_52);


        gridLayout->addLayout(verticalLayout_3, 0, 1, 1, 1);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName("verticalLayout_6");
        label_28 = new QLabel(groupBox_ScaleRaton_DenoiseLevel);
        label_28->setObjectName("label_28");

        verticalLayout_6->addWidget(label_28);

        horizontalLayout_50 = new QHBoxLayout();
        horizontalLayout_50->setObjectName("horizontalLayout_50");
        pushButton_MultipleOfFPS_VFI_MIN = new QPushButton(groupBox_ScaleRaton_DenoiseLevel);
        pushButton_MultipleOfFPS_VFI_MIN->setObjectName("pushButton_MultipleOfFPS_VFI_MIN");
        pushButton_MultipleOfFPS_VFI_MIN->setText(QString::fromUtf8("-"));

        horizontalLayout_50->addWidget(pushButton_MultipleOfFPS_VFI_MIN);

        spinBox_MultipleOfFPS_VFI = new QSpinBox(groupBox_ScaleRaton_DenoiseLevel);
        spinBox_MultipleOfFPS_VFI->setObjectName("spinBox_MultipleOfFPS_VFI");
        spinBox_MultipleOfFPS_VFI->setMinimum(2);

        horizontalLayout_50->addWidget(spinBox_MultipleOfFPS_VFI);

        pushButton_MultipleOfFPS_VFI_ADD = new QPushButton(groupBox_ScaleRaton_DenoiseLevel);
        pushButton_MultipleOfFPS_VFI_ADD->setObjectName("pushButton_MultipleOfFPS_VFI_ADD");
        pushButton_MultipleOfFPS_VFI_ADD->setText(QString::fromUtf8("+"));

        horizontalLayout_50->addWidget(pushButton_MultipleOfFPS_VFI_ADD);


        verticalLayout_6->addLayout(horizontalLayout_50);


        gridLayout->addLayout(verticalLayout_6, 0, 3, 1, 1);


        verticalLayout_RightPane->addWidget(groupBox_ScaleRaton_DenoiseLevel);

        verticalSpacer_RightPane = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_RightPane->addItem(verticalSpacer_RightPane);

        homeMainSplitter->addWidget(groupBox_Setting);

        homeTabLayout->addWidget(homeMainSplitter);

        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/new/prefix1/icon/Home.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_Home, icon14, QString());
        tab_EngineSettings = new QWidget();
        tab_EngineSettings->setObjectName("tab_EngineSettings");
        gridLayout_29 = new QGridLayout(tab_EngineSettings);
        gridLayout_29->setObjectName("gridLayout_29");
        gridLayout_29->setHorizontalSpacing(0);
        gridLayout_29->setContentsMargins(6, 6, 6, 6);
        groupBox_NumOfThreads = new QGroupBox(tab_EngineSettings);
        groupBox_NumOfThreads->setObjectName("groupBox_NumOfThreads");
        groupBox_NumOfThreads->setEnabled(true);
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(groupBox_NumOfThreads->sizePolicy().hasHeightForWidth());
        groupBox_NumOfThreads->setSizePolicy(sizePolicy4);
        groupBox_NumOfThreads->setMaximumSize(QSize(16777215, 16777215));
        groupBox_NumOfThreads->setAlignment(Qt::AlignCenter);
        groupBox_NumOfThreads->setFlat(false);
        gridLayout_8 = new QGridLayout(groupBox_NumOfThreads);
        gridLayout_8->setObjectName("gridLayout_8");
        gridLayout_8->setContentsMargins(6, 6, 6, 6);
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_6, 0, 2, 1, 1);

        frame_16 = new QFrame(groupBox_NumOfThreads);
        frame_16->setObjectName("frame_16");
        sizePolicy4.setHeightForWidth(frame_16->sizePolicy().hasHeightForWidth());
        frame_16->setSizePolicy(sizePolicy4);
        frame_16->setFrameShape(QFrame::NoFrame);
        frame_16->setFrameShadow(QFrame::Raised);
        gridLayout_57 = new QGridLayout(frame_16);
        gridLayout_57->setObjectName("gridLayout_57");
        gridLayout_57->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(frame_16);
        label_11->setObjectName("label_11");
        sizePolicy3.setHeightForWidth(label_11->sizePolicy().hasHeightForWidth());
        label_11->setSizePolicy(sizePolicy3);
        label_11->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_57->addWidget(label_11, 0, 2, 1, 1);

        line = new QFrame(frame_16);
        line->setObjectName("line");
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_57->addWidget(line, 0, 4, 1, 1);

        label_13 = new QLabel(frame_16);
        label_13->setObjectName("label_13");
        sizePolicy3.setHeightForWidth(label_13->sizePolicy().hasHeightForWidth());
        label_13->setSizePolicy(sizePolicy3);
        label_13->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_57->addWidget(label_13, 0, 8, 1, 1);

        line_2 = new QFrame(frame_16);
        line_2->setObjectName("line_2");
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);

        gridLayout_57->addWidget(line_2, 0, 7, 1, 1);

        spinBox_ThreadNum_gif_internal = new QSpinBox(frame_16);
        spinBox_ThreadNum_gif_internal->setObjectName("spinBox_ThreadNum_gif_internal");
        sizePolicy4.setHeightForWidth(spinBox_ThreadNum_gif_internal->sizePolicy().hasHeightForWidth());
        spinBox_ThreadNum_gif_internal->setSizePolicy(sizePolicy4);
        spinBox_ThreadNum_gif_internal->setMinimum(1);
        spinBox_ThreadNum_gif_internal->setMaximum(999999999);
        spinBox_ThreadNum_gif_internal->setSingleStep(1);
        spinBox_ThreadNum_gif_internal->setValue(1);

        gridLayout_57->addWidget(spinBox_ThreadNum_gif_internal, 0, 6, 1, 1);

        spinBox_ThreadNum_video_internal = new QSpinBox(frame_16);
        spinBox_ThreadNum_video_internal->setObjectName("spinBox_ThreadNum_video_internal");
        sizePolicy4.setHeightForWidth(spinBox_ThreadNum_video_internal->sizePolicy().hasHeightForWidth());
        spinBox_ThreadNum_video_internal->setSizePolicy(sizePolicy4);
        spinBox_ThreadNum_video_internal->setMinimum(1);
        spinBox_ThreadNum_video_internal->setMaximum(999999999);
        spinBox_ThreadNum_video_internal->setSingleStep(1);
        spinBox_ThreadNum_video_internal->setValue(1);

        gridLayout_57->addWidget(spinBox_ThreadNum_video_internal, 0, 9, 1, 1);

        label_12 = new QLabel(frame_16);
        label_12->setObjectName("label_12");
        sizePolicy3.setHeightForWidth(label_12->sizePolicy().hasHeightForWidth());
        label_12->setSizePolicy(sizePolicy3);
        label_12->setMaximumSize(QSize(16777215, 16777215));

        gridLayout_57->addWidget(label_12, 0, 5, 1, 1);

        spinBox_ThreadNum_image = new QSpinBox(frame_16);
        spinBox_ThreadNum_image->setObjectName("spinBox_ThreadNum_image");
        sizePolicy4.setHeightForWidth(spinBox_ThreadNum_image->sizePolicy().hasHeightForWidth());
        spinBox_ThreadNum_image->setSizePolicy(sizePolicy4);
        spinBox_ThreadNum_image->setMinimum(1);
        spinBox_ThreadNum_image->setMaximum(999999999);
        spinBox_ThreadNum_image->setSingleStep(1);
        spinBox_ThreadNum_image->setValue(1);

        gridLayout_57->addWidget(spinBox_ThreadNum_image, 0, 3, 1, 1);


        gridLayout_8->addWidget(frame_16, 0, 1, 1, 1);

        horizontalSpacer_29 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_29, 0, 0, 1, 1);

        horizontalSpacer_49 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_49, 1, 0, 1, 1);

        label_NumThreadsActive = new QLabel(groupBox_NumOfThreads);
        label_NumThreadsActive->setObjectName("label_NumThreadsActive");

        gridLayout_8->addWidget(label_NumThreadsActive, 1, 1, 1, 1);

        horizontalSpacer_50 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_8->addItem(horizontalSpacer_50, 1, 2, 1, 1);

        gridLayout_8->setColumnStretch(0, 20);
        gridLayout_8->setColumnStretch(1, 18);
        gridLayout_8->setColumnStretch(2, 20);

        gridLayout_29->addWidget(groupBox_NumOfThreads, 1, 0, 1, 1);

        groupBox_Engine = new QGroupBox(tab_EngineSettings);
        groupBox_Engine->setObjectName("groupBox_Engine");
        groupBox_Engine->setMinimumSize(QSize(0, 0));
        groupBox_Engine->setAlignment(Qt::AlignCenter);
        groupBox_Engine->setFlat(false);
        groupBox_Engine->setCheckable(false);
        gridLayout_6 = new QGridLayout(groupBox_Engine);
        gridLayout_6->setObjectName("gridLayout_6");
        gridLayout_6->setContentsMargins(6, 6, 6, 6);
        line_6 = new QFrame(groupBox_Engine);
        line_6->setObjectName("line_6");
        line_6->setFrameShape(QFrame::HLine);
        line_6->setFrameShadow(QFrame::Sunken);

        gridLayout_6->addWidget(line_6, 2, 0, 1, 4);

        frame_18 = new QFrame(groupBox_Engine);
        frame_18->setObjectName("frame_18");
        frame_18->setMinimumSize(QSize(0, 0));
        frame_18->setFrameShape(QFrame::NoFrame);
        frame_18->setFrameShadow(QFrame::Raised);
        gridLayout_59 = new QGridLayout(frame_18);
        gridLayout_59->setObjectName("gridLayout_59");
        gridLayout_59->setContentsMargins(0, 0, 0, 0);
        frame_17 = new QFrame(frame_18);
        frame_17->setObjectName("frame_17");
        sizePolicy4.setHeightForWidth(frame_17->sizePolicy().hasHeightForWidth());
        frame_17->setSizePolicy(sizePolicy4);
        frame_17->setFrameShape(QFrame::NoFrame);
        frame_17->setFrameShadow(QFrame::Raised);
        gridLayout_58 = new QGridLayout(frame_17);
        gridLayout_58->setObjectName("gridLayout_58");
        gridLayout_58->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(frame_17);
        label_7->setObjectName("label_7");
        sizePolicy3.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy3);
        label_7->setMaximumSize(QSize(16777215, 16777215));
        label_7->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_58->addWidget(label_7, 0, 4, 1, 1);

        line_12 = new QFrame(frame_17);
        line_12->setObjectName("line_12");
        line_12->setFrameShape(QFrame::VLine);
        line_12->setFrameShadow(QFrame::Sunken);

        gridLayout_58->addWidget(line_12, 0, 6, 1, 1);

        comboBox_Engine_GIF = new QComboBox(frame_17);
        comboBox_Engine_GIF->addItem(QString::fromUtf8("srmd-ncnn-vulkan"));
        comboBox_Engine_GIF->addItem(QString::fromUtf8("Anime4K"));
        comboBox_Engine_GIF->addItem(QString::fromUtf8("realsr-ncnn-vulkan"));
        comboBox_Engine_GIF->addItem(QString());
        comboBox_Engine_GIF->addItem(QString::fromUtf8("RealESRGAN-NCNN-Vulkan"));
        comboBox_Engine_GIF->addItem(QString::fromUtf8("RealCUGAN-NCNN-Vulkan"));
        comboBox_Engine_GIF->setObjectName("comboBox_Engine_GIF");
        comboBox_Engine_GIF->setEnabled(true);
        comboBox_Engine_GIF->setMinimumSize(QSize(190, 0));
        comboBox_Engine_GIF->setCurrentText(QString::fromUtf8("RealESRGAN-NCNN-Vulkan"));
        comboBox_Engine_GIF->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        gridLayout_58->addWidget(comboBox_Engine_GIF, 0, 5, 1, 1);

        horizontalSpacer_30 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_58->addItem(horizontalSpacer_30, 0, 0, 1, 1);

        label_6 = new QLabel(frame_17);
        label_6->setObjectName("label_6");
        sizePolicy3.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy3);
        label_6->setMaximumSize(QSize(16777215, 16777215));
        label_6->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_58->addWidget(label_6, 0, 1, 1, 1);

        label_8 = new QLabel(frame_17);
        label_8->setObjectName("label_8");
        sizePolicy3.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy3);
        label_8->setMaximumSize(QSize(16777215, 16777215));
        label_8->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_58->addWidget(label_8, 0, 7, 1, 1);

        line_11 = new QFrame(frame_17);
        line_11->setObjectName("line_11");
        line_11->setFrameShape(QFrame::VLine);
        line_11->setFrameShadow(QFrame::Sunken);

        gridLayout_58->addWidget(line_11, 0, 3, 1, 1);

        comboBox_Engine_Video = new QComboBox(frame_17);
        comboBox_Engine_Video->addItem(QString::fromUtf8("waifu2x-ncnn-vulkan"));
        comboBox_Engine_Video->addItem(QString::fromUtf8("waifu2x-converter"));
        comboBox_Engine_Video->addItem(QString::fromUtf8("Anime4k"));
        comboBox_Engine_Video->addItem(QString::fromUtf8("srmd-ncnn-vulkan"));
        comboBox_Engine_Video->addItem(QString::fromUtf8("waifu2x-caffe"));
        comboBox_Engine_Video->addItem(QString::fromUtf8("realsr-ncnn-vulkan"));
        comboBox_Engine_Video->addItem(QString());
        comboBox_Engine_Video->setObjectName("comboBox_Engine_Video");
        comboBox_Engine_Video->setEnabled(true);
        comboBox_Engine_Video->setMinimumSize(QSize(190, 0));
        comboBox_Engine_Video->setCurrentText(QString::fromUtf8("waifu2x-ncnn-vulkan"));
        comboBox_Engine_Video->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        gridLayout_58->addWidget(comboBox_Engine_Video, 0, 8, 1, 1);

        comboBox_Engine_Image = new QComboBox(frame_17);
        comboBox_Engine_Image->addItem(QString::fromUtf8("srmd-ncnn-vulkan"));
        comboBox_Engine_Image->addItem(QString::fromUtf8("Anime4K"));
        comboBox_Engine_Image->addItem(QString::fromUtf8("realsr-ncnn-vulkan"));
        comboBox_Engine_Image->addItem(QString());
        comboBox_Engine_Image->addItem(QString::fromUtf8("RealESRGAN-NCNN-Vulkan"));
        comboBox_Engine_Image->addItem(QString::fromUtf8("RealCUGAN-NCNN-Vulkan"));
        comboBox_Engine_Image->setObjectName("comboBox_Engine_Image");
        comboBox_Engine_Image->setEnabled(true);
        comboBox_Engine_Image->setMinimumSize(QSize(190, 0));
        comboBox_Engine_Image->setMaximumSize(QSize(16777215, 16777215));
        comboBox_Engine_Image->setCurrentText(QString::fromUtf8("RealESRGAN-NCNN-Vulkan"));
        comboBox_Engine_Image->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        gridLayout_58->addWidget(comboBox_Engine_Image, 0, 2, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_58->addItem(horizontalSpacer_8, 0, 9, 1, 1);


        gridLayout_59->addWidget(frame_17, 0, 0, 1, 1);


        gridLayout_6->addWidget(frame_18, 0, 0, 1, 4);

        tabWidget_Engines = new QTabWidget(groupBox_Engine);
        tabWidget_Engines->setObjectName("tabWidget_Engines");
        tabWidget_Engines->setTabPosition(QTabWidget::North);
        tabWidget_Engines->setUsesScrollButtons(false);
        tab_W2xNcnnVulkan = new QWidget();
        tab_W2xNcnnVulkan->setObjectName("tab_W2xNcnnVulkan");
        gridLayout_53 = new QGridLayout(tab_W2xNcnnVulkan);
        gridLayout_53->setObjectName("gridLayout_53");
        gridLayout_53->setVerticalSpacing(7);
        gridLayout_53->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        label_10 = new QLabel(tab_W2xNcnnVulkan);
        label_10->setObjectName("label_10");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(label_10->sizePolicy().hasHeightForWidth());
        label_10->setSizePolicy(sizePolicy5);

        horizontalLayout_13->addWidget(label_10);

        comboBox_GPUID = new QComboBox(tab_W2xNcnnVulkan);
        comboBox_GPUID->addItem(QString::fromUtf8("auto"));
        comboBox_GPUID->setObjectName("comboBox_GPUID");
        comboBox_GPUID->setEnabled(true);
        sizePolicy4.setHeightForWidth(comboBox_GPUID->sizePolicy().hasHeightForWidth());
        comboBox_GPUID->setSizePolicy(sizePolicy4);
        comboBox_GPUID->setMinimumSize(QSize(180, 0));
        comboBox_GPUID->setCurrentText(QString::fromUtf8("auto"));

        horizontalLayout_13->addWidget(comboBox_GPUID);

        pushButton_DetectGPU = new QPushButton(tab_W2xNcnnVulkan);
        pushButton_DetectGPU->setObjectName("pushButton_DetectGPU");
        pushButton_DetectGPU->setEnabled(true);
        sizePolicy5.setHeightForWidth(pushButton_DetectGPU->sizePolicy().hasHeightForWidth());
        pushButton_DetectGPU->setSizePolicy(sizePolicy5);

        horizontalLayout_13->addWidget(pushButton_DetectGPU);

        checkBox_MultiGPU_Waifu2xNCNNVulkan = new QCheckBox(tab_W2xNcnnVulkan);
        checkBox_MultiGPU_Waifu2xNCNNVulkan->setObjectName("checkBox_MultiGPU_Waifu2xNCNNVulkan");
        sizePolicy4.setHeightForWidth(checkBox_MultiGPU_Waifu2xNCNNVulkan->sizePolicy().hasHeightForWidth());
        checkBox_MultiGPU_Waifu2xNCNNVulkan->setSizePolicy(sizePolicy4);

        horizontalLayout_13->addWidget(checkBox_MultiGPU_Waifu2xNCNNVulkan);

        horizontalSpacer_28 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_28);


        gridLayout_53->addLayout(horizontalLayout_13, 8, 0, 1, 2);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        label_9 = new QLabel(tab_W2xNcnnVulkan);
        label_9->setObjectName("label_9");
        sizePolicy5.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy5);
        label_9->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_12->addWidget(label_9);

        frame_TileSize_W2xNcnnVulkan = new QFrame(tab_W2xNcnnVulkan);
        frame_TileSize_W2xNcnnVulkan->setObjectName("frame_TileSize_W2xNcnnVulkan");
        sizePolicy5.setHeightForWidth(frame_TileSize_W2xNcnnVulkan->sizePolicy().hasHeightForWidth());
        frame_TileSize_W2xNcnnVulkan->setSizePolicy(sizePolicy5);
        frame_TileSize_W2xNcnnVulkan->setFrameShape(QFrame::NoFrame);
        frame_TileSize_W2xNcnnVulkan->setFrameShadow(QFrame::Raised);
        gridLayout_31 = new QGridLayout(frame_TileSize_W2xNcnnVulkan);
        gridLayout_31->setObjectName("gridLayout_31");
        gridLayout_31->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout_31->setContentsMargins(0, 0, 0, 0);
        pushButton_TileSize_Minus_W2xNCNNVulkan = new QPushButton(frame_TileSize_W2xNcnnVulkan);
        pushButton_TileSize_Minus_W2xNCNNVulkan->setObjectName("pushButton_TileSize_Minus_W2xNCNNVulkan");
        sizePolicy5.setHeightForWidth(pushButton_TileSize_Minus_W2xNCNNVulkan->sizePolicy().hasHeightForWidth());
        pushButton_TileSize_Minus_W2xNCNNVulkan->setSizePolicy(sizePolicy5);
        pushButton_TileSize_Minus_W2xNCNNVulkan->setMaximumSize(QSize(30, 30));
        pushButton_TileSize_Minus_W2xNCNNVulkan->setText(QString::fromUtf8("-"));

        gridLayout_31->addWidget(pushButton_TileSize_Minus_W2xNCNNVulkan, 0, 0, 1, 1);

        spinBox_TileSize = new QSpinBox(frame_TileSize_W2xNcnnVulkan);
        spinBox_TileSize->setObjectName("spinBox_TileSize");
        sizePolicy5.setHeightForWidth(spinBox_TileSize->sizePolicy().hasHeightForWidth());
        spinBox_TileSize->setSizePolicy(sizePolicy5);
        spinBox_TileSize->setMinimum(32);
        spinBox_TileSize->setMaximum(999999999);
        spinBox_TileSize->setSingleStep(10);
        spinBox_TileSize->setValue(128);

        gridLayout_31->addWidget(spinBox_TileSize, 0, 1, 1, 1);

        pushButton_TileSize_Add_W2xNCNNVulkan = new QPushButton(frame_TileSize_W2xNcnnVulkan);
        pushButton_TileSize_Add_W2xNCNNVulkan->setObjectName("pushButton_TileSize_Add_W2xNCNNVulkan");
        sizePolicy5.setHeightForWidth(pushButton_TileSize_Add_W2xNCNNVulkan->sizePolicy().hasHeightForWidth());
        pushButton_TileSize_Add_W2xNCNNVulkan->setSizePolicy(sizePolicy5);
        pushButton_TileSize_Add_W2xNCNNVulkan->setMaximumSize(QSize(30, 30));
        pushButton_TileSize_Add_W2xNCNNVulkan->setText(QString::fromUtf8("+"));

        gridLayout_31->addWidget(pushButton_TileSize_Add_W2xNCNNVulkan, 0, 2, 1, 1);


        horizontalLayout_12->addWidget(frame_TileSize_W2xNcnnVulkan);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_7);


        gridLayout_53->addLayout(horizontalLayout_12, 4, 0, 1, 2);

        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setSpacing(0);
        horizontalLayout_24->setObjectName("horizontalLayout_24");
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan = new QGroupBox(tab_W2xNcnnVulkan);
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setObjectName("groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan");
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setEnabled(false);
        sizePolicy4.setHeightForWidth(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->sizePolicy().hasHeightForWidth());
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setSizePolicy(sizePolicy4);
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setMaximumSize(QSize(16777215, 16777215));
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setAlignment(Qt::AlignCenter);
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setFlat(true);
        gridLayout_39 = new QGridLayout(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        gridLayout_39->setObjectName("gridLayout_39");
        gridLayout_39->setHorizontalSpacing(7);
        gridLayout_39->setContentsMargins(0, 6, 0, 6);
        comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan = new QComboBox(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan->setObjectName("comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan");
        comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan->setCurrentText(QString::fromUtf8(""));

        gridLayout_39->addWidget(comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan, 0, 1, 1, 1);

        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan = new QSpinBox(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setObjectName("spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan");
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setMinimum(32);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setMaximum(999999999);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setSingleStep(10);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setValue(128);

        gridLayout_39->addWidget(spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan, 0, 5, 1, 1);

        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan = new QCheckBox(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setObjectName("checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan");
        sizePolicy5.setHeightForWidth(checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->sizePolicy().hasHeightForWidth());
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setSizePolicy(sizePolicy5);
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setChecked(true);

        gridLayout_39->addWidget(checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan, 0, 2, 1, 1);

        pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan = new QPushButton(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan->setObjectName("pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan");

        gridLayout_39->addWidget(pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan, 0, 6, 1, 1);

        label_64 = new QLabel(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        label_64->setObjectName("label_64");
        sizePolicy3.setHeightForWidth(label_64->sizePolicy().hasHeightForWidth());
        label_64->setSizePolicy(sizePolicy3);
        label_64->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_39->addWidget(label_64, 0, 4, 1, 1);

        label_65 = new QLabel(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        label_65->setObjectName("label_65");
        sizePolicy3.setHeightForWidth(label_65->sizePolicy().hasHeightForWidth());
        label_65->setSizePolicy(sizePolicy3);
        label_65->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_39->addWidget(label_65, 0, 0, 1, 1);

        line_21 = new QFrame(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);
        line_21->setObjectName("line_21");
        line_21->setFrameShape(QFrame::VLine);
        line_21->setFrameShadow(QFrame::Sunken);

        gridLayout_39->addWidget(line_21, 0, 3, 1, 1);


        horizontalLayout_24->addWidget(groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan);

        horizontalSpacer_31 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_24->addItem(horizontalSpacer_31);

        horizontalLayout_24->setStretch(0, 1);
        horizontalLayout_24->setStretch(1, 2);

        gridLayout_53->addLayout(horizontalLayout_24, 10, 0, 1, 2);

        line_31 = new QFrame(tab_W2xNcnnVulkan);
        line_31->setObjectName("line_31");
        line_31->setFrameShape(QFrame::HLine);
        line_31->setFrameShadow(QFrame::Sunken);

        gridLayout_53->addWidget(line_31, 5, 0, 1, 2);

        line_30 = new QFrame(tab_W2xNcnnVulkan);
        line_30->setObjectName("line_30");
        line_30->setFrameShape(QFrame::HLine);
        line_30->setFrameShadow(QFrame::Sunken);

        gridLayout_53->addWidget(line_30, 2, 0, 1, 2);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        label_47 = new QLabel(tab_W2xNcnnVulkan);
        label_47->setObjectName("label_47");
        sizePolicy5.setHeightForWidth(label_47->sizePolicy().hasHeightForWidth());
        label_47->setSizePolicy(sizePolicy5);

        horizontalLayout_14->addWidget(label_47);

        comboBox_version_Waifu2xNCNNVulkan = new QComboBox(tab_W2xNcnnVulkan);
        comboBox_version_Waifu2xNCNNVulkan->addItem(QString());
        comboBox_version_Waifu2xNCNNVulkan->addItem(QString());
        comboBox_version_Waifu2xNCNNVulkan->addItem(QString());
        comboBox_version_Waifu2xNCNNVulkan->setObjectName("comboBox_version_Waifu2xNCNNVulkan");

        horizontalLayout_14->addWidget(comboBox_version_Waifu2xNCNNVulkan);

        horizontalSpacer_18 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_18);


        gridLayout_53->addLayout(horizontalLayout_14, 6, 0, 1, 2);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName("horizontalLayout_15");
        label_26 = new QLabel(tab_W2xNcnnVulkan);
        label_26->setObjectName("label_26");
        sizePolicy5.setHeightForWidth(label_26->sizePolicy().hasHeightForWidth());
        label_26->setSizePolicy(sizePolicy5);

        horizontalLayout_15->addWidget(label_26);

        comboBox_model_vulkan = new QComboBox(tab_W2xNcnnVulkan);
        comboBox_model_vulkan->addItem(QString::fromUtf8("upconv_7"));
        comboBox_model_vulkan->addItem(QString());
        comboBox_model_vulkan->setObjectName("comboBox_model_vulkan");

        horizontalLayout_15->addWidget(comboBox_model_vulkan);

        checkBox_TTA_vulkan = new QCheckBox(tab_W2xNcnnVulkan);
        checkBox_TTA_vulkan->setObjectName("checkBox_TTA_vulkan");
        sizePolicy4.setHeightForWidth(checkBox_TTA_vulkan->sizePolicy().hasHeightForWidth());
        checkBox_TTA_vulkan->setSizePolicy(sizePolicy4);
        checkBox_TTA_vulkan->setText(QString::fromUtf8("TTA"));

        horizontalLayout_15->addWidget(checkBox_TTA_vulkan);

        horizontalSpacer_22 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_22);


        gridLayout_53->addLayout(horizontalLayout_15, 1, 0, 1, 2);

        line_32 = new QFrame(tab_W2xNcnnVulkan);
        line_32->setObjectName("line_32");
        line_32->setFrameShape(QFrame::HLine);
        line_32->setFrameShadow(QFrame::Sunken);

        gridLayout_53->addWidget(line_32, 7, 0, 1, 2);

        tabWidget_Engines->addTab(tab_W2xNcnnVulkan, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_W2xNcnnVulkan), QString::fromUtf8("Waifu2x-ncnn-vulkan"));
        tab_W2xConverter = new QWidget();
        tab_W2xConverter->setObjectName("tab_W2xConverter");
        gridLayout_54 = new QGridLayout(tab_W2xConverter);
        gridLayout_54->setObjectName("gridLayout_54");
        gridLayout_54->setVerticalSpacing(7);
        gridLayout_54->setContentsMargins(6, 6, 6, 6);
        line_34 = new QFrame(tab_W2xConverter);
        line_34->setObjectName("line_34");
        line_34->setFrameShape(QFrame::HLine);
        line_34->setFrameShadow(QFrame::Sunken);

        gridLayout_54->addWidget(line_34, 2, 0, 1, 2);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName("horizontalLayout_17");
        checkBox_ForceOpenCL_converter = new QCheckBox(tab_W2xConverter);
        checkBox_ForceOpenCL_converter->setObjectName("checkBox_ForceOpenCL_converter");
        sizePolicy5.setHeightForWidth(checkBox_ForceOpenCL_converter->sizePolicy().hasHeightForWidth());
        checkBox_ForceOpenCL_converter->setSizePolicy(sizePolicy5);

        horizontalLayout_17->addWidget(checkBox_ForceOpenCL_converter);

        checkBox_DisableGPU_converter = new QCheckBox(tab_W2xConverter);
        checkBox_DisableGPU_converter->setObjectName("checkBox_DisableGPU_converter");
        sizePolicy5.setHeightForWidth(checkBox_DisableGPU_converter->sizePolicy().hasHeightForWidth());
        checkBox_DisableGPU_converter->setSizePolicy(sizePolicy5);

        horizontalLayout_17->addWidget(checkBox_DisableGPU_converter);

        checkBox_TTA_converter = new QCheckBox(tab_W2xConverter);
        checkBox_TTA_converter->setObjectName("checkBox_TTA_converter");
        sizePolicy5.setHeightForWidth(checkBox_TTA_converter->sizePolicy().hasHeightForWidth());
        checkBox_TTA_converter->setSizePolicy(sizePolicy5);
        checkBox_TTA_converter->setText(QString::fromUtf8("TTA"));

        horizontalLayout_17->addWidget(checkBox_TTA_converter);

        horizontalSpacer_20 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_17->addItem(horizontalSpacer_20);


        gridLayout_54->addLayout(horizontalLayout_17, 3, 0, 1, 2);

        line_33 = new QFrame(tab_W2xConverter);
        line_33->setObjectName("line_33");
        line_33->setFrameShape(QFrame::HLine);
        line_33->setFrameShadow(QFrame::Sunken);

        gridLayout_54->addWidget(line_33, 4, 0, 1, 2);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName("horizontalLayout_16");
        label_53 = new QLabel(tab_W2xConverter);
        label_53->setObjectName("label_53");
        sizePolicy5.setHeightForWidth(label_53->sizePolicy().hasHeightForWidth());
        label_53->setSizePolicy(sizePolicy5);

        horizontalLayout_16->addWidget(label_53);

        comboBox_TargetProcessor_converter = new QComboBox(tab_W2xConverter);
        comboBox_TargetProcessor_converter->addItem(QString::fromUtf8("auto"));
        comboBox_TargetProcessor_converter->setObjectName("comboBox_TargetProcessor_converter");
        comboBox_TargetProcessor_converter->setEnabled(true);
        comboBox_TargetProcessor_converter->setMinimumSize(QSize(180, 0));
        comboBox_TargetProcessor_converter->setCurrentText(QString::fromUtf8("auto"));

        horizontalLayout_16->addWidget(comboBox_TargetProcessor_converter);

        pushButton_DumpProcessorList_converter = new QPushButton(tab_W2xConverter);
        pushButton_DumpProcessorList_converter->setObjectName("pushButton_DumpProcessorList_converter");
        pushButton_DumpProcessorList_converter->setEnabled(true);
        sizePolicy5.setHeightForWidth(pushButton_DumpProcessorList_converter->sizePolicy().hasHeightForWidth());
        pushButton_DumpProcessorList_converter->setSizePolicy(sizePolicy5);

        horizontalLayout_16->addWidget(pushButton_DumpProcessorList_converter);

        checkBox_MultiGPU_Waifu2xConverter = new QCheckBox(tab_W2xConverter);
        checkBox_MultiGPU_Waifu2xConverter->setObjectName("checkBox_MultiGPU_Waifu2xConverter");
        sizePolicy4.setHeightForWidth(checkBox_MultiGPU_Waifu2xConverter->sizePolicy().hasHeightForWidth());
        checkBox_MultiGPU_Waifu2xConverter->setSizePolicy(sizePolicy4);

        horizontalLayout_16->addWidget(checkBox_MultiGPU_Waifu2xConverter);

        horizontalSpacer_21 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_21);


        gridLayout_54->addLayout(horizontalLayout_16, 5, 0, 1, 2);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName("horizontalLayout_18");
        label_54 = new QLabel(tab_W2xConverter);
        label_54->setObjectName("label_54");
        sizePolicy5.setHeightForWidth(label_54->sizePolicy().hasHeightForWidth());
        label_54->setSizePolicy(sizePolicy5);

        horizontalLayout_18->addWidget(label_54);

        frame_BlockSize_W2xConverter = new QFrame(tab_W2xConverter);
        frame_BlockSize_W2xConverter->setObjectName("frame_BlockSize_W2xConverter");
        sizePolicy5.setHeightForWidth(frame_BlockSize_W2xConverter->sizePolicy().hasHeightForWidth());
        frame_BlockSize_W2xConverter->setSizePolicy(sizePolicy5);
        frame_BlockSize_W2xConverter->setFrameShape(QFrame::NoFrame);
        frame_BlockSize_W2xConverter->setFrameShadow(QFrame::Raised);
        gridLayout_35 = new QGridLayout(frame_BlockSize_W2xConverter);
        gridLayout_35->setObjectName("gridLayout_35");
        gridLayout_35->setContentsMargins(0, 0, 0, 0);
        pushButton_BlockSize_Minus_W2xConverter = new QPushButton(frame_BlockSize_W2xConverter);
        pushButton_BlockSize_Minus_W2xConverter->setObjectName("pushButton_BlockSize_Minus_W2xConverter");
        pushButton_BlockSize_Minus_W2xConverter->setMaximumSize(QSize(30, 30));
        pushButton_BlockSize_Minus_W2xConverter->setText(QString::fromUtf8("-"));

        gridLayout_35->addWidget(pushButton_BlockSize_Minus_W2xConverter, 0, 0, 1, 1);

        spinBox_BlockSize_converter = new QSpinBox(frame_BlockSize_W2xConverter);
        spinBox_BlockSize_converter->setObjectName("spinBox_BlockSize_converter");
        spinBox_BlockSize_converter->setMinimum(32);
        spinBox_BlockSize_converter->setMaximum(999999999);
        spinBox_BlockSize_converter->setSingleStep(10);
        spinBox_BlockSize_converter->setValue(256);

        gridLayout_35->addWidget(spinBox_BlockSize_converter, 0, 1, 1, 1);

        pushButton_BlockSize_Add_W2xConverter = new QPushButton(frame_BlockSize_W2xConverter);
        pushButton_BlockSize_Add_W2xConverter->setObjectName("pushButton_BlockSize_Add_W2xConverter");
        pushButton_BlockSize_Add_W2xConverter->setMaximumSize(QSize(30, 30));
        pushButton_BlockSize_Add_W2xConverter->setText(QString::fromUtf8("+"));

        gridLayout_35->addWidget(pushButton_BlockSize_Add_W2xConverter, 0, 2, 1, 1);


        horizontalLayout_18->addWidget(frame_BlockSize_W2xConverter);

        horizontalSpacer_19 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_18->addItem(horizontalSpacer_19);


        gridLayout_54->addLayout(horizontalLayout_18, 1, 0, 1, 2);

        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setObjectName("horizontalLayout_25");
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter = new QGroupBox(tab_W2xConverter);
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setObjectName("groupBox_GPUSettings_MultiGPU_Waifu2xConverter");
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setEnabled(false);
        sizePolicy4.setHeightForWidth(groupBox_GPUSettings_MultiGPU_Waifu2xConverter->sizePolicy().hasHeightForWidth());
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setSizePolicy(sizePolicy4);
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setMaximumSize(QSize(16777215, 16777215));
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setAlignment(Qt::AlignCenter);
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setFlat(true);
        gridLayout_44 = new QGridLayout(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        gridLayout_44->setObjectName("gridLayout_44");
        gridLayout_44->setContentsMargins(6, 6, 6, 6);
        pushButton_ShowMultiGPUSettings_Waifu2xConverter = new QPushButton(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        pushButton_ShowMultiGPUSettings_Waifu2xConverter->setObjectName("pushButton_ShowMultiGPUSettings_Waifu2xConverter");

        gridLayout_44->addWidget(pushButton_ShowMultiGPUSettings_Waifu2xConverter, 0, 6, 1, 1);

        label_75 = new QLabel(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        label_75->setObjectName("label_75");
        sizePolicy5.setHeightForWidth(label_75->sizePolicy().hasHeightForWidth());
        label_75->setSizePolicy(sizePolicy5);
        label_75->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_44->addWidget(label_75, 0, 0, 1, 1);

        label_74 = new QLabel(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        label_74->setObjectName("label_74");
        sizePolicy5.setHeightForWidth(label_74->sizePolicy().hasHeightForWidth());
        label_74->setSizePolicy(sizePolicy5);
        label_74->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_44->addWidget(label_74, 0, 4, 1, 1);

        comboBox_GPUIDs_MultiGPU_Waifu2xConverter = new QComboBox(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        comboBox_GPUIDs_MultiGPU_Waifu2xConverter->setObjectName("comboBox_GPUIDs_MultiGPU_Waifu2xConverter");
        comboBox_GPUIDs_MultiGPU_Waifu2xConverter->setCurrentText(QString::fromUtf8(""));

        gridLayout_44->addWidget(comboBox_GPUIDs_MultiGPU_Waifu2xConverter, 0, 1, 1, 1);

        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter = new QCheckBox(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter->setObjectName("checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter");
        sizePolicy5.setHeightForWidth(checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter->sizePolicy().hasHeightForWidth());
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter->setSizePolicy(sizePolicy5);
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter->setChecked(true);

        gridLayout_44->addWidget(checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter, 0, 2, 1, 1);

        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter = new QSpinBox(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter->setObjectName("spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter");
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter->setMinimum(32);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter->setMaximum(999999999);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter->setSingleStep(10);
        spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter->setValue(256);

        gridLayout_44->addWidget(spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter, 0, 5, 1, 1);

        line_22 = new QFrame(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);
        line_22->setObjectName("line_22");
        line_22->setFrameShape(QFrame::VLine);
        line_22->setFrameShadow(QFrame::Sunken);

        gridLayout_44->addWidget(line_22, 0, 3, 1, 1);


        horizontalLayout_25->addWidget(groupBox_GPUSettings_MultiGPU_Waifu2xConverter);

        horizontalSpacer_32 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_25->addItem(horizontalSpacer_32);

        horizontalLayout_25->setStretch(0, 1);
        horizontalLayout_25->setStretch(1, 2);

        gridLayout_54->addLayout(horizontalLayout_25, 6, 0, 1, 2);

        tabWidget_Engines->addTab(tab_W2xConverter, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_W2xConverter), QString::fromUtf8("Waifu2x-converter"));
        tab_A4k = new QWidget();
        tab_A4k->setObjectName("tab_A4k");
        sizePolicy.setHeightForWidth(tab_A4k->sizePolicy().hasHeightForWidth());
        tab_A4k->setSizePolicy(sizePolicy);
        gridLayout_62 = new QGridLayout(tab_A4k);
        gridLayout_62->setObjectName("gridLayout_62");
        gridLayout_62->setContentsMargins(6, 6, 6, 6);
        frame_21 = new QFrame(tab_A4k);
        frame_21->setObjectName("frame_21");
        sizePolicy4.setHeightForWidth(frame_21->sizePolicy().hasHeightForWidth());
        frame_21->setSizePolicy(sizePolicy4);
        frame_21->setFrameShape(QFrame::StyledPanel);
        frame_21->setFrameShadow(QFrame::Raised);
        gridLayout_61 = new QGridLayout(frame_21);
        gridLayout_61->setObjectName("gridLayout_61");
        gridLayout_61->setContentsMargins(0, 0, 0, 0);
        checkBox_OpenCLParallelIO_A4k = new QCheckBox(frame_21);
        checkBox_OpenCLParallelIO_A4k->setObjectName("checkBox_OpenCLParallelIO_A4k");
        checkBox_OpenCLParallelIO_A4k->setEnabled(false);
        sizePolicy4.setHeightForWidth(checkBox_OpenCLParallelIO_A4k->sizePolicy().hasHeightForWidth());
        checkBox_OpenCLParallelIO_A4k->setSizePolicy(sizePolicy4);

        gridLayout_61->addWidget(checkBox_OpenCLParallelIO_A4k, 0, 6, 1, 1);

        checkBox_ACNet_Anime4K = new QCheckBox(frame_21);
        checkBox_ACNet_Anime4K->setObjectName("checkBox_ACNet_Anime4K");
        checkBox_ACNet_Anime4K->setEnabled(true);
        sizePolicy4.setHeightForWidth(checkBox_ACNet_Anime4K->sizePolicy().hasHeightForWidth());
        checkBox_ACNet_Anime4K->setSizePolicy(sizePolicy4);
        checkBox_ACNet_Anime4K->setText(QString::fromUtf8("ACNet"));

        gridLayout_61->addWidget(checkBox_ACNet_Anime4K, 0, 0, 1, 1);

        checkBox_HDNMode_Anime4k = new QCheckBox(frame_21);
        checkBox_HDNMode_Anime4k->setObjectName("checkBox_HDNMode_Anime4k");
        checkBox_HDNMode_Anime4k->setEnabled(false);
        sizePolicy4.setHeightForWidth(checkBox_HDNMode_Anime4k->sizePolicy().hasHeightForWidth());
        checkBox_HDNMode_Anime4k->setSizePolicy(sizePolicy4);
        checkBox_HDNMode_Anime4k->setChecked(true);

        gridLayout_61->addWidget(checkBox_HDNMode_Anime4k, 0, 1, 1, 1);

        checkBox_FastMode_Anime4K = new QCheckBox(frame_21);
        checkBox_FastMode_Anime4K->setObjectName("checkBox_FastMode_Anime4K");
        sizePolicy4.setHeightForWidth(checkBox_FastMode_Anime4K->sizePolicy().hasHeightForWidth());
        checkBox_FastMode_Anime4K->setSizePolicy(sizePolicy4);

        gridLayout_61->addWidget(checkBox_FastMode_Anime4K, 0, 2, 1, 1);

        label_23 = new QLabel(frame_21);
        label_23->setObjectName("label_23");

        gridLayout_61->addWidget(label_23, 0, 4, 1, 1);

        spinBox_OpenCLCommandQueues_A4k = new QSpinBox(frame_21);
        spinBox_OpenCLCommandQueues_A4k->setObjectName("spinBox_OpenCLCommandQueues_A4k");
        spinBox_OpenCLCommandQueues_A4k->setEnabled(false);
        sizePolicy4.setHeightForWidth(spinBox_OpenCLCommandQueues_A4k->sizePolicy().hasHeightForWidth());
        spinBox_OpenCLCommandQueues_A4k->setSizePolicy(sizePolicy4);
        spinBox_OpenCLCommandQueues_A4k->setMinimum(1);
        spinBox_OpenCLCommandQueues_A4k->setMaximum(999999999);

        gridLayout_61->addWidget(spinBox_OpenCLCommandQueues_A4k, 0, 5, 1, 1);

        line_26 = new QFrame(frame_21);
        line_26->setObjectName("line_26");
        line_26->setFrameShape(QFrame::VLine);
        line_26->setFrameShadow(QFrame::Sunken);

        gridLayout_61->addWidget(line_26, 0, 3, 1, 1);

        horizontalSpacer_46 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_61->addItem(horizontalSpacer_46, 0, 7, 1, 1);


        gridLayout_62->addWidget(frame_21, 0, 0, 1, 1);

        line_5 = new QFrame(tab_A4k);
        line_5->setObjectName("line_5");
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);

        gridLayout_62->addWidget(line_5, 1, 0, 1, 1);

        frame_20 = new QFrame(tab_A4k);
        frame_20->setObjectName("frame_20");
        sizePolicy4.setHeightForWidth(frame_20->sizePolicy().hasHeightForWidth());
        frame_20->setSizePolicy(sizePolicy4);
        frame_20->setFrameShape(QFrame::NoFrame);
        frame_20->setFrameShadow(QFrame::Sunken);
        gridLayout_5 = new QGridLayout(frame_20);
        gridLayout_5->setObjectName("gridLayout_5");
        gridLayout_5->setContentsMargins(0, 0, 0, 0);
        lineEdit_GPUs_Anime4k = new QLineEdit(frame_20);
        lineEdit_GPUs_Anime4k->setObjectName("lineEdit_GPUs_Anime4k");
        lineEdit_GPUs_Anime4k->setEnabled(false);
        sizePolicy4.setHeightForWidth(lineEdit_GPUs_Anime4k->sizePolicy().hasHeightForWidth());
        lineEdit_GPUs_Anime4k->setSizePolicy(sizePolicy4);
        lineEdit_GPUs_Anime4k->setMinimumSize(QSize(230, 0));
        lineEdit_GPUs_Anime4k->setText(QString::fromUtf8("0,0:"));
        lineEdit_GPUs_Anime4k->setClearButtonEnabled(true);

        gridLayout_5->addWidget(lineEdit_GPUs_Anime4k, 0, 4, 1, 1);

        pushButton_ListGPUs_Anime4k = new QPushButton(frame_20);
        pushButton_ListGPUs_Anime4k->setObjectName("pushButton_ListGPUs_Anime4k");
        pushButton_ListGPUs_Anime4k->setEnabled(false);
        sizePolicy5.setHeightForWidth(pushButton_ListGPUs_Anime4k->sizePolicy().hasHeightForWidth());
        pushButton_ListGPUs_Anime4k->setSizePolicy(sizePolicy5);
        pushButton_ListGPUs_Anime4k->setMaximumSize(QSize(16777215, 35));

        gridLayout_5->addWidget(pushButton_ListGPUs_Anime4k, 0, 6, 1, 1);

        comboBox_GPGPUModel_A4k = new QComboBox(frame_20);
        comboBox_GPGPUModel_A4k->addItem(QString::fromUtf8("OpenCL"));
        comboBox_GPGPUModel_A4k->addItem(QString::fromUtf8("CUDA"));
        comboBox_GPGPUModel_A4k->setObjectName("comboBox_GPGPUModel_A4k");
        comboBox_GPGPUModel_A4k->setEnabled(false);
        sizePolicy5.setHeightForWidth(comboBox_GPGPUModel_A4k->sizePolicy().hasHeightForWidth());
        comboBox_GPGPUModel_A4k->setSizePolicy(sizePolicy5);
        comboBox_GPGPUModel_A4k->setMinimumSize(QSize(130, 0));

        gridLayout_5->addWidget(comboBox_GPGPUModel_A4k, 0, 1, 1, 1);

        line_13 = new QFrame(frame_20);
        line_13->setObjectName("line_13");
        line_13->setFrameShape(QFrame::VLine);
        line_13->setFrameShadow(QFrame::Sunken);

        gridLayout_5->addWidget(line_13, 0, 2, 1, 1);

        checkBox_GPUMode_Anime4K = new QCheckBox(frame_20);
        checkBox_GPUMode_Anime4K->setObjectName("checkBox_GPUMode_Anime4K");
        sizePolicy5.setHeightForWidth(checkBox_GPUMode_Anime4K->sizePolicy().hasHeightForWidth());
        checkBox_GPUMode_Anime4K->setSizePolicy(sizePolicy5);

        gridLayout_5->addWidget(checkBox_GPUMode_Anime4K, 0, 0, 1, 1);

        pushButton_VerifyGPUsConfig_Anime4k = new QPushButton(frame_20);
        pushButton_VerifyGPUsConfig_Anime4k->setObjectName("pushButton_VerifyGPUsConfig_Anime4k");
        pushButton_VerifyGPUsConfig_Anime4k->setEnabled(false);
        sizePolicy5.setHeightForWidth(pushButton_VerifyGPUsConfig_Anime4k->sizePolicy().hasHeightForWidth());
        pushButton_VerifyGPUsConfig_Anime4k->setSizePolicy(sizePolicy5);

        gridLayout_5->addWidget(pushButton_VerifyGPUsConfig_Anime4k, 0, 5, 1, 1);

        checkBox_SpecifyGPU_Anime4k = new QCheckBox(frame_20);
        checkBox_SpecifyGPU_Anime4k->setObjectName("checkBox_SpecifyGPU_Anime4k");
        checkBox_SpecifyGPU_Anime4k->setEnabled(false);
        sizePolicy5.setHeightForWidth(checkBox_SpecifyGPU_Anime4k->sizePolicy().hasHeightForWidth());
        checkBox_SpecifyGPU_Anime4k->setSizePolicy(sizePolicy5);

        gridLayout_5->addWidget(checkBox_SpecifyGPU_Anime4k, 0, 3, 1, 1);

        horizontalSpacer_45 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_45, 0, 7, 1, 1);


        gridLayout_62->addWidget(frame_20, 2, 0, 1, 1);

        line_14 = new QFrame(tab_A4k);
        line_14->setObjectName("line_14");
        line_14->setFrameShape(QFrame::HLine);
        line_14->setFrameShadow(QFrame::Sunken);

        gridLayout_62->addWidget(line_14, 3, 0, 1, 1);

        widget = new QWidget(tab_A4k);
        widget->setObjectName("widget");
        sizePolicy4.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy4);
        widget->setMaximumSize(QSize(16777215, 42));
        gridLayout_38 = new QGridLayout(widget);
        gridLayout_38->setObjectName("gridLayout_38");
        gridLayout_38->setContentsMargins(0, 0, 0, 0);
        label_50 = new QLabel(widget);
        label_50->setObjectName("label_50");
        sizePolicy3.setHeightForWidth(label_50->sizePolicy().hasHeightForWidth());
        label_50->setSizePolicy(sizePolicy3);

        gridLayout_38->addWidget(label_50, 0, 4, 1, 1);

        doubleSpinBox_PushGradientStrength_Anime4K = new QDoubleSpinBox(widget);
        doubleSpinBox_PushGradientStrength_Anime4K->setObjectName("doubleSpinBox_PushGradientStrength_Anime4K");
        sizePolicy4.setHeightForWidth(doubleSpinBox_PushGradientStrength_Anime4K->sizePolicy().hasHeightForWidth());
        doubleSpinBox_PushGradientStrength_Anime4K->setSizePolicy(sizePolicy4);
        doubleSpinBox_PushGradientStrength_Anime4K->setMinimumSize(QSize(100, 0));
        doubleSpinBox_PushGradientStrength_Anime4K->setMaximum(1.000000000000000);
        doubleSpinBox_PushGradientStrength_Anime4K->setSingleStep(0.100000000000000);
        doubleSpinBox_PushGradientStrength_Anime4K->setValue(1.000000000000000);

        gridLayout_38->addWidget(doubleSpinBox_PushGradientStrength_Anime4K, 0, 7, 1, 1);

        spinBox_PushColorCount_Anime4K = new QSpinBox(widget);
        spinBox_PushColorCount_Anime4K->setObjectName("spinBox_PushColorCount_Anime4K");
        sizePolicy4.setHeightForWidth(spinBox_PushColorCount_Anime4K->sizePolicy().hasHeightForWidth());
        spinBox_PushColorCount_Anime4K->setSizePolicy(sizePolicy4);
        spinBox_PushColorCount_Anime4K->setMinimum(1);
        spinBox_PushColorCount_Anime4K->setMaximum(999999999);
        spinBox_PushColorCount_Anime4K->setValue(2);

        gridLayout_38->addWidget(spinBox_PushColorCount_Anime4K, 0, 3, 1, 1);

        doubleSpinBox_PushColorStrength_Anime4K = new QDoubleSpinBox(widget);
        doubleSpinBox_PushColorStrength_Anime4K->setObjectName("doubleSpinBox_PushColorStrength_Anime4K");
        sizePolicy4.setHeightForWidth(doubleSpinBox_PushColorStrength_Anime4K->sizePolicy().hasHeightForWidth());
        doubleSpinBox_PushColorStrength_Anime4K->setSizePolicy(sizePolicy4);
        doubleSpinBox_PushColorStrength_Anime4K->setMinimumSize(QSize(100, 0));
        doubleSpinBox_PushColorStrength_Anime4K->setMaximum(1.000000000000000);
        doubleSpinBox_PushColorStrength_Anime4K->setSingleStep(0.100000000000000);
        doubleSpinBox_PushColorStrength_Anime4K->setValue(0.300000000000000);

        gridLayout_38->addWidget(doubleSpinBox_PushColorStrength_Anime4K, 0, 5, 1, 1);

        label_48 = new QLabel(widget);
        label_48->setObjectName("label_48");
        sizePolicy5.setHeightForWidth(label_48->sizePolicy().hasHeightForWidth());
        label_48->setSizePolicy(sizePolicy5);

        gridLayout_38->addWidget(label_48, 0, 0, 1, 1);

        spinBox_Passes_Anime4K = new QSpinBox(widget);
        spinBox_Passes_Anime4K->setObjectName("spinBox_Passes_Anime4K");
        sizePolicy4.setHeightForWidth(spinBox_Passes_Anime4K->sizePolicy().hasHeightForWidth());
        spinBox_Passes_Anime4K->setSizePolicy(sizePolicy4);
        spinBox_Passes_Anime4K->setMinimum(1);
        spinBox_Passes_Anime4K->setMaximum(999999999);
        spinBox_Passes_Anime4K->setValue(2);

        gridLayout_38->addWidget(spinBox_Passes_Anime4K, 0, 1, 1, 1);

        label_51 = new QLabel(widget);
        label_51->setObjectName("label_51");
        sizePolicy3.setHeightForWidth(label_51->sizePolicy().hasHeightForWidth());
        label_51->setSizePolicy(sizePolicy3);

        gridLayout_38->addWidget(label_51, 0, 6, 1, 1);

        label_49 = new QLabel(widget);
        label_49->setObjectName("label_49");
        sizePolicy3.setHeightForWidth(label_49->sizePolicy().hasHeightForWidth());
        label_49->setSizePolicy(sizePolicy3);

        gridLayout_38->addWidget(label_49, 0, 2, 1, 1);

        horizontalSpacer_47 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_38->addItem(horizontalSpacer_47, 0, 8, 1, 1);


        gridLayout_62->addWidget(widget, 4, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        groupBox_PreProcessing_Anime4k = new QGroupBox(tab_A4k);
        groupBox_PreProcessing_Anime4k->setObjectName("groupBox_PreProcessing_Anime4k");
        groupBox_PreProcessing_Anime4k->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_25 = new QGridLayout(groupBox_PreProcessing_Anime4k);
        gridLayout_25->setObjectName("gridLayout_25");
        gridLayout_25->setContentsMargins(6, 6, 6, 6);
        line_8 = new QFrame(groupBox_PreProcessing_Anime4k);
        line_8->setObjectName("line_8");
        line_8->setFrameShape(QFrame::HLine);
        line_8->setFrameShadow(QFrame::Sunken);

        gridLayout_25->addWidget(line_8, 1, 0, 1, 4);

        checkBox_MeanBlur_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_MeanBlur_Pre_Anime4k->setObjectName("checkBox_MeanBlur_Pre_Anime4k");

        gridLayout_25->addWidget(checkBox_MeanBlur_Pre_Anime4k, 2, 1, 1, 1);

        checkBox_MedianBlur_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_MedianBlur_Pre_Anime4k->setObjectName("checkBox_MedianBlur_Pre_Anime4k");

        gridLayout_25->addWidget(checkBox_MedianBlur_Pre_Anime4k, 2, 0, 1, 1);

        checkBox_GaussianBlurWeak_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_GaussianBlurWeak_Pre_Anime4k->setObjectName("checkBox_GaussianBlurWeak_Pre_Anime4k");

        gridLayout_25->addWidget(checkBox_GaussianBlurWeak_Pre_Anime4k, 3, 1, 1, 3);

        checkBox_CASSharping_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_CASSharping_Pre_Anime4k->setObjectName("checkBox_CASSharping_Pre_Anime4k");
        checkBox_CASSharping_Pre_Anime4k->setChecked(true);

        gridLayout_25->addWidget(checkBox_CASSharping_Pre_Anime4k, 2, 2, 1, 2);

        checkBox_GaussianBlur_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_GaussianBlur_Pre_Anime4k->setObjectName("checkBox_GaussianBlur_Pre_Anime4k");

        gridLayout_25->addWidget(checkBox_GaussianBlur_Pre_Anime4k, 3, 0, 1, 1);

        checkBox_BilateralFilter_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_BilateralFilter_Pre_Anime4k->setObjectName("checkBox_BilateralFilter_Pre_Anime4k");

        gridLayout_25->addWidget(checkBox_BilateralFilter_Pre_Anime4k, 4, 0, 1, 1);

        checkBox_BilateralFilterFaster_Pre_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_BilateralFilterFaster_Pre_Anime4k->setObjectName("checkBox_BilateralFilterFaster_Pre_Anime4k");

        gridLayout_25->addWidget(checkBox_BilateralFilterFaster_Pre_Anime4k, 4, 1, 1, 3);

        checkBox_EnablePreProcessing_Anime4k = new QCheckBox(groupBox_PreProcessing_Anime4k);
        checkBox_EnablePreProcessing_Anime4k->setObjectName("checkBox_EnablePreProcessing_Anime4k");
        checkBox_EnablePreProcessing_Anime4k->setChecked(true);

        gridLayout_25->addWidget(checkBox_EnablePreProcessing_Anime4k, 0, 0, 1, 3);


        horizontalLayout_2->addWidget(groupBox_PreProcessing_Anime4k);

        groupBox_PostProcessing_Anime4k = new QGroupBox(tab_A4k);
        groupBox_PostProcessing_Anime4k->setObjectName("groupBox_PostProcessing_Anime4k");
        gridLayout_24 = new QGridLayout(groupBox_PostProcessing_Anime4k);
        gridLayout_24->setObjectName("gridLayout_24");
        gridLayout_24->setContentsMargins(6, 6, 6, 6);
        checkBox_EnablePostProcessing_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_EnablePostProcessing_Anime4k->setObjectName("checkBox_EnablePostProcessing_Anime4k");
        checkBox_EnablePostProcessing_Anime4k->setChecked(true);

        gridLayout_24->addWidget(checkBox_EnablePostProcessing_Anime4k, 0, 0, 1, 3);

        line_9 = new QFrame(groupBox_PostProcessing_Anime4k);
        line_9->setObjectName("line_9");
        line_9->setFrameShape(QFrame::HLine);
        line_9->setFrameShadow(QFrame::Sunken);

        gridLayout_24->addWidget(line_9, 1, 0, 1, 4);

        checkBox_MedianBlur_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_MedianBlur_Post_Anime4k->setObjectName("checkBox_MedianBlur_Post_Anime4k");

        gridLayout_24->addWidget(checkBox_MedianBlur_Post_Anime4k, 2, 0, 1, 1);

        checkBox_MeanBlur_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_MeanBlur_Post_Anime4k->setObjectName("checkBox_MeanBlur_Post_Anime4k");

        gridLayout_24->addWidget(checkBox_MeanBlur_Post_Anime4k, 2, 1, 1, 1);

        checkBox_CASSharping_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_CASSharping_Post_Anime4k->setObjectName("checkBox_CASSharping_Post_Anime4k");
        checkBox_CASSharping_Post_Anime4k->setChecked(false);

        gridLayout_24->addWidget(checkBox_CASSharping_Post_Anime4k, 2, 2, 1, 2);

        checkBox_GaussianBlur_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_GaussianBlur_Post_Anime4k->setObjectName("checkBox_GaussianBlur_Post_Anime4k");

        gridLayout_24->addWidget(checkBox_GaussianBlur_Post_Anime4k, 3, 0, 1, 1);

        checkBox_BilateralFilter_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_BilateralFilter_Post_Anime4k->setObjectName("checkBox_BilateralFilter_Post_Anime4k");
        checkBox_BilateralFilter_Post_Anime4k->setChecked(true);

        gridLayout_24->addWidget(checkBox_BilateralFilter_Post_Anime4k, 4, 0, 1, 1);

        checkBox_GaussianBlurWeak_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_GaussianBlurWeak_Post_Anime4k->setObjectName("checkBox_GaussianBlurWeak_Post_Anime4k");
        checkBox_GaussianBlurWeak_Post_Anime4k->setChecked(true);

        gridLayout_24->addWidget(checkBox_GaussianBlurWeak_Post_Anime4k, 3, 1, 1, 3);

        checkBox_BilateralFilterFaster_Post_Anime4k = new QCheckBox(groupBox_PostProcessing_Anime4k);
        checkBox_BilateralFilterFaster_Post_Anime4k->setObjectName("checkBox_BilateralFilterFaster_Post_Anime4k");

        gridLayout_24->addWidget(checkBox_BilateralFilterFaster_Post_Anime4k, 4, 1, 1, 3);


        horizontalLayout_2->addWidget(groupBox_PostProcessing_Anime4k);


        gridLayout_62->addLayout(horizontalLayout_2, 5, 0, 1, 1);

        tabWidget_Engines->addTab(tab_A4k, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_A4k), QString::fromUtf8("Anime4K"));
        tab_SrmdNcnnVulkan = new QWidget();
        tab_SrmdNcnnVulkan->setObjectName("tab_SrmdNcnnVulkan");
        gridLayout_55 = new QGridLayout(tab_SrmdNcnnVulkan);
        gridLayout_55->setObjectName("gridLayout_55");
        gridLayout_55->setVerticalSpacing(7);
        gridLayout_55->setContentsMargins(6, 6, 6, 6);
        horizontalLayout_26 = new QHBoxLayout();
        horizontalLayout_26->setObjectName("horizontalLayout_26");
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan = new QGroupBox(tab_SrmdNcnnVulkan);
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setObjectName("groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan");
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setEnabled(false);
        sizePolicy4.setHeightForWidth(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->sizePolicy().hasHeightForWidth());
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setSizePolicy(sizePolicy4);
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setMaximumSize(QSize(16777215, 16777215));
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setAlignment(Qt::AlignCenter);
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setFlat(true);
        gridLayout_41 = new QGridLayout(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        gridLayout_41->setObjectName("gridLayout_41");
        gridLayout_41->setContentsMargins(6, 6, 6, 6);
        checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan = new QCheckBox(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setObjectName("checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan");
        sizePolicy5.setHeightForWidth(checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan->sizePolicy().hasHeightForWidth());
        checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setSizePolicy(sizePolicy5);
        checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setChecked(true);

        gridLayout_41->addWidget(checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan, 0, 2, 1, 1);

        label_68 = new QLabel(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        label_68->setObjectName("label_68");
        sizePolicy3.setHeightForWidth(label_68->sizePolicy().hasHeightForWidth());
        label_68->setSizePolicy(sizePolicy3);
        label_68->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_41->addWidget(label_68, 0, 4, 1, 1);

        comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan = new QComboBox(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan->setObjectName("comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan");
        comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan->setCurrentText(QString::fromUtf8(""));

        gridLayout_41->addWidget(comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan, 0, 1, 1, 1);

        pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan = new QPushButton(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan->setObjectName("pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan");

        gridLayout_41->addWidget(pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan, 0, 6, 1, 1);

        spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan = new QSpinBox(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setObjectName("spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan");
        spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setMinimum(32);
        spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setMaximum(999999999);
        spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setSingleStep(10);
        spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setValue(128);

        gridLayout_41->addWidget(spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan, 0, 5, 1, 1);

        label_69 = new QLabel(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        label_69->setObjectName("label_69");
        sizePolicy3.setHeightForWidth(label_69->sizePolicy().hasHeightForWidth());
        label_69->setSizePolicy(sizePolicy3);
        label_69->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_41->addWidget(label_69, 0, 0, 1, 1);

        line_23 = new QFrame(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);
        line_23->setObjectName("line_23");
        line_23->setFrameShape(QFrame::VLine);
        line_23->setFrameShadow(QFrame::Sunken);

        gridLayout_41->addWidget(line_23, 0, 3, 1, 1);


        horizontalLayout_26->addWidget(groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan);

        horizontalSpacer_23 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_26->addItem(horizontalSpacer_23);

        horizontalLayout_26->setStretch(0, 1);
        horizontalLayout_26->setStretch(1, 2);

        gridLayout_55->addLayout(horizontalLayout_26, 3, 0, 1, 4);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName("horizontalLayout_19");
        label_41 = new QLabel(tab_SrmdNcnnVulkan);
        label_41->setObjectName("label_41");
        sizePolicy5.setHeightForWidth(label_41->sizePolicy().hasHeightForWidth());
        label_41->setSizePolicy(sizePolicy5);

        horizontalLayout_19->addWidget(label_41);

        comboBox_GPUID_srmd = new QComboBox(tab_SrmdNcnnVulkan);
        comboBox_GPUID_srmd->addItem(QString::fromUtf8("auto"));
        comboBox_GPUID_srmd->setObjectName("comboBox_GPUID_srmd");
        comboBox_GPUID_srmd->setMinimumSize(QSize(180, 0));
        comboBox_GPUID_srmd->setCurrentText(QString::fromUtf8("auto"));

        horizontalLayout_19->addWidget(comboBox_GPUID_srmd);

        pushButton_DetectGPUID_srmd = new QPushButton(tab_SrmdNcnnVulkan);
        pushButton_DetectGPUID_srmd->setObjectName("pushButton_DetectGPUID_srmd");
        sizePolicy5.setHeightForWidth(pushButton_DetectGPUID_srmd->sizePolicy().hasHeightForWidth());
        pushButton_DetectGPUID_srmd->setSizePolicy(sizePolicy5);

        horizontalLayout_19->addWidget(pushButton_DetectGPUID_srmd);

        checkBox_MultiGPU_SrmdNCNNVulkan = new QCheckBox(tab_SrmdNcnnVulkan);
        checkBox_MultiGPU_SrmdNCNNVulkan->setObjectName("checkBox_MultiGPU_SrmdNCNNVulkan");
        sizePolicy5.setHeightForWidth(checkBox_MultiGPU_SrmdNCNNVulkan->sizePolicy().hasHeightForWidth());
        checkBox_MultiGPU_SrmdNCNNVulkan->setSizePolicy(sizePolicy5);

        horizontalLayout_19->addWidget(checkBox_MultiGPU_SrmdNCNNVulkan);

        horizontalSpacer_34 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_19->addItem(horizontalSpacer_34);


        gridLayout_55->addLayout(horizontalLayout_19, 2, 0, 1, 4);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName("horizontalLayout_20");
        label_40 = new QLabel(tab_SrmdNcnnVulkan);
        label_40->setObjectName("label_40");
        sizePolicy5.setHeightForWidth(label_40->sizePolicy().hasHeightForWidth());
        label_40->setSizePolicy(sizePolicy5);

        horizontalLayout_20->addWidget(label_40);

        frame_TileSize_SrmdNcnnVulkan = new QFrame(tab_SrmdNcnnVulkan);
        frame_TileSize_SrmdNcnnVulkan->setObjectName("frame_TileSize_SrmdNcnnVulkan");
        sizePolicy5.setHeightForWidth(frame_TileSize_SrmdNcnnVulkan->sizePolicy().hasHeightForWidth());
        frame_TileSize_SrmdNcnnVulkan->setSizePolicy(sizePolicy5);
        frame_TileSize_SrmdNcnnVulkan->setFrameShape(QFrame::NoFrame);
        frame_TileSize_SrmdNcnnVulkan->setFrameShadow(QFrame::Raised);
        gridLayout_36 = new QGridLayout(frame_TileSize_SrmdNcnnVulkan);
        gridLayout_36->setObjectName("gridLayout_36");
        gridLayout_36->setContentsMargins(0, 0, 0, 0);
        pushButton_Add_TileSize_SrmdNCNNVulkan = new QPushButton(frame_TileSize_SrmdNcnnVulkan);
        pushButton_Add_TileSize_SrmdNCNNVulkan->setObjectName("pushButton_Add_TileSize_SrmdNCNNVulkan");
        pushButton_Add_TileSize_SrmdNCNNVulkan->setMaximumSize(QSize(30, 30));
        pushButton_Add_TileSize_SrmdNCNNVulkan->setText(QString::fromUtf8("+"));

        gridLayout_36->addWidget(pushButton_Add_TileSize_SrmdNCNNVulkan, 0, 2, 1, 1);

        pushButton_Minus_TileSize_SrmdNCNNVulkan = new QPushButton(frame_TileSize_SrmdNcnnVulkan);
        pushButton_Minus_TileSize_SrmdNCNNVulkan->setObjectName("pushButton_Minus_TileSize_SrmdNCNNVulkan");
        pushButton_Minus_TileSize_SrmdNCNNVulkan->setMaximumSize(QSize(30, 30));
        pushButton_Minus_TileSize_SrmdNCNNVulkan->setText(QString::fromUtf8("-"));

        gridLayout_36->addWidget(pushButton_Minus_TileSize_SrmdNCNNVulkan, 0, 0, 1, 1);

        spinBox_TileSize_srmd = new QSpinBox(frame_TileSize_SrmdNcnnVulkan);
        spinBox_TileSize_srmd->setObjectName("spinBox_TileSize_srmd");
        sizePolicy5.setHeightForWidth(spinBox_TileSize_srmd->sizePolicy().hasHeightForWidth());
        spinBox_TileSize_srmd->setSizePolicy(sizePolicy5);
        spinBox_TileSize_srmd->setMinimum(32);
        spinBox_TileSize_srmd->setMaximum(999999999);
        spinBox_TileSize_srmd->setSingleStep(10);
        spinBox_TileSize_srmd->setValue(128);

        gridLayout_36->addWidget(spinBox_TileSize_srmd, 0, 1, 1, 1);


        horizontalLayout_20->addWidget(frame_TileSize_SrmdNcnnVulkan);

        checkBox_TTA_srmd = new QCheckBox(tab_SrmdNcnnVulkan);
        checkBox_TTA_srmd->setObjectName("checkBox_TTA_srmd");
        sizePolicy4.setHeightForWidth(checkBox_TTA_srmd->sizePolicy().hasHeightForWidth());
        checkBox_TTA_srmd->setSizePolicy(sizePolicy4);
        checkBox_TTA_srmd->setText(QString::fromUtf8("TTA"));

        horizontalLayout_20->addWidget(checkBox_TTA_srmd);

        horizontalSpacer_33 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_33);


        gridLayout_55->addLayout(horizontalLayout_20, 0, 0, 1, 4);

        line_37 = new QFrame(tab_SrmdNcnnVulkan);
        line_37->setObjectName("line_37");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(line_37->sizePolicy().hasHeightForWidth());
        line_37->setSizePolicy(sizePolicy6);
        line_37->setFrameShape(QFrame::HLine);
        line_37->setFrameShadow(QFrame::Sunken);

        gridLayout_55->addWidget(line_37, 1, 0, 1, 4);

        tabWidget_Engines->addTab(tab_SrmdNcnnVulkan, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_SrmdNcnnVulkan), QString::fromUtf8("SRMD-ncnn-vulkan"));
        tab_W2xCaffe = new QWidget();
        tab_W2xCaffe->setObjectName("tab_W2xCaffe");
        gridLayout_30 = new QGridLayout(tab_W2xCaffe);
        gridLayout_30->setObjectName("gridLayout_30");
        horizontalLayout_29 = new QHBoxLayout();
        horizontalLayout_29->setObjectName("horizontalLayout_29");
        label_56 = new QLabel(tab_W2xCaffe);
        label_56->setObjectName("label_56");
        sizePolicy5.setHeightForWidth(label_56->sizePolicy().hasHeightForWidth());
        label_56->setSizePolicy(sizePolicy5);

        horizontalLayout_29->addWidget(label_56);

        comboBox_Model_3D_Waifu2xCaffe = new QComboBox(tab_W2xCaffe);
        comboBox_Model_3D_Waifu2xCaffe->addItem(QString());
        comboBox_Model_3D_Waifu2xCaffe->addItem(QString());
        comboBox_Model_3D_Waifu2xCaffe->setObjectName("comboBox_Model_3D_Waifu2xCaffe");
        comboBox_Model_3D_Waifu2xCaffe->setMinimumSize(QSize(220, 0));
        comboBox_Model_3D_Waifu2xCaffe->setCurrentText(QString::fromUtf8("upconv_7_photo"));

        horizontalLayout_29->addWidget(comboBox_Model_3D_Waifu2xCaffe);

        horizontalSpacer_38 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_29->addItem(horizontalSpacer_38);


        gridLayout_30->addLayout(horizontalLayout_29, 4, 0, 1, 1);

        horizontalLayout_31 = new QHBoxLayout();
        horizontalLayout_31->setObjectName("horizontalLayout_31");
        label_43 = new QLabel(tab_W2xCaffe);
        label_43->setObjectName("label_43");
        sizePolicy5.setHeightForWidth(label_43->sizePolicy().hasHeightForWidth());
        label_43->setSizePolicy(sizePolicy5);

        horizontalLayout_31->addWidget(label_43);

        spinBox_BatchSize_Waifu2xCaffe = new QSpinBox(tab_W2xCaffe);
        spinBox_BatchSize_Waifu2xCaffe->setObjectName("spinBox_BatchSize_Waifu2xCaffe");
        spinBox_BatchSize_Waifu2xCaffe->setMinimumSize(QSize(120, 0));
        spinBox_BatchSize_Waifu2xCaffe->setMinimum(1);
        spinBox_BatchSize_Waifu2xCaffe->setMaximum(999999999);

        horizontalLayout_31->addWidget(spinBox_BatchSize_Waifu2xCaffe);

        horizontalSpacer_36 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_31->addItem(horizontalSpacer_36);


        gridLayout_30->addLayout(horizontalLayout_31, 8, 0, 1, 1);

        line_41 = new QFrame(tab_W2xCaffe);
        line_41->setObjectName("line_41");
        line_41->setFrameShape(QFrame::HLine);
        line_41->setFrameShadow(QFrame::Sunken);

        gridLayout_30->addWidget(line_41, 7, 0, 1, 1);

        horizontalLayout_27 = new QHBoxLayout();
        horizontalLayout_27->setObjectName("horizontalLayout_27");
        label_14 = new QLabel(tab_W2xCaffe);
        label_14->setObjectName("label_14");
        sizePolicy5.setHeightForWidth(label_14->sizePolicy().hasHeightForWidth());
        label_14->setSizePolicy(sizePolicy5);

        horizontalLayout_27->addWidget(label_14);

        comboBox_ProcessMode_Waifu2xCaffe = new QComboBox(tab_W2xCaffe);
        comboBox_ProcessMode_Waifu2xCaffe->addItem(QString::fromUtf8("CPU"));
        comboBox_ProcessMode_Waifu2xCaffe->addItem(QString::fromUtf8("GPU"));
        comboBox_ProcessMode_Waifu2xCaffe->addItem(QString::fromUtf8("cuDNN"));
        comboBox_ProcessMode_Waifu2xCaffe->setObjectName("comboBox_ProcessMode_Waifu2xCaffe");
        comboBox_ProcessMode_Waifu2xCaffe->setMinimumSize(QSize(130, 0));
        comboBox_ProcessMode_Waifu2xCaffe->setCurrentText(QString::fromUtf8("CPU"));

        horizontalLayout_27->addWidget(comboBox_ProcessMode_Waifu2xCaffe);

        checkBox_TTA_Waifu2xCaffe = new QCheckBox(tab_W2xCaffe);
        checkBox_TTA_Waifu2xCaffe->setObjectName("checkBox_TTA_Waifu2xCaffe");
        checkBox_TTA_Waifu2xCaffe->setText(QString::fromUtf8("TTA"));

        horizontalLayout_27->addWidget(checkBox_TTA_Waifu2xCaffe);

        horizontalSpacer_40 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_27->addItem(horizontalSpacer_40);


        gridLayout_30->addLayout(horizontalLayout_27, 0, 0, 1, 1);

        horizontalLayout_33 = new QHBoxLayout();
        horizontalLayout_33->setObjectName("horizontalLayout_33");
        checkBox_EnableMultiGPU_Waifu2xCaffe = new QCheckBox(tab_W2xCaffe);
        checkBox_EnableMultiGPU_Waifu2xCaffe->setObjectName("checkBox_EnableMultiGPU_Waifu2xCaffe");
        checkBox_EnableMultiGPU_Waifu2xCaffe->setEnabled(false);
        sizePolicy5.setHeightForWidth(checkBox_EnableMultiGPU_Waifu2xCaffe->sizePolicy().hasHeightForWidth());
        checkBox_EnableMultiGPU_Waifu2xCaffe->setSizePolicy(sizePolicy5);

        horizontalLayout_33->addWidget(checkBox_EnableMultiGPU_Waifu2xCaffe);

        lineEdit_MultiGPUInfo_Waifu2xCaffe = new QLineEdit(tab_W2xCaffe);
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setObjectName("lineEdit_MultiGPUInfo_Waifu2xCaffe");
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setEnabled(false);
        sizePolicy4.setHeightForWidth(lineEdit_MultiGPUInfo_Waifu2xCaffe->sizePolicy().hasHeightForWidth());
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setSizePolicy(sizePolicy4);
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setMinimumSize(QSize(300, 0));
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setText(QString::fromUtf8("0,1,128:"));
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setClearButtonEnabled(true);

        horizontalLayout_33->addWidget(lineEdit_MultiGPUInfo_Waifu2xCaffe);

        pushButton_VerifyGPUsConfig_Waifu2xCaffe = new QPushButton(tab_W2xCaffe);
        pushButton_VerifyGPUsConfig_Waifu2xCaffe->setObjectName("pushButton_VerifyGPUsConfig_Waifu2xCaffe");
        pushButton_VerifyGPUsConfig_Waifu2xCaffe->setEnabled(false);
        sizePolicy5.setHeightForWidth(pushButton_VerifyGPUsConfig_Waifu2xCaffe->sizePolicy().hasHeightForWidth());
        pushButton_VerifyGPUsConfig_Waifu2xCaffe->setSizePolicy(sizePolicy5);

        horizontalLayout_33->addWidget(pushButton_VerifyGPUsConfig_Waifu2xCaffe);

        horizontalSpacer_35 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_33->addItem(horizontalSpacer_35);


        gridLayout_30->addLayout(horizontalLayout_33, 12, 0, 1, 1);

        line_40 = new QFrame(tab_W2xCaffe);
        line_40->setObjectName("line_40");
        line_40->setFrameShape(QFrame::HLine);
        line_40->setFrameShadow(QFrame::Sunken);

        gridLayout_30->addWidget(line_40, 5, 0, 1, 1);

        line_42 = new QFrame(tab_W2xCaffe);
        line_42->setObjectName("line_42");
        line_42->setFrameShape(QFrame::HLine);
        line_42->setFrameShadow(QFrame::Sunken);

        gridLayout_30->addWidget(line_42, 9, 0, 1, 1);

        line_39 = new QFrame(tab_W2xCaffe);
        line_39->setObjectName("line_39");
        line_39->setFrameShape(QFrame::HLine);
        line_39->setFrameShadow(QFrame::Sunken);

        gridLayout_30->addWidget(line_39, 3, 0, 1, 1);

        horizontalLayout_28 = new QHBoxLayout();
        horizontalLayout_28->setObjectName("horizontalLayout_28");
        label_25 = new QLabel(tab_W2xCaffe);
        label_25->setObjectName("label_25");
        sizePolicy5.setHeightForWidth(label_25->sizePolicy().hasHeightForWidth());
        label_25->setSizePolicy(sizePolicy5);

        horizontalLayout_28->addWidget(label_25);

        comboBox_Model_2D_Waifu2xCaffe = new QComboBox(tab_W2xCaffe);
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("anime_style_art"));
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("anime_style_art_rgb"));
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("cunet"));
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("photo"));
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("upconv_7_anime_style_art_rgb"));
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("upconv_7_photo"));
        comboBox_Model_2D_Waifu2xCaffe->addItem(QString::fromUtf8("upresnet10"));
        comboBox_Model_2D_Waifu2xCaffe->setObjectName("comboBox_Model_2D_Waifu2xCaffe");
        comboBox_Model_2D_Waifu2xCaffe->setCurrentText(QString::fromUtf8("anime_style_art_rgb"));

        horizontalLayout_28->addWidget(comboBox_Model_2D_Waifu2xCaffe);

        horizontalSpacer_39 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_28->addItem(horizontalSpacer_39);


        gridLayout_30->addLayout(horizontalLayout_28, 2, 0, 1, 1);

        horizontalLayout_30 = new QHBoxLayout();
        horizontalLayout_30->setObjectName("horizontalLayout_30");
        label_38 = new QLabel(tab_W2xCaffe);
        label_38->setObjectName("label_38");
        sizePolicy5.setHeightForWidth(label_38->sizePolicy().hasHeightForWidth());
        label_38->setSizePolicy(sizePolicy5);

        horizontalLayout_30->addWidget(label_38);

        spinBox_GPUID_Waifu2xCaffe = new QSpinBox(tab_W2xCaffe);
        spinBox_GPUID_Waifu2xCaffe->setObjectName("spinBox_GPUID_Waifu2xCaffe");
        spinBox_GPUID_Waifu2xCaffe->setMinimumSize(QSize(150, 0));
        spinBox_GPUID_Waifu2xCaffe->setMaximum(999999999);
        spinBox_GPUID_Waifu2xCaffe->setSingleStep(1);

        horizontalLayout_30->addWidget(spinBox_GPUID_Waifu2xCaffe);

        horizontalSpacer_37 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_30->addItem(horizontalSpacer_37);


        gridLayout_30->addLayout(horizontalLayout_30, 6, 0, 1, 1);

        line_38 = new QFrame(tab_W2xCaffe);
        line_38->setObjectName("line_38");
        line_38->setFrameShape(QFrame::HLine);
        line_38->setFrameShadow(QFrame::Sunken);

        gridLayout_30->addWidget(line_38, 1, 0, 1, 1);

        horizontalLayout_32 = new QHBoxLayout();
        horizontalLayout_32->setObjectName("horizontalLayout_32");
        label_52 = new QLabel(tab_W2xCaffe);
        label_52->setObjectName("label_52");
        sizePolicy5.setHeightForWidth(label_52->sizePolicy().hasHeightForWidth());
        label_52->setSizePolicy(sizePolicy5);

        horizontalLayout_32->addWidget(label_52);

        pushButton_SplitSize_Minus_Waifu2xCaffe = new QPushButton(tab_W2xCaffe);
        pushButton_SplitSize_Minus_Waifu2xCaffe->setObjectName("pushButton_SplitSize_Minus_Waifu2xCaffe");
        pushButton_SplitSize_Minus_Waifu2xCaffe->setMaximumSize(QSize(30, 16777215));
        pushButton_SplitSize_Minus_Waifu2xCaffe->setText(QString::fromUtf8("-"));

        horizontalLayout_32->addWidget(pushButton_SplitSize_Minus_Waifu2xCaffe);

        spinBox_SplitSize_Waifu2xCaffe = new QSpinBox(tab_W2xCaffe);
        spinBox_SplitSize_Waifu2xCaffe->setObjectName("spinBox_SplitSize_Waifu2xCaffe");
        sizePolicy5.setHeightForWidth(spinBox_SplitSize_Waifu2xCaffe->sizePolicy().hasHeightForWidth());
        spinBox_SplitSize_Waifu2xCaffe->setSizePolicy(sizePolicy5);
        spinBox_SplitSize_Waifu2xCaffe->setReadOnly(true);
        spinBox_SplitSize_Waifu2xCaffe->setButtonSymbols(QAbstractSpinBox::NoButtons);
        spinBox_SplitSize_Waifu2xCaffe->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
        spinBox_SplitSize_Waifu2xCaffe->setMinimum(1);
        spinBox_SplitSize_Waifu2xCaffe->setMaximum(999999999);
        spinBox_SplitSize_Waifu2xCaffe->setValue(128);

        horizontalLayout_32->addWidget(spinBox_SplitSize_Waifu2xCaffe);

        pushButton_SplitSize_Add_Waifu2xCaffe = new QPushButton(tab_W2xCaffe);
        pushButton_SplitSize_Add_Waifu2xCaffe->setObjectName("pushButton_SplitSize_Add_Waifu2xCaffe");
        pushButton_SplitSize_Add_Waifu2xCaffe->setMaximumSize(QSize(30, 16777215));
        pushButton_SplitSize_Add_Waifu2xCaffe->setText(QString::fromUtf8("+"));

        horizontalLayout_32->addWidget(pushButton_SplitSize_Add_Waifu2xCaffe);

        horizontalSpacer_24 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_32->addItem(horizontalSpacer_24);


        gridLayout_30->addLayout(horizontalLayout_32, 10, 0, 1, 1);

        line_43 = new QFrame(tab_W2xCaffe);
        line_43->setObjectName("line_43");
        line_43->setFrameShape(QFrame::HLine);
        line_43->setFrameShadow(QFrame::Sunken);

        gridLayout_30->addWidget(line_43, 11, 0, 1, 1);

        tabWidget_Engines->addTab(tab_W2xCaffe, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_W2xCaffe), QString::fromUtf8("Waifu2x-Caffe"));
        tab_RealSRNcnnVulkan = new QWidget();
        tab_RealSRNcnnVulkan->setObjectName("tab_RealSRNcnnVulkan");
        gridLayout_34 = new QGridLayout(tab_RealSRNcnnVulkan);
        gridLayout_34->setObjectName("gridLayout_34");
        gridLayout_34->setVerticalSpacing(7);
        gridLayout_34->setContentsMargins(6, 6, 6, 6);
        line_36 = new QFrame(tab_RealSRNcnnVulkan);
        line_36->setObjectName("line_36");
        line_36->setFrameShape(QFrame::HLine);
        line_36->setFrameShadow(QFrame::Sunken);

        gridLayout_34->addWidget(line_36, 4, 0, 1, 2);

        line_35 = new QFrame(tab_RealSRNcnnVulkan);
        line_35->setObjectName("line_35");
        line_35->setFrameShape(QFrame::HLine);
        line_35->setFrameShadow(QFrame::Sunken);

        gridLayout_34->addWidget(line_35, 2, 0, 1, 2);

        horizontalLayout_23 = new QHBoxLayout();
        horizontalLayout_23->setObjectName("horizontalLayout_23");
        label_60 = new QLabel(tab_RealSRNcnnVulkan);
        label_60->setObjectName("label_60");
        sizePolicy5.setHeightForWidth(label_60->sizePolicy().hasHeightForWidth());
        label_60->setSizePolicy(sizePolicy5);

        horizontalLayout_23->addWidget(label_60);

        comboBox_Model_RealsrNCNNVulkan = new QComboBox(tab_RealSRNcnnVulkan);
        comboBox_Model_RealsrNCNNVulkan->addItem(QString());
        comboBox_Model_RealsrNCNNVulkan->addItem(QString());
        comboBox_Model_RealsrNCNNVulkan->setObjectName("comboBox_Model_RealsrNCNNVulkan");

        horizontalLayout_23->addWidget(comboBox_Model_RealsrNCNNVulkan);

        checkBox_TTA_RealsrNCNNVulkan = new QCheckBox(tab_RealSRNcnnVulkan);
        checkBox_TTA_RealsrNCNNVulkan->setObjectName("checkBox_TTA_RealsrNCNNVulkan");
        checkBox_TTA_RealsrNCNNVulkan->setText(QString::fromUtf8("TTA"));

        horizontalLayout_23->addWidget(checkBox_TTA_RealsrNCNNVulkan);

        horizontalSpacer_41 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_23->addItem(horizontalSpacer_41);


        gridLayout_34->addLayout(horizontalLayout_23, 1, 0, 1, 2);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName("horizontalLayout_21");
        label_62 = new QLabel(tab_RealSRNcnnVulkan);
        label_62->setObjectName("label_62");
        sizePolicy5.setHeightForWidth(label_62->sizePolicy().hasHeightForWidth());
        label_62->setSizePolicy(sizePolicy5);

        horizontalLayout_21->addWidget(label_62);

        comboBox_GPUID_RealsrNCNNVulkan = new QComboBox(tab_RealSRNcnnVulkan);
        comboBox_GPUID_RealsrNCNNVulkan->addItem(QString::fromUtf8("auto"));
        comboBox_GPUID_RealsrNCNNVulkan->setObjectName("comboBox_GPUID_RealsrNCNNVulkan");
        comboBox_GPUID_RealsrNCNNVulkan->setEnabled(true);
        comboBox_GPUID_RealsrNCNNVulkan->setMinimumSize(QSize(180, 0));
        comboBox_GPUID_RealsrNCNNVulkan->setCurrentText(QString::fromUtf8("auto"));

        horizontalLayout_21->addWidget(comboBox_GPUID_RealsrNCNNVulkan);

        pushButton_DetectGPU_RealsrNCNNVulkan = new QPushButton(tab_RealSRNcnnVulkan);
        pushButton_DetectGPU_RealsrNCNNVulkan->setObjectName("pushButton_DetectGPU_RealsrNCNNVulkan");
        pushButton_DetectGPU_RealsrNCNNVulkan->setEnabled(true);
        sizePolicy5.setHeightForWidth(pushButton_DetectGPU_RealsrNCNNVulkan->sizePolicy().hasHeightForWidth());
        pushButton_DetectGPU_RealsrNCNNVulkan->setSizePolicy(sizePolicy5);

        horizontalLayout_21->addWidget(pushButton_DetectGPU_RealsrNCNNVulkan);

        checkBox_MultiGPU_RealSRNcnnVulkan = new QCheckBox(tab_RealSRNcnnVulkan);
        checkBox_MultiGPU_RealSRNcnnVulkan->setObjectName("checkBox_MultiGPU_RealSRNcnnVulkan");
        sizePolicy4.setHeightForWidth(checkBox_MultiGPU_RealSRNcnnVulkan->sizePolicy().hasHeightForWidth());
        checkBox_MultiGPU_RealSRNcnnVulkan->setSizePolicy(sizePolicy4);

        horizontalLayout_21->addWidget(checkBox_MultiGPU_RealSRNcnnVulkan);

        horizontalSpacer_42 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_42);


        gridLayout_34->addLayout(horizontalLayout_21, 5, 0, 1, 2);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName("horizontalLayout_22");
        label_61 = new QLabel(tab_RealSRNcnnVulkan);
        label_61->setObjectName("label_61");
        sizePolicy5.setHeightForWidth(label_61->sizePolicy().hasHeightForWidth());
        label_61->setSizePolicy(sizePolicy5);

        horizontalLayout_22->addWidget(label_61);

        frame_TileSize_RealsrNcnnVulkan = new QFrame(tab_RealSRNcnnVulkan);
        frame_TileSize_RealsrNcnnVulkan->setObjectName("frame_TileSize_RealsrNcnnVulkan");
        frame_TileSize_RealsrNcnnVulkan->setEnabled(true);
        sizePolicy5.setHeightForWidth(frame_TileSize_RealsrNcnnVulkan->sizePolicy().hasHeightForWidth());
        frame_TileSize_RealsrNcnnVulkan->setSizePolicy(sizePolicy5);
        frame_TileSize_RealsrNcnnVulkan->setMinimumSize(QSize(0, 0));
        frame_TileSize_RealsrNcnnVulkan->setFrameShape(QFrame::NoFrame);
        frame_TileSize_RealsrNcnnVulkan->setFrameShadow(QFrame::Raised);
        gridLayout_56 = new QGridLayout(frame_TileSize_RealsrNcnnVulkan);
        gridLayout_56->setObjectName("gridLayout_56");
        gridLayout_56->setContentsMargins(0, 0, 0, 0);
        pushButton_Minus_TileSize_RealsrNCNNVulkan = new QPushButton(frame_TileSize_RealsrNcnnVulkan);
        pushButton_Minus_TileSize_RealsrNCNNVulkan->setObjectName("pushButton_Minus_TileSize_RealsrNCNNVulkan");
        pushButton_Minus_TileSize_RealsrNCNNVulkan->setMaximumSize(QSize(30, 30));
        pushButton_Minus_TileSize_RealsrNCNNVulkan->setText(QString::fromUtf8("-"));

        gridLayout_56->addWidget(pushButton_Minus_TileSize_RealsrNCNNVulkan, 0, 0, 1, 1);

        spinBox_TileSize_RealsrNCNNVulkan = new QSpinBox(frame_TileSize_RealsrNcnnVulkan);
        spinBox_TileSize_RealsrNCNNVulkan->setObjectName("spinBox_TileSize_RealsrNCNNVulkan");
        spinBox_TileSize_RealsrNCNNVulkan->setMinimum(32);
        spinBox_TileSize_RealsrNCNNVulkan->setMaximum(999999999);
        spinBox_TileSize_RealsrNCNNVulkan->setSingleStep(10);
        spinBox_TileSize_RealsrNCNNVulkan->setValue(128);

        gridLayout_56->addWidget(spinBox_TileSize_RealsrNCNNVulkan, 0, 1, 1, 1);

        pushButton_Add_TileSize_RealsrNCNNVulkan = new QPushButton(frame_TileSize_RealsrNcnnVulkan);
        pushButton_Add_TileSize_RealsrNCNNVulkan->setObjectName("pushButton_Add_TileSize_RealsrNCNNVulkan");
        pushButton_Add_TileSize_RealsrNCNNVulkan->setMaximumSize(QSize(30, 30));
        pushButton_Add_TileSize_RealsrNCNNVulkan->setText(QString::fromUtf8("+"));

        gridLayout_56->addWidget(pushButton_Add_TileSize_RealsrNCNNVulkan, 0, 2, 1, 1);


        horizontalLayout_22->addWidget(frame_TileSize_RealsrNcnnVulkan);

        horizontalSpacer_26 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_22->addItem(horizontalSpacer_26);


        gridLayout_34->addLayout(horizontalLayout_22, 3, 0, 1, 2);

        horizontalLayout_34 = new QHBoxLayout();
        horizontalLayout_34->setObjectName("horizontalLayout_34");
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan = new QGroupBox(tab_RealSRNcnnVulkan);
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setObjectName("groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan");
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setEnabled(false);
        sizePolicy4.setHeightForWidth(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->sizePolicy().hasHeightForWidth());
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setSizePolicy(sizePolicy4);
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setMaximumSize(QSize(16777215, 16777215));
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setAlignment(Qt::AlignCenter);
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setFlat(true);
        gridLayout_43 = new QGridLayout(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        gridLayout_43->setObjectName("gridLayout_43");
        gridLayout_43->setContentsMargins(6, 6, 6, 6);
        checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan = new QCheckBox(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setObjectName("checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan");
        sizePolicy5.setHeightForWidth(checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->sizePolicy().hasHeightForWidth());
        checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setSizePolicy(sizePolicy5);
        checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(true);

        gridLayout_43->addWidget(checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan, 0, 2, 1, 1);

        label_73 = new QLabel(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        label_73->setObjectName("label_73");
        sizePolicy3.setHeightForWidth(label_73->sizePolicy().hasHeightForWidth());
        label_73->setSizePolicy(sizePolicy3);
        label_73->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_43->addWidget(label_73, 0, 0, 1, 1);

        label_72 = new QLabel(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        label_72->setObjectName("label_72");
        sizePolicy3.setHeightForWidth(label_72->sizePolicy().hasHeightForWidth());
        label_72->setSizePolicy(sizePolicy3);
        label_72->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_43->addWidget(label_72, 0, 4, 1, 1);

        pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan = new QPushButton(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan->setObjectName("pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan");

        gridLayout_43->addWidget(pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan, 0, 6, 1, 1);

        comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan = new QComboBox(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan->setObjectName("comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan");
        comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan->setCurrentText(QString::fromUtf8(""));

        gridLayout_43->addWidget(comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan, 0, 1, 1, 1);

        spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan = new QSpinBox(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setObjectName("spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan");
        spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setMinimum(32);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setMaximum(999999999);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setSingleStep(10);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(128);

        gridLayout_43->addWidget(spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan, 0, 5, 1, 1);

        line_24 = new QFrame(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);
        line_24->setObjectName("line_24");
        line_24->setFrameShape(QFrame::VLine);
        line_24->setFrameShadow(QFrame::Sunken);

        gridLayout_43->addWidget(line_24, 0, 3, 1, 1);


        horizontalLayout_34->addWidget(groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan);

        horizontalSpacer_25 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_34->addItem(horizontalSpacer_25);

        horizontalLayout_34->setStretch(0, 1);
        horizontalLayout_34->setStretch(1, 2);

        gridLayout_34->addLayout(horizontalLayout_34, 6, 0, 1, 2);

        tabWidget_Engines->addTab(tab_RealSRNcnnVulkan, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_RealSRNcnnVulkan), QString::fromUtf8("RealSR-ncnn-vulkan"));
        tab_RealESRGAN = new QWidget();
        tab_RealESRGAN->setObjectName("tab_RealESRGAN");
        gridLayout_RealESRGAN = new QGridLayout(tab_RealESRGAN);
        gridLayout_RealESRGAN->setObjectName("gridLayout_RealESRGAN");
        horizontalLayout_RealESRGAN_Model = new QHBoxLayout();
        horizontalLayout_RealESRGAN_Model->setObjectName("horizontalLayout_RealESRGAN_Model");
        label_Model_RealESRGAN = new QLabel(tab_RealESRGAN);
        label_Model_RealESRGAN->setObjectName("label_Model_RealESRGAN");
        sizePolicy5.setHeightForWidth(label_Model_RealESRGAN->sizePolicy().hasHeightForWidth());
        label_Model_RealESRGAN->setSizePolicy(sizePolicy5);

        horizontalLayout_RealESRGAN_Model->addWidget(label_Model_RealESRGAN);

        comboBox_Model_RealESRGAN = new QComboBox(tab_RealESRGAN);
        comboBox_Model_RealESRGAN->addItem(QString());
        comboBox_Model_RealESRGAN->addItem(QString());
        comboBox_Model_RealESRGAN->addItem(QString());
        comboBox_Model_RealESRGAN->setObjectName("comboBox_Model_RealESRGAN");

        horizontalLayout_RealESRGAN_Model->addWidget(comboBox_Model_RealESRGAN);

        checkBox_TTA_RealESRGAN = new QCheckBox(tab_RealESRGAN);
        checkBox_TTA_RealESRGAN->setObjectName("checkBox_TTA_RealESRGAN");
        checkBox_TTA_RealESRGAN->setText(QString::fromUtf8("TTA"));

        horizontalLayout_RealESRGAN_Model->addWidget(checkBox_TTA_RealESRGAN);

        horizontalSpacer_RealESRGAN_Model = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_RealESRGAN_Model->addItem(horizontalSpacer_RealESRGAN_Model);


        gridLayout_RealESRGAN->addLayout(horizontalLayout_RealESRGAN_Model, 1, 0, 1, 2);

        horizontalLayout_RealESRGAN_TileSize = new QHBoxLayout();
        horizontalLayout_RealESRGAN_TileSize->setObjectName("horizontalLayout_RealESRGAN_TileSize");
        label_TileSize_RealESRGAN = new QLabel(tab_RealESRGAN);
        label_TileSize_RealESRGAN->setObjectName("label_TileSize_RealESRGAN");
        sizePolicy5.setHeightForWidth(label_TileSize_RealESRGAN->sizePolicy().hasHeightForWidth());
        label_TileSize_RealESRGAN->setSizePolicy(sizePolicy5);

        horizontalLayout_RealESRGAN_TileSize->addWidget(label_TileSize_RealESRGAN);

        frame_TileSize_RealESRGAN = new QFrame(tab_RealESRGAN);
        frame_TileSize_RealESRGAN->setObjectName("frame_TileSize_RealESRGAN");
        frame_TileSize_RealESRGAN->setEnabled(true);
        sizePolicy5.setHeightForWidth(frame_TileSize_RealESRGAN->sizePolicy().hasHeightForWidth());
        frame_TileSize_RealESRGAN->setSizePolicy(sizePolicy5);
        frame_TileSize_RealESRGAN->setMinimumSize(QSize(0, 0));
        frame_TileSize_RealESRGAN->setFrameShape(QFrame::NoFrame);
        frame_TileSize_RealESRGAN->setFrameShadow(QFrame::Raised);
        gridLayout_TileSize_RealESRGAN = new QGridLayout(frame_TileSize_RealESRGAN);
        gridLayout_TileSize_RealESRGAN->setObjectName("gridLayout_TileSize_RealESRGAN");
        gridLayout_TileSize_RealESRGAN->setContentsMargins(0, 0, 0, 0);
        pushButton_Minus_TileSize_RealESRGAN = new QPushButton(frame_TileSize_RealESRGAN);
        pushButton_Minus_TileSize_RealESRGAN->setObjectName("pushButton_Minus_TileSize_RealESRGAN");
        pushButton_Minus_TileSize_RealESRGAN->setMaximumSize(QSize(30, 30));
        pushButton_Minus_TileSize_RealESRGAN->setText(QString::fromUtf8("-"));

        gridLayout_TileSize_RealESRGAN->addWidget(pushButton_Minus_TileSize_RealESRGAN, 0, 0, 1, 1);

        spinBox_TileSize_RealESRGAN = new QSpinBox(frame_TileSize_RealESRGAN);
        spinBox_TileSize_RealESRGAN->setObjectName("spinBox_TileSize_RealESRGAN");
        spinBox_TileSize_RealESRGAN->setMinimum(32);
        spinBox_TileSize_RealESRGAN->setMaximum(999999999);
        spinBox_TileSize_RealESRGAN->setSingleStep(10);
        spinBox_TileSize_RealESRGAN->setValue(128);

        gridLayout_TileSize_RealESRGAN->addWidget(spinBox_TileSize_RealESRGAN, 0, 1, 1, 1);

        pushButton_Add_TileSize_RealESRGAN = new QPushButton(frame_TileSize_RealESRGAN);
        pushButton_Add_TileSize_RealESRGAN->setObjectName("pushButton_Add_TileSize_RealESRGAN");
        pushButton_Add_TileSize_RealESRGAN->setMaximumSize(QSize(30, 30));
        pushButton_Add_TileSize_RealESRGAN->setText(QString::fromUtf8("+"));

        gridLayout_TileSize_RealESRGAN->addWidget(pushButton_Add_TileSize_RealESRGAN, 0, 2, 1, 1);


        horizontalLayout_RealESRGAN_TileSize->addWidget(frame_TileSize_RealESRGAN);

        horizontalSpacer_TileSize_RealESRGAN = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_RealESRGAN_TileSize->addItem(horizontalSpacer_TileSize_RealESRGAN);


        gridLayout_RealESRGAN->addLayout(horizontalLayout_RealESRGAN_TileSize, 3, 0, 1, 2);

        horizontalLayout_GPU_RealESRGAN = new QHBoxLayout();
        horizontalLayout_GPU_RealESRGAN->setObjectName("horizontalLayout_GPU_RealESRGAN");
        label_GPUID_RealESRGAN = new QLabel(tab_RealESRGAN);
        label_GPUID_RealESRGAN->setObjectName("label_GPUID_RealESRGAN");
        sizePolicy5.setHeightForWidth(label_GPUID_RealESRGAN->sizePolicy().hasHeightForWidth());
        label_GPUID_RealESRGAN->setSizePolicy(sizePolicy5);

        horizontalLayout_GPU_RealESRGAN->addWidget(label_GPUID_RealESRGAN);

        comboBox_GPUID_RealESRGAN = new QComboBox(tab_RealESRGAN);
        comboBox_GPUID_RealESRGAN->addItem(QString::fromUtf8("auto"));
        comboBox_GPUID_RealESRGAN->setObjectName("comboBox_GPUID_RealESRGAN");
        comboBox_GPUID_RealESRGAN->setEnabled(true);
        comboBox_GPUID_RealESRGAN->setMinimumSize(QSize(180, 0));
        comboBox_GPUID_RealESRGAN->setCurrentText(QString::fromUtf8("auto"));

        horizontalLayout_GPU_RealESRGAN->addWidget(comboBox_GPUID_RealESRGAN);

        pushButton_DetectGPU_RealESRGAN = new QPushButton(tab_RealESRGAN);
        pushButton_DetectGPU_RealESRGAN->setObjectName("pushButton_DetectGPU_RealESRGAN");
        pushButton_DetectGPU_RealESRGAN->setEnabled(true);
        sizePolicy5.setHeightForWidth(pushButton_DetectGPU_RealESRGAN->sizePolicy().hasHeightForWidth());
        pushButton_DetectGPU_RealESRGAN->setSizePolicy(sizePolicy5);

        horizontalLayout_GPU_RealESRGAN->addWidget(pushButton_DetectGPU_RealESRGAN);

        checkBox_MultiGPU_RealESRGANEngine = new QCheckBox(tab_RealESRGAN);
        checkBox_MultiGPU_RealESRGANEngine->setObjectName("checkBox_MultiGPU_RealESRGANEngine");
        sizePolicy4.setHeightForWidth(checkBox_MultiGPU_RealESRGANEngine->sizePolicy().hasHeightForWidth());
        checkBox_MultiGPU_RealESRGANEngine->setSizePolicy(sizePolicy4);

        horizontalLayout_GPU_RealESRGAN->addWidget(checkBox_MultiGPU_RealESRGANEngine);

        horizontalSpacer_GPU_RealESRGAN = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_GPU_RealESRGAN->addItem(horizontalSpacer_GPU_RealESRGAN);


        gridLayout_RealESRGAN->addLayout(horizontalLayout_GPU_RealESRGAN, 5, 0, 1, 2);

        horizontalLayout_MultiGPUConfig_RealESRGAN = new QHBoxLayout();
        horizontalLayout_MultiGPUConfig_RealESRGAN->setObjectName("horizontalLayout_MultiGPUConfig_RealESRGAN");
        groupBox_GPUSettings_MultiGPU_RealESRGAN = new QGroupBox(tab_RealESRGAN);
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setObjectName("groupBox_GPUSettings_MultiGPU_RealESRGAN");
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setEnabled(false);
        sizePolicy4.setHeightForWidth(groupBox_GPUSettings_MultiGPU_RealESRGAN->sizePolicy().hasHeightForWidth());
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setSizePolicy(sizePolicy4);
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setMaximumSize(QSize(16777215, 16777215));
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setAlignment(Qt::AlignCenter);
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setFlat(true);
        gridLayout_MultiGPUConfigBox_RealESRGAN = new QGridLayout(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        gridLayout_MultiGPUConfigBox_RealESRGAN->setObjectName("gridLayout_MultiGPUConfigBox_RealESRGAN");
        gridLayout_MultiGPUConfigBox_RealESRGAN->setContentsMargins(6, 6, 6, 6);
        checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN = new QCheckBox(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setObjectName("checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN");
        sizePolicy5.setHeightForWidth(checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->sizePolicy().hasHeightForWidth());
        checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setSizePolicy(sizePolicy5);
        checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setChecked(true);

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN, 0, 2, 1, 1);

        label_MultiGPUID_RealESRGAN = new QLabel(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        label_MultiGPUID_RealESRGAN->setObjectName("label_MultiGPUID_RealESRGAN");
        sizePolicy3.setHeightForWidth(label_MultiGPUID_RealESRGAN->sizePolicy().hasHeightForWidth());
        label_MultiGPUID_RealESRGAN->setSizePolicy(sizePolicy3);
        label_MultiGPUID_RealESRGAN->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(label_MultiGPUID_RealESRGAN, 0, 0, 1, 1);

        label_MultiGPUTile_RealESRGAN = new QLabel(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        label_MultiGPUTile_RealESRGAN->setObjectName("label_MultiGPUTile_RealESRGAN");
        sizePolicy3.setHeightForWidth(label_MultiGPUTile_RealESRGAN->sizePolicy().hasHeightForWidth());
        label_MultiGPUTile_RealESRGAN->setSizePolicy(sizePolicy3);
        label_MultiGPUTile_RealESRGAN->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(label_MultiGPUTile_RealESRGAN, 0, 4, 1, 1);

        pushButton_ShowMultiGPUSettings_RealESRGAN = new QPushButton(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        pushButton_ShowMultiGPUSettings_RealESRGAN->setObjectName("pushButton_ShowMultiGPUSettings_RealESRGAN");

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(pushButton_ShowMultiGPUSettings_RealESRGAN, 0, 6, 1, 1);

        comboBox_GPUIDs_MultiGPU_RealESRGAN = new QComboBox(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        comboBox_GPUIDs_MultiGPU_RealESRGAN->setObjectName("comboBox_GPUIDs_MultiGPU_RealESRGAN");
        comboBox_GPUIDs_MultiGPU_RealESRGAN->setCurrentText(QString::fromUtf8(""));

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(comboBox_GPUIDs_MultiGPU_RealESRGAN, 0, 1, 1, 1);

        spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN = new QSpinBox(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setObjectName("spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN");
        spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setMinimum(32);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setMaximum(999999999);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setSingleStep(10);
        spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setValue(128);

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN, 0, 5, 1, 1);

        line_MultiGPUSeparator_RealESRGAN = new QFrame(groupBox_GPUSettings_MultiGPU_RealESRGAN);
        line_MultiGPUSeparator_RealESRGAN->setObjectName("line_MultiGPUSeparator_RealESRGAN");
        line_MultiGPUSeparator_RealESRGAN->setFrameShape(QFrame::VLine);
        line_MultiGPUSeparator_RealESRGAN->setFrameShadow(QFrame::Sunken);

        gridLayout_MultiGPUConfigBox_RealESRGAN->addWidget(line_MultiGPUSeparator_RealESRGAN, 0, 3, 1, 1);


        horizontalLayout_MultiGPUConfig_RealESRGAN->addWidget(groupBox_GPUSettings_MultiGPU_RealESRGAN);

        horizontalSpacer_MultiGPUConfig_RealESRGAN = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_MultiGPUConfig_RealESRGAN->addItem(horizontalSpacer_MultiGPUConfig_RealESRGAN);

        horizontalLayout_MultiGPUConfig_RealESRGAN->setStretch(0, 1);
        horizontalLayout_MultiGPUConfig_RealESRGAN->setStretch(1, 2);

        gridLayout_RealESRGAN->addLayout(horizontalLayout_MultiGPUConfig_RealESRGAN, 6, 0, 1, 2);

        line_RealESRGAN_Sep1 = new QFrame(tab_RealESRGAN);
        line_RealESRGAN_Sep1->setObjectName("line_RealESRGAN_Sep1");
        line_RealESRGAN_Sep1->setFrameShape(QFrame::HLine);
        line_RealESRGAN_Sep1->setFrameShadow(QFrame::Sunken);

        gridLayout_RealESRGAN->addWidget(line_RealESRGAN_Sep1, 2, 0, 1, 2);

        line_RealESRGAN_Sep2 = new QFrame(tab_RealESRGAN);
        line_RealESRGAN_Sep2->setObjectName("line_RealESRGAN_Sep2");
        line_RealESRGAN_Sep2->setFrameShape(QFrame::HLine);
        line_RealESRGAN_Sep2->setFrameShadow(QFrame::Sunken);

        gridLayout_RealESRGAN->addWidget(line_RealESRGAN_Sep2, 4, 0, 1, 2);

        tabWidget_Engines->addTab(tab_RealESRGAN, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_RealESRGAN), QString::fromUtf8("RealESRGAN-NCNN-Vulkan"));
        tab_RealCUGAN = new QWidget();
        tab_RealCUGAN->setObjectName("tab_RealCUGAN");
        gridLayout_RealCUGAN = new QGridLayout(tab_RealCUGAN);
        gridLayout_RealCUGAN->setObjectName("gridLayout_RealCUGAN");
        comboBox_Model_RealCUGAN = new QComboBox(tab_RealCUGAN);
        comboBox_Model_RealCUGAN->setObjectName("comboBox_Model_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(comboBox_Model_RealCUGAN, 0, 0, 1, 1);

        spinBox_Scale_RealCUGAN = new QSpinBox(tab_RealCUGAN);
        spinBox_Scale_RealCUGAN->setObjectName("spinBox_Scale_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(spinBox_Scale_RealCUGAN, 1, 0, 1, 1);

        spinBox_DenoiseLevel_RealCUGAN = new QSpinBox(tab_RealCUGAN);
        spinBox_DenoiseLevel_RealCUGAN->setObjectName("spinBox_DenoiseLevel_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(spinBox_DenoiseLevel_RealCUGAN, 2, 0, 1, 1);

        spinBox_TileSize_RealCUGAN = new QSpinBox(tab_RealCUGAN);
        spinBox_TileSize_RealCUGAN->setObjectName("spinBox_TileSize_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(spinBox_TileSize_RealCUGAN, 3, 0, 1, 1);

        checkBox_TTA_RealCUGAN = new QCheckBox(tab_RealCUGAN);
        checkBox_TTA_RealCUGAN->setObjectName("checkBox_TTA_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(checkBox_TTA_RealCUGAN, 4, 0, 1, 1);

        comboBox_GPUID_RealCUGAN = new QComboBox(tab_RealCUGAN);
        comboBox_GPUID_RealCUGAN->setObjectName("comboBox_GPUID_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(comboBox_GPUID_RealCUGAN, 5, 0, 1, 1);

        pushButton_DetectGPU_RealCUGAN = new QPushButton(tab_RealCUGAN);
        pushButton_DetectGPU_RealCUGAN->setObjectName("pushButton_DetectGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(pushButton_DetectGPU_RealCUGAN, 6, 0, 1, 1);

        checkBox_MultiGPU_RealCUGAN = new QCheckBox(tab_RealCUGAN);
        checkBox_MultiGPU_RealCUGAN->setObjectName("checkBox_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(checkBox_MultiGPU_RealCUGAN, 7, 0, 1, 1);

        groupBox_GPUSettings_MultiGPU_RealCUGAN = new QGroupBox(tab_RealCUGAN);
        groupBox_GPUSettings_MultiGPU_RealCUGAN->setObjectName("groupBox_GPUSettings_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(groupBox_GPUSettings_MultiGPU_RealCUGAN, 8, 0, 1, 1);

        comboBox_GPUIDs_MultiGPU_RealCUGAN = new QComboBox(tab_RealCUGAN);
        comboBox_GPUIDs_MultiGPU_RealCUGAN->setObjectName("comboBox_GPUIDs_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(comboBox_GPUIDs_MultiGPU_RealCUGAN, 9, 0, 1, 1);

        listWidget_GPUList_MultiGPU_RealCUGAN = new QListWidget(tab_RealCUGAN);
        listWidget_GPUList_MultiGPU_RealCUGAN->setObjectName("listWidget_GPUList_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(listWidget_GPUList_MultiGPU_RealCUGAN, 10, 0, 1, 1);

        pushButton_AddGPU_MultiGPU_RealCUGAN = new QPushButton(tab_RealCUGAN);
        pushButton_AddGPU_MultiGPU_RealCUGAN->setObjectName("pushButton_AddGPU_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(pushButton_AddGPU_MultiGPU_RealCUGAN, 11, 0, 1, 1);

        pushButton_RemoveGPU_MultiGPU_RealCUGAN = new QPushButton(tab_RealCUGAN);
        pushButton_RemoveGPU_MultiGPU_RealCUGAN->setObjectName("pushButton_RemoveGPU_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(pushButton_RemoveGPU_MultiGPU_RealCUGAN, 12, 0, 1, 1);

        pushButton_ClearGPU_MultiGPU_RealCUGAN = new QPushButton(tab_RealCUGAN);
        pushButton_ClearGPU_MultiGPU_RealCUGAN->setObjectName("pushButton_ClearGPU_MultiGPU_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(pushButton_ClearGPU_MultiGPU_RealCUGAN, 13, 0, 1, 1);

        pushButton_TileSize_Add_RealCUGAN = new QPushButton(tab_RealCUGAN);
        pushButton_TileSize_Add_RealCUGAN->setObjectName("pushButton_TileSize_Add_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(pushButton_TileSize_Add_RealCUGAN, 14, 0, 1, 1);

        pushButton_TileSize_Minus_RealCUGAN = new QPushButton(tab_RealCUGAN);
        pushButton_TileSize_Minus_RealCUGAN->setObjectName("pushButton_TileSize_Minus_RealCUGAN");

        gridLayout_RealCUGAN->addWidget(pushButton_TileSize_Minus_RealCUGAN, 15, 0, 1, 1);

        tabWidget_Engines->addTab(tab_RealCUGAN, QString());
        tabWidget_Engines->setTabText(tabWidget_Engines->indexOf(tab_RealCUGAN), QString::fromUtf8("RealCUGAN-NCNN-Vulkan"));

        gridLayout_6->addWidget(tabWidget_Engines, 3, 0, 1, 4);


        gridLayout_29->addWidget(groupBox_Engine, 0, 0, 1, 1);

        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/new/prefix1/icon/EngineSettings.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_EngineSettings, icon15, QString());
        tab_VideoSettings = new QWidget();
        tab_VideoSettings->setObjectName("tab_VideoSettings");
        verticalLayout_VideoSettingsTab = new QVBoxLayout(tab_VideoSettings);
        verticalLayout_VideoSettingsTab->setSpacing(5);
        verticalLayout_VideoSettingsTab->setObjectName("verticalLayout_VideoSettingsTab");
        verticalLayout_VideoSettingsTab->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_top_video = new QHBoxLayout();
        horizontalLayout_top_video->setObjectName("horizontalLayout_top_video");
        groupBox_AudioDenoise = new QGroupBox(tab_VideoSettings);
        groupBox_AudioDenoise->setObjectName("groupBox_AudioDenoise");
        sizePolicy5.setHeightForWidth(groupBox_AudioDenoise->sizePolicy().hasHeightForWidth());
        groupBox_AudioDenoise->setSizePolicy(sizePolicy5);
        groupBox_AudioDenoise->setMaximumSize(QSize(16777215, 55));
        gridLayout_16 = new QGridLayout(groupBox_AudioDenoise);
        gridLayout_16->setObjectName("gridLayout_16");
        gridLayout_16->setContentsMargins(6, 6, 6, 6);
        doubleSpinBox_AudioDenoiseLevel = new QDoubleSpinBox(groupBox_AudioDenoise);
        doubleSpinBox_AudioDenoiseLevel->setObjectName("doubleSpinBox_AudioDenoiseLevel");
        doubleSpinBox_AudioDenoiseLevel->setEnabled(false);
        sizePolicy4.setHeightForWidth(doubleSpinBox_AudioDenoiseLevel->sizePolicy().hasHeightForWidth());
        doubleSpinBox_AudioDenoiseLevel->setSizePolicy(sizePolicy4);
        doubleSpinBox_AudioDenoiseLevel->setMinimumSize(QSize(80, 0));
        doubleSpinBox_AudioDenoiseLevel->setDecimals(2);
        doubleSpinBox_AudioDenoiseLevel->setMinimum(0.010000000000000);
        doubleSpinBox_AudioDenoiseLevel->setMaximum(1.000000000000000);
        doubleSpinBox_AudioDenoiseLevel->setSingleStep(0.010000000000000);
        doubleSpinBox_AudioDenoiseLevel->setValue(0.200000000000000);

        gridLayout_16->addWidget(doubleSpinBox_AudioDenoiseLevel, 0, 1, 1, 1);

        label_46 = new QLabel(groupBox_AudioDenoise);
        label_46->setObjectName("label_46");
        sizePolicy3.setHeightForWidth(label_46->sizePolicy().hasHeightForWidth());
        label_46->setSizePolicy(sizePolicy3);
        label_46->setMinimumSize(QSize(0, 0));

        gridLayout_16->addWidget(label_46, 0, 0, 1, 1);

        checkBox_AudioDenoise = new QCheckBox(groupBox_AudioDenoise);
        checkBox_AudioDenoise->setObjectName("checkBox_AudioDenoise");
        sizePolicy5.setHeightForWidth(checkBox_AudioDenoise->sizePolicy().hasHeightForWidth());
        checkBox_AudioDenoise->setSizePolicy(sizePolicy5);

        gridLayout_16->addWidget(checkBox_AudioDenoise, 0, 3, 1, 1);

        line_7 = new QFrame(groupBox_AudioDenoise);
        line_7->setObjectName("line_7");
        line_7->setFrameShape(QFrame::VLine);
        line_7->setFrameShadow(QFrame::Sunken);

        gridLayout_16->addWidget(line_7, 0, 2, 1, 1);


        horizontalLayout_top_video->addWidget(groupBox_AudioDenoise);

        groupBox_ProcessVideoBySegment = new QGroupBox(tab_VideoSettings);
        groupBox_ProcessVideoBySegment->setObjectName("groupBox_ProcessVideoBySegment");
        gridLayout_69 = new QGridLayout(groupBox_ProcessVideoBySegment);
        gridLayout_69->setObjectName("gridLayout_69");
        label_SegmentDuration = new QLabel(groupBox_ProcessVideoBySegment);
        label_SegmentDuration->setObjectName("label_SegmentDuration");
        sizePolicy5.setHeightForWidth(label_SegmentDuration->sizePolicy().hasHeightForWidth());
        label_SegmentDuration->setSizePolicy(sizePolicy5);

        gridLayout_69->addWidget(label_SegmentDuration, 0, 0, 1, 1);

        spinBox_VideoSplitDuration = new QSpinBox(groupBox_ProcessVideoBySegment);
        spinBox_VideoSplitDuration->setObjectName("spinBox_VideoSplitDuration");
        sizePolicy4.setHeightForWidth(spinBox_VideoSplitDuration->sizePolicy().hasHeightForWidth());
        spinBox_VideoSplitDuration->setSizePolicy(sizePolicy4);
        spinBox_VideoSplitDuration->setMinimum(1);
        spinBox_VideoSplitDuration->setMaximum(999999999);
        spinBox_VideoSplitDuration->setValue(30);

        gridLayout_69->addWidget(spinBox_VideoSplitDuration, 0, 1, 1, 1);

        checkBox_ProcessVideoBySegment = new QCheckBox(groupBox_ProcessVideoBySegment);
        checkBox_ProcessVideoBySegment->setObjectName("checkBox_ProcessVideoBySegment");
        checkBox_ProcessVideoBySegment->setChecked(true);

        gridLayout_69->addWidget(checkBox_ProcessVideoBySegment, 0, 3, 1, 1);

        line_46 = new QFrame(groupBox_ProcessVideoBySegment);
        line_46->setObjectName("line_46");
        line_46->setFrameShape(QFrame::VLine);
        line_46->setFrameShadow(QFrame::Sunken);

        gridLayout_69->addWidget(line_46, 0, 2, 1, 1);


        horizontalLayout_top_video->addWidget(groupBox_ProcessVideoBySegment);

        horizontalSpacer_top_video_stretch = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_top_video->addItem(horizontalSpacer_top_video_stretch);


        verticalLayout_VideoSettingsTab->addLayout(horizontalLayout_top_video);

        groupBox_FrameInterpolation = new QGroupBox(tab_VideoSettings);
        groupBox_FrameInterpolation->setObjectName("groupBox_FrameInterpolation");
        sizePolicy.setHeightForWidth(groupBox_FrameInterpolation->sizePolicy().hasHeightForWidth());
        groupBox_FrameInterpolation->setSizePolicy(sizePolicy);
        groupBox_FrameInterpolation->setCheckable(true);
        groupBox_FrameInterpolation->setChecked(false);
        gridLayout_68 = new QGridLayout(groupBox_FrameInterpolation);
        gridLayout_68->setObjectName("gridLayout_68");
        gridLayout_68->setContentsMargins(0, 0, 0, 0);
        frame_15 = new QFrame(groupBox_FrameInterpolation);
        frame_15->setObjectName("frame_15");
        sizePolicy4.setHeightForWidth(frame_15->sizePolicy().hasHeightForWidth());
        frame_15->setSizePolicy(sizePolicy4);
        frame_15->setFrameShape(QFrame::NoFrame);
        frame_15->setFrameShadow(QFrame::Raised);
        gridLayout_67 = new QGridLayout(frame_15);
        gridLayout_67->setObjectName("gridLayout_67");
        gridLayout_67->setContentsMargins(0, 0, 0, 0);
        checkBox_UHD_VFI = new QCheckBox(frame_15);
        checkBox_UHD_VFI->setObjectName("checkBox_UHD_VFI");
        sizePolicy4.setHeightForWidth(checkBox_UHD_VFI->sizePolicy().hasHeightForWidth());
        checkBox_UHD_VFI->setSizePolicy(sizePolicy4);

        gridLayout_67->addWidget(checkBox_UHD_VFI, 0, 6, 1, 1);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_67->addItem(horizontalSpacer_12, 0, 8, 1, 1);

        line_28 = new QFrame(frame_15);
        line_28->setObjectName("line_28");
        line_28->setFrameShape(QFrame::VLine);
        line_28->setFrameShadow(QFrame::Sunken);

        gridLayout_67->addWidget(line_28, 0, 4, 1, 1);

        frame_14 = new QFrame(frame_15);
        frame_14->setObjectName("frame_14");
        sizePolicy5.setHeightForWidth(frame_14->sizePolicy().hasHeightForWidth());
        frame_14->setSizePolicy(sizePolicy5);
        frame_14->setFrameShape(QFrame::NoFrame);
        frame_14->setFrameShadow(QFrame::Raised);
        gridLayout_65 = new QGridLayout(frame_14);
        gridLayout_65->setObjectName("gridLayout_65");
        gridLayout_65->setContentsMargins(0, 0, 0, 0);
        label_37 = new QLabel(frame_14);
        label_37->setObjectName("label_37");
        sizePolicy5.setHeightForWidth(label_37->sizePolicy().hasHeightForWidth());
        label_37->setSizePolicy(sizePolicy5);

        gridLayout_65->addWidget(label_37, 0, 0, 1, 1);

        comboBox_Engine_VFI = new QComboBox(frame_14);
        comboBox_Engine_VFI->addItem(QString::fromUtf8("rife-ncnn-vulkan"));
        comboBox_Engine_VFI->addItem(QString::fromUtf8("cain-ncnn-vulkan"));
        comboBox_Engine_VFI->addItem(QString::fromUtf8("dain-ncnn-vulkan"));
        comboBox_Engine_VFI->setObjectName("comboBox_Engine_VFI");
        sizePolicy5.setHeightForWidth(comboBox_Engine_VFI->sizePolicy().hasHeightForWidth());
        comboBox_Engine_VFI->setSizePolicy(sizePolicy5);

        gridLayout_65->addWidget(comboBox_Engine_VFI, 0, 1, 1, 1);


        gridLayout_67->addWidget(frame_14, 0, 3, 1, 1);

        checkBox_TTA_VFI = new QCheckBox(frame_15);
        checkBox_TTA_VFI->setObjectName("checkBox_TTA_VFI");
        sizePolicy4.setHeightForWidth(checkBox_TTA_VFI->sizePolicy().hasHeightForWidth());
        checkBox_TTA_VFI->setSizePolicy(sizePolicy4);

        gridLayout_67->addWidget(checkBox_TTA_VFI, 0, 5, 1, 1);

        checkBox_VfiAfterScale_VFI = new QCheckBox(frame_15);
        checkBox_VfiAfterScale_VFI->setObjectName("checkBox_VfiAfterScale_VFI");

        gridLayout_67->addWidget(checkBox_VfiAfterScale_VFI, 0, 7, 1, 1);


        gridLayout_68->addWidget(frame_15, 0, 0, 1, 3);

        line_27 = new QFrame(groupBox_FrameInterpolation);
        line_27->setObjectName("line_27");
        line_27->setFrameShape(QFrame::HLine);
        line_27->setFrameShadow(QFrame::Sunken);

        gridLayout_68->addWidget(line_27, 1, 0, 1, 3);

        line_29 = new QFrame(groupBox_FrameInterpolation);
        line_29->setObjectName("line_29");
        line_29->setFrameShape(QFrame::VLine);
        line_29->setFrameShadow(QFrame::Sunken);

        gridLayout_68->addWidget(line_29, 2, 1, 3, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        label_30 = new QLabel(groupBox_FrameInterpolation);
        label_30->setObjectName("label_30");
        sizePolicy3.setHeightForWidth(label_30->sizePolicy().hasHeightForWidth());
        label_30->setSizePolicy(sizePolicy3);

        horizontalLayout_4->addWidget(label_30);

        comboBox_GPUID_VFI = new QComboBox(groupBox_FrameInterpolation);
        comboBox_GPUID_VFI->addItem(QString::fromUtf8("auto"));
        comboBox_GPUID_VFI->setObjectName("comboBox_GPUID_VFI");

        horizontalLayout_4->addWidget(comboBox_GPUID_VFI);

        pushButton_DetectGPU_VFI = new QPushButton(groupBox_FrameInterpolation);
        pushButton_DetectGPU_VFI->setObjectName("pushButton_DetectGPU_VFI");
        sizePolicy5.setHeightForWidth(pushButton_DetectGPU_VFI->sizePolicy().hasHeightForWidth());
        pushButton_DetectGPU_VFI->setSizePolicy(sizePolicy5);

        horizontalLayout_4->addWidget(pushButton_DetectGPU_VFI);


        gridLayout_68->addLayout(horizontalLayout_4, 2, 2, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        checkBox_MultiGPU_VFI = new QCheckBox(groupBox_FrameInterpolation);
        checkBox_MultiGPU_VFI->setObjectName("checkBox_MultiGPU_VFI");
        sizePolicy3.setHeightForWidth(checkBox_MultiGPU_VFI->sizePolicy().hasHeightForWidth());
        checkBox_MultiGPU_VFI->setSizePolicy(sizePolicy3);

        horizontalLayout_6->addWidget(checkBox_MultiGPU_VFI);

        lineEdit_MultiGPU_IDs_VFI = new QLineEdit(groupBox_FrameInterpolation);
        lineEdit_MultiGPU_IDs_VFI->setObjectName("lineEdit_MultiGPU_IDs_VFI");
        sizePolicy4.setHeightForWidth(lineEdit_MultiGPU_IDs_VFI->sizePolicy().hasHeightForWidth());
        lineEdit_MultiGPU_IDs_VFI->setSizePolicy(sizePolicy4);

        horizontalLayout_6->addWidget(lineEdit_MultiGPU_IDs_VFI);

        pushButton_Verify_MultiGPU_VFI = new QPushButton(groupBox_FrameInterpolation);
        pushButton_Verify_MultiGPU_VFI->setObjectName("pushButton_Verify_MultiGPU_VFI");
        sizePolicy5.setHeightForWidth(pushButton_Verify_MultiGPU_VFI->sizePolicy().hasHeightForWidth());
        pushButton_Verify_MultiGPU_VFI->setSizePolicy(sizePolicy5);

        horizontalLayout_6->addWidget(pushButton_Verify_MultiGPU_VFI);


        gridLayout_68->addLayout(horizontalLayout_6, 3, 2, 1, 1);

        horizontalLayout_49 = new QHBoxLayout();
        horizontalLayout_49->setObjectName("horizontalLayout_49");
        label_19 = new QLabel(groupBox_FrameInterpolation);
        label_19->setObjectName("label_19");
        sizePolicy3.setHeightForWidth(label_19->sizePolicy().hasHeightForWidth());
        label_19->setSizePolicy(sizePolicy3);

        horizontalLayout_49->addWidget(label_19);

        spinBox_TileSize_VFI = new QSpinBox(groupBox_FrameInterpolation);
        spinBox_TileSize_VFI->setObjectName("spinBox_TileSize_VFI");
        spinBox_TileSize_VFI->setFocusPolicy(Qt::NoFocus);
        spinBox_TileSize_VFI->setMinimum(128);
        spinBox_TileSize_VFI->setMaximum(999999999);
        spinBox_TileSize_VFI->setSingleStep(32);

        horizontalLayout_49->addWidget(spinBox_TileSize_VFI);

        horizontalSpacer_48 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_49->addItem(horizontalSpacer_48);


        gridLayout_68->addLayout(horizontalLayout_49, 4, 2, 1, 1);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName("verticalLayout_7");
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        label_3 = new QLabel(groupBox_FrameInterpolation);
        label_3->setObjectName("label_3");
        sizePolicy3.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy3);

        horizontalLayout_11->addWidget(label_3);

        comboBox_Model_VFI = new QComboBox(groupBox_FrameInterpolation);
        comboBox_Model_VFI->addItem(QString::fromUtf8("rife"));
        comboBox_Model_VFI->addItem(QString::fromUtf8("rife-anime"));
        comboBox_Model_VFI->addItem(QString::fromUtf8("rife-HD"));
        comboBox_Model_VFI->addItem(QString::fromUtf8("rife-UHD"));
        comboBox_Model_VFI->addItem(QString::fromUtf8("rife-v2"));
        comboBox_Model_VFI->addItem(QString::fromUtf8("rife-v2.4"));
        comboBox_Model_VFI->setObjectName("comboBox_Model_VFI");

        horizontalLayout_11->addWidget(comboBox_Model_VFI);


        verticalLayout_7->addLayout(horizontalLayout_11);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        checkBox_MultiThread_VFI = new QCheckBox(groupBox_FrameInterpolation);
        checkBox_MultiThread_VFI->setObjectName("checkBox_MultiThread_VFI");
        sizePolicy5.setHeightForWidth(checkBox_MultiThread_VFI->sizePolicy().hasHeightForWidth());
        checkBox_MultiThread_VFI->setSizePolicy(sizePolicy5);

        horizontalLayout_9->addWidget(checkBox_MultiThread_VFI);

        spinBox_NumOfThreads_VFI = new QSpinBox(groupBox_FrameInterpolation);
        spinBox_NumOfThreads_VFI->setObjectName("spinBox_NumOfThreads_VFI");
        sizePolicy4.setHeightForWidth(spinBox_NumOfThreads_VFI->sizePolicy().hasHeightForWidth());
        spinBox_NumOfThreads_VFI->setSizePolicy(sizePolicy4);
        spinBox_NumOfThreads_VFI->setMinimum(1);
        spinBox_NumOfThreads_VFI->setMaximum(999999999);

        horizontalLayout_9->addWidget(spinBox_NumOfThreads_VFI);

        checkBox_AutoAdjustNumOfThreads_VFI = new QCheckBox(groupBox_FrameInterpolation);
        checkBox_AutoAdjustNumOfThreads_VFI->setObjectName("checkBox_AutoAdjustNumOfThreads_VFI");
        sizePolicy5.setHeightForWidth(checkBox_AutoAdjustNumOfThreads_VFI->sizePolicy().hasHeightForWidth());
        checkBox_AutoAdjustNumOfThreads_VFI->setSizePolicy(sizePolicy5);
        checkBox_AutoAdjustNumOfThreads_VFI->setChecked(false);

        horizontalLayout_9->addWidget(checkBox_AutoAdjustNumOfThreads_VFI);


        verticalLayout_7->addLayout(horizontalLayout_9);


        gridLayout_68->addLayout(verticalLayout_7, 2, 0, 3, 1);


        verticalLayout_VideoSettingsTab->addWidget(groupBox_FrameInterpolation);

        groupBox_video_settings = new QGroupBox(tab_VideoSettings);
        groupBox_video_settings->setObjectName("groupBox_video_settings");
        sizePolicy.setHeightForWidth(groupBox_video_settings->sizePolicy().hasHeightForWidth());
        groupBox_video_settings->setSizePolicy(sizePolicy);
        groupBox_video_settings->setMaximumSize(QSize(16777215, 16777215));
        groupBox_video_settings->setCheckable(true);
        groupBox_video_settings->setChecked(false);
        horizontalLayout_video_settings_group = new QHBoxLayout(groupBox_video_settings);
        horizontalLayout_video_settings_group->setObjectName("horizontalLayout_video_settings_group");
        horizontalLayout_video_settings_group->setContentsMargins(6, 6, 6, 6);
        groupBox_OutputVideoSettings = new QGroupBox(groupBox_video_settings);
        groupBox_OutputVideoSettings->setObjectName("groupBox_OutputVideoSettings");
        groupBox_OutputVideoSettings->setEnabled(false);
        formLayout_OutputVideoSettings = new QFormLayout(groupBox_OutputVideoSettings);
        formLayout_OutputVideoSettings->setSpacing(5);
        formLayout_OutputVideoSettings->setObjectName("formLayout_OutputVideoSettings");
        formLayout_OutputVideoSettings->setContentsMargins(5, 5, 5, 5);
        label_31 = new QLabel(groupBox_OutputVideoSettings);
        label_31->setObjectName("label_31");
        sizePolicy3.setHeightForWidth(label_31->sizePolicy().hasHeightForWidth());
        label_31->setSizePolicy(sizePolicy3);

        formLayout_OutputVideoSettings->setWidget(0, QFormLayout::LabelRole, label_31);

        lineEdit_encoder_vid = new QLineEdit(groupBox_OutputVideoSettings);
        lineEdit_encoder_vid->setObjectName("lineEdit_encoder_vid");
        lineEdit_encoder_vid->setText(QString::fromUtf8("libx264"));

        formLayout_OutputVideoSettings->setWidget(0, QFormLayout::FieldRole, lineEdit_encoder_vid);

        label_27 = new QLabel(groupBox_OutputVideoSettings);
        label_27->setObjectName("label_27");
        sizePolicy3.setHeightForWidth(label_27->sizePolicy().hasHeightForWidth());
        label_27->setSizePolicy(sizePolicy3);

        formLayout_OutputVideoSettings->setWidget(1, QFormLayout::LabelRole, label_27);

        spinBox_bitrate_vid = new QSpinBox(groupBox_OutputVideoSettings);
        spinBox_bitrate_vid->setObjectName("spinBox_bitrate_vid");
        spinBox_bitrate_vid->setSuffix(QString::fromUtf8(" K"));
        spinBox_bitrate_vid->setMinimum(0);
        spinBox_bitrate_vid->setMaximum(999999999);
        spinBox_bitrate_vid->setSingleStep(100);
        spinBox_bitrate_vid->setValue(6000);

        formLayout_OutputVideoSettings->setWidget(1, QFormLayout::FieldRole, spinBox_bitrate_vid);

        label_32 = new QLabel(groupBox_OutputVideoSettings);
        label_32->setObjectName("label_32");
        sizePolicy3.setHeightForWidth(label_32->sizePolicy().hasHeightForWidth());
        label_32->setSizePolicy(sizePolicy3);

        formLayout_OutputVideoSettings->setWidget(2, QFormLayout::LabelRole, label_32);

        lineEdit_encoder_audio = new QLineEdit(groupBox_OutputVideoSettings);
        lineEdit_encoder_audio->setObjectName("lineEdit_encoder_audio");
        lineEdit_encoder_audio->setText(QString::fromUtf8("aac"));

        formLayout_OutputVideoSettings->setWidget(2, QFormLayout::FieldRole, lineEdit_encoder_audio);

        label_29 = new QLabel(groupBox_OutputVideoSettings);
        label_29->setObjectName("label_29");
        sizePolicy3.setHeightForWidth(label_29->sizePolicy().hasHeightForWidth());
        label_29->setSizePolicy(sizePolicy3);

        formLayout_OutputVideoSettings->setWidget(3, QFormLayout::LabelRole, label_29);

        spinBox_bitrate_audio = new QSpinBox(groupBox_OutputVideoSettings);
        spinBox_bitrate_audio->setObjectName("spinBox_bitrate_audio");
        spinBox_bitrate_audio->setSuffix(QString::fromUtf8(" K"));
        spinBox_bitrate_audio->setMinimum(0);
        spinBox_bitrate_audio->setMaximum(999999999);
        spinBox_bitrate_audio->setSingleStep(10);
        spinBox_bitrate_audio->setValue(320);

        formLayout_OutputVideoSettings->setWidget(3, QFormLayout::FieldRole, spinBox_bitrate_audio);

        label_33 = new QLabel(groupBox_OutputVideoSettings);
        label_33->setObjectName("label_33");
        sizePolicy3.setHeightForWidth(label_33->sizePolicy().hasHeightForWidth());
        label_33->setSizePolicy(sizePolicy3);

        formLayout_OutputVideoSettings->setWidget(4, QFormLayout::LabelRole, label_33);

        lineEdit_pixformat = new QLineEdit(groupBox_OutputVideoSettings);
        lineEdit_pixformat->setObjectName("lineEdit_pixformat");
        lineEdit_pixformat->setText(QString::fromUtf8("yuv420p"));

        formLayout_OutputVideoSettings->setWidget(4, QFormLayout::FieldRole, lineEdit_pixformat);

        line_17 = new QFrame(groupBox_OutputVideoSettings);
        line_17->setObjectName("line_17");
        line_17->setFrameShape(QFrame::HLine);
        line_17->setFrameShadow(QFrame::Sunken);

        formLayout_OutputVideoSettings->setWidget(5, QFormLayout::SpanningRole, line_17);

        label_42 = new QLabel(groupBox_OutputVideoSettings);
        label_42->setObjectName("label_42");
        sizePolicy3.setHeightForWidth(label_42->sizePolicy().hasHeightForWidth());
        label_42->setSizePolicy(sizePolicy3);

        formLayout_OutputVideoSettings->setWidget(6, QFormLayout::LabelRole, label_42);

        lineEdit_ExCommand_output = new QLineEdit(groupBox_OutputVideoSettings);
        lineEdit_ExCommand_output->setObjectName("lineEdit_ExCommand_output");
        lineEdit_ExCommand_output->setText(QString::fromUtf8(""));
        lineEdit_ExCommand_output->setClearButtonEnabled(true);

        formLayout_OutputVideoSettings->setWidget(6, QFormLayout::FieldRole, lineEdit_ExCommand_output);


        horizontalLayout_video_settings_group->addWidget(groupBox_OutputVideoSettings);

        groupBox_ToMp4VideoSettings = new QGroupBox(groupBox_video_settings);
        groupBox_ToMp4VideoSettings->setObjectName("groupBox_ToMp4VideoSettings");
        groupBox_ToMp4VideoSettings->setEnabled(false);
        formLayout_ToMp4VideoSettings = new QFormLayout(groupBox_ToMp4VideoSettings);
        formLayout_ToMp4VideoSettings->setSpacing(5);
        formLayout_ToMp4VideoSettings->setObjectName("formLayout_ToMp4VideoSettings");
        formLayout_ToMp4VideoSettings->setContentsMargins(5, 5, 5, 5);
        label_34 = new QLabel(groupBox_ToMp4VideoSettings);
        label_34->setObjectName("label_34");
        sizePolicy3.setHeightForWidth(label_34->sizePolicy().hasHeightForWidth());
        label_34->setSizePolicy(sizePolicy3);

        formLayout_ToMp4VideoSettings->setWidget(0, QFormLayout::LabelRole, label_34);

        spinBox_bitrate_vid_2mp4 = new QSpinBox(groupBox_ToMp4VideoSettings);
        spinBox_bitrate_vid_2mp4->setObjectName("spinBox_bitrate_vid_2mp4");
        spinBox_bitrate_vid_2mp4->setSuffix(QString::fromUtf8(" K"));
        spinBox_bitrate_vid_2mp4->setMinimum(0);
        spinBox_bitrate_vid_2mp4->setMaximum(999999999);
        spinBox_bitrate_vid_2mp4->setSingleStep(100);
        spinBox_bitrate_vid_2mp4->setValue(2500);

        formLayout_ToMp4VideoSettings->setWidget(0, QFormLayout::FieldRole, spinBox_bitrate_vid_2mp4);

        label_35 = new QLabel(groupBox_ToMp4VideoSettings);
        label_35->setObjectName("label_35");
        sizePolicy3.setHeightForWidth(label_35->sizePolicy().hasHeightForWidth());
        label_35->setSizePolicy(sizePolicy3);

        formLayout_ToMp4VideoSettings->setWidget(1, QFormLayout::LabelRole, label_35);

        spinBox_bitrate_audio_2mp4 = new QSpinBox(groupBox_ToMp4VideoSettings);
        spinBox_bitrate_audio_2mp4->setObjectName("spinBox_bitrate_audio_2mp4");
        spinBox_bitrate_audio_2mp4->setSuffix(QString::fromUtf8(" K"));
        spinBox_bitrate_audio_2mp4->setMinimum(0);
        spinBox_bitrate_audio_2mp4->setMaximum(999999999);
        spinBox_bitrate_audio_2mp4->setSingleStep(10);
        spinBox_bitrate_audio_2mp4->setValue(320);

        formLayout_ToMp4VideoSettings->setWidget(1, QFormLayout::FieldRole, spinBox_bitrate_audio_2mp4);

        horizontalLayout_39_form = new QHBoxLayout();
        horizontalLayout_39_form->setObjectName("horizontalLayout_39_form");
        checkBox_acodec_copy_2mp4 = new QCheckBox(groupBox_ToMp4VideoSettings);
        checkBox_acodec_copy_2mp4->setObjectName("checkBox_acodec_copy_2mp4");
        sizePolicy5.setHeightForWidth(checkBox_acodec_copy_2mp4->sizePolicy().hasHeightForWidth());
        checkBox_acodec_copy_2mp4->setSizePolicy(sizePolicy5);

        horizontalLayout_39_form->addWidget(checkBox_acodec_copy_2mp4);

        checkBox_vcodec_copy_2mp4 = new QCheckBox(groupBox_ToMp4VideoSettings);
        checkBox_vcodec_copy_2mp4->setObjectName("checkBox_vcodec_copy_2mp4");
        sizePolicy5.setHeightForWidth(checkBox_vcodec_copy_2mp4->sizePolicy().hasHeightForWidth());
        checkBox_vcodec_copy_2mp4->setSizePolicy(sizePolicy5);

        horizontalLayout_39_form->addWidget(checkBox_vcodec_copy_2mp4);


        formLayout_ToMp4VideoSettings->setLayout(2, QFormLayout::FieldRole, horizontalLayout_39_form);

        checkBox_IgnoreFrameRateMode = new QCheckBox(groupBox_ToMp4VideoSettings);
        checkBox_IgnoreFrameRateMode->setObjectName("checkBox_IgnoreFrameRateMode");

        formLayout_ToMp4VideoSettings->setWidget(3, QFormLayout::FieldRole, checkBox_IgnoreFrameRateMode);

        line_18 = new QFrame(groupBox_ToMp4VideoSettings);
        line_18->setObjectName("line_18");
        line_18->setFrameShape(QFrame::HLine);
        line_18->setFrameShadow(QFrame::Sunken);

        formLayout_ToMp4VideoSettings->setWidget(4, QFormLayout::SpanningRole, line_18);

        label_45 = new QLabel(groupBox_ToMp4VideoSettings);
        label_45->setObjectName("label_45");
        sizePolicy3.setHeightForWidth(label_45->sizePolicy().hasHeightForWidth());
        label_45->setSizePolicy(sizePolicy3);

        formLayout_ToMp4VideoSettings->setWidget(5, QFormLayout::LabelRole, label_45);

        lineEdit_ExCommand_2mp4 = new QLineEdit(groupBox_ToMp4VideoSettings);
        lineEdit_ExCommand_2mp4->setObjectName("lineEdit_ExCommand_2mp4");
        lineEdit_ExCommand_2mp4->setText(QString::fromUtf8(""));
        lineEdit_ExCommand_2mp4->setClearButtonEnabled(true);

        formLayout_ToMp4VideoSettings->setWidget(5, QFormLayout::FieldRole, lineEdit_ExCommand_2mp4);


        horizontalLayout_video_settings_group->addWidget(groupBox_ToMp4VideoSettings);

        horizontalLayout_video_settings_group->setStretch(0, 1);
        horizontalLayout_video_settings_group->setStretch(1, 1);

        verticalLayout_VideoSettingsTab->addWidget(groupBox_video_settings);

        horizontalLayout_bottom_buttons_video = new QHBoxLayout();
        horizontalLayout_bottom_buttons_video->setObjectName("horizontalLayout_bottom_buttons_video");
        horizontalSpacer_bottom_video_spacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_bottom_buttons_video->addItem(horizontalSpacer_bottom_video_spacer);

        pushButton_ResetVideoSettings = new QPushButton(tab_VideoSettings);
        pushButton_ResetVideoSettings->setObjectName("pushButton_ResetVideoSettings");
        sizePolicy4.setHeightForWidth(pushButton_ResetVideoSettings->sizePolicy().hasHeightForWidth());
        pushButton_ResetVideoSettings->setSizePolicy(sizePolicy4);

        horizontalLayout_bottom_buttons_video->addWidget(pushButton_ResetVideoSettings);

        pushButton_encodersList = new QPushButton(tab_VideoSettings);
        pushButton_encodersList->setObjectName("pushButton_encodersList");
        sizePolicy4.setHeightForWidth(pushButton_encodersList->sizePolicy().hasHeightForWidth());
        pushButton_encodersList->setSizePolicy(sizePolicy4);

        horizontalLayout_bottom_buttons_video->addWidget(pushButton_encodersList);


        verticalLayout_VideoSettingsTab->addLayout(horizontalLayout_bottom_buttons_video);

        verticalSpacer_VideoSettingsTab = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_VideoSettingsTab->addItem(verticalSpacer_VideoSettingsTab);

        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/new/prefix1/icon/VideoSettings.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_VideoSettings, icon16, QString());
        tab_AdditionalSettings = new QWidget();
        tab_AdditionalSettings->setObjectName("tab_AdditionalSettings");
        horizontalLayout_AdditionalSettingsTab = new QHBoxLayout(tab_AdditionalSettings);
        horizontalLayout_AdditionalSettingsTab->setSpacing(5);
        horizontalLayout_AdditionalSettingsTab->setObjectName("horizontalLayout_AdditionalSettingsTab");
        horizontalLayout_AdditionalSettingsTab->setContentsMargins(6, 6, 6, 6);
        verticalLayout_col1_additional = new QVBoxLayout();
        verticalLayout_col1_additional->setObjectName("verticalLayout_col1_additional");
        groupBox_InputExt = new QGroupBox(tab_AdditionalSettings);
        groupBox_InputExt->setObjectName("groupBox_InputExt");
        groupBox_InputExt->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_10 = new QGridLayout(groupBox_InputExt);
        gridLayout_10->setObjectName("gridLayout_10");
        horizontalLayout_47 = new QHBoxLayout();
        horizontalLayout_47->setObjectName("horizontalLayout_47");
        label_4 = new QLabel(groupBox_InputExt);
        label_4->setObjectName("label_4");
        sizePolicy5.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy5);
        label_4->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_47->addWidget(label_4);

        Ext_image = new QLineEdit(groupBox_InputExt);
        Ext_image->setObjectName("Ext_image");
        Ext_image->setMinimumSize(QSize(300, 0));
        Ext_image->setAcceptDrops(false);
        Ext_image->setText(QString::fromUtf8("png:jpg:jpeg:tif:tiff:bmp"));

        horizontalLayout_47->addWidget(Ext_image);


        gridLayout_10->addLayout(horizontalLayout_47, 0, 0, 1, 1);

        horizontalLayout_48 = new QHBoxLayout();
        horizontalLayout_48->setObjectName("horizontalLayout_48");
        label_5 = new QLabel(groupBox_InputExt);
        label_5->setObjectName("label_5");
        sizePolicy5.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy5);
        label_5->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_48->addWidget(label_5);

        Ext_video = new QLineEdit(groupBox_InputExt);
        Ext_video->setObjectName("Ext_video");
        Ext_video->setMinimumSize(QSize(300, 0));
        Ext_video->setAcceptDrops(false);
        Ext_video->setText(QString::fromUtf8("mp4:3gp:avi:flv:mkv:mov:mpg:ogg:vob:webm:wmv"));

        horizontalLayout_48->addWidget(Ext_video);


        gridLayout_10->addLayout(horizontalLayout_48, 1, 0, 1, 1);

        horizontalLayout_35 = new QHBoxLayout();
        horizontalLayout_35->setObjectName("horizontalLayout_35");
        checkBox_PreProcessImage = new QCheckBox(groupBox_InputExt);
        checkBox_PreProcessImage->setObjectName("checkBox_PreProcessImage");

        horizontalLayout_35->addWidget(checkBox_PreProcessImage);

        checkBox_AutoDetectAlphaChannel = new QCheckBox(groupBox_InputExt);
        checkBox_AutoDetectAlphaChannel->setObjectName("checkBox_AutoDetectAlphaChannel");
        checkBox_AutoDetectAlphaChannel->setEnabled(true);
        checkBox_AutoDetectAlphaChannel->setChecked(true);

        horizontalLayout_35->addWidget(checkBox_AutoDetectAlphaChannel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_35->addItem(horizontalSpacer_3);


        gridLayout_10->addLayout(horizontalLayout_35, 2, 0, 1, 1);

        checkBox_AlwaysPreProcessAlphaPNG = new QCheckBox(groupBox_InputExt);
        checkBox_AlwaysPreProcessAlphaPNG->setObjectName("checkBox_AlwaysPreProcessAlphaPNG");
        sizePolicy4.setHeightForWidth(checkBox_AlwaysPreProcessAlphaPNG->sizePolicy().hasHeightForWidth());
        checkBox_AlwaysPreProcessAlphaPNG->setSizePolicy(sizePolicy4);
        checkBox_AlwaysPreProcessAlphaPNG->setChecked(false);

        gridLayout_10->addWidget(checkBox_AlwaysPreProcessAlphaPNG, 3, 0, 1, 1);


        verticalLayout_col1_additional->addWidget(groupBox_InputExt);

        groupBox_3 = new QGroupBox(tab_AdditionalSettings);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_17 = new QGridLayout(groupBox_3);
        gridLayout_17->setObjectName("gridLayout_17");
        horizontalLayout_37 = new QHBoxLayout();
        horizontalLayout_37->setObjectName("horizontalLayout_37");
        label_24 = new QLabel(groupBox_3);
        label_24->setObjectName("label_24");
        sizePolicy5.setHeightForWidth(label_24->sizePolicy().hasHeightForWidth());
        label_24->setSizePolicy(sizePolicy5);
        label_24->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_37->addWidget(label_24);

        fontComboBox_CustFont = new QFontComboBox(groupBox_3);
        fontComboBox_CustFont->setObjectName("fontComboBox_CustFont");
        fontComboBox_CustFont->setEnabled(false);
        fontComboBox_CustFont->setCurrentText(QString::fromUtf8("\345\256\213\344\275\223"));
        QFont font5;
        font5.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        fontComboBox_CustFont->setCurrentFont(font5);

        horizontalLayout_37->addWidget(fontComboBox_CustFont);


        gridLayout_17->addLayout(horizontalLayout_37, 1, 0, 1, 1);

        horizontalLayout_38 = new QHBoxLayout();
        horizontalLayout_38->setObjectName("horizontalLayout_38");
        pushButton_Save_GlobalFontSize = new QPushButton(groupBox_3);
        pushButton_Save_GlobalFontSize->setObjectName("pushButton_Save_GlobalFontSize");
        pushButton_Save_GlobalFontSize->setEnabled(false);
        sizePolicy4.setHeightForWidth(pushButton_Save_GlobalFontSize->sizePolicy().hasHeightForWidth());
        pushButton_Save_GlobalFontSize->setSizePolicy(sizePolicy4);
        pushButton_Save_GlobalFontSize->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_38->addWidget(pushButton_Save_GlobalFontSize);

        checkBox_isCustFontEnable = new QCheckBox(groupBox_3);
        checkBox_isCustFontEnable->setObjectName("checkBox_isCustFontEnable");
        sizePolicy5.setHeightForWidth(checkBox_isCustFontEnable->sizePolicy().hasHeightForWidth());
        checkBox_isCustFontEnable->setSizePolicy(sizePolicy5);

        horizontalLayout_38->addWidget(checkBox_isCustFontEnable);


        gridLayout_17->addLayout(horizontalLayout_38, 2, 0, 1, 1);

        horizontalLayout_36 = new QHBoxLayout();
        horizontalLayout_36->setObjectName("horizontalLayout_36");
        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName("label_22");
        sizePolicy5.setHeightForWidth(label_22->sizePolicy().hasHeightForWidth());
        label_22->setSizePolicy(sizePolicy5);
        label_22->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_36->addWidget(label_22);

        spinBox_GlobalFontSize = new QSpinBox(groupBox_3);
        spinBox_GlobalFontSize->setObjectName("spinBox_GlobalFontSize");
        spinBox_GlobalFontSize->setEnabled(false);
        sizePolicy4.setHeightForWidth(spinBox_GlobalFontSize->sizePolicy().hasHeightForWidth());
        spinBox_GlobalFontSize->setSizePolicy(sizePolicy4);
        spinBox_GlobalFontSize->setMinimumSize(QSize(80, 0));
        spinBox_GlobalFontSize->setMaximumSize(QSize(16777215, 16777215));
        spinBox_GlobalFontSize->setMinimum(1);
        spinBox_GlobalFontSize->setMaximum(999);
        spinBox_GlobalFontSize->setValue(15);

        horizontalLayout_36->addWidget(spinBox_GlobalFontSize);


        gridLayout_17->addLayout(horizontalLayout_36, 0, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout_17->addItem(horizontalSpacer_2, 0, 1, 3, 1);


        verticalLayout_col1_additional->addWidget(groupBox_3);

        groupBox_other_1 = new QGroupBox(tab_AdditionalSettings);
        groupBox_other_1->setObjectName("groupBox_other_1");
        sizePolicy.setHeightForWidth(groupBox_other_1->sizePolicy().hasHeightForWidth());
        groupBox_other_1->setSizePolicy(sizePolicy);
        groupBox_other_1->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_7 = new QGridLayout(groupBox_other_1);
        gridLayout_7->setObjectName("gridLayout_7");
        horizontalLayout_57 = new QHBoxLayout();
        horizontalLayout_57->setObjectName("horizontalLayout_57");
        pushButton_SaveSettings = new QPushButton(groupBox_other_1);
        pushButton_SaveSettings->setObjectName("pushButton_SaveSettings");
        pushButton_SaveSettings->setEnabled(true);

        horizontalLayout_57->addWidget(pushButton_SaveSettings);

        pushButton_ResetSettings = new QPushButton(groupBox_other_1);
        pushButton_ResetSettings->setObjectName("pushButton_ResetSettings");

        horizontalLayout_57->addWidget(pushButton_ResetSettings);

        pushButton_CheckUpdate = new QPushButton(groupBox_other_1);
        pushButton_CheckUpdate->setObjectName("pushButton_CheckUpdate");
        QFont font6;
        font6.setPointSize(9);
        font6.setUnderline(false);
        pushButton_CheckUpdate->setFont(font6);

        horizontalLayout_57->addWidget(pushButton_CheckUpdate);


        gridLayout_7->addLayout(horizontalLayout_57, 0, 0, 1, 1);

        horizontalLayout_58 = new QHBoxLayout();
        horizontalLayout_58->setObjectName("horizontalLayout_58");
        pushButton_Report = new QPushButton(groupBox_other_1);
        pushButton_Report->setObjectName("pushButton_Report");
        QFont font7;
        font7.setUnderline(false);
        pushButton_Report->setFont(font7);

        horizontalLayout_58->addWidget(pushButton_Report);

        pushButton_ReadMe = new QPushButton(groupBox_other_1);
        pushButton_ReadMe->setObjectName("pushButton_ReadMe");
        pushButton_ReadMe->setFont(font7);

        horizontalLayout_58->addWidget(pushButton_ReadMe);

        pushButton_wiki = new QPushButton(groupBox_other_1);
        pushButton_wiki->setObjectName("pushButton_wiki");

        horizontalLayout_58->addWidget(pushButton_wiki);


        gridLayout_7->addLayout(horizontalLayout_58, 1, 0, 1, 1);

        horizontalLayout_59 = new QHBoxLayout();
        horizontalLayout_59->setObjectName("horizontalLayout_59");
        pushButton_about = new QPushButton(groupBox_other_1);
        pushButton_about->setObjectName("pushButton_about");
        pushButton_about->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_59->addWidget(pushButton_about);

        pushButton_SupportersList = new QPushButton(groupBox_other_1);
        pushButton_SupportersList->setObjectName("pushButton_SupportersList");

        horizontalLayout_59->addWidget(pushButton_SupportersList);

        horizontalLayout_59->setStretch(0, 1);
        horizontalLayout_59->setStretch(1, 2);

        gridLayout_7->addLayout(horizontalLayout_59, 2, 0, 1, 1);


        verticalLayout_col1_additional->addWidget(groupBox_other_1);


        horizontalLayout_AdditionalSettingsTab->addLayout(verticalLayout_col1_additional);

        verticalLayout_col2_additional = new QVBoxLayout();
        verticalLayout_col2_additional->setObjectName("verticalLayout_col2_additional");
        checkBox_UpdatePopup = new QCheckBox(tab_AdditionalSettings);
        checkBox_UpdatePopup->setObjectName("checkBox_UpdatePopup");
        sizePolicy6.setHeightForWidth(checkBox_UpdatePopup->sizePolicy().hasHeightForWidth());
        checkBox_UpdatePopup->setSizePolicy(sizePolicy6);
        checkBox_UpdatePopup->setChecked(true);

        verticalLayout_col2_additional->addWidget(checkBox_UpdatePopup);

        checkBox_MinimizeToTaskbar = new QCheckBox(tab_AdditionalSettings);
        checkBox_MinimizeToTaskbar->setObjectName("checkBox_MinimizeToTaskbar");

        verticalLayout_col2_additional->addWidget(checkBox_MinimizeToTaskbar);

        checkBox_AutoSaveSettings = new QCheckBox(tab_AdditionalSettings);
        checkBox_AutoSaveSettings->setObjectName("checkBox_AutoSaveSettings");
        checkBox_AutoSaveSettings->setChecked(true);

        verticalLayout_col2_additional->addWidget(checkBox_AutoSaveSettings);

        checkBox_AlwaysHideTextBrowser = new QCheckBox(tab_AdditionalSettings);
        checkBox_AlwaysHideTextBrowser->setObjectName("checkBox_AlwaysHideTextBrowser");

        verticalLayout_col2_additional->addWidget(checkBox_AlwaysHideTextBrowser);

        checkBox_ScanSubFolders = new QCheckBox(tab_AdditionalSettings);
        checkBox_ScanSubFolders->setObjectName("checkBox_ScanSubFolders");
        checkBox_ScanSubFolders->setChecked(true);

        verticalLayout_col2_additional->addWidget(checkBox_ScanSubFolders);

        checkBox_PromptWhenExit = new QCheckBox(tab_AdditionalSettings);
        checkBox_PromptWhenExit->setObjectName("checkBox_PromptWhenExit");
        checkBox_PromptWhenExit->setChecked(true);

        verticalLayout_col2_additional->addWidget(checkBox_PromptWhenExit);

        checkBox_DisableResize_gif = new QCheckBox(tab_AdditionalSettings);
        checkBox_DisableResize_gif->setObjectName("checkBox_DisableResize_gif");

        verticalLayout_col2_additional->addWidget(checkBox_DisableResize_gif);

        checkBox_BanGitee = new QCheckBox(tab_AdditionalSettings);
        checkBox_BanGitee->setObjectName("checkBox_BanGitee");

        verticalLayout_col2_additional->addWidget(checkBox_BanGitee);

        verticalSpacer_col2_additional = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_col2_additional->addItem(verticalSpacer_col2_additional);


        horizontalLayout_AdditionalSettingsTab->addLayout(verticalLayout_col2_additional);

        verticalLayout_col3_additional = new QVBoxLayout();
        verticalLayout_col3_additional->setObjectName("verticalLayout_col3_additional");
        checkBox_FileListAutoSlide = new QCheckBox(tab_AdditionalSettings);
        checkBox_FileListAutoSlide->setObjectName("checkBox_FileListAutoSlide");
        checkBox_FileListAutoSlide->setChecked(true);

        verticalLayout_col3_additional->addWidget(checkBox_FileListAutoSlide);

        checkBox_ShowInterPro = new QCheckBox(tab_AdditionalSettings);
        checkBox_ShowInterPro->setObjectName("checkBox_ShowInterPro");
        checkBox_ShowInterPro->setChecked(true);

        verticalLayout_col3_additional->addWidget(checkBox_ShowInterPro);

        checkBox_AlwaysHideSettings = new QCheckBox(tab_AdditionalSettings);
        checkBox_AlwaysHideSettings->setObjectName("checkBox_AlwaysHideSettings");

        verticalLayout_col3_additional->addWidget(checkBox_AlwaysHideSettings);

        checkBox_FileList_Interactive = new QCheckBox(tab_AdditionalSettings);
        checkBox_FileList_Interactive->setObjectName("checkBox_FileList_Interactive");
        checkBox_FileList_Interactive->setChecked(false);

        verticalLayout_col3_additional->addWidget(checkBox_FileList_Interactive);

        checkBox_KeepVideoCache = new QCheckBox(tab_AdditionalSettings);
        checkBox_KeepVideoCache->setObjectName("checkBox_KeepVideoCache");
        checkBox_KeepVideoCache->setChecked(false);

        verticalLayout_col3_additional->addWidget(checkBox_KeepVideoCache);

        checkBox_SummaryPopup = new QCheckBox(tab_AdditionalSettings);
        checkBox_SummaryPopup->setObjectName("checkBox_SummaryPopup");
        checkBox_SummaryPopup->setChecked(true);

        verticalLayout_col3_additional->addWidget(checkBox_SummaryPopup);

        checkBox_NfSound = new QCheckBox(tab_AdditionalSettings);
        checkBox_NfSound->setObjectName("checkBox_NfSound");
        checkBox_NfSound->setChecked(true);

        verticalLayout_col3_additional->addWidget(checkBox_NfSound);

        verticalSpacer_col3_additional = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_col3_additional->addItem(verticalSpacer_col3_additional);


        horizontalLayout_AdditionalSettingsTab->addLayout(verticalLayout_col3_additional);

        tabWidget->addTab(tab_AdditionalSettings, icon11, QString());
        tab_CompatibilityTest = new QWidget();
        tab_CompatibilityTest->setObjectName("tab_CompatibilityTest");
        verticalLayout_CompatTestTab = new QVBoxLayout(tab_CompatibilityTest);
        verticalLayout_CompatTestTab->setSpacing(6);
        verticalLayout_CompatTestTab->setObjectName("verticalLayout_CompatTestTab");
        verticalLayout_CompatTestTab->setContentsMargins(6, 6, 6, 6);
        groupBox_CompatibilityTestRes = new QGroupBox(tab_CompatibilityTest);
        groupBox_CompatibilityTestRes->setObjectName("groupBox_CompatibilityTestRes");
        groupBox_CompatibilityTestRes->setEnabled(true);
        gridLayout_CompatTestResInternal = new QGridLayout(groupBox_CompatibilityTestRes);
        gridLayout_CompatTestResInternal->setObjectName("gridLayout_CompatTestResInternal");
        gridLayout_CompatTestResInternal->setContentsMargins(6, 6, 6, 6);
        label_58 = new QLabel(groupBox_CompatibilityTestRes);
        label_58->setObjectName("label_58");
        sizePolicy4.setHeightForWidth(label_58->sizePolicy().hasHeightForWidth());
        label_58->setSizePolicy(sizePolicy4);
        label_58->setMaximumSize(QSize(16777215, 30));

        gridLayout_CompatTestResInternal->addWidget(label_58, 0, 0, 1, 3);

        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setObjectName("checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW");
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setEnabled(true);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setFocusPolicy(Qt::NoFocus);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/waifu2x-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setCheckable(true);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setChecked(false);

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW, 1, 0, 1, 1);

        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setObjectName("checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P");
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setEnabled(true);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setFocusPolicy(Qt::NoFocus);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/waifu2x-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setText(QString::fromUtf8("Waifu2x-ncnn-vulkan(20200414(fp16p))"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P, 1, 1, 1, 1);

        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setObjectName("checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD");
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setEnabled(true);
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setFocusPolicy(Qt::NoFocus);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/waifu2x-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD, 1, 2, 1, 1);

        checkBox_isCompatible_SRMD_NCNN_Vulkan = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_SRMD_NCNN_Vulkan->setObjectName("checkBox_isCompatible_SRMD_NCNN_Vulkan");
        checkBox_isCompatible_SRMD_NCNN_Vulkan->setEnabled(true);
        checkBox_isCompatible_SRMD_NCNN_Vulkan->setFocusPolicy(Qt::NoFocus);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_SRMD_NCNN_Vulkan->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/srmd-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_SRMD_NCNN_Vulkan->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));
        checkBox_isCompatible_SRMD_NCNN_Vulkan->setText(QString::fromUtf8("SRMD-ncnn-vulkan"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_SRMD_NCNN_Vulkan, 2, 0, 1, 1);

        checkBox_isCompatible_Waifu2x_Converter = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_Converter->setObjectName("checkBox_isCompatible_Waifu2x_Converter");
        checkBox_isCompatible_Waifu2x_Converter->setEnabled(true);
        checkBox_isCompatible_Waifu2x_Converter->setMouseTracking(false);
        checkBox_isCompatible_Waifu2x_Converter->setFocusPolicy(Qt::NoFocus);
        checkBox_isCompatible_Waifu2x_Converter->setContextMenuPolicy(Qt::NoContextMenu);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Converter->setToolTip(QString::fromUtf8("By @DeadSix27(https://github.com/DeadSix27)\n"
"\n"
"https://github.com/DeadSix27/waifu2x-converter-cpp"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Converter->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));
        checkBox_isCompatible_Waifu2x_Converter->setText(QString::fromUtf8("Waifu2x-converter"));
        checkBox_isCompatible_Waifu2x_Converter->setCheckable(true);

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_Converter, 2, 1, 1, 1);

        checkBox_isCompatible_Anime4k_CPU = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Anime4k_CPU->setObjectName("checkBox_isCompatible_Anime4k_CPU");
        checkBox_isCompatible_Anime4k_CPU->setEnabled(true);
        checkBox_isCompatible_Anime4k_CPU->setFocusPolicy(Qt::NoFocus);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Anime4k_CPU->setToolTip(QString::fromUtf8("By @TianZerL(https://github.com/TianZerL)\n"
"\n"
"https://github.com/TianZerL/Anime4KCPP"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Anime4k_CPU->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));
        checkBox_isCompatible_Anime4k_CPU->setText(QString::fromUtf8("Anime4K(CPU)"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Anime4k_CPU, 2, 2, 1, 1);

        checkBox_isCompatible_Anime4k_GPU = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Anime4k_GPU->setObjectName("checkBox_isCompatible_Anime4k_GPU");
        checkBox_isCompatible_Anime4k_GPU->setEnabled(true);
        checkBox_isCompatible_Anime4k_GPU->setFocusPolicy(Qt::NoFocus);
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Anime4k_GPU->setToolTip(QString::fromUtf8("By @TianZerL(https://github.com/TianZerL)\n"
"\n"
"https://github.com/TianZerL/Anime4KCPP"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Anime4k_GPU->setStyleSheet(QString::fromUtf8("QCheckBox:disabled{\n"
"	color: rgb(0, 0, 0);\n"
"}"));
        checkBox_isCompatible_Anime4k_GPU->setText(QString::fromUtf8("Anime4K(GPU)"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Anime4k_GPU, 3, 0, 1, 1);

        checkBox_isCompatible_Waifu2x_Caffe_CPU = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_Caffe_CPU->setObjectName("checkBox_isCompatible_Waifu2x_Caffe_CPU");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Caffe_CPU->setToolTip(QString::fromUtf8("By @lltcggie(https://github.com/lltcggie)\n"
"\n"
"https://github.com/lltcggie/waifu2x-caffe"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Caffe_CPU->setText(QString::fromUtf8("Waifu2x-caffe(CPU)"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_Caffe_CPU, 3, 1, 1, 1);

        checkBox_isCompatible_Waifu2x_Caffe_GPU = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_Caffe_GPU->setObjectName("checkBox_isCompatible_Waifu2x_Caffe_GPU");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Caffe_GPU->setToolTip(QString::fromUtf8("By @lltcggie(https://github.com/lltcggie)\n"
"\n"
"https://github.com/lltcggie/waifu2x-caffe"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Caffe_GPU->setText(QString::fromUtf8("Waifu2x-caffe(GPU)"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_Caffe_GPU, 3, 2, 1, 1);

        checkBox_isCompatible_Waifu2x_Caffe_cuDNN = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Waifu2x_Caffe_cuDNN->setObjectName("checkBox_isCompatible_Waifu2x_Caffe_cuDNN");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Caffe_cuDNN->setToolTip(QString::fromUtf8("By @lltcggie(https://github.com/lltcggie)\n"
"\n"
"https://github.com/lltcggie/waifu2x-caffe"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Waifu2x_Caffe_cuDNN->setText(QString::fromUtf8("Waifu2x-caffe(cuDNN)"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Waifu2x_Caffe_cuDNN, 4, 0, 1, 1);

        checkBox_isCompatible_Realsr_NCNN_Vulkan = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Realsr_NCNN_Vulkan->setObjectName("checkBox_isCompatible_Realsr_NCNN_Vulkan");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Realsr_NCNN_Vulkan->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/realsr-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Realsr_NCNN_Vulkan->setText(QString::fromUtf8("Realsr-ncnn-vulkan"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Realsr_NCNN_Vulkan, 4, 1, 1, 1);

        checkBox_isCompatible_SRMD_CUDA = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_SRMD_CUDA->setObjectName("checkBox_isCompatible_SRMD_CUDA");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_SRMD_CUDA->setToolTip(QString::fromUtf8("By @MrZihan(https://github.com/MrZihan)\n"
"\n"
"https://github.com/MrZihan/Super-resolution-SR-CUDA"));
#endif // QT_CONFIG(tooltip)

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_SRMD_CUDA, 4, 2, 1, 1);

        line_16 = new QFrame(groupBox_CompatibilityTestRes);
        line_16->setObjectName("line_16");
        line_16->setFrameShape(QFrame::HLine);
        line_16->setFrameShadow(QFrame::Sunken);

        gridLayout_CompatTestResInternal->addWidget(line_16, 5, 0, 1, 3);

        label_59 = new QLabel(groupBox_CompatibilityTestRes);
        label_59->setObjectName("label_59");
        sizePolicy4.setHeightForWidth(label_59->sizePolicy().hasHeightForWidth());
        label_59->setSizePolicy(sizePolicy4);
        label_59->setMaximumSize(QSize(16777215, 30));

        gridLayout_CompatTestResInternal->addWidget(label_59, 6, 0, 1, 3);

        checkBox_isCompatible_SoX = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_SoX->setObjectName("checkBox_isCompatible_SoX");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_SoX->setToolTip(QString::fromUtf8("http://sox.sourceforge.net/"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_SoX->setText(QString::fromUtf8("SoX"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_SoX, 7, 0, 1, 1);

        checkBox_isCompatible_ImageMagick = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_ImageMagick->setObjectName("checkBox_isCompatible_ImageMagick");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_ImageMagick->setToolTip(QString::fromUtf8("https://imagemagick.org/"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_ImageMagick->setText(QString::fromUtf8("ImageMagick"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_ImageMagick, 7, 1, 1, 1);

        checkBox_isCompatible_Gifsicle = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_Gifsicle->setObjectName("checkBox_isCompatible_Gifsicle");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_Gifsicle->setToolTip(QString::fromUtf8("https://www.lcdf.org/gifsicle/"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_Gifsicle->setText(QString::fromUtf8("Gifsicle"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_Gifsicle, 7, 2, 1, 1);

        checkBox_isCompatible_FFprobe = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_FFprobe->setObjectName("checkBox_isCompatible_FFprobe");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_FFprobe->setToolTip(QString::fromUtf8("https://www.ffmpeg.org/"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_FFprobe->setText(QString::fromUtf8("FFprobe"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_FFprobe, 8, 0, 1, 1);

        checkBox_isCompatible_FFmpeg = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_FFmpeg->setObjectName("checkBox_isCompatible_FFmpeg");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_FFmpeg->setToolTip(QString::fromUtf8("https://www.ffmpeg.org/"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_FFmpeg->setText(QString::fromUtf8("FFmpeg"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_FFmpeg, 8, 1, 1, 1);

        line_19 = new QFrame(groupBox_CompatibilityTestRes);
        line_19->setObjectName("line_19");
        line_19->setFrameShape(QFrame::HLine);
        line_19->setFrameShadow(QFrame::Sunken);

        gridLayout_CompatTestResInternal->addWidget(line_19, 9, 0, 1, 3);

        label_36 = new QLabel(groupBox_CompatibilityTestRes);
        label_36->setObjectName("label_36");
        sizePolicy4.setHeightForWidth(label_36->sizePolicy().hasHeightForWidth());
        label_36->setSizePolicy(sizePolicy4);

        gridLayout_CompatTestResInternal->addWidget(label_36, 10, 0, 1, 3);

        checkBox_isCompatible_RifeNcnnVulkan = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_RifeNcnnVulkan->setObjectName("checkBox_isCompatible_RifeNcnnVulkan");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_RifeNcnnVulkan->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/rife-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_RifeNcnnVulkan->setText(QString::fromUtf8("RIFE-NCNN-Vulkan"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_RifeNcnnVulkan, 11, 0, 1, 1);

        checkBox_isCompatible_CainNcnnVulkan = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_CainNcnnVulkan->setObjectName("checkBox_isCompatible_CainNcnnVulkan");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_CainNcnnVulkan->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/cain-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_CainNcnnVulkan->setText(QString::fromUtf8("CAIN-NCNN-Vulkan"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_CainNcnnVulkan, 11, 1, 1, 1);

        checkBox_isCompatible_DainNcnnVulkan = new QCheckBox(groupBox_CompatibilityTestRes);
        checkBox_isCompatible_DainNcnnVulkan->setObjectName("checkBox_isCompatible_DainNcnnVulkan");
#if QT_CONFIG(tooltip)
        checkBox_isCompatible_DainNcnnVulkan->setToolTip(QString::fromUtf8("By @nihui(https://github.com/nihui)\n"
"\n"
"https://github.com/nihui/dain-ncnn-vulkan"));
#endif // QT_CONFIG(tooltip)
        checkBox_isCompatible_DainNcnnVulkan->setText(QString::fromUtf8("DAIN-NCNN-Vulkan"));

        gridLayout_CompatTestResInternal->addWidget(checkBox_isCompatible_DainNcnnVulkan, 11, 2, 1, 1);


        verticalLayout_CompatTestTab->addWidget(groupBox_CompatibilityTestRes);

        label_2 = new QLabel(tab_CompatibilityTest);
        label_2->setObjectName("label_2");
        label_2->setMinimumSize(QSize(660, 260));
        label_2->setMaximumSize(QSize(16777215, 260));
        label_2->setFrameShape(QFrame::Box);
        label_2->setFrameShadow(QFrame::Sunken);
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_2->setWordWrap(true);

        verticalLayout_CompatTestTab->addWidget(label_2);

        progressBar_CompatibilityTest = new QProgressBar(tab_CompatibilityTest);
        progressBar_CompatibilityTest->setObjectName("progressBar_CompatibilityTest");
        sizePolicy4.setHeightForWidth(progressBar_CompatibilityTest->sizePolicy().hasHeightForWidth());
        progressBar_CompatibilityTest->setSizePolicy(sizePolicy4);
        progressBar_CompatibilityTest->setMaximum(19);
        progressBar_CompatibilityTest->setValue(0);
        progressBar_CompatibilityTest->setInvertedAppearance(false);
        progressBar_CompatibilityTest->setTextDirection(QProgressBar::TopToBottom);

        verticalLayout_CompatTestTab->addWidget(progressBar_CompatibilityTest);

        pushButton_compatibilityTest = new QPushButton(tab_CompatibilityTest);
        pushButton_compatibilityTest->setObjectName("pushButton_compatibilityTest");
        sizePolicy4.setHeightForWidth(pushButton_compatibilityTest->sizePolicy().hasHeightForWidth());
        pushButton_compatibilityTest->setSizePolicy(sizePolicy4);

        verticalLayout_CompatTestTab->addWidget(pushButton_compatibilityTest);

        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/new/prefix1/icon/CompatibilityTest.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_CompatibilityTest, icon17, QString());
        splitter_4->addWidget(tabWidget);
        splitter_TextBrowser = new QSplitter(splitter_4);
        splitter_TextBrowser->setObjectName("splitter_TextBrowser");
        sizePolicy1.setHeightForWidth(splitter_TextBrowser->sizePolicy().hasHeightForWidth());
        splitter_TextBrowser->setSizePolicy(sizePolicy1);
        splitter_TextBrowser->setMinimumSize(QSize(0, 95));
        splitter_TextBrowser->setMaximumSize(QSize(16777215, 16777215));
        splitter_TextBrowser->setContextMenuPolicy(Qt::DefaultContextMenu);
        splitter_TextBrowser->setLineWidth(0);
        splitter_TextBrowser->setOrientation(Qt::Horizontal);
        splitter_TextBrowser->setHandleWidth(5);
        textBrowser = new QTextBrowser(splitter_TextBrowser);
        textBrowser->setObjectName("textBrowser");
        sizePolicy.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy);
        textBrowser->setMinimumSize(QSize(0, 0));
        textBrowser->setMaximumSize(QSize(16777215, 16777215));
        QFont font8;
        font8.setFamilies({QString::fromUtf8("Arial")});
        textBrowser->setFont(font8);
        textBrowser->setAcceptDrops(false);
        splitter_TextBrowser->addWidget(textBrowser);
        groupBox_textBrowserSettings = new QGroupBox(splitter_TextBrowser);
        groupBox_textBrowserSettings->setObjectName("groupBox_textBrowserSettings");
        sizePolicy3.setHeightForWidth(groupBox_textBrowserSettings->sizePolicy().hasHeightForWidth());
        groupBox_textBrowserSettings->setSizePolicy(sizePolicy3);
        groupBox_textBrowserSettings->setMinimumSize(QSize(0, 0));
        groupBox_textBrowserSettings->setMaximumSize(QSize(16777215, 16777215));
        groupBox_textBrowserSettings->setStyleSheet(QString::fromUtf8(""));
        groupBox_textBrowserSettings->setTitle(QString::fromUtf8(""));
        groupBox_textBrowserSettings->setFlat(false);
        gridLayout_9 = new QGridLayout(groupBox_textBrowserSettings);
        gridLayout_9->setSpacing(5);
        gridLayout_9->setObjectName("gridLayout_9");
        gridLayout_9->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout_9->setContentsMargins(6, 6, 6, 6);
        pushButton_clear_textbrowser = new QPushButton(groupBox_textBrowserSettings);
        pushButton_clear_textbrowser->setObjectName("pushButton_clear_textbrowser");
        QSizePolicy sizePolicy7(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(pushButton_clear_textbrowser->sizePolicy().hasHeightForWidth());
        pushButton_clear_textbrowser->setSizePolicy(sizePolicy7);
        pushButton_clear_textbrowser->setIconSize(QSize(25, 25));

        gridLayout_9->addWidget(pushButton_clear_textbrowser, 1, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(5);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setSizeConstraint(QLayout::SetFixedSize);
        label_TextBroFontSize = new QLabel(groupBox_textBrowserSettings);
        label_TextBroFontSize->setObjectName("label_TextBroFontSize");
        sizePolicy5.setHeightForWidth(label_TextBroFontSize->sizePolicy().hasHeightForWidth());
        label_TextBroFontSize->setSizePolicy(sizePolicy5);

        horizontalLayout_7->addWidget(label_TextBroFontSize);

        spinBox_textbrowser_fontsize = new QSpinBox(groupBox_textBrowserSettings);
        spinBox_textbrowser_fontsize->setObjectName("spinBox_textbrowser_fontsize");
        sizePolicy4.setHeightForWidth(spinBox_textbrowser_fontsize->sizePolicy().hasHeightForWidth());
        spinBox_textbrowser_fontsize->setSizePolicy(sizePolicy4);
        spinBox_textbrowser_fontsize->setMinimum(1);
        spinBox_textbrowser_fontsize->setValue(9);

        horizontalLayout_7->addWidget(spinBox_textbrowser_fontsize);

        checkBox_OutPath_KeepOriginalFileName = new QCheckBox(groupBox_textBrowserSettings);
        checkBox_OutPath_KeepOriginalFileName->setObjectName("checkBox_OutPath_KeepOriginalFileName");

        horizontalLayout_7->addWidget(checkBox_OutPath_KeepOriginalFileName);


        gridLayout_9->addLayout(horizontalLayout_7, 0, 0, 1, 1);

        splitter_TextBrowser->addWidget(groupBox_textBrowserSettings);
        splitter_4->addWidget(splitter_TextBrowser);

        gridLayout_40->addWidget(splitter_4, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);
        fileListTabWidget->setCurrentIndex(0);
        comboBox_AspectRatio_custRes->setCurrentIndex(1);
        tabWidget_Engines->setCurrentIndex(0);
        comboBox_version_Waifu2xNCNNVulkan->setCurrentIndex(0);
        comboBox_ProcessMode_Waifu2xCaffe->setCurrentIndex(0);
        comboBox_Model_2D_Waifu2xCaffe->setCurrentIndex(1);
        comboBox_Model_VFI->setCurrentIndex(5);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        label_DonateText->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p align=\"center\">Please donate to support the developers, so we can bring further updates for this software, thank you! (\357\275\241\357\275\245\342\210\200\357\275\245)\357\276\211\357\276\236 </p><p align=\"center\">If you don't wanna see the Donate page anymore, you can <span style=\" color:#ff0004;\">get the </span><span style=\" font-weight:600; color:#ff0004;\">Premium</span><span style=\" color:#ff0004;\"> version by support me on Patreon.</span></p></body></html>", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_PayPal->setToolTip(QCoreApplication::translate("MainWindow", "Donate to the developer through PayPal.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pushButton_Patreon->setToolTip(QCoreApplication::translate("MainWindow", "Support this project on Patreon and get Premium version.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_Patreon->setText(QCoreApplication::translate("MainWindow", " Get Premium version on Patreon", nullptr));
        label_DonateQRCode->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(tab_Donate), QCoreApplication::translate("MainWindow", "Donate", nullptr));
        groupBox_FileList->setTitle(QCoreApplication::translate("MainWindow", "Files List", nullptr));
#if QT_CONFIG(tooltip)
        tableView_image->setToolTip(QCoreApplication::translate("MainWindow", "<html><body>\n"
"<p><b>Press [Delete]</b> key to remove file from the list.</p>\n"
"<p><b>Press [Ctrl + A]</b> key to apply custom resolution.</p>\n"
"<p><b>Press [Ctrl + C]</b> key to cancel custom resolution.</p>\n"
"<br/>\n"
"<p>Right click to show more options.</p>\n"
"</body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fileListTabWidget->setTabText(fileListTabWidget->indexOf(tab_Images), QCoreApplication::translate("MainWindow", "Images", nullptr));
#if QT_CONFIG(tooltip)
        tableView_gif->setToolTip(QCoreApplication::translate("MainWindow", "<html><body>\n"
"<p><b>Press [Delete]</b> key to remove file from the list.</p>\n"
"<p><b>Press [Ctrl + A]</b> key to apply custom resolution.</p>\n"
"<p><b>Press [Ctrl + C]</b> key to cancel custom resolution.</p>\n"
"<br/>\n"
"<p>Right click to show more options.</p>\n"
"</body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fileListTabWidget->setTabText(fileListTabWidget->indexOf(tab_Gifs), QCoreApplication::translate("MainWindow", "GIF/APNG", nullptr));
#if QT_CONFIG(tooltip)
        tableView_video->setToolTip(QCoreApplication::translate("MainWindow", "<html><body>\n"
"<p><b>Press [Delete]</b> key to remove file from the list.</p>\n"
"<p><b>Press [Ctrl + A]</b> key to apply custom resolution.</p>\n"
"<p><b>Press [Ctrl + C]</b> key to cancel custom resolution.</p>\n"
"<br/>\n"
"<p>Right click to show more options.</p>\n"
"</body></html>", nullptr));
#endif // QT_CONFIG(tooltip)
        fileListTabWidget->setTabText(fileListTabWidget->indexOf(tab_Videos), QCoreApplication::translate("MainWindow", "Videos", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_ClearList->setToolTip(QCoreApplication::translate("MainWindow", "Remove all the files in the File list.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ClearList->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_RemoveItem->setToolTip(QCoreApplication::translate("MainWindow", "Remove the selected file from File List.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_RemoveItem->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_SaveFileList->setToolTip(QCoreApplication::translate("MainWindow", "Save Files List", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_SaveFileList->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_ReadFileList->setToolTip(QCoreApplication::translate("MainWindow", "Read Files List", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ReadFileList->setText(QString());
#if QT_CONFIG(tooltip)
        pushButton_BrowserFile->setToolTip(QCoreApplication::translate("MainWindow", "Browse and add files.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_BrowserFile->setText(QString());
        groupBox_CurrentFile->setTitle(QCoreApplication::translate("MainWindow", "Current File:", nullptr));
#if QT_CONFIG(tooltip)
        label_FrameProgress_CurrentFile->setToolTip(QCoreApplication::translate("MainWindow", "Finished/Total", nullptr));
#endif // QT_CONFIG(tooltip)
        label_TimeCost_CurrentFile->setText(QCoreApplication::translate("MainWindow", "Time taken:NULL", nullptr));
        label_TimeRemain_CurrentFile->setText(QCoreApplication::translate("MainWindow", "Time remaining:NULL", nullptr));
        label_ETA_CurrentFile->setText(QCoreApplication::translate("MainWindow", "ETA:NULL", nullptr));
        groupBox_Progress->setTitle(QString());
#if QT_CONFIG(tooltip)
        label_progressBar_filenum->setToolTip(QCoreApplication::translate("MainWindow", "Finished/Total", nullptr));
#endif // QT_CONFIG(tooltip)
        label_TimeCost->setText(QCoreApplication::translate("MainWindow", "Time taken:NULL", nullptr));
        label_TimeRemain->setText(QCoreApplication::translate("MainWindow", "Time remaining:NULL", nullptr));
        label_ETA->setText(QCoreApplication::translate("MainWindow", "ETA:NULL", nullptr));
        pushButton_Start->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        pushButton_Stop->setText(QCoreApplication::translate("MainWindow", "Pause", nullptr));
        pushButton_ForceRetry->setText(QCoreApplication::translate("MainWindow", "Force retry", nullptr));
        groupBox_Setting->setTitle(QString());
        pushButton_HideSettings->setText(QCoreApplication::translate("MainWindow", "Hide settings", nullptr));
        pushButton_HideTextBro->setText(QCoreApplication::translate("MainWindow", "Hide Text Browser", nullptr));
        groupBox_CustRes->setTitle(QCoreApplication::translate("MainWindow", "Custom resolution", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Width:", nullptr));
        spinBox_CustRes_width->setSuffix(QCoreApplication::translate("MainWindow", " pixels", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "Height:", nullptr));
        spinBox_CustRes_height->setSuffix(QCoreApplication::translate("MainWindow", " pixels", nullptr));
        label_44->setText(QCoreApplication::translate("MainWindow", "Aspect Ratio:", nullptr));
        comboBox_AspectRatio_custRes->setItemText(0, QCoreApplication::translate("MainWindow", "Ignore Aspect Ratio", nullptr));
        comboBox_AspectRatio_custRes->setItemText(1, QCoreApplication::translate("MainWindow", "Keep Aspect Ratio", nullptr));
        comboBox_AspectRatio_custRes->setItemText(2, QCoreApplication::translate("MainWindow", "Keep Aspect Ratio By Expanding", nullptr));

        pushButton_CustRes_apply->setText(QCoreApplication::translate("MainWindow", "Apply", nullptr));
        pushButton_CustRes_cancel->setText(QCoreApplication::translate("MainWindow", "Cancel", nullptr));
        checkBox_custres_isAll->setText(QCoreApplication::translate("MainWindow", "Apply to all", nullptr));
        checkBox_AutoSkip_CustomRes->setText(QCoreApplication::translate("MainWindow", "Auto Skip", nullptr));
        groupBox_ImageOutputSettings->setTitle(QCoreApplication::translate("MainWindow", "Image Output Settings", nullptr));
        label_63->setText(QCoreApplication::translate("MainWindow", "Image quality:", nullptr));
        label_ImageStyle_W2xNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Image Style(waifu2x-ncnn-vulkan):", nullptr));
        label_ImageStyle_W2xCaffe->setText(QCoreApplication::translate("MainWindow", "Image Style(waifu2x-caffe):", nullptr));

        label_20->setText(QCoreApplication::translate("MainWindow", "Save image as:", nullptr));
        comboBox_ImageStyle->setItemText(0, QCoreApplication::translate("MainWindow", "2D Anime", nullptr));
        comboBox_ImageStyle->setItemText(1, QCoreApplication::translate("MainWindow", "3D Real-life", nullptr));

        comboBox_ImageStyle_Waifu2xCaffe->setItemText(0, QCoreApplication::translate("MainWindow", "2D Anime", nullptr));
        comboBox_ImageStyle_Waifu2xCaffe->setItemText(1, QCoreApplication::translate("MainWindow", "3D Real-life", nullptr));

        checkBox_DelOriginal->setText(QCoreApplication::translate("MainWindow", "Delete original files", nullptr));
        checkBox_ReplaceOriginalFile->setText(QCoreApplication::translate("MainWindow", "Replace original file", nullptr));
        checkBox_OptGIF->setText(QCoreApplication::translate("MainWindow", "Optimize GIF", nullptr));
        comboBox_FinishAction->setItemText(0, QCoreApplication::translate("MainWindow", "Do nothing(when finished)", nullptr));

        checkBox_ReProcFinFiles->setText(QCoreApplication::translate("MainWindow", "Re-process finished files", nullptr));
        groupBox_OutPut->setTitle(QCoreApplication::translate("MainWindow", "Output path(Folder):", nullptr));
        checkBox_OutPath_isEnabled->setText(QCoreApplication::translate("MainWindow", "Enabled", nullptr));
        checkBox_AutoOpenOutputPath->setText(QCoreApplication::translate("MainWindow", "Auto-open after finished", nullptr));
        checkBox_OutPath_Overwrite->setText(QCoreApplication::translate("MainWindow", "Keep parent folder", nullptr));
        label_57->setText(QCoreApplication::translate("MainWindow", "Scale ratio:", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Image", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Animated Image", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Video", nullptr));
        label_66->setText(QCoreApplication::translate("MainWindow", "Denoise level:", nullptr));
        label_ImageDenoiseLevel->setText(QCoreApplication::translate("MainWindow", "Image", nullptr));
        label_GIFDenoiseLevel->setText(QCoreApplication::translate("MainWindow", "Animated Image", nullptr));
        label_VideoDenoiseLevel->setText(QCoreApplication::translate("MainWindow", "Video", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "Multiple of FPS:", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_Home), QCoreApplication::translate("MainWindow", "Home", nullptr));
#if QT_CONFIG(tooltip)
        groupBox_NumOfThreads->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        groupBox_NumOfThreads->setTitle(QCoreApplication::translate("MainWindow", "Number of threads", nullptr));
#if QT_CONFIG(tooltip)
        label_11->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        label_11->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(whatsthis)
        label_11->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
        label_11->setText(QCoreApplication::translate("MainWindow", "Image:", nullptr));
#if QT_CONFIG(tooltip)
        label_13->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        label_13->setText(QCoreApplication::translate("MainWindow", "Video:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_ThreadNum_gif_internal->setToolTip(QCoreApplication::translate("MainWindow", "The number of threads that the software processes GIF,\n"
"which will determine how many frames the software will\n"
"process at the same time.\n"
"\n"
"The actual maximum number of threads is limited by the\n"
"number of enabled GPUs when using Anime4k, Waifu2x-Caffe\n"
"and Waifu2x-converter.(Only when processing GIF and Video)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        spinBox_ThreadNum_video_internal->setToolTip(QCoreApplication::translate("MainWindow", "The number of threads that the software processes Video,\n"
"which will determine how many frames the software will\n"
"process at the same time.\n"
"\n"
"The actual maximum number of threads is limited by the\n"
"number of enabled GPUs when using Anime4k, Waifu2x-Caffe\n"
"and Waifu2x-converter.(Only when processing GIF and Video)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_12->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        label_12->setText(QCoreApplication::translate("MainWindow", "Animated Image:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_ThreadNum_image->setToolTip(QCoreApplication::translate("MainWindow", "The number of threads that the software processes Image,\n"
"which will determine how many images the software will\n"
"process at the same time.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_NumThreadsActive->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        groupBox_Engine->setTitle(QCoreApplication::translate("MainWindow", "Engine", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Animated Image:", nullptr));
        comboBox_Engine_GIF->setItemText(3, QCoreApplication::translate("MainWindow", "[BETA] srmd-cuda", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Engine_GIF->setToolTip(QCoreApplication::translate("MainWindow", "Waifu2x-NCNN-Vulkan [Speed:\342\230\205\342\230\205   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Converter   [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime]\n"
"Anime4K             [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\206  ] [2D Anime]\n"
"SRMD-NCNN-Vulkan    [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Caffe       [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"RealSR-NCNN-Vulkan  [Speed:\342\230\206     Image quality:\342\230\205\342\230\205\342\230\205] [3D Real-life]\n"
"SRMD-CUDA           [Speed:\342\230\205\342\230\205\342\230\206 Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]", nullptr));
#endif // QT_CONFIG(tooltip)
        label_6->setText(QCoreApplication::translate("MainWindow", "Image:", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Video:", nullptr));
        comboBox_Engine_Video->setItemText(6, QCoreApplication::translate("MainWindow", "[BETA] srmd-cuda", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Engine_Video->setToolTip(QCoreApplication::translate("MainWindow", "Waifu2x-NCNN-Vulkan [Speed:\342\230\205\342\230\205   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Converter   [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime]\n"
"Anime4K             [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\206  ] [2D Anime]\n"
"SRMD-NCNN-Vulkan    [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Caffe       [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"RealSR-NCNN-Vulkan  [Speed:\342\230\206     Image quality:\342\230\205\342\230\205\342\230\205] [3D Real-life]\n"
"SRMD-CUDA           [Speed:\342\230\205\342\230\205\342\230\206 Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]", nullptr));
#endif // QT_CONFIG(tooltip)
        comboBox_Engine_Image->setItemText(3, QCoreApplication::translate("MainWindow", "[BETA] srmd-cuda", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Engine_Image->setToolTip(QCoreApplication::translate("MainWindow", "Waifu2x-NCNN-Vulkan [Speed:\342\230\205\342\230\205   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Converter   [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime]\n"
"Anime4K             [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\206  ] [2D Anime]\n"
"SRMD-NCNN-Vulkan    [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Caffe       [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"RealSR-NCNN-Vulkan  [Speed:\342\230\206     Image quality:\342\230\205\342\230\205\342\230\205] [3D Real-life]\n"
"SRMD-CUDA           [Speed:\342\230\205\342\230\205\342\230\206 Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_10->setToolTip(QCoreApplication::translate("MainWindow", "Select the GPU to use when enabling the waifu2x-ncnn-vulkan engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_10->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_GPUID->setToolTip(QCoreApplication::translate("MainWindow", "Select the GPU to use when enabling the waifu2x-ncnn-vulkan engine.\n"
"\342\230\205 -1 is CPU,others are GPUs \342\230\205", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_DetectGPU->setText(QCoreApplication::translate("MainWindow", "Detect available GPU ID", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_MultiGPU_Waifu2xNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "1.You need at least 2 available GPUs.\n"
"\n"
"2.If you wanna get all GPUs running together,\n"
"the number of threads must = the number of GPUs.\n"
"\n"
"3.You need to [Detect available GPU ID] first.\n"
"\n"
"*Only latest version fully supports Multi-GPU*", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_MultiGPU_Waifu2xNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Enable Multi-GPU", nullptr));
#if QT_CONFIG(tooltip)
        label_9->setToolTip(QCoreApplication::translate("MainWindow", "Tile size: This value will affects GPU memory usage.\n"
"Larger tile size means waifu2x will use more GPU memory and run faster.\n"
"Smaller tile size means waifu2x will use less GPU memory and run slower.\n"
"(Only works when using waifu2x-ncnn-vulkan engine.)", nullptr));
#endif // QT_CONFIG(tooltip)
        label_9->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_TileSize->setToolTip(QCoreApplication::translate("MainWindow", "Tile size: This value will affects GPU memory usage.\n"
"Larger Tile size means waifu2x will use more GPU memory and run faster.\n"
"Smaller Tile size means waifu2x will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox_GPUSettings_MultiGPU_Waifu2xNCNNVulkan->setTitle(QCoreApplication::translate("MainWindow", "GPU Settings(Multi-GPU)", nullptr));
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "Show current Multi-GPU settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Show GPUs Settings", nullptr));
        label_64->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
        label_65->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));
        label_47->setText(QCoreApplication::translate("MainWindow", "Version:", nullptr));
        comboBox_version_Waifu2xNCNNVulkan->setItemText(0, QCoreApplication::translate("MainWindow", "Latest(Alpha channel,TTA,Multi-GPU)", nullptr));
        comboBox_version_Waifu2xNCNNVulkan->setItemText(1, QCoreApplication::translate("MainWindow", "20200414-fp16p(TTA,Multi-GPU(Image only))", nullptr));
        comboBox_version_Waifu2xNCNNVulkan->setItemText(2, QCoreApplication::translate("MainWindow", "Old(Multi-GPU(Image only))", nullptr));

        label_26->setText(QCoreApplication::translate("MainWindow", "Model:", nullptr));
        comboBox_model_vulkan->setItemText(1, QCoreApplication::translate("MainWindow", "cunet(Only support 2D Anime)", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_model_vulkan->setToolTip(QCoreApplication::translate("MainWindow", "We recommend you use the default upconv_7 model.\n"
"Only upconv_7 model supports image style switching.\n"
"cunet model only supports 2D anime image style,\n"
"and it's much more slower than upconv_7 model.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_TTA_vulkan->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.\n"
"[TTA] is not available for [OLD Version] of waifu2x-ncnn-vulkan.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setTabToolTip(tabWidget_Engines->indexOf(tab_W2xNcnnVulkan), QCoreApplication::translate("MainWindow", "Waifu2x-NCNN-Vulkan:\n"
"\n"
"- Supports [2D Anime] and [3D Real-life] image style. But not at the\n"
"same time, you need to change [Image style] settings at [Home] tab.\n"
"\n"
"- More suitable for processing [2D Anime] image.\n"
"\n"
"- [ Speed:\342\230\205\342\230\205  Image Quality:\342\230\205\342\230\205\342\230\205 ]", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_ForceOpenCL_converter->setToolTip(QCoreApplication::translate("MainWindow", "Force to use OpenCL on Intel Platform.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_ForceOpenCL_converter->setText(QCoreApplication::translate("MainWindow", "Force OpenCL", nullptr));
        checkBox_DisableGPU_converter->setText(QCoreApplication::translate("MainWindow", "Disable GPU", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_TTA_converter->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_53->setText(QCoreApplication::translate("MainWindow", "Target processor:", nullptr));

        pushButton_DumpProcessorList_converter->setText(QCoreApplication::translate("MainWindow", "Dump processor list", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_MultiGPU_Waifu2xConverter->setToolTip(QCoreApplication::translate("MainWindow", "1.You need at least 2 available processors.\n"
"\n"
"2.If you wanna get all processors running together,\n"
"the number of threads must = the number of processors.\n"
"\n"
"3.You need to [Dump processor list] first.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_MultiGPU_Waifu2xConverter->setText(QCoreApplication::translate("MainWindow", "Enable Multi-Processor", nullptr));
#if QT_CONFIG(tooltip)
        label_54->setToolTip(QCoreApplication::translate("MainWindow", "Block size: This value will affects GPU memory usage.\n"
"Larger Block size means waifu2x will use more GPU memory and run faster.\n"
"Smaller Block size means waifu2x will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_54->setText(QCoreApplication::translate("MainWindow", "Block size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_BlockSize_converter->setToolTip(QCoreApplication::translate("MainWindow", "Block size: This value will affects GPU memory usage.\n"
"Larger Block size means waifu2x will use more GPU memory and run faster.\n"
"Smaller Block size means waifu2x will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox_GPUSettings_MultiGPU_Waifu2xConverter->setTitle(QCoreApplication::translate("MainWindow", "Processor Settings(Multi-Processor)", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_Waifu2xConverter->setToolTip(QCoreApplication::translate("MainWindow", "Show current Multi-Processor settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_Waifu2xConverter->setText(QCoreApplication::translate("MainWindow", "Show Processor Settings", nullptr));
        label_75->setText(QCoreApplication::translate("MainWindow", "Processor ID:", nullptr));
        label_74->setText(QCoreApplication::translate("MainWindow", "Block size:", nullptr));
        checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setTabToolTip(tabWidget_Engines->indexOf(tab_W2xConverter), QCoreApplication::translate("MainWindow", "Waifu2x-Converter:\n"
"\n"
"- Only supports [2D Anime] image style.\n"
"\n"
"- [ Speed:\342\230\205\342\230\206  Image Quality:\342\230\205\342\230\205\342\230\206 ]", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_OpenCLParallelIO_A4k->setToolTip(QCoreApplication::translate("MainWindow", "OpenCL parallel IO command queue\n"
"*Enable this might cause compatibility problem*", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_OpenCLParallelIO_A4k->setText(QCoreApplication::translate("MainWindow", "OpenCL parallel IO", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_ACNet_Anime4K->setToolTip(QCoreApplication::translate("MainWindow", "Enable ACNet.\n"
"Will improve image quality.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_HDNMode_Anime4k->setToolTip(QCoreApplication::translate("MainWindow", "Enable HDN mode for ACNet.\n"
"Will improve image quality.\n"
"\n"
"*This option will be deactivated\n"
"when [ACNet] is disabled*", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_HDNMode_Anime4k->setText(QCoreApplication::translate("MainWindow", "HDN mode", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_FastMode_Anime4K->setToolTip(QCoreApplication::translate("MainWindow", "Faster but maybe low quality.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_FastMode_Anime4K->setText(QCoreApplication::translate("MainWindow", "Fast mode", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "OpenCL command queues:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_OpenCLCommandQueues_A4k->setToolTip(QCoreApplication::translate("MainWindow", "Number of OpenCL command queues", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        lineEdit_GPUs_Anime4k->setToolTip(QCoreApplication::translate("MainWindow", "Format: Platform ID,Device ID:\n"
"\n"
"Example: 0,0:0,1:1,0:\n"
"\n"
"You must follow the format,otherwise the software may crash.\n"
"\n"
"If you wanna get all GPUs running together,\n"
"the number of threads must = the number of GPUs.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ListGPUs_Anime4k->setText(QCoreApplication::translate("MainWindow", "List GPUs", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_GPGPUModel_A4k->setToolTip(QCoreApplication::translate("MainWindow", "GPGPU model.\n"
"\n"
"Only NVIDIA products support CUDA.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_GPUMode_Anime4K->setToolTip(QCoreApplication::translate("MainWindow", "Enable GPU acceleration.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_GPUMode_Anime4K->setText(QCoreApplication::translate("MainWindow", "GPU Mode", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_VerifyGPUsConfig_Anime4k->setToolTip(QCoreApplication::translate("MainWindow", "Verify your GPUs configuration.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_VerifyGPUsConfig_Anime4k->setText(QCoreApplication::translate("MainWindow", "Verify", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_SpecifyGPU_Anime4k->setToolTip(QCoreApplication::translate("MainWindow", "You can specify multiple GPUs to fully\n"
"utilize the capabilities of your PC.\n"
"(If there are multiple GPUs available.)", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_SpecifyGPU_Anime4k->setText(QCoreApplication::translate("MainWindow", "Specify GPUs:", nullptr));
#if QT_CONFIG(tooltip)
        label_50->setToolTip(QCoreApplication::translate("MainWindow", "Strength for pushing color,range 0 to 1,higher for thinner.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_50->setText(QCoreApplication::translate("MainWindow", "Push color strength:", nullptr));
#if QT_CONFIG(tooltip)
        label_48->setToolTip(QCoreApplication::translate("MainWindow", "Passes for processing.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_48->setText(QCoreApplication::translate("MainWindow", "Passes:", nullptr));
#if QT_CONFIG(tooltip)
        label_51->setToolTip(QCoreApplication::translate("MainWindow", "Strength for pushing gradient,range 0 to 1,higher for sharper.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_51->setText(QCoreApplication::translate("MainWindow", "Push gradient strength:", nullptr));
#if QT_CONFIG(tooltip)
        label_49->setToolTip(QCoreApplication::translate("MainWindow", "Limit the number of color pushes.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_49->setText(QCoreApplication::translate("MainWindow", "Push color count:", nullptr));
#if QT_CONFIG(tooltip)
        groupBox_PreProcessing_Anime4k->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        groupBox_PreProcessing_Anime4k->setTitle(QCoreApplication::translate("MainWindow", "Pre-processing", nullptr));
        checkBox_MeanBlur_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "Mean blur", nullptr));
        checkBox_MedianBlur_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "Median blur", nullptr));
        checkBox_GaussianBlurWeak_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "Gaussian blur weak", nullptr));
        checkBox_CASSharping_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "CAS Sharpening", nullptr));
        checkBox_GaussianBlur_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "Gaussian blur", nullptr));
        checkBox_BilateralFilter_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "Bilateral filter", nullptr));
        checkBox_BilateralFilterFaster_Pre_Anime4k->setText(QCoreApplication::translate("MainWindow", "Bilateral filter faster", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_EnablePreProcessing_Anime4k->setToolTip(QCoreApplication::translate("MainWindow", "Apply filters to image before process it.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_EnablePreProcessing_Anime4k->setText(QCoreApplication::translate("MainWindow", "Enable Pre-processing", nullptr));
#if QT_CONFIG(tooltip)
        groupBox_PostProcessing_Anime4k->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        groupBox_PostProcessing_Anime4k->setTitle(QCoreApplication::translate("MainWindow", "Post-processing", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_EnablePostProcessing_Anime4k->setToolTip(QCoreApplication::translate("MainWindow", "Add filters to the image after processing it.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_EnablePostProcessing_Anime4k->setText(QCoreApplication::translate("MainWindow", "Enable Post-processing", nullptr));
        checkBox_MedianBlur_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "Median blur", nullptr));
        checkBox_MeanBlur_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "Mean blur", nullptr));
        checkBox_CASSharping_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "CAS Sharpening", nullptr));
        checkBox_GaussianBlur_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "Gaussian blur", nullptr));
        checkBox_BilateralFilter_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "Bilateral filter", nullptr));
        checkBox_GaussianBlurWeak_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "Gaussian blur weak", nullptr));
        checkBox_BilateralFilterFaster_Post_Anime4k->setText(QCoreApplication::translate("MainWindow", "Bilateral filter faster", nullptr));
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setTabToolTip(tabWidget_Engines->indexOf(tab_A4k), QCoreApplication::translate("MainWindow", "Anime4K:\n"
"\n"
"- Only supports [2D Anime] image style.\n"
"\n"
"- More suitable for image with not too bad quality.\n"
"\n"
"- You can enable [ACNet] algorithm to improve image quality.\n"
"\n"
"- Although [GPU Mode] is not enabled by default, in most cases,\n"
"enabling [GPU Mode] can greatly increase the processing speed.\n"
"(Please make sure your PC is compatible with [GPU Mode] first)\n"
"\n"
"- [ Speed:\342\230\205\342\230\205\342\230\205  Image Quality:\342\230\205 ]", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox_GPUSettings_MultiGPU_SrmdNCNNVulkan->setTitle(QCoreApplication::translate("MainWindow", "GPU Settings(Multi-GPU)", nullptr));
        checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
        label_68->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "Show current Multi-GPU settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Show GPUs Settings", nullptr));
        label_69->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));
        label_41->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_GPUID_srmd->setToolTip(QCoreApplication::translate("MainWindow", "Select the GPU to use when enabling the srmd-ncnn-vulkan engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_DetectGPUID_srmd->setText(QCoreApplication::translate("MainWindow", "Detect available GPU ID", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_MultiGPU_SrmdNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "1.You need at least 2 available GPUs.\n"
"\n"
"2.If you wanna get all GPUs running together,\n"
"the number of threads must = the number of GPUs.\n"
"\n"
"3.You need to [Detect available GPU ID] first.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_MultiGPU_SrmdNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Enable Multi-GPU", nullptr));
#if QT_CONFIG(tooltip)
        label_40->setToolTip(QCoreApplication::translate("MainWindow", "Tile size: This value will affects GPU memory usage.\n"
"Larger tile size means SRMD will use more GPU memory and run faster.\n"
"Smaller tile size means SRMD will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_40->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_TileSize_srmd->setToolTip(QCoreApplication::translate("MainWindow", "Tile size: This value will affects GPU memory usage.\n"
"Larger tile size means SRMD will use more GPU memory and run faster.\n"
"Smaller tile size means SRMD will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_TTA_srmd->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setTabToolTip(tabWidget_Engines->indexOf(tab_SrmdNcnnVulkan), QCoreApplication::translate("MainWindow", "SRMD-NCNN-Vulkan:\n"
"\n"
"- Supports [2D Anime] and [3D Real-life] image style at the same time.\n"
"\n"
"- More suitable for processing [3D Real-life] images.\n"
"\n"
"- [ Speed:\342\230\205\342\230\206  Image Quality:\342\230\205\342\230\205\342\230\205 ]", nullptr));
#endif // QT_CONFIG(tooltip)
        label_56->setText(QCoreApplication::translate("MainWindow", "3D Real-life Model:", nullptr));
        comboBox_Model_3D_Waifu2xCaffe->setItemText(0, QCoreApplication::translate("MainWindow", "upconv_7_photo", nullptr));
        comboBox_Model_3D_Waifu2xCaffe->setItemText(1, QCoreApplication::translate("MainWindow", "photo", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Model_3D_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Model for processing 3D Real-life style image.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_43->setToolTip(QCoreApplication::translate("MainWindow", "The batch size defines the number of samples that\n"
"will be propagated through the network.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_43->setText(QCoreApplication::translate("MainWindow", "Batch size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_BatchSize_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "The batch size defines the number of samples that\n"
"will be propagated through the network.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_14->setText(QCoreApplication::translate("MainWindow", "Process mode:", nullptr));

#if QT_CONFIG(tooltip)
        checkBox_TTA_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_EnableMultiGPU_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "You need to switch [Process mode] to\n"
"[GPU] or [cuDNN] to enable Multi-GPU.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_EnableMultiGPU_Waifu2xCaffe->setText(QCoreApplication::translate("MainWindow", "Multi-GPU:", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_MultiGPUInfo_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Format: GPU ID,Batch size,Split size:\n"
"\n"
"Example: 0,1,128:1,2,64:2,1,128:\n"
"\n"
"You must follow the format,otherwise the software may crash.\n"
"\n"
"If you wanna get all GPUs running together,\n"
"the number of threads must = the number of GPUs.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pushButton_VerifyGPUsConfig_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Verify your GPUs configuration.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_VerifyGPUsConfig_Waifu2xCaffe->setText(QCoreApplication::translate("MainWindow", "Verify", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "2D Anime Model:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Model_2D_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Model for processing 2D Anime style image.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_38->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_GPUID_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Select the GPU to use when enabling the waifu2x-caffe engine.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_52->setToolTip(QCoreApplication::translate("MainWindow", "Split size: This value will affects GPU memory usage.\n"
"Larger Split size means waifu2x will use more GPU memory and run faster.\n"
"Smaller Split size means waifu2x will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_52->setText(QCoreApplication::translate("MainWindow", "Split size:", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_SplitSize_Minus_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Decrease split size.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        spinBox_SplitSize_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Split size: This value will affects GPU memory usage.\n"
"Larger Split size means waifu2x will use more GPU memory and run faster.\n"
"Smaller Split size means waifu2x will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pushButton_SplitSize_Add_Waifu2xCaffe->setToolTip(QCoreApplication::translate("MainWindow", "Increase split size.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setTabToolTip(tabWidget_Engines->indexOf(tab_W2xCaffe), QCoreApplication::translate("MainWindow", "Waifu2x-Caffe:\n"
"\n"
"- Supports [2D Anime] and [3D Real-life] image style. But not at the\n"
"same time, you need to change [Image style] settings at [Home] tab.\n"
"\n"
"- More suitable for processing [2D Anime] images.\n"
"\n"
"- When using [CPU Process mode], it's very slow, so only using this\n"
"engine when your PC is compatible with [GPU(or cuDNN) Process mode]\n"
"is recommended.\n"
"\n"
"- [ Speed:\342\230\205\342\230\206  Image Quality:\342\230\205\342\230\205\342\230\205 ]", nullptr));
#endif // QT_CONFIG(tooltip)
        label_60->setText(QCoreApplication::translate("MainWindow", "Model:", nullptr));
        comboBox_Model_RealsrNCNNVulkan->setItemText(0, QCoreApplication::translate("MainWindow", "models-DF2K_JPEG(Supports denoise)", nullptr));
        comboBox_Model_RealsrNCNNVulkan->setItemText(1, QCoreApplication::translate("MainWindow", "models-DF2K(Does NOT supports denoise)", nullptr));

#if QT_CONFIG(tooltip)
        checkBox_TTA_RealsrNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_62->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_GPUID_RealsrNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "Select the GPU to use when enabling the Realsr-ncnn-vulkan engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_DetectGPU_RealsrNCNNVulkan->setText(QCoreApplication::translate("MainWindow", "Detect available GPU ID", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_MultiGPU_RealSRNcnnVulkan->setToolTip(QCoreApplication::translate("MainWindow", "1.You need at least 2 available GPUs.\n"
"\n"
"2.If you wanna get all GPUs running together,\n"
"the number of threads must = the number of GPUs.\n"
"\n"
"3.You need to [Detect available GPU ID] first.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_MultiGPU_RealSRNcnnVulkan->setText(QCoreApplication::translate("MainWindow", "Enable Multi-GPU", nullptr));
        label_61->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_TileSize_RealsrNCNNVulkan->setToolTip(QCoreApplication::translate("MainWindow", "Tile size: This value will affects GPU memory usage.\n"
"Larger tile size means Realsr will use more GPU memory and run faster.\n"
"Smaller tile size means Realsr will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox_GPUSettings_MultiGPU_RealSRNcnnVulkan->setTitle(QCoreApplication::translate("MainWindow", "GPU Settings(Multi-GPU)", nullptr));
        checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
        label_73->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));
        label_72->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan->setToolTip(QCoreApplication::translate("MainWindow", "Show current Multi-GPU settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_RealesrganNcnnVulkan->setText(QCoreApplication::translate("MainWindow", "Show GPUs Settings", nullptr));
#if QT_CONFIG(tooltip)
        tabWidget_Engines->setTabToolTip(tabWidget_Engines->indexOf(tab_RealSRNcnnVulkan), QCoreApplication::translate("MainWindow", "RealSR-NCNN-Vulkan:\n"
"\n"
"- Only support [3D Real-life] image style.\n"
"\n"
"- Best engine for processing [3D Real-life] images.\n"
"\n"
"- Much more slower than other engines.\n"
"\n"
"- [ Speed:\342\230\206  Image Quality:\342\230\205\342\230\205\342\230\205 ]", nullptr));
#endif // QT_CONFIG(tooltip)
        label_Model_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "Model:", nullptr));
        comboBox_Model_RealESRGAN->setItemText(0, QCoreApplication::translate("MainWindow", "realesr-animevideov3 (Supports denoise, video)", nullptr));
        comboBox_Model_RealESRGAN->setItemText(1, QCoreApplication::translate("MainWindow", "realesrgan-x4plus (Supports denoise)", nullptr));
        comboBox_Model_RealESRGAN->setItemText(2, QCoreApplication::translate("MainWindow", "realesrgan-x4plus-anime (Supports denoise, anime)", nullptr));

#if QT_CONFIG(tooltip)
        checkBox_TTA_RealESRGAN->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_TileSize_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_TileSize_RealESRGAN->setToolTip(QCoreApplication::translate("MainWindow", "Tile size: This value will affects GPU memory usage.\n"
"Larger tile size means RealESRGAN will use more GPU memory and run faster.\n"
"Smaller tile size means RealESRGAN will use less GPU memory and run slower.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_GPUID_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_GPUID_RealESRGAN->setToolTip(QCoreApplication::translate("MainWindow", "Select the GPU to use when enabling the RealESRGAN-ncnn-vulkan engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_DetectGPU_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "Detect available GPU ID", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_MultiGPU_RealESRGANEngine->setToolTip(QCoreApplication::translate("MainWindow", "1.You need at least 2 available GPUs.\n"
"\n"
"2.If you wanna get all GPUs running together,\n"
"the number of threads must = the number of GPUs.\n"
"\n"
"3.You need to [Detect available GPU ID] first.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_MultiGPU_RealESRGANEngine->setText(QCoreApplication::translate("MainWindow", "Enable Multi-GPU", nullptr));
        groupBox_GPUSettings_MultiGPU_RealESRGAN->setTitle(QCoreApplication::translate("MainWindow", "GPU Settings(Multi-GPU)", nullptr));
        checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
        label_MultiGPUID_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));
        label_MultiGPUTile_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_RealESRGAN->setToolTip(QCoreApplication::translate("MainWindow", "Show current Multi-GPU settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ShowMultiGPUSettings_RealESRGAN->setText(QCoreApplication::translate("MainWindow", "Show GPUs Settings", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_EngineSettings), QCoreApplication::translate("MainWindow", "Engine settings", nullptr));
        groupBox_AudioDenoise->setTitle(QCoreApplication::translate("MainWindow", "Audio denoise(for video)", nullptr));
#if QT_CONFIG(tooltip)
        doubleSpinBox_AudioDenoiseLevel->setToolTip(QCoreApplication::translate("MainWindow", "How much noise should be removed is specified by amount-a\n"
"number between 0.01 and 1 with a default of 0.20. Higher\n"
"numbers will remove more noise but present a greater\n"
"likelihood of removing wanted components of the audio signal.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_46->setText(QCoreApplication::translate("MainWindow", "Denoise level:", nullptr));
        checkBox_AudioDenoise->setText(QCoreApplication::translate("MainWindow", "Enabled", nullptr));
        groupBox_ProcessVideoBySegment->setTitle(QCoreApplication::translate("MainWindow", "Process video by segment", nullptr));
#if QT_CONFIG(tooltip)
        label_SegmentDuration->setToolTip(QCoreApplication::translate("MainWindow", "When processing video in segments, the length of each video clip.\n"
"This will determine how much hard disk space the program will occupy\n"
"when processing video in segments.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_SegmentDuration->setText(QCoreApplication::translate("MainWindow", "Segment duration:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_VideoSplitDuration->setToolTip(QCoreApplication::translate("MainWindow", "When processing video in segments, the length of each video clip.\n"
"This will determine how much hard disk space the program will occupy\n"
"when processing video in segments.", nullptr));
#endif // QT_CONFIG(tooltip)
        spinBox_VideoSplitDuration->setSuffix(QCoreApplication::translate("MainWindow", " secs", nullptr));
        spinBox_VideoSplitDuration->setPrefix(QString());
#if QT_CONFIG(tooltip)
        checkBox_ProcessVideoBySegment->setToolTip(QCoreApplication::translate("MainWindow", "Processing video in segments can greatly reduce the\n"
"hard disk space occupied by processing video. But it\n"
"will increase the time required to process video.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_ProcessVideoBySegment->setText(QCoreApplication::translate("MainWindow", "Enabled", nullptr));
        groupBox_FrameInterpolation->setTitle(QCoreApplication::translate("MainWindow", "Frame Interpolation", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_UHD_VFI->setToolTip(QCoreApplication::translate("MainWindow", "You should enable this option when processing Ultra High Definition videos.\n"
"\n"
"The software will automatically enable UHD Mode when it detects input with\n"
"resolution higher than 3840\303\2272160.\n"
"\n"
"Only works when using [rife-ncnn-vulkan] engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_UHD_VFI->setText(QCoreApplication::translate("MainWindow", "UHD Mode", nullptr));
        label_37->setText(QCoreApplication::translate("MainWindow", "Frame Interpolation Engine:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Engine_VFI->setToolTip(QCoreApplication::translate("MainWindow", "RIFE-NCNN-Vulkan [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime][3D Real-life]\n"
"CAIN-NCNN-Vulkan [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime][3D Real-life]\n"
"DAIN-NCNN-Vulkan [Speed:\342\230\206     Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_TTA_VFI->setToolTip(QCoreApplication::translate("MainWindow", "TTA(Test-Time Augmentation):\n"
"The processing time is eight times longer than when unchecked,\n"
"and the peak signal-to-noise ratio (PSNR) is increased by 0.15,\n"
"but the effect may not be obvious. It is not recommended to enable it.\n"
"\n"
"Only works when using [rife-ncnn-vulkan] engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_TTA_VFI->setText(QCoreApplication::translate("MainWindow", "TTA Mode", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_VfiAfterScale_VFI->setToolTip(QCoreApplication::translate("MainWindow", "Interpolate frames after upscaled.\n"
"\n"
"By default the frames will be interpolated before upscaled,\n"
"because PC with low vram will encounter difficulties when\n"
"interpolate high res frames.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_VfiAfterScale_VFI->setText(QCoreApplication::translate("MainWindow", "After Upscale", nullptr));
        label_30->setText(QCoreApplication::translate("MainWindow", "GPU ID:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_GPUID_VFI->setToolTip(QCoreApplication::translate("MainWindow", "-1 is CPU, others are GPUs", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_DetectGPU_VFI->setText(QCoreApplication::translate("MainWindow", "Detect available GPU ID", nullptr));
        checkBox_MultiGPU_VFI->setText(QCoreApplication::translate("MainWindow", "Multi GPU", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_MultiGPU_IDs_VFI->setToolTip(QCoreApplication::translate("MainWindow", "Format: GPU ID,GPU ID\n"
"\n"
"Example: 0,1\n"
"\n"
"You must follow the format,otherwise the software may crash.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pushButton_Verify_MultiGPU_VFI->setToolTip(QCoreApplication::translate("MainWindow", "Verify your Multi GPU configuration.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_Verify_MultiGPU_VFI->setText(QCoreApplication::translate("MainWindow", "Verify", nullptr));
#if QT_CONFIG(tooltip)
        label_19->setToolTip(QCoreApplication::translate("MainWindow", "This value will affects GPU memory usage.\n"
"Only works when using [dain-ncnn-vulkan] engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_19->setText(QCoreApplication::translate("MainWindow", "Tile size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_TileSize_VFI->setToolTip(QCoreApplication::translate("MainWindow", "This value will affects GPU memory usage.\n"
"Only works when using [dain-ncnn-vulkan] engine.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_3->setText(QCoreApplication::translate("MainWindow", "Model:", nullptr));

#if QT_CONFIG(tooltip)
        comboBox_Model_VFI->setToolTip(QCoreApplication::translate("MainWindow", "Choose the model for rife-ncnn-vulkan engine.\n"
"\n"
"We recommend you to use the default model rife-v2.4.\n"
"\n"
"Only works when using [rife-ncnn-vulkan] engine.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_MultiThread_VFI->setToolTip(QCoreApplication::translate("MainWindow", "The multi-threading of the frame interpolation engines is NOT\n"
"very stable, we don't recommend you to enable this option.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_MultiThread_VFI->setText(QCoreApplication::translate("MainWindow", "Multithreading:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_NumOfThreads_VFI->setToolTip(QCoreApplication::translate("MainWindow", "Number of threads.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_AutoAdjustNumOfThreads_VFI->setToolTip(QCoreApplication::translate("MainWindow", "When successive failures are detected, the number\n"
"of threads will be adjusted automatically.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_AutoAdjustNumOfThreads_VFI->setText(QCoreApplication::translate("MainWindow", "Auto adjust", nullptr));
        groupBox_video_settings->setTitle(QCoreApplication::translate("MainWindow", "Custom video settings", nullptr));
        groupBox_OutputVideoSettings->setTitle(QCoreApplication::translate("MainWindow", "When output the result video", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "Encoder(video):", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_encoder_vid->setToolTip(QCoreApplication::translate("MainWindow", "If you leave it empty, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_27->setText(QCoreApplication::translate("MainWindow", "Bitrate(video):", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_bitrate_vid->setToolTip(QCoreApplication::translate("MainWindow", "If you set it to 0, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_32->setText(QCoreApplication::translate("MainWindow", "Encoder(audio):", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_encoder_audio->setToolTip(QCoreApplication::translate("MainWindow", "If you leave it empty, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_29->setText(QCoreApplication::translate("MainWindow", "Bitrate(audio):", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_bitrate_audio->setToolTip(QCoreApplication::translate("MainWindow", "If you set it to 0, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_33->setText(QCoreApplication::translate("MainWindow", "Pixel format:", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_pixformat->setToolTip(QCoreApplication::translate("MainWindow", "If you leave it empty, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_42->setText(QCoreApplication::translate("MainWindow", "Extra command:", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_ExCommand_output->setToolTip(QCoreApplication::translate("MainWindow", "You can enter ffmpeg instructions here.", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox_ToMp4VideoSettings->setTitle(QCoreApplication::translate("MainWindow", "When convert source video to CFR mp4", nullptr));
        label_34->setText(QCoreApplication::translate("MainWindow", "Bitrate(video):", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_bitrate_vid_2mp4->setToolTip(QCoreApplication::translate("MainWindow", "If you set it to 0, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_35->setText(QCoreApplication::translate("MainWindow", "Bitrate(audio):", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_bitrate_audio_2mp4->setToolTip(QCoreApplication::translate("MainWindow", "If you set it to 0, software will use auto settings.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_acodec_copy_2mp4->setToolTip(QCoreApplication::translate("MainWindow", "When this is enabled, ffmpeg will copy the audio part of\n"
"the original video directly to the mp4, may cause error.\n"
"\n"
"Command: -acodec copy\n"
"\n"
"This option will not take effect when source video's frame\n"
"rate mode is variable.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_acodec_copy_2mp4->setText(QCoreApplication::translate("MainWindow", "Copy audio stream", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_vcodec_copy_2mp4->setToolTip(QCoreApplication::translate("MainWindow", "When this is enabled, ffmpeg will copy the video part of\n"
"the original video directly to the mp4, may cause error.\n"
"\n"
"Command: -vcodec copy\n"
"\n"
"This option will not take effect when source video's frame\n"
"rate mode is variable.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_vcodec_copy_2mp4->setText(QCoreApplication::translate("MainWindow", "Copy video stream", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_IgnoreFrameRateMode->setToolTip(QCoreApplication::translate("MainWindow", "Ignore the frame rate mode of the source video.\n"
"Might cause ERROR.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_IgnoreFrameRateMode->setText(QCoreApplication::translate("MainWindow", "Ignore frame rate mode", nullptr));
        label_45->setText(QCoreApplication::translate("MainWindow", "Extra command:", nullptr));
#if QT_CONFIG(tooltip)
        lineEdit_ExCommand_2mp4->setToolTip(QCoreApplication::translate("MainWindow", "You can enter ffmpeg instructions here.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pushButton_ResetVideoSettings->setToolTip(QCoreApplication::translate("MainWindow", "Reset video settings", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_ResetVideoSettings->setText(QCoreApplication::translate("MainWindow", "Reset video settings", nullptr));
        pushButton_encodersList->setText(QCoreApplication::translate("MainWindow", "List Available Encoders", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_VideoSettings), QCoreApplication::translate("MainWindow", "Video settings", nullptr));
        groupBox_InputExt->setTitle(QCoreApplication::translate("MainWindow", "Input file extensions", nullptr));
#if QT_CONFIG(tooltip)
        label_4->setToolTip(QCoreApplication::translate("MainWindow", "When you find that the file format you want to process cannot\n"
"be added to the file list, you can add the file extension yourself\n"
"in the additional settings.\n"
"\n"
"Each extension needs to be separated by :\n"
"\n"
"(There is no guarantee that the file format you add will\n"
"be processed successfully)", nullptr));
#endif // QT_CONFIG(tooltip)
        label_4->setText(QCoreApplication::translate("MainWindow", "Image:", nullptr));
#if QT_CONFIG(tooltip)
        Ext_image->setToolTip(QCoreApplication::translate("MainWindow", "When you find that the file format you want to process cannot\n"
"be added to the file list, you can add the file extension yourself\n"
"in the additional settings.\n"
"\n"
"Each extension needs to be separated by :\n"
"\n"
"(There is no guarantee that the file format you add will\n"
"be processed successfully)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        label_5->setToolTip(QCoreApplication::translate("MainWindow", "When you find that the file format you want to process cannot\n"
"be added to the file list, you can add the file extension yourself\n"
"in the additional settings.\n"
"\n"
"Each extension needs to be separated by :\n"
"\n"
"(There is no guarantee that the file format you add will\n"
"be processed successfully)", nullptr));
#endif // QT_CONFIG(tooltip)
        label_5->setText(QCoreApplication::translate("MainWindow", "Video:", nullptr));
#if QT_CONFIG(tooltip)
        Ext_video->setToolTip(QCoreApplication::translate("MainWindow", "When you find that the file format you want to process cannot\n"
"be added to the file list, you can add the file extension yourself\n"
"in the additional settings.\n"
"\n"
"Each extension needs to be separated by :\n"
"\n"
"(There is no guarantee that the file format you add will\n"
"be processed successfully)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        checkBox_PreProcessImage->setToolTip(QCoreApplication::translate("MainWindow", "Convert every image to PNG before processing it.\n"
"Enabling this option will improve compatibility. However, it will\n"
"increase the time and storage space required to process image.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_PreProcessImage->setText(QCoreApplication::translate("MainWindow", "Pre-process all images", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_AutoDetectAlphaChannel->setToolTip(QCoreApplication::translate("MainWindow", "When the alpha channel is detected in the picture, software will\n"
"automatically force the picture to be saved as PNG.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_AutoDetectAlphaChannel->setText(QCoreApplication::translate("MainWindow", "Auto detect Alpha channel", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_AlwaysPreProcessAlphaPNG->setToolTip(QCoreApplication::translate("MainWindow", "This software already can intelligently detect whether the Alpha channel is lost,\n"
"and automatically reprocess the picture. And image will be pre-processed during the\n"
"reporcess so the alpha channel won't lost again.\n"
"\n"
"However, this will cause additional time consumption. If re-processing\n"
"frequently occurs when processing images, you can enable this option to save time.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_AlwaysPreProcessAlphaPNG->setText(QCoreApplication::translate("MainWindow", "Always pre-process images with Alpha Channel", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Custom Font Settings (Restart the software to take effect)", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "Custom font:", nullptr));
        pushButton_Save_GlobalFontSize->setText(QCoreApplication::translate("MainWindow", "Save Custom Font Settings", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_isCustFontEnable->setToolTip(QCoreApplication::translate("MainWindow", "Enable Custom Font Settings.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_isCustFontEnable->setText(QCoreApplication::translate("MainWindow", "Enable", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "Custom font size:", nullptr));
        groupBox_other_1->setTitle(QString());
        pushButton_SaveSettings->setText(QCoreApplication::translate("MainWindow", "Save settings", nullptr));
        pushButton_ResetSettings->setText(QCoreApplication::translate("MainWindow", "Reset settings", nullptr));
        pushButton_CheckUpdate->setText(QCoreApplication::translate("MainWindow", "Check update", nullptr));
        pushButton_Report->setText(QCoreApplication::translate("MainWindow", "Report issue", nullptr));
        pushButton_ReadMe->setText(QCoreApplication::translate("MainWindow", "Official website", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_wiki->setToolTip(QCoreApplication::translate("MainWindow", "Open Waifu2x-Extension-GUI online wiki.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_wiki->setText(QCoreApplication::translate("MainWindow", "Wiki", nullptr));
        pushButton_about->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        pushButton_SupportersList->setText(QCoreApplication::translate("MainWindow", "Top Supporters", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_UpdatePopup->setToolTip(QCoreApplication::translate("MainWindow", "A pop-up window will prompts when an update is detected.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_UpdatePopup->setText(QCoreApplication::translate("MainWindow", "Update popup", nullptr));
        checkBox_MinimizeToTaskbar->setText(QCoreApplication::translate("MainWindow", "Minimize to taskbar", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_AutoSaveSettings->setToolTip(QCoreApplication::translate("MainWindow", "Settings will be saved automatically when you close the software.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_AutoSaveSettings->setText(QCoreApplication::translate("MainWindow", "Auto save settings", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_AlwaysHideTextBrowser->setToolTip(QCoreApplication::translate("MainWindow", "[Text browser] will be automatically hidden when the software starts.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_AlwaysHideTextBrowser->setText(QCoreApplication::translate("MainWindow", "Always hide Text Browser", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_ScanSubFolders->setToolTip(QCoreApplication::translate("MainWindow", "When adding a folder to the file list, scan the files in\n"
"the subfolder and add them to the file list.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_ScanSubFolders->setText(QCoreApplication::translate("MainWindow", "Scan sub-folders", nullptr));
        checkBox_PromptWhenExit->setText(QCoreApplication::translate("MainWindow", "Prompt when exit", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_DisableResize_gif->setToolTip(QCoreApplication::translate("MainWindow", "This option will be automatically enabled to improve\n"
"performance and fix issues when the software detects\n"
"that your PC has compatibility issues with ImageMagick.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_DisableResize_gif->setText(QCoreApplication::translate("MainWindow", "Disable -resize", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_BanGitee->setToolTip(QCoreApplication::translate("MainWindow", "Stop downloading any data from Gitee.\n"
"\n"
"When you cannot access github, enabling\n"
"this option will affect the automatic\n"
"detection of new updates.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_BanGitee->setText(QCoreApplication::translate("MainWindow", "Ban Gitee", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_FileListAutoSlide->setToolTip(QCoreApplication::translate("MainWindow", "When the file processing status changes, the file list scrolls automatically.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_FileListAutoSlide->setText(QCoreApplication::translate("MainWindow", "Automatic file list scrolling", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_ShowInterPro->setToolTip(QCoreApplication::translate("MainWindow", "Output internal Scale and Denoise progress of GIF and video process inside textbrowser.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_ShowInterPro->setText(QCoreApplication::translate("MainWindow", "Show internal progress", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_AlwaysHideSettings->setToolTip(QCoreApplication::translate("MainWindow", "[Settings] will be automatically hidden when the software starts.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_AlwaysHideSettings->setText(QCoreApplication::translate("MainWindow", "Always hide Settings", nullptr));
        checkBox_FileList_Interactive->setText(QCoreApplication::translate("MainWindow", "Interactive file list", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_KeepVideoCache->setToolTip(QCoreApplication::translate("MainWindow", "Keep video cache after processing the video.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_KeepVideoCache->setText(QCoreApplication::translate("MainWindow", "Keep video cache", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_SummaryPopup->setToolTip(QCoreApplication::translate("MainWindow", "Show the summary pop-up window after processing all files.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_SummaryPopup->setText(QCoreApplication::translate("MainWindow", "Summary popup", nullptr));
#if QT_CONFIG(tooltip)
        checkBox_NfSound->setToolTip(QCoreApplication::translate("MainWindow", "Play Notification sound.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_NfSound->setText(QCoreApplication::translate("MainWindow", "Notification sound", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_AdditionalSettings), QCoreApplication::translate("MainWindow", "Additional settings", nullptr));
        groupBox_CompatibilityTestRes->setTitle(QCoreApplication::translate("MainWindow", "Compatibility test results", nullptr));
        label_58->setText(QCoreApplication::translate("MainWindow", "Super-Resolution Engines:", nullptr));
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setText(QCoreApplication::translate("MainWindow", "Waifu2x-ncnn-vulkan(Latest)", nullptr));
        checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setText(QCoreApplication::translate("MainWindow", "Waifu2x-ncnn-vulkan(OLD)", nullptr));
        checkBox_isCompatible_SRMD_CUDA->setText(QCoreApplication::translate("MainWindow", "SRMD-CUDA [BETA]", nullptr));
        label_59->setText(QCoreApplication::translate("MainWindow", "Plugins:", nullptr));
        label_36->setText(QCoreApplication::translate("MainWindow", "Frame Interpolation Engines:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "[FFmpeg, FFprobe, ImageMagick, Gifsicle, SoX], one of the Super-Resolution engines and one of the Frame Interpolation engines must be compatible with your computer, to make sure you can use all functions in this software.\n"
"\n"
"Waifu2x-NCNN-Vulkan [Speed:\342\230\205\342\230\205   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Converter   [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime]\n"
"Anime4K             [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\206  ] [2D Anime]\n"
"SRMD-NCNN-Vulkan    [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"Waifu2x-Caffe       [Speed:\342\230\205\342\230\206   Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"RealSR-NCNN-Vulkan  [Speed:\342\230\206     Image quality:\342\230\205\342\230\205\342\230\205] [3D Real-life]\n"
"SRMD-CUDA           [Speed:\342\230"
                        "\205\342\230\205\342\230\206 Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]\n"
"\n"
"RIFE-NCNN-Vulkan [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime][3D Real-life]\n"
"CAIN-NCNN-Vulkan [Speed:\342\230\205\342\230\205\342\230\205 Image quality:\342\230\205\342\230\205\342\230\206] [2D Anime][3D Real-life]\n"
"DAIN-NCNN-Vulkan [Speed:\342\230\206     Image quality:\342\230\205\342\230\205\342\230\205] [2D Anime][3D Real-life]", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_compatibilityTest->setToolTip(QCoreApplication::translate("MainWindow", "Run a compatibility test to see which engines your computer is compatible with.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_compatibilityTest->setText(QCoreApplication::translate("MainWindow", "Start compatibility test", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_CompatibilityTest), QCoreApplication::translate("MainWindow", "Compatibility test", nullptr));
#if QT_CONFIG(tooltip)
        pushButton_clear_textbrowser->setToolTip(QCoreApplication::translate("MainWindow", "Clear text browser.", nullptr));
#endif // QT_CONFIG(tooltip)
        pushButton_clear_textbrowser->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
#if QT_CONFIG(tooltip)
        label_TextBroFontSize->setToolTip(QCoreApplication::translate("MainWindow", "Change the font size of the text browser.", nullptr));
#endif // QT_CONFIG(tooltip)
        label_TextBroFontSize->setText(QCoreApplication::translate("MainWindow", "Font size:", nullptr));
#if QT_CONFIG(tooltip)
        spinBox_textbrowser_fontsize->setToolTip(QCoreApplication::translate("MainWindow", "Change the font size of the text browser.", nullptr));
#endif // QT_CONFIG(tooltip)
        checkBox_OutPath_KeepOriginalFileName->setText(QCoreApplication::translate("MainWindow", "Keep original file name", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
