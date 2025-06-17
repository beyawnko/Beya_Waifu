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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UiController.h"
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QTextCodec>
#endif

/*
Save settings
Remove the original settings file before saving
*/
int MainWindow::Settings_Save()
{
    QString settings_ini = Current_Path+"/settings.ini";
    QFile::remove(settings_ini);
    //=================
    QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
    //================= Add warning =========================
    configIniWrite->setValue("/Warning/.", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    //==================== Save version identifier ==================================
    configIniWrite->setValue("/settings/VERSION", VERSION);
    //===== UI style settings =====
    configIniWrite->setValue("/settings/DarkMode", 1); // 0=light,1=dark,2=auto
    //======= Save scale and denoise values  =================================
    configIniWrite->setValue("/settings/ImageScaleRatio", ui->doubleSpinBox_ScaleRatio_image->value());
    configIniWrite->setValue("/settings/GIFScaleRatio", ui->doubleSpinBox_ScaleRatio_gif->value());
    configIniWrite->setValue("/settings/VideoScaleRatio", ui->doubleSpinBox_ScaleRatio_video->value());
    configIniWrite->setValue("/settings/ImageDenoiseLevel", ui->spinBox_DenoiseLevel_image->value());
    configIniWrite->setValue("/settings/GIFDenoiseLevel", ui->spinBox_DenoiseLevel_gif->value());
    configIniWrite->setValue("/settings/VideoDenoiseLevel", ui->spinBox_DenoiseLevel_video->value());
    //============ Save custom width, height and settings ============================
    configIniWrite->setValue("/settings/CustResWidth", ui->spinBox_CustRes_width->value());
    configIniWrite->setValue("/settings/CustResHeight", ui->spinBox_CustRes_height->value());
    configIniWrite->setValue("/settings/CustResAspectRatioMode", ui->comboBox_AspectRatio_custRes->currentIndex());
    //============ Save thread count ====================================
    configIniWrite->setValue("/settings/ImageThreadNum", ui->spinBox_ThreadNum_image->value());
    configIniWrite->setValue("/settings/GIFThreadNumInternal", ui->spinBox_ThreadNum_gif_internal->value());
    configIniWrite->setValue("/settings/VideoThreadNumInternal", ui->spinBox_ThreadNum_video_internal->value());
    configIniWrite->setValue("/settings/MaxThreadCount", globalMaxThreadCount);
    //================== Save engine settings =========================
    configIniWrite->setValue("/settings/ImageEngine", ui->comboBox_Engine_Image->currentIndex());
    configIniWrite->setValue("/settings/GIFEngine", ui->comboBox_Engine_GIF->currentIndex());
    configIniWrite->setValue("/settings/VideoEngine", ui->comboBox_Engine_Video->currentIndex());
    configIniWrite->setValue("/settings/ImageStyle", ui->comboBox_ImageStyle->currentIndex());
    configIniWrite->setValue("/settings/ModelVulkan", ui->comboBox_model_vulkan->currentIndex());
    configIniWrite->setValue("/settings/TileSize", ui->spinBox_TileSize->value());
    configIniWrite->setValue("/settings/BlockSizeConverter", ui->spinBox_BlockSize_converter->value());
    configIniWrite->setValue("/settings/DisableGPUConverter", ui->checkBox_DisableGPU_converter->isChecked());
    configIniWrite->setValue("/settings/ForceOpenCLConverter", ui->checkBox_ForceOpenCL_converter->isChecked());
    configIniWrite->setValue("/settings/TTAVulkan", ui->checkBox_TTA_vulkan->isChecked());
    configIniWrite->setValue("/settings/TTAConverter", ui->checkBox_TTA_converter->isChecked());
    configIniWrite->setValue("/settings/TTA_SRMD", ui->checkBox_TTA_srmd->isChecked());
    configIniWrite->setValue("/settings/TileSize_SRMD", ui->spinBox_TileSize_srmd->value());
    configIniWrite->setValue("/settings/Version_Waifu2xNCNNVulkan", ui->comboBox_version_Waifu2xNCNNVulkan->currentIndex());
    //===
    configIniWrite->setValue("/settings/TTA_Waifu2xCaffe", ui->checkBox_TTA_Waifu2xCaffe->isChecked());
    configIniWrite->setValue("/settings/Model_2D_Waifu2xCaffe", ui->comboBox_Model_2D_Waifu2xCaffe->currentIndex());
    configIniWrite->setValue("/settings/Model_3D_Waifu2xCaffe", ui->comboBox_Model_3D_Waifu2xCaffe->currentIndex());
    configIniWrite->setValue("/settings/ProcessMode_Waifu2xCaffe", ui->comboBox_ProcessMode_Waifu2xCaffe->currentIndex());
    configIniWrite->setValue("/settings/BatchSize_Waifu2xCaffe", ui->spinBox_BatchSize_Waifu2xCaffe->value());
    configIniWrite->setValue("/settings/GPUID_Waifu2xCaffe", ui->spinBox_GPUID_Waifu2xCaffe->value());
    configIniWrite->setValue("/settings/SplitSize_Waifu2xCaffe", ui->spinBox_SplitSize_Waifu2xCaffe->value());
    configIniWrite->setValue("/settings/checkBox_EnableMultiGPU_Waifu2xCaffe", ui->checkBox_EnableMultiGPU_Waifu2xCaffe->isChecked());
    configIniWrite->setValue("/settings/lineEdit_MultiGPUInfo_Waifu2xCaffe", ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->text());
    //===
    configIniWrite->setValue("/settings/checkBox_TTA_RealsrNCNNVulkan", checkBox_TTA_RealsrNCNNVulkan->isChecked());
    configIniWrite->setValue("/settings/comboBox_Model_RealsrNCNNVulkan", comboBox_Model_RealsrNCNNVulkan->currentIndex());
    configIniWrite->setValue("/settings/spinBox_TileSize_RealsrNCNNVulkan", spinBox_TileSize_RealsrNCNNVulkan->value());
    //GPU ID List
    configIniWrite->setValue("/settings/CurrentGPUID_Waifu2xNCNNVulkan", ui->comboBox_GPUID->currentIndex());
    configIniWrite->setValue("/settings/Available_GPUID_Waifu2xNCNNVulkan", Available_GPUID_Waifu2xNcnnVulkan);
    configIniWrite->setValue("/settings/GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan", QVariant::fromValue(GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan));
    configIniWrite->setValue("/settings/checkBox_MultiGPU_Waifu2xNCNNVulkan", ui->checkBox_MultiGPU_Waifu2xNCNNVulkan->isChecked());
    //==
    configIniWrite->setValue("/settings/comboBox_GPUID_RealsrNCNNVulkan", comboBox_GPUID_RealsrNCNNVulkan->currentIndex());
    configIniWrite->setValue("/settings/Available_GPUID_Realsr_ncnn_vulkan", Available_GPUID_Realsr_ncnn_vulkan);
    configIniWrite->setValue("/settings/GPUIDs_List_MultiGPU_RealsrganNcnnVulkan", QVariant::fromValue(GPUIDs_List_MultiGPU_RealesrganNcnnVulkan));
    configIniWrite->setValue("/settings/checkBox_MultiGPU_RealesrganNcnnVulkan", checkBox_MultiGPU_RealesrganNcnnVulkan->isChecked());
    //==
    configIniWrite->setValue("/settings/comboBox_TargetProcessor_converter", ui->comboBox_TargetProcessor_converter->currentIndex());
    configIniWrite->setValue("/settings/Available_ProcessorList_converter", Available_ProcessorList_converter);
    configIniWrite->setValue("/settings/GPUIDs_List_MultiGPU_Waifu2xConverter", QVariant::fromValue(GPUIDs_List_MultiGPU_Waifu2xConverter));
    configIniWrite->setValue("/settings/checkBox_MultiGPU_Waifu2xConverter", ui->checkBox_MultiGPU_Waifu2xConverter->isChecked());
    //==
    configIniWrite->setValue("/settings/comboBox_GPUID_srmd", ui->comboBox_GPUID_srmd->currentIndex());
    configIniWrite->setValue("/settings/Available_GPUID_srmd", Available_GPUID_srmd);
    configIniWrite->setValue("/settings/GPUIDs_List_MultiGPU_SrmdNcnnVulkan", QVariant::fromValue(GPUIDs_List_MultiGPU_SrmdNcnnVulkan));
    configIniWrite->setValue("/settings/checkBox_MultiGPU_SrmdNCNNVulkan", ui->checkBox_MultiGPU_SrmdNCNNVulkan->isChecked());
    //== RealCUGAN
    if(comboBox_Model_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_Model", comboBox_Model_RealCUGAN->currentIndex());
    if(spinBox_Scale_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_Scale", spinBox_Scale_RealCUGAN->value());
    if(spinBox_DenoiseLevel_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_DenoiseLevel", spinBox_DenoiseLevel_RealCUGAN->value());
    if(spinBox_TileSize_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_TileSize", spinBox_TileSize_RealCUGAN->value());
    if(checkBox_TTA_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_TTA", checkBox_TTA_RealCUGAN->isChecked());
    if(comboBox_GPUID_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_GPUID", comboBox_GPUID_RealCUGAN->currentIndex());
    configIniWrite->setValue("/settings/RealCUGAN_Available_GPUID", Available_GPUID_RealCUGAN);
    if(checkBox_MultiGPU_RealCUGAN) configIniWrite->setValue("/settings/RealCUGAN_MultiGPU_Enabled", checkBox_MultiGPU_RealCUGAN->isChecked());
    configIniWrite->setValue("/settings/RealCUGAN_GPUJobConfig_MultiGPU", QVariant::fromValue(m_realcugan_gpuJobConfig_temp));
    //== RealESRGAN
    if(comboBox_Model_RealsrNCNNVulkan) configIniWrite->setValue("/settings/RealESRGAN_ModelName", comboBox_Model_RealsrNCNNVulkan->currentText());
    if(spinBox_TileSize_RealsrNCNNVulkan) configIniWrite->setValue("/settings/RealESRGAN_TileSize", spinBox_TileSize_RealsrNCNNVulkan->value());
    if(checkBox_TTA_RealsrNCNNVulkan) configIniWrite->setValue("/settings/RealESRGAN_TTA", checkBox_TTA_RealsrNCNNVulkan->isChecked());
    if(comboBox_GPUID_RealsrNCNNVulkan) configIniWrite->setValue("/settings/RealESRGAN_GPUID", comboBox_GPUID_RealsrNCNNVulkan->currentIndex());
    configIniWrite->setValue("/settings/RealESRGAN_Available_GPUID", Available_GPUID_RealESRGAN_ncnn_vulkan);
    if(checkBox_MultiGPU_RealesrganNcnnVulkan) configIniWrite->setValue("/settings/RealESRGAN_MultiGPU_Enabled", checkBox_MultiGPU_RealesrganNcnnVulkan->isChecked());
    configIniWrite->setValue("/settings/RealESRGAN_GPUJobConfig_MultiGPU", QVariant::fromValue(m_realesrgan_gpuJobConfig_temp));
    //================== Save file extensions =================================
    configIniWrite->setValue("/settings/ImageEXT", ui->Ext_image->text());
    configIniWrite->setValue("/settings/VideoEXT", ui->Ext_video->text());
    //=================== Save miscellaneous settings =================================
    configIniWrite->setValue("/settings/checkBox_SummaryPopup", ui->checkBox_SummaryPopup->isChecked());
    configIniWrite->setValue("/settings/checkBox_DisableResize_gif", ui->checkBox_DisableResize_gif->isChecked());
    configIniWrite->setValue("/settings/checkBox_AutoSkip_CustomRes", ui->checkBox_AutoSkip_CustomRes->isChecked());
    configIniWrite->setValue("/settings/checkBox_AlwaysPreProcessAlphaPNG", ui->checkBox_AlwaysPreProcessAlphaPNG->isChecked());
    configIniWrite->setValue("/settings/spinBox_ImageQualityLevel", ui->spinBox_ImageQualityLevel->value());
    configIniWrite->setValue("/settings/comboBox_ImageSaveFormat", ui->comboBox_ImageSaveFormat->currentIndex());
    configIniWrite->setValue("/settings/checkBox_KeepParentFolder", ui->checkBox_KeepParentFolder->isChecked());
    configIniWrite->setValue("/settings/checkBox_BanGitee", ui->checkBox_BanGitee->isChecked());
    configIniWrite->setValue("/settings/comboBox_UpdateChannel", ui->comboBox_UpdateChannel->currentIndex());
    configIniWrite->setValue("/settings/checkBox_MinimizeToTaskbar", ui->checkBox_MinimizeToTaskbar->isChecked());
    configIniWrite->setValue("/settings/checkBox_custres_isAll", ui->checkBox_custres_isAll->isChecked());
    configIniWrite->setValue("/settings/DelOriginal", ui->checkBox_DelOriginal->isChecked());
    configIniWrite->setValue("/settings/OptGIF", ui->checkBox_OptGIF->isChecked());
    configIniWrite->setValue("/settings/NFSound", ui->checkBox_NfSound->isChecked());
    configIniWrite->setValue("/settings/ReProFinFiles", ui->checkBox_ReProcFinFiles->isChecked());
    configIniWrite->setValue("/settings/ShowInterPro", ui->checkBox_ShowInterPro->isChecked());
    configIniWrite->setValue("/settings/UpdatePopup", ui->checkBox_UpdatePopup->isChecked());
    configIniWrite->setValue("/settings/FileListAutoScroll", ui->checkBox_FileListAutoSlide->isChecked());
    configIniWrite->setValue("/settings/AutoSaveSettings", ui->checkBox_AutoSaveSettings->isChecked());
    configIniWrite->setValue("/settings/AlwaysHideSettings", ui->checkBox_AlwaysHideSettings->isChecked());
    configIniWrite->setValue("/settings/AlwaysHideTextBrowser", ui->checkBox_AlwaysHideTextBrowser->isChecked());
    configIniWrite->setValue("/settings/ScanSubFolders", ui->checkBox_ScanSubFolders->isChecked());
    configIniWrite->setValue("/settings/InteractiveFileList", ui->checkBox_FileList_Interactive->isChecked());
    configIniWrite->setValue("/settings/RetryTimes", ui->spinBox_retry->value());
    configIniWrite->setValue("/settings/AutoDetectAlphaChannel", ui->checkBox_AutoDetectAlphaChannel->isChecked());
    configIniWrite->setValue("/settings/PromptWhenExit", ui->checkBox_PromptWhenExit->isChecked());
    configIniWrite->setValue("/settings/KeepVideoCache", ui->checkBox_KeepVideoCache->isChecked());
    configIniWrite->setValue("/settings/checkBox_ReplaceOriginalFile", ui->checkBox_ReplaceOriginalFile->isChecked());
    //===
    configIniWrite->setValue("/settings/QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile", QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->isChecked());
    //===
    configIniWrite->setValue("/settings/QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal", QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->isChecked());
    //===
    configIniWrite->setValue("/settings/ProcessVideoBySegment", ui->checkBox_ProcessVideoBySegment->isChecked());
    configIniWrite->setValue("/settings/SegmentDuration", ui->spinBox_SegmentDuration->value());
    //=====
    configIniWrite->setValue("/settings/AudioDenoise", ui->checkBox_AudioDenoise->isChecked());
    configIniWrite->setValue("/settings/AudioDenoiseLevel", ui->doubleSpinBox_AudioDenoiseLevel->value());
    //=====
    configIniWrite->setValue("/settings/checkBox_PreProcessImage", ui->checkBox_PreProcessImage->isChecked());
    //===================== Save text browser settings =====================
    configIniWrite->setValue("/settings/TextBrowserFontSize", ui->spinBox_textbrowser_fontsize->value());
    //===================== Save language settings ================================
    configIniWrite->setValue("/settings/Language", ui->comboBox_language->currentIndex());
    //================== Save global font settings =========================
    configIniWrite->setValue("/settings/GlobalFontSize", ui->spinBox_GlobalFontSize->value());
    configIniWrite->setValue("/settings/CustFont", ui->fontComboBox_CustFont->currentFont());
    configIniWrite->setValue("/settings/CustFont_isEnabled", ui->checkBox_isCustFontEnable->isChecked());
    //=================== Save video settings ===========================
    configIniWrite->setValue("/settings/VideoSettingsIsEnabled", ui->groupBox_video_settings->isChecked());
    configIniWrite->setValue("/settings/EncoderVideo", ui->lineEdit_encoder_vid->text());
    configIniWrite->setValue("/settings/EncoderAudio", ui->lineEdit_encoder_audio->text());
    configIniWrite->setValue("/settings/PixelFormat", ui->lineEdit_pixformat->text());
    configIniWrite->setValue("/settings/BitrateVideo", ui->spinBox_bitrate_vid->value());
    configIniWrite->setValue("/settings/BitrateAudio", ui->spinBox_bitrate_audio->value());
    configIniWrite->setValue("/settings/ExtraCommandOutput", ui->lineEdit_ExCommand_output->text());
    //===
    configIniWrite->setValue("/settings/BitrateVideo2mp4", ui->spinBox_bitrate_vid_2mp4->value());
    configIniWrite->setValue("/settings/BitrateAudio2mp4", ui->spinBox_bitrate_audio_2mp4->value());
    configIniWrite->setValue("/settings/vcodecCopy", ui->checkBox_vcodec_copy_2mp4->isChecked());
    configIniWrite->setValue("/settings/acodecCopy", ui->checkBox_acodec_copy_2mp4->isChecked());
    configIniWrite->setValue("/settings/checkBox_IgnoreFrameRateMode", ui->checkBox_IgnoreFrameRateMode->isChecked());
    configIniWrite->setValue("/settings/ExtraCommand2mp4", ui->lineEdit_ExCommand_2mp4->text());
    //==================== Save output path settings ========================
    configIniWrite->setValue("/settings/OutPutPath", ui->lineEdit_outputPath->text());
    configIniWrite->setValue("/settings/OutPutPathIsEnabled", ui->checkBox_OutPath_isEnabled->isChecked());
    configIniWrite->setValue("/settings/checkBox_OutPath_KeepOriginalFileName", ui->checkBox_OutPath_KeepOriginalFileName->isChecked());
    configIniWrite->setValue("/settings/checkBox_OutPath_Overwrite", ui->checkBox_OutPath_Overwrite->isChecked());
    configIniWrite->setValue("/settings/checkBox_AutoOpenOutputPath", ui->checkBox_AutoOpenOutputPath->isChecked());
    //=================== Save Anime4k settings =============================
    configIniWrite->setValue("/settings/spinBox_OpenCLCommandQueues_A4k", ui->spinBox_OpenCLCommandQueues_A4k->value());
    configIniWrite->setValue("/settings/checkBox_OpenCLParallelIO_A4k", ui->checkBox_OpenCLParallelIO_A4k->isChecked());
    configIniWrite->setValue("/settings/comboBox_GPGPUModel_A4k", ui->comboBox_GPGPUModel_A4k->currentIndex());
    configIniWrite->setValue("/settings/checkBox_HDNMode_Anime4k", ui->checkBox_HDNMode_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_FastMode_Anime4K", ui->checkBox_FastMode_Anime4K->isChecked());
    configIniWrite->setValue("/settings/checkBox_ACNet_Anime4K", ui->checkBox_ACNet_Anime4K->isChecked());
    configIniWrite->setValue("/settings/checkBox_GPUMode_Anime4K", ui->checkBox_GPUMode_Anime4K->isChecked());
    configIniWrite->setValue("/settings/spinBox_Passes_Anime4K", ui->spinBox_Passes_Anime4K->value());
    configIniWrite->setValue("/settings/spinBox_PushColorCount_Anime4K", ui->spinBox_PushColorCount_Anime4K->value());
    configIniWrite->setValue("/settings/doubleSpinBox_PushColorStrength_Anime4K", ui->doubleSpinBox_PushColorStrength_Anime4K->value());
    configIniWrite->setValue("/settings/doubleSpinBox_PushGradientStrength_Anime4K", ui->doubleSpinBox_PushGradientStrength_Anime4K->value());
    configIniWrite->setValue("/settings/checkBox_SpecifyGPU_Anime4k", ui->checkBox_SpecifyGPU_Anime4k->isChecked());
    configIniWrite->setValue("/settings/lineEdit_GPUs_Anime4k", ui->lineEdit_GPUs_Anime4k->text());
    //Pre-Processing
    configIniWrite->setValue("/settings/checkBox_EnablePreProcessing_Anime4k", ui->checkBox_EnablePreProcessing_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_MedianBlur_Pre_Anime4k", ui->checkBox_MedianBlur_Pre_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_MeanBlur_Pre_Anime4k", ui->checkBox_MeanBlur_Pre_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_CASSharping_Pre_Anime4k", ui->checkBox_CASSharping_Pre_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_GaussianBlurWeak_Pre_Anime4k", ui->checkBox_GaussianBlurWeak_Pre_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_GaussianBlur_Pre_Anime4k", ui->checkBox_GaussianBlur_Pre_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_BilateralFilter_Pre_Anime4k", ui->checkBox_BilateralFilter_Pre_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_BilateralFilterFaster_Pre_Anime4k", ui->checkBox_BilateralFilterFaster_Pre_Anime4k->isChecked());
    //Post-Processing
    configIniWrite->setValue("/settings/checkBox_EnablePostProcessing_Anime4k", ui->checkBox_EnablePostProcessing_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_MedianBlur_Post_Anime4k", ui->checkBox_MedianBlur_Post_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_MeanBlur_Post_Anime4k", ui->checkBox_MeanBlur_Post_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_CASSharping_Post_Anime4k", ui->checkBox_CASSharping_Post_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_GaussianBlurWeak_Post_Anime4k", ui->checkBox_GaussianBlurWeak_Post_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_GaussianBlur_Post_Anime4k", ui->checkBox_GaussianBlur_Post_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_BilateralFilter_Post_Anime4k", ui->checkBox_BilateralFilter_Post_Anime4k->isChecked());
    configIniWrite->setValue("/settings/checkBox_BilateralFilterFaster_Post_Anime4k", ui->checkBox_BilateralFilterFaster_Post_Anime4k->isChecked());
    //========================= Save compatibility test results ================
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW", ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P", ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD", ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_Converter", ui->checkBox_isCompatible_Waifu2x_Converter->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_SRMD_NCNN_Vulkan", ui->checkBox_isCompatible_SRMD_NCNN_Vulkan->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_SRMD_CUDA", ui->checkBox_isCompatible_SRMD_CUDA->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Anime4k_CPU", ui->checkBox_isCompatible_Anime4k_CPU->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Anime4k_GPU", ui->checkBox_isCompatible_Anime4k_GPU->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_FFmpeg", ui->checkBox_isCompatible_FFmpeg->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_FFprobe", ui->checkBox_isCompatible_FFprobe->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_ImageMagick", ui->checkBox_isCompatible_ImageMagick->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Gifsicle", ui->checkBox_isCompatible_Gifsicle->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_SoX", ui->checkBox_isCompatible_SoX->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_Caffe_CPU", ui->checkBox_isCompatible_Waifu2x_Caffe_CPU->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_Caffe_GPU", ui->checkBox_isCompatible_Waifu2x_Caffe_GPU->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Waifu2x_Caffe_cuDNN", ui->checkBox_isCompatible_Waifu2x_Caffe_cuDNN->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_Realsr_NCNN_Vulkan", ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_RifeNcnnVulkan", ui->checkBox_isCompatible_RifeNcnnVulkan->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_CainNcnnVulkan", ui->checkBox_isCompatible_CainNcnnVulkan->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_DainNcnnVulkan", ui->checkBox_isCompatible_DainNcnnVulkan->isChecked());
    configIniWrite->setValue("/settings/checkBox_isCompatible_RealCUGAN_NCNN_Vulkan", isCompatible_RealCUGAN_NCNN_Vulkan);
    if(ui->checkBox_isCompatible_Realsr_NCNN_Vulkan) configIniWrite->setValue("/settings/checkBox_isCompatible_RealESRGAN_NCNN_Vulkan", isCompatible_RealESRGAN_NCNN_Vulkan);
    //======================== Save VFI settings ========================
    configIniWrite->setValue("/settings/checkBox_VfiAfterScale_VFI", ui->checkBox_VfiAfterScale_VFI->isChecked());
    configIniWrite->setValue("/settings/checkBox_MultiThread_VFI", ui->checkBox_MultiThread_VFI->isChecked());
    configIniWrite->setValue("/settings/checkBox_AutoAdjustNumOfThreads_VFI", ui->checkBox_AutoAdjustNumOfThreads_VFI->isChecked());
    configIniWrite->setValue("/settings/checkBox_FrameInterpolationOnly_Video", ui->checkBox_FrameInterpolationOnly_Video->isChecked());
    configIniWrite->setValue("/settings/groupBox_FrameInterpolation", ui->groupBox_FrameInterpolation->isChecked());
    configIniWrite->setValue("/settings/checkBox_MultiGPU_VFI", ui->checkBox_MultiGPU_VFI->isChecked());
    configIniWrite->setValue("/settings/checkBox_TTA_VFI", ui->checkBox_TTA_VFI->isChecked());
    configIniWrite->setValue("/settings/checkBox_UHD_VFI", ui->checkBox_UHD_VFI->isChecked());
    configIniWrite->setValue("/settings/comboBox_Model_VFI", ui->comboBox_Model_VFI->currentIndex());
    configIniWrite->setValue("/settings/comboBox_GPUID_VFI", ui->comboBox_GPUID_VFI->currentIndex());
    configIniWrite->setValue("/settings/Available_GPUID_FrameInterpolation", Available_GPUID_FrameInterpolation);
    configIniWrite->setValue("/settings/lineEdit_MultiGPU_IDs_VFI", ui->lineEdit_MultiGPU_IDs_VFI->text());
    configIniWrite->setValue("/settings/spinBox_NumOfThreads_VFI", ui->spinBox_NumOfThreads_VFI->value());
    configIniWrite->setValue("/settings/comboBox_Engine_VFI", ui->comboBox_Engine_VFI->currentIndex());
    configIniWrite->setValue("/settings/spinBox_MultipleOfFPS_VFI", ui->spinBox_MultipleOfFPS_VFI->value());
    configIniWrite->setValue("/settings/spinBox_TileSize_VFI", ui->spinBox_TileSize_VFI->value());
    //========
    return 0;
}
/*
Read and apply settings
If the settings file does not exist, create a default one, then read and apply settings
*/
int MainWindow::Settings_Read_Apply()
{
    QString settings_ini = Current_Path+"/settings.ini";
    if(!QFile::exists(settings_ini))
    {
        QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->setChecked(1);
        QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setChecked(1);
        if(isBetaVer)comboBox_UpdateChannel_setCurrentIndex_self(1);
        Settings_Save();
        Settings_Read_Apply();
        return 0;
    }
    else
    {
        QSettings *configIniRead_ver = new QSettings(settings_ini, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        configIniRead_ver->setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
        QString Settings_VERSION = configIniRead_ver->value("/settings/VERSION").toString();
        if(Settings_VERSION!=VERSION)
        {
            isReadOldSettings=true;
            QFile::rename(settings_ini,Current_Path+"/settings_old.ini");
            if(isBetaVer)comboBox_UpdateChannel_setCurrentIndex_self(1);
            Settings_Save();
            Settings_Read_Apply();
            return 0;
        }
    }
    //=================
    // QSettings *configIniRead = new QSettings(settings_ini, QSettings::IniFormat); // Unused variable
// #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    // configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
// #endif
    //=================== Load global font settings =========================
    {
        QVariant tmp = Settings_Read_value("/settings/GlobalFontSize");
        if (tmp.isValid()) ui->spinBox_GlobalFontSize->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/CustFont");
        if (tmp.isValid()) ui->fontComboBox_CustFont->setCurrentFont(tmp.value<QFont>());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/CustFont_isEnabled");
        if (tmp.isValid()) ui->checkBox_isCustFontEnable->setChecked(tmp.toBool());
    }
    uiController.setFontFixed(ui->checkBox_isCustFontEnable,
                              ui->fontComboBox_CustFont,
                              ui->spinBox_GlobalFontSize);
    //=======  Load scale and denoise values  ======
    {
        QVariant tmp = Settings_Read_value("/settings/ImageScaleRatio");
        if (tmp.isValid()) ui->doubleSpinBox_ScaleRatio_image->setValue(tmp.toDouble());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/GIFScaleRatio");
        if (tmp.isValid()) ui->doubleSpinBox_ScaleRatio_gif->setValue(tmp.toDouble());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/VideoScaleRatio");
        if (tmp.isValid()) ui->doubleSpinBox_ScaleRatio_video->setValue(tmp.toDouble());
    }
    //============= Load custom width and height ============================
    {
        QVariant tmp = Settings_Read_value("/settings/CustResWidth");
        if (tmp.isValid()) ui->spinBox_CustRes_width->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/CustResHeight");
        if (tmp.isValid()) ui->spinBox_CustRes_height->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/CustResAspectRatioMode");
        if (tmp.isValid()) ui->comboBox_AspectRatio_custRes->setCurrentIndex(tmp.toInt());
    }
    //============ Load thread count ==================================
    {
        QVariant tmp = Settings_Read_value("/settings/ImageThreadNum");
        if (tmp.isValid()) ui->spinBox_ThreadNum_image->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/GIFThreadNumInternal");
        if (tmp.isValid()) ui->spinBox_ThreadNum_gif_internal->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/VideoThreadNumInternal");
        if (tmp.isValid()) ui->spinBox_ThreadNum_video_internal->setValue(tmp.toInt());
    }
    
    QVariant maxThreadCountSetting = Settings_Read_value("/settings/MaxThreadCount");
    globalMaxThreadCount = maxThreadCountSetting.isValid() ? maxThreadCountSetting.toInt() : 0;
    QThreadPool::globalInstance()->setMaxThreadCount(globalMaxThreadCount);
    //================ Load engine settings ================================
    isShowAnime4kWarning=false;
    
    QVariant imageEngineSetting = Settings_Read_value("/settings/ImageEngine");
    ui->comboBox_Engine_Image->setCurrentIndex(imageEngineSetting.isValid() ? imageEngineSetting.toInt() : 0);

    QVariant gifEngineSetting = Settings_Read_value("/settings/GIFEngine");
    ui->comboBox_Engine_GIF->setCurrentIndex(gifEngineSetting.isValid() ? gifEngineSetting.toInt() : 0);
    
    QVariant videoEngineSetting = Settings_Read_value("/settings/VideoEngine");
    ui->comboBox_Engine_Video->setCurrentIndex(videoEngineSetting.isValid() ? videoEngineSetting.toInt() : 0);

    {
        QVariant tmp = Settings_Read_value("/settings/ImageStyle");
        if (tmp.isValid()) ui->comboBox_ImageStyle->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ModelVulkan");
        if (tmp.isValid()) ui->comboBox_model_vulkan->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/TileSize");
        if (tmp.isValid()) ui->spinBox_TileSize->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/BlockSizeConverter");
        if (tmp.isValid()) ui->spinBox_BlockSize_converter->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/DisableGPUConverter");
        if (tmp.isValid()) ui->checkBox_DisableGPU_converter->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ForceOpenCLConverter");
        if (tmp.isValid()) ui->checkBox_ForceOpenCL_converter->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/TTAVulkan");
        if (tmp.isValid()) ui->checkBox_TTA_vulkan->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/TTAConverter");
        if (tmp.isValid()) ui->checkBox_TTA_converter->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/TTA_SRMD");
        if (tmp.isValid()) ui->checkBox_TTA_srmd->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/TileSize_SRMD");
        if (tmp.isValid()) ui->spinBox_TileSize_srmd->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/Version_Waifu2xNCNNVulkan");
        if (tmp.isValid()) ui->comboBox_version_Waifu2xNCNNVulkan->setCurrentIndex(tmp.toInt());
    }
    //===
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_TTA_RealsrNCNNVulkan");
        if (tmp.isValid()) checkBox_TTA_RealsrNCNNVulkan->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_Model_RealsrNCNNVulkan");
        if (tmp.isValid()) comboBox_Model_RealsrNCNNVulkan->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_TileSize_RealsrNCNNVulkan");
        if (tmp.isValid()) spinBox_TileSize_RealsrNCNNVulkan->setValue(tmp.toInt());
    }
    //===
    {
        QVariant tmp = Settings_Read_value("/settings/TTA_Waifu2xCaffe");
        if (tmp.isValid()) ui->checkBox_TTA_Waifu2xCaffe->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/Model_2D_Waifu2xCaffe");
        if (tmp.isValid()) ui->comboBox_Model_2D_Waifu2xCaffe->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/Model_3D_Waifu2xCaffe");
        if (tmp.isValid()) ui->comboBox_Model_3D_Waifu2xCaffe->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ProcessMode_Waifu2xCaffe");
        if (tmp.isValid()) ui->comboBox_ProcessMode_Waifu2xCaffe->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/BatchSize_Waifu2xCaffe");
        if (tmp.isValid()) ui->spinBox_BatchSize_Waifu2xCaffe->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/GPUID_Waifu2xCaffe");
        if (tmp.isValid()) ui->spinBox_GPUID_Waifu2xCaffe->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/SplitSize_Waifu2xCaffe");
        if (tmp.isValid()) ui->spinBox_SplitSize_Waifu2xCaffe->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_EnableMultiGPU_Waifu2xCaffe");
        if (tmp.isValid()) ui->checkBox_EnableMultiGPU_Waifu2xCaffe->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/lineEdit_MultiGPUInfo_Waifu2xCaffe");
        if (tmp.isValid()) ui->lineEdit_MultiGPUInfo_Waifu2xCaffe->setText(tmp.toString());
    }
    //GPU ID List
    //Waifu2x-NCNN-Vulkan
    {
        QVariant tmp = Settings_Read_value("/settings/Available_GPUID_Waifu2xNCNNVulkan");
        if (tmp.isValid()) Available_GPUID_Waifu2xNcnnVulkan = tmp.toStringList();
    }
    Waifu2x_DetectGPU_finished();
    {
        QVariant tmp = Settings_Read_value("/settings/CurrentGPUID_Waifu2xNCNNVulkan");
        if (tmp.isValid()) ui->comboBox_GPUID->setCurrentIndex(tmp.toInt());
    }
    // Load multi-GPU settings
    {
        QVariant tmp = Settings_Read_value("/settings/GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan");
        if (tmp.isValid()) GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan = tmp.value<QList<QMap<QString, QString>> >();
    }
    if(GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan.isEmpty()==false && ui->comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan) // Check if combobox exists
    {
        QMap<QString,QString> GPUInfo_waifu2xNcnnVulkan = GPUIDs_List_MultiGPU_Waifu2xNCNNVulkan.at(ui->comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan->currentIndex());
        if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan) ui->checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setChecked(GPUInfo_waifu2xNcnnVulkan["isEnabled"] == "true");
        if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan) ui->spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan->setValue(GPUInfo_waifu2xNcnnVulkan["TileSize"].toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_Waifu2xNCNNVulkan");
        if (tmp.isValid()) if(ui->checkBox_MultiGPU_Waifu2xNCNNVulkan) ui->checkBox_MultiGPU_Waifu2xNCNNVulkan->setChecked(tmp.toBool());
    }
    //Realsr_ncnn_vulkan
    {
        QVariant tmp = Settings_Read_value("/settings/Available_GPUID_Realsr_ncnn_vulkan");
        if (tmp.isValid()) Available_GPUID_Realsr_ncnn_vulkan = tmp.toStringList();
    }
    Realsr_ncnn_vulkan_DetectGPU_finished();
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_GPUID_RealsrNCNNVulkan");
        if (tmp.isValid()) if(comboBox_GPUID_RealsrNCNNVulkan) comboBox_GPUID_RealsrNCNNVulkan->setCurrentIndex(tmp.toInt());
    }
    // Load multi-GPU settings
    {
        QVariant tmp = Settings_Read_value("/settings/GPUIDs_List_MultiGPU_RealsrganNcnnVulkan");
        if (tmp.isValid()) GPUIDs_List_MultiGPU_RealesrganNcnnVulkan = tmp.value<QList<QMap<QString, QString>> >();
    }
    if(GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.isEmpty()==false && comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan)
    {
        QMap<QString,QString> GPUInfo_RealesrganNcnnVulkan = GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.at(comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan->currentIndex());
        if(checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(GPUInfo_RealesrganNcnnVulkan["isEnabled"] == "true");
        if(spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(GPUInfo_RealesrganNcnnVulkan["TileSize"].toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_RealesrganNcnnVulkan");
        if (tmp.isValid()) if(checkBox_MultiGPU_RealesrganNcnnVulkan) checkBox_MultiGPU_RealesrganNcnnVulkan->setChecked(tmp.toBool());
    }
    //Waifu2x-Converter
    {
        QVariant tmp = Settings_Read_value("/settings/Available_ProcessorList_converter");
        if (tmp.isValid()) Available_ProcessorList_converter = tmp.toStringList();
    }
    Waifu2x_DumpProcessorList_converter_finished();
    if(ui->comboBox_TargetProcessor_converter) {
        QVariant converterProcessor = Settings_Read_value("/settings/comboBox_TargetProcessor_converter");
        if (converterProcessor.isValid()) {
            ui->comboBox_TargetProcessor_converter->setCurrentIndex(converterProcessor.toInt());
        }
    }
    this->on_comboBox_TargetProcessor_converter_currentIndexChanged(0);
    // Load multi-GPU settings
    {
        QVariant tmp = Settings_Read_value("/settings/GPUIDs_List_MultiGPU_Waifu2xConverter");
        if (tmp.isValid()) GPUIDs_List_MultiGPU_Waifu2xConverter = tmp.value<QList<QMap<QString, QString>> >();
    }
    if(GPUIDs_List_MultiGPU_Waifu2xConverter.isEmpty()==false && ui->comboBox_GPUIDs_MultiGPU_Waifu2xConverter)
    {
        QMap<QString,QString> GPUInfo_Waifu2xConverter = GPUIDs_List_MultiGPU_Waifu2xConverter.at(ui->comboBox_GPUIDs_MultiGPU_Waifu2xConverter->currentIndex());
        if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter) ui->checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter->setChecked(GPUInfo_Waifu2xConverter["isEnabled"] == "true");
        if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter) ui->spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter->setValue(GPUInfo_Waifu2xConverter["TileSize"].toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_Waifu2xConverter");
        if (tmp.isValid()) if(ui->checkBox_MultiGPU_Waifu2xConverter) ui->checkBox_MultiGPU_Waifu2xConverter->setChecked(tmp.toBool());
    }
    //SRMD-NCNN-Vulkan
    {
        QVariant tmp = Settings_Read_value("/settings/Available_GPUID_srmd");
        if (tmp.isValid()) Available_GPUID_srmd = tmp.toStringList();
    }
    SRMD_DetectGPU_finished();
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_GPUID_srmd");
        if (tmp.isValid()) if(ui->comboBox_GPUID_srmd) ui->comboBox_GPUID_srmd->setCurrentIndex(tmp.toInt());
    }
    // Load multi-GPU settings
    {
        QVariant tmp = Settings_Read_value("/settings/GPUIDs_List_MultiGPU_SrmdNcnnVulkan");
        if (tmp.isValid()) GPUIDs_List_MultiGPU_SrmdNcnnVulkan = tmp.value<QList<QMap<QString, QString>> >();
    }
    if(GPUIDs_List_MultiGPU_SrmdNcnnVulkan.isEmpty()==false && ui->comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan)
    {
        QMap<QString,QString> GPUInfo_SrmdNcnnVulkan = GPUIDs_List_MultiGPU_SrmdNcnnVulkan.at(ui->comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan->currentIndex());
        if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan) ui->checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setChecked(GPUInfo_SrmdNcnnVulkan["isEnabled"] == "true");
        if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan) ui->spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan->setValue(GPUInfo_SrmdNcnnVulkan["TileSize"].toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_SrmdNCNNVulkan");
        if (tmp.isValid()) if(ui->checkBox_MultiGPU_SrmdNCNNVulkan) ui->checkBox_MultiGPU_SrmdNCNNVulkan->setChecked(tmp.toBool());
    }

    // RealCUGAN Settings
    QVariant realCuganModel = Settings_Read_value("/settings/RealCUGAN_Model");
    if(comboBox_Model_RealCUGAN && realCuganModel.isValid()) comboBox_Model_RealCUGAN->setCurrentIndex(realCuganModel.toInt());

    QVariant scaleSetting = Settings_Read_value("/settings/RealCUGAN_Scale");
    if(spinBox_Scale_RealCUGAN && scaleSetting.isValid()) {
        spinBox_Scale_RealCUGAN->setValue(scaleSetting.toInt());
    }
    
    QVariant denoiseSetting = Settings_Read_value("/settings/RealCUGAN_DenoiseLevel");
    if(spinBox_DenoiseLevel_RealCUGAN && denoiseSetting.isValid()) {
        spinBox_DenoiseLevel_RealCUGAN->setValue(denoiseSetting.toInt());
    }

    QVariant tileSizeCugan = Settings_Read_value("/settings/RealCUGAN_TileSize");
    if(spinBox_TileSize_RealCUGAN && tileSizeCugan.isValid()) spinBox_TileSize_RealCUGAN->setValue(tileSizeCugan.toInt());
    
    QVariant ttaCugan = Settings_Read_value("/settings/RealCUGAN_TTA");
    if(checkBox_TTA_RealCUGAN && ttaCugan.isValid()) checkBox_TTA_RealCUGAN->setChecked(ttaCugan.toBool());
    
    {
        QVariant tmp = Settings_Read_value("/settings/RealCUGAN_Available_GPUID");
        if (tmp.isValid()) Available_GPUID_RealCUGAN = tmp.toStringList();
    }
    Realcugan_NCNN_Vulkan_DetectGPU_finished();

    QVariant gpuIDCugan = Settings_Read_value("/settings/RealCUGAN_GPUID");
    if(comboBox_GPUID_RealCUGAN && gpuIDCugan.isValid()) comboBox_GPUID_RealCUGAN->setCurrentIndex(gpuIDCugan.toInt());

    QVariant multiGpuCugan = Settings_Read_value("/settings/RealCUGAN_MultiGPU_Enabled");
    if(checkBox_MultiGPU_RealCUGAN && multiGpuCugan.isValid()) checkBox_MultiGPU_RealCUGAN->setChecked(multiGpuCugan.toBool());
    
    {
        QVariant tmp = Settings_Read_value("/settings/RealCUGAN_GPUJobConfig_MultiGPU");
        if (tmp.isValid()) m_realcugan_gpuJobConfig_temp = tmp.value<QList<QMap<QString, QString>>>();
    }
    if (checkBox_MultiGPU_RealCUGAN && checkBox_MultiGPU_RealCUGAN->isChecked()) {
        if (listWidget_GPUList_MultiGPU_RealCUGAN) {
            listWidget_GPUList_MultiGPU_RealCUGAN->clear();
            for (const auto& job : m_realcugan_gpuJobConfig_temp) {
                QString gpuDesc;
                QString fullDesc = "";
                for(const QString& desc : Available_GPUID_RealCUGAN){
                    if(desc.startsWith(job.value("id") + ":")){
                        fullDesc = desc;
                        break;
                    }
                }
                if (fullDesc.isEmpty()) fullDesc = job.value("id");

                gpuDesc = fullDesc + QString(" (T:%1, Tile:%2)").arg(job.value("threads","1")).arg(job.value("tilesize","0"));
                QListWidgetItem *newItem = new QListWidgetItem(gpuDesc);
                newItem->setData(Qt::UserRole, job.value("id"));
                newItem->setData(Qt::UserRole + 1, job.value("threads","1").toInt());
                newItem->setData(Qt::UserRole + 2, job.value("tilesize","0").toInt());
                listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
            }
        }
    }

    // RealESRGAN Settings
    if(comboBox_Model_RealsrNCNNVulkan) {
        QString modelName;
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_ModelName");
        if (tmp.isValid()) modelName = tmp.toString();

        if (!modelName.isEmpty()) {
            int modelIndex = comboBox_Model_RealsrNCNNVulkan->findText(modelName);
            if (modelIndex != -1) comboBox_Model_RealsrNCNNVulkan->setCurrentIndex(modelIndex);
        }
    }
    {
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_TileSize");
        if (tmp.isValid()) if(spinBox_TileSize_RealsrNCNNVulkan) spinBox_TileSize_RealsrNCNNVulkan->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_TTA");
        if (tmp.isValid()) if(checkBox_TTA_RealsrNCNNVulkan) checkBox_TTA_RealsrNCNNVulkan->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_Available_GPUID");
        if (tmp.isValid()) Available_GPUID_RealESRGAN_ncnn_vulkan = tmp.toStringList();
    }
    RealESRGAN_ncnn_vulkan_DetectGPU_finished();

    QVariant gpuIDEsrgan = Settings_Read_value("/settings/RealESRGAN_GPUID");
    if(comboBox_GPUID_RealsrNCNNVulkan && gpuIDEsrgan.isValid()) comboBox_GPUID_RealsrNCNNVulkan->setCurrentIndex(gpuIDEsrgan.toInt());
    
    QVariant multiGpuEsrgan = Settings_Read_value("/settings/RealESRGAN_MultiGPU_Enabled");
    if(checkBox_MultiGPU_RealesrganNcnnVulkan && multiGpuEsrgan.isValid()) checkBox_MultiGPU_RealesrganNcnnVulkan->setChecked(multiGpuEsrgan.toBool());
    
    {
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_GPUJobConfig_MultiGPU");
        if (tmp.isValid()) m_realesrgan_gpuJobConfig_temp = tmp.value<QList<QMap<QString, QString>>>();
    }
    if (checkBox_MultiGPU_RealesrganNcnnVulkan && checkBox_MultiGPU_RealesrganNcnnVulkan->isChecked()) {
        RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay();
    }


    //================= Load file extensions ===========================
    {
        QVariant tmp = Settings_Read_value("/settings/ImageEXT");
        if (tmp.isValid()) ui->Ext_image->setText(tmp.toString());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/VideoEXT");
        if (tmp.isValid()) ui->Ext_video->setText(tmp.toString());
    }
    //================== Load miscellaneous settings ==================================
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_SummaryPopup");
        if (tmp.isValid()) ui->checkBox_SummaryPopup->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_DisableResize_gif");
        if (tmp.isValid()) ui->checkBox_DisableResize_gif->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_AutoSkip_CustomRes");
        if (tmp.isValid()) ui->checkBox_AutoSkip_CustomRes->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_AlwaysPreProcessAlphaPNG");
        if (tmp.isValid()) ui->checkBox_AlwaysPreProcessAlphaPNG->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_ImageQualityLevel");
        if (tmp.isValid()) ui->spinBox_ImageQualityLevel->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_ImageSaveFormat");
        if (tmp.isValid()) ui->comboBox_ImageSaveFormat->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_KeepParentFolder");
        if (tmp.isValid()) ui->checkBox_KeepParentFolder->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_BanGitee");
        if (tmp.isValid()) ui->checkBox_BanGitee->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_UpdateChannel");
        if (tmp.isValid()) comboBox_UpdateChannel_setCurrentIndex_self(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MinimizeToTaskbar");
        if (tmp.isValid()) ui->checkBox_MinimizeToTaskbar->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_custres_isAll");
        if (tmp.isValid()) ui->checkBox_custres_isAll->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/DelOriginal");
        if (tmp.isValid()) ui->checkBox_DelOriginal->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/OptGIF");
        if (tmp.isValid()) ui->checkBox_OptGIF->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/NFSound");
        if (tmp.isValid()) ui->checkBox_NfSound->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ReProFinFiles");
        if (tmp.isValid()) ui->checkBox_ReProcFinFiles->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ShowInterPro");
        if (tmp.isValid()) ui->checkBox_ShowInterPro->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/UpdatePopup");
        if (tmp.isValid()) ui->checkBox_UpdatePopup->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/FileListAutoScroll");
        if (tmp.isValid()) ui->checkBox_FileListAutoSlide->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/AutoSaveSettings");
        if (tmp.isValid()) ui->checkBox_AutoSaveSettings->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/AlwaysHideSettings");
        if (tmp.isValid()) ui->checkBox_AlwaysHideSettings->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/AlwaysHideTextBrowser");
        if (tmp.isValid()) ui->checkBox_AlwaysHideTextBrowser->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ScanSubFolders");
        if (tmp.isValid()) ui->checkBox_ScanSubFolders->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/InteractiveFileList");
        if (tmp.isValid()) ui->checkBox_FileList_Interactive->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/RetryTimes");
        if (tmp.isValid()) ui->spinBox_retry->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/AutoDetectAlphaChannel");
        if (tmp.isValid()) ui->checkBox_AutoDetectAlphaChannel->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/PromptWhenExit");
        if (tmp.isValid()) ui->checkBox_PromptWhenExit->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/KeepVideoCache");
        if (tmp.isValid()) ui->checkBox_KeepVideoCache->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_ReplaceOriginalFile");
        if (tmp.isValid()) ui->checkBox_ReplaceOriginalFile->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile");
        if (tmp.isValid()) QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal");
        if (tmp.isValid()) QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setChecked(tmp.toBool());
    }
    //===
    {
        QVariant tmp = Settings_Read_value("/settings/ProcessVideoBySegment");
        if (tmp.isValid()) ui->checkBox_ProcessVideoBySegment->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/SegmentDuration");
        if (tmp.isValid()) ui->spinBox_SegmentDuration->setValue(tmp.toInt());
    }
    //=========
    {
        QVariant tmp = Settings_Read_value("/settings/AudioDenoise");
        if (tmp.isValid()) ui->checkBox_AudioDenoise->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/AudioDenoiseLevel");
        if (tmp.isValid()) ui->doubleSpinBox_AudioDenoiseLevel->setValue(tmp.toDouble());
    }
    //=========
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_PreProcessImage");
        if (tmp.isValid()) ui->checkBox_PreProcessImage->setChecked(tmp.toBool());
    }
    //=================== Load text browser settings ==========================
    {
        QVariant tmp = Settings_Read_value("/settings/TextBrowserFontSize");
        if (tmp.isValid()) ui->spinBox_textbrowser_fontsize->setValue(tmp.toInt());
    }
    //=================== Load video settings ===========================
    {
        QVariant tmp = Settings_Read_value("/settings/VideoSettingsIsEnabled");
        if (tmp.isValid()) ui->groupBox_video_settings->setChecked(tmp.toBool());
    }
    //===
    {
        QVariant tmp = Settings_Read_value("/settings/EncoderVideo");
        if (tmp.isValid()) ui->lineEdit_encoder_vid->setText(tmp.toString());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/EncoderAudio");
        if (tmp.isValid()) ui->lineEdit_encoder_audio->setText(tmp.toString());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/PixelFormat");
        if (tmp.isValid()) ui->lineEdit_pixformat->setText(tmp.toString());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/BitrateVideo");
        if (tmp.isValid()) ui->spinBox_bitrate_vid->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/BitrateAudio");
        if (tmp.isValid()) ui->spinBox_bitrate_audio->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ExtraCommandOutput");
        if (tmp.isValid()) ui->lineEdit_ExCommand_output->setText(tmp.toString());
    }
    //===
    {
        QVariant tmp = Settings_Read_value("/settings/BitrateVideo2mp4");
        if (tmp.isValid()) ui->spinBox_bitrate_vid_2mp4->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/BitrateAudio2mp4");
        if (tmp.isValid()) ui->spinBox_bitrate_audio_2mp4->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/vcodecCopy");
        if (tmp.isValid()) ui->checkBox_vcodec_copy_2mp4->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/acodecCopy");
        if (tmp.isValid()) ui->checkBox_acodec_copy_2mp4->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_IgnoreFrameRateMode");
        if (tmp.isValid()) ui->checkBox_IgnoreFrameRateMode->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/ExtraCommand2mp4");
        if (tmp.isValid()) ui->lineEdit_ExCommand_2mp4->setText(tmp.toString());
    }
    //=============== Load output path settings ===========================
    {
        QVariant tmp = Settings_Read_value("/settings/OutPutPath");
        if (tmp.isValid()) ui->lineEdit_outputPath->setText(tmp.toString());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/OutPutPathIsEnabled");
        if (tmp.isValid()) ui->checkBox_OutPath_isEnabled->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_OutPath_KeepOriginalFileName");
        if (tmp.isValid()) ui->checkBox_OutPath_KeepOriginalFileName->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_OutPath_Overwrite");
        if (tmp.isValid()) ui->checkBox_OutPath_Overwrite->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_AutoOpenOutputPath");
        if (tmp.isValid()) ui->checkBox_AutoOpenOutputPath->setChecked(tmp.toBool());
    }
    //================== Load Anime4k settings ===================================
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_OpenCLCommandQueues_A4k");
        if (tmp.isValid()) ui->spinBox_OpenCLCommandQueues_A4k->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_OpenCLParallelIO_A4k");
        if (tmp.isValid()) ui->checkBox_OpenCLParallelIO_A4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_GPGPUModel_A4k");
        if (tmp.isValid()) ui->comboBox_GPGPUModel_A4k->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_HDNMode_Anime4k");
        if (tmp.isValid()) ui->checkBox_HDNMode_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_FastMode_Anime4K");
        if (tmp.isValid()) ui->checkBox_FastMode_Anime4K->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_ACNet_Anime4K");
        if (tmp.isValid()) ui->checkBox_ACNet_Anime4K->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_GPUMode_Anime4K");
        if (tmp.isValid()) ui->checkBox_GPUMode_Anime4K->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_Passes_Anime4K");
        if (tmp.isValid()) ui->spinBox_Passes_Anime4K->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_PushColorCount_Anime4K");
        if (tmp.isValid()) ui->spinBox_PushColorCount_Anime4K->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/doubleSpinBox_PushColorStrength_Anime4K");
        if (tmp.isValid()) ui->doubleSpinBox_PushColorStrength_Anime4K->setValue(tmp.toDouble());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/doubleSpinBox_PushGradientStrength_Anime4K");
        if (tmp.isValid()) ui->doubleSpinBox_PushGradientStrength_Anime4K->setValue(tmp.toDouble());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_SpecifyGPU_Anime4k");
        if (tmp.isValid()) ui->checkBox_SpecifyGPU_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/lineEdit_GPUs_Anime4k");
        if (tmp.isValid()) ui->lineEdit_GPUs_Anime4k->setText(tmp.toString());
    }
    //Pre-Processing
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_EnablePreProcessing_Anime4k");
        if (tmp.isValid()) ui->checkBox_EnablePreProcessing_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MedianBlur_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_MedianBlur_Pre_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MeanBlur_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_MeanBlur_Pre_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_CASSharping_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_CASSharping_Pre_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_GaussianBlurWeak_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_GaussianBlurWeak_Pre_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_GaussianBlur_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_GaussianBlur_Pre_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_BilateralFilter_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_BilateralFilter_Pre_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_BilateralFilterFaster_Pre_Anime4k");
        if (tmp.isValid()) ui->checkBox_BilateralFilterFaster_Pre_Anime4k->setChecked(tmp.toBool());
    }
    //Post-Processing
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_EnablePostProcessing_Anime4k");
        if (tmp.isValid()) ui->checkBox_EnablePostProcessing_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MedianBlur_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_MedianBlur_Post_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MeanBlur_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_MeanBlur_Post_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_CASSharping_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_CASSharping_Post_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_GaussianBlurWeak_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_GaussianBlurWeak_Post_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_GaussianBlur_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_GaussianBlur_Post_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_BilateralFilter_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_BilateralFilter_Post_Anime4k->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_BilateralFilterFaster_Post_Anime4k");
        if (tmp.isValid()) ui->checkBox_BilateralFilterFaster_Post_Anime4k->setChecked(tmp.toBool());
    }
    //===================== Load compatibility test results ============================
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW");
        if (tmp.isValid()) isCompatible_Waifu2x_NCNN_Vulkan_NEW = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P");
        if (tmp.isValid()) isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD");
        if (tmp.isValid()) isCompatible_Waifu2x_NCNN_Vulkan_OLD = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_Converter");
        if (tmp.isValid()) isCompatible_Waifu2x_Converter = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_SRMD_NCNN_Vulkan");
        if (tmp.isValid()) isCompatible_SRMD_NCNN_Vulkan = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_SRMD_CUDA");
        if (tmp.isValid()) isCompatible_SRMD_CUDA = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Anime4k_CPU");
        if (tmp.isValid()) isCompatible_Anime4k_CPU = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Anime4k_GPU");
        if (tmp.isValid()) isCompatible_Anime4k_GPU = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_FFmpeg");
        if (tmp.isValid()) isCompatible_FFmpeg = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_FFprobe");
        if (tmp.isValid()) isCompatible_FFprobe = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_ImageMagick");
        if (tmp.isValid()) isCompatible_ImageMagick = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Gifsicle");
        if (tmp.isValid()) isCompatible_Gifsicle = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_SoX");
        if (tmp.isValid()) isCompatible_SoX = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_Caffe_CPU");
        if (tmp.isValid()) isCompatible_Waifu2x_Caffe_CPU = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_Caffe_GPU");
        if (tmp.isValid()) isCompatible_Waifu2x_Caffe_GPU = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Waifu2x_Caffe_cuDNN");
        if (tmp.isValid()) isCompatible_Waifu2x_Caffe_cuDNN = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_Realsr_NCNN_Vulkan");
        if (tmp.isValid()) isCompatible_Realsr_NCNN_Vulkan = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_RifeNcnnVulkan");
        if (tmp.isValid()) isCompatible_RifeNcnnVulkan = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_CainNcnnVulkan");
        if (tmp.isValid()) isCompatible_CainNcnnVulkan = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_DainNcnnVulkan");
        if (tmp.isValid()) isCompatible_DainNcnnVulkan = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_RealCUGAN_NCNN_Vulkan");
        if (tmp.isValid()) isCompatible_RealCUGAN_NCNN_Vulkan = tmp.toBool();
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_isCompatible_RealESRGAN_NCNN_Vulkan");
        if (tmp.isValid()) isCompatible_RealESRGAN_NCNN_Vulkan = tmp.toBool();
    }
    //===
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW);
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P);
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_OLD);
    ui->checkBox_isCompatible_Waifu2x_Converter->setChecked(isCompatible_Waifu2x_Converter);
    ui->checkBox_isCompatible_SRMD_NCNN_Vulkan->setChecked(isCompatible_SRMD_NCNN_Vulkan);
    ui->checkBox_isCompatible_SRMD_CUDA->setChecked(isCompatible_SRMD_CUDA);
    ui->checkBox_isCompatible_Anime4k_CPU->setChecked(isCompatible_Anime4k_CPU);
    ui->checkBox_isCompatible_Anime4k_GPU->setChecked(isCompatible_Anime4k_GPU);
    ui->checkBox_isCompatible_FFmpeg->setChecked(isCompatible_FFmpeg);
    ui->checkBox_isCompatible_FFprobe->setChecked(isCompatible_FFprobe);
    ui->checkBox_isCompatible_ImageMagick->setChecked(isCompatible_ImageMagick);
    ui->checkBox_isCompatible_Gifsicle->setChecked(isCompatible_Gifsicle);
    ui->checkBox_isCompatible_SoX->setChecked(isCompatible_SoX);
    ui->checkBox_isCompatible_Waifu2x_Caffe_CPU->setChecked(isCompatible_Waifu2x_Caffe_CPU);
    ui->checkBox_isCompatible_Waifu2x_Caffe_GPU->setChecked(isCompatible_Waifu2x_Caffe_GPU);
    ui->checkBox_isCompatible_Waifu2x_Caffe_cuDNN->setChecked(isCompatible_Waifu2x_Caffe_cuDNN);
    ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->setChecked(isCompatible_Realsr_NCNN_Vulkan);
    ui->checkBox_isCompatible_RifeNcnnVulkan->setChecked(isCompatible_RifeNcnnVulkan);
    ui->checkBox_isCompatible_CainNcnnVulkan->setChecked(isCompatible_CainNcnnVulkan);
    ui->checkBox_isCompatible_DainNcnnVulkan->setChecked(isCompatible_DainNcnnVulkan);
    if(ui->checkBox_isCompatible_Realsr_NCNN_Vulkan) ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->setChecked(isCompatible_RealCUGAN_NCNN_Vulkan); // Corrected: Use Realsr checkbox for RealCUGAN variable
    if(ui->checkBox_isCompatible_Realsr_NCNN_Vulkan) ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->setChecked(isCompatible_RealESRGAN_NCNN_Vulkan);
    //======================== Load VFI settings ========================
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_VfiAfterScale_VFI");
        if (tmp.isValid()) ui->checkBox_VfiAfterScale_VFI->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiThread_VFI");
        if (tmp.isValid()) ui->checkBox_MultiThread_VFI->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_AutoAdjustNumOfThreads_VFI");
        if (tmp.isValid()) ui->checkBox_AutoAdjustNumOfThreads_VFI->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_FrameInterpolationOnly_Video");
        if (tmp.isValid()) ui->checkBox_FrameInterpolationOnly_Video->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/groupBox_FrameInterpolation");
        if (tmp.isValid()) ui->groupBox_FrameInterpolation->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_VFI");
        if (tmp.isValid()) ui->checkBox_MultiGPU_VFI->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_TTA_VFI");
        if (tmp.isValid()) ui->checkBox_TTA_VFI->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_UHD_VFI");
        if (tmp.isValid()) ui->checkBox_UHD_VFI->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_Model_VFI");
        if (tmp.isValid()) ui->comboBox_Model_VFI->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/Available_GPUID_FrameInterpolation");
        if (tmp.isValid()) Available_GPUID_FrameInterpolation = tmp.toStringList();
    }
    FrameInterpolation_DetectGPU_finished();
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_GPUID_VFI");
        if (tmp.isValid()) ui->comboBox_GPUID_VFI->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/lineEdit_MultiGPU_IDs_VFI");
        if (tmp.isValid()) ui->lineEdit_MultiGPU_IDs_VFI->setText(tmp.toString());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_NumOfThreads_VFI");
        if (tmp.isValid()) ui->spinBox_NumOfThreads_VFI->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_Engine_VFI");
        if (tmp.isValid()) Old_FrameInterpolation_Engine_Index = tmp.toInt();
    }
    ui->comboBox_Engine_VFI->setCurrentIndex(Old_FrameInterpolation_Engine_Index);
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_MultipleOfFPS_VFI");
        if (tmp.isValid()) ui->spinBox_MultipleOfFPS_VFI->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_TileSize_VFI");
        if (tmp.isValid()) ui->spinBox_TileSize_VFI->setValue(tmp.toInt());
    }
    //==================== Load language settings =====================
    {
        QVariant tmp = Settings_Read_value("/settings/Language");
        if (tmp.isValid()) ui->comboBox_language->setCurrentIndex(tmp.toInt());
    }
    on_comboBox_language_currentIndexChanged(0);
    //====================================================
    on_groupBox_FrameInterpolation_clicked();
    isCustomVideoSettingsClicked=false;
    on_groupBox_video_settings_clicked();
    isCustomVideoSettingsClicked=true;
    on_checkBox_AlwaysHideSettings_stateChanged(0);
    on_checkBox_AlwaysHideTextBrowser_stateChanged(0);
    on_checkBox_DelOriginal_stateChanged(0);
    on_checkBox_FileList_Interactive_stateChanged(0);
    on_checkBox_OutPath_isEnabled_stateChanged(0);
    on_checkBox_AudioDenoise_stateChanged(0);
    on_checkBox_ProcessVideoBySegment_stateChanged(0);
    on_checkBox_EnablePreProcessing_Anime4k_stateChanged(0);
    on_checkBox_EnablePostProcessing_Anime4k_stateChanged(0);
    on_checkBox_SpecifyGPU_Anime4k_stateChanged(0);
    on_checkBox_GPUMode_Anime4K_stateChanged(0);
    on_checkBox_ShowInterPro_stateChanged(0);
    //====
    on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(0);
    on_comboBox_Engine_GIF_currentIndexChanged(ui->comboBox_Engine_GIF->currentIndex());
    isShowAnime4kWarning=false;
    on_comboBox_Engine_Image_currentIndexChanged(ui->comboBox_Engine_Image->currentIndex());
    on_comboBox_Engine_Video_currentIndexChanged(ui->comboBox_Engine_Video->currentIndex());
    on_comboBox_ImageStyle_currentIndexChanged(0);
    on_comboBox_model_vulkan_currentIndexChanged(0);
    {
        QVariant tmp = Settings_Read_value("/settings/ImageDenoiseLevel");
        if (tmp.isValid()) ui->spinBox_DenoiseLevel_image->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/GIFDenoiseLevel");
        if (tmp.isValid()) ui->spinBox_DenoiseLevel_gif->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/VideoDenoiseLevel");
        if (tmp.isValid()) ui->spinBox_DenoiseLevel_video->setValue(tmp.toInt());
    }
    //=====
    on_spinBox_textbrowser_fontsize_valueChanged(0);
    //===
    on_comboBox_AspectRatio_custRes_currentIndexChanged(0);
    //=====
    Init_Table();
    //====
    on_checkBox_acodec_copy_2mp4_stateChanged(1);
    on_checkBox_vcodec_copy_2mp4_stateChanged(1);
    on_checkBox_MultiThread_VFI_stateChanged(1);
    //==================================
    isReadOldSettings=false;
    QFile::remove(Current_Path+"/settings_old.ini");
    Settings_Save(); // Re-save to ensure new fields are present if it was an old version
    //==================================
    return 0;
}

QVariant MainWindow::Settings_Read_value(QString Key)
{
    QString settings_ini_old = Current_Path+"/settings_old.ini";
    QString settings_ini_new = Current_Path+"/settings.ini";
    QSettings *configIniRead_new = new QSettings(settings_ini_new, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    configIniRead_new->setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
    //====
    if(isReadOldSettings&&QFile::exists(settings_ini_old))
    {
        QSettings *configIniRead_old = new QSettings(settings_ini_old, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        configIniRead_old->setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
        //====
        if(configIniRead_old->contains(Key))
        {
            return configIniRead_old->value(Key);
        }
        return configIniRead_new->value(Key);
    }
    return configIniRead_new->value(Key);
}

/*
Save settings pushbutton
Save settings and show a popup
*/
void MainWindow::on_pushButton_SaveSettings_clicked()
{
    Settings_Save();
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("Settings saved successfully!"));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(false);
    MSG->show();
}
/*
Reset settings
Delete the settings file, set reset flag to true, and show a popup
*/
void MainWindow::on_pushButton_ResetSettings_clicked()
{
    QMessageBox Msg(QMessageBox::Question, QString(tr("Warning")), QString(tr("Do you really wanna RESET all the settings?")));
    Msg.setIcon(QMessageBox::Warning);
    Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
    Msg.exec();
    if (Msg.clickedButton() == pNoBtn)return;
    //============
    QString settings_ini = Current_Path+"/settings.ini";
    QFile::remove(settings_ini);
    Settings_isReseted = true;
    //============
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("The settings file has been reset, please restart the software manually for the default settings to take effect."));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(false);
    MSG->show();
}

void MainWindow::RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay()
{
    if (!checkBox_MultiGPU_RealesrganNcnnVulkan || !checkBox_MultiGPU_RealesrganNcnnVulkan->isChecked() || !comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan) {
        if(checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(false);
        if(spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(0);
        return;
    }

    QString selectedGPUID = comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan->currentText();
    if(selectedGPUID.isEmpty()) return;

    bool found = false;
    for (const auto& gpuConfig : m_realesrgan_gpuJobConfig_temp) {
        if (gpuConfig.value("id") == selectedGPUID) {
            if(checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(gpuConfig.value("enabled", "false") == "true");
            if(spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(gpuConfig.value("tilesize", "0").toInt());
            found = true;
            break;
        }
    }

    if (!found) {
        if(checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(false);
        if(spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan && spinBox_TileSize_RealsrNCNNVulkan) {
            spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(spinBox_TileSize_RealsrNCNNVulkan->value());
        } else if(spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) {
            spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(0);
        }
    }
}