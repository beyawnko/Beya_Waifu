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
// int MainWindow::Settings_Save() -> Definition is now a stub in mainwindow.cpp
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
        QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setChecked(1);// 
//         if(isBetaVer)comboBox_UpdateChannel_setCurrentIndex_self(1);
        Settings_Save();
//         Settings_Read_Apply();
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
//             if(isBetaVer)comboBox_UpdateChannel_setCurrentIndex_self(1);
//             Settings_Save();
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
        if (tmp.isValid()) ui->checkBox_TTA_RealsrNCNNVulkan->setChecked(tmp.toBool());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_Model_RealsrNCNNVulkan");
        if (tmp.isValid()) ui->comboBox_Model_RealsrNCNNVulkan->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/spinBox_TileSize_RealsrNCNNVulkan");
        if (tmp.isValid()) ui->spinBox_TileSize_RealsrNCNNVulkan->setValue(tmp.toInt());
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
        QVariant tmp = Settings_Read_value("/settings/Available_GPUID_Waifu2xNcnnVulkan");
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
    Realsr_ncnn_vulkan_DetectGPU_finished();
    {
        QVariant tmp = Settings_Read_value("/settings/comboBox_GPUID_RealsrNCNNVulkan");
        if (tmp.isValid()) if(ui->comboBox_GPUID_RealsrNCNNVulkan) ui->comboBox_GPUID_RealsrNCNNVulkan->setCurrentIndex(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_RealesrganNcnnVulkan");
        if (tmp.isValid()) if(ui->checkBox_MultiGPU_RealesrganNcnnVulkan) ui->checkBox_MultiGPU_RealesrganNcnnVulkan->setChecked(tmp.toBool());
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
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_MultiGPU_SrmdNCNNVulkan");
        if (tmp.isValid()) if(ui->checkBox_MultiGPU_SrmdNCNNVulkan) ui->checkBox_MultiGPU_SrmdNCNNVulkan->setChecked(tmp.toBool());
    }

    // RealCUGAN Settings
    QVariant realCuganModel = Settings_Read_value("/settings/RealCUGAN_Model");
    if(ui->comboBox_Model_RealCUGAN && realCuganModel.isValid()) ui->comboBox_Model_RealCUGAN->setCurrentIndex(realCuganModel.toInt());

    QVariant scaleSetting = Settings_Read_value("/settings/RealCUGAN_Scale");
    if(ui->spinBox_Scale_RealCUGAN && scaleSetting.isValid()) {
        ui->spinBox_Scale_RealCUGAN->setValue(scaleSetting.toInt());
    }
    
    QVariant denoiseSetting = Settings_Read_value("/settings/RealCUGAN_DenoiseLevel");
    if(ui->spinBox_DenoiseLevel_RealCUGAN && denoiseSetting.isValid()) {
        ui->spinBox_DenoiseLevel_RealCUGAN->setValue(denoiseSetting.toInt());
    }

    QVariant tileSizeCugan = Settings_Read_value("/settings/RealCUGAN_TileSize");
    if(ui->spinBox_TileSize_RealCUGAN && tileSizeCugan.isValid()) ui->spinBox_TileSize_RealCUGAN->setValue(tileSizeCugan.toInt());
    
    QVariant ttaCugan = Settings_Read_value("/settings/RealCUGAN_TTA");
    if(ui->checkBox_TTA_RealCUGAN && ttaCugan.isValid()) ui->checkBox_TTA_RealCUGAN->setChecked(ttaCugan.toBool());
    
    {
        QVariant tmp = Settings_Read_value("/settings/RealCUGAN_Available_GPUID");
        if (tmp.isValid()) Available_GPUID_RealCUGAN = tmp.toStringList();
    }
    Realcugan_NCNN_Vulkan_DetectGPU_finished();

    QVariant gpuIDCugan = Settings_Read_value("/settings/RealCUGAN_GPUID");
    if(ui->comboBox_GPUID_RealCUGAN && gpuIDCugan.isValid()) ui->comboBox_GPUID_RealCUGAN->setCurrentIndex(gpuIDCugan.toInt());

    QVariant multiGpuCugan = Settings_Read_value("/settings/RealCUGAN_MultiGPU_Enabled");
    if(ui->checkBox_MultiGPU_RealCUGAN && multiGpuCugan.isValid()) ui->checkBox_MultiGPU_RealCUGAN->setChecked(multiGpuCugan.toBool());
    
    {
        QVariant tmp = Settings_Read_value("/settings/RealCUGAN_GPUJobConfig_MultiGPU");
        if (tmp.isValid()) m_realcugan_gpuJobConfig_temp = tmp.value<QList<QMap<QString, QString>>>();
    }
    if (ui->checkBox_MultiGPU_RealCUGAN && ui->checkBox_MultiGPU_RealCUGAN->isChecked()) {
        if (ui->listWidget_GPUList_MultiGPU_RealCUGAN) {
            ui->listWidget_GPUList_MultiGPU_RealCUGAN->clear();
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
                ui->listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
            }
        }
    }

    // RealESRGAN Settings
    if(ui->comboBox_Model_RealsrNCNNVulkan) {
        QString modelName;
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_ModelName");
        if (tmp.isValid()) modelName = tmp.toString();

        if (!modelName.isEmpty()) {
            int modelIndex = ui->comboBox_Model_RealsrNCNNVulkan->findText(modelName);
            if (modelIndex != -1) ui->comboBox_Model_RealsrNCNNVulkan->setCurrentIndex(modelIndex);
        }
    }
    {
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_TileSize");
        if (tmp.isValid()) if(ui->spinBox_TileSize_RealsrNCNNVulkan) ui->spinBox_TileSize_RealsrNCNNVulkan->setValue(tmp.toInt());
    }
    {
        QVariant tmp = Settings_Read_value("/settings/RealESRGAN_TTA");
        if (tmp.isValid()) if(ui->checkBox_TTA_RealsrNCNNVulkan) ui->checkBox_TTA_RealsrNCNNVulkan->setChecked(tmp.toBool());
    }
    RealESRGAN_ncnn_vulkan_DetectGPU_finished();

    QVariant gpuIDEsrgan = Settings_Read_value("/settings/RealESRGAN_GPUID");
    if(ui->comboBox_GPUID_RealsrNCNNVulkan && gpuIDEsrgan.isValid()) ui->comboBox_GPUID_RealsrNCNNVulkan->setCurrentIndex(gpuIDEsrgan.toInt());
    
    QVariant multiGpuEsrgan = Settings_Read_value("/settings/RealESRGAN_MultiGPU_Enabled");
    if(ui->checkBox_MultiGPU_RealesrganNcnnVulkan && multiGpuEsrgan.isValid()) ui->checkBox_MultiGPU_RealesrganNcnnVulkan->setChecked(multiGpuEsrgan.toBool());
    
    if (ui->checkBox_MultiGPU_RealesrganNcnnVulkan && ui->checkBox_MultiGPU_RealesrganNcnnVulkan->isChecked()) {
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
    //{ // Error: ui->checkBox_KeepParentFolder not a member
    //    QVariant tmp = Settings_Read_value("/settings/checkBox_KeepParentFolder");
    //    if (tmp.isValid()) ui->checkBox_KeepParentFolder->setChecked(tmp.toBool());
    //}
    {
        QVariant tmp = Settings_Read_value("/settings/checkBox_BanGitee");
        if (tmp.isValid()) ui->checkBox_BanGitee->setChecked(tmp.toBool());
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
//         if (tmp.isValid()) ui->spinBox_retry->setValue(tmp.toInt());
    }
    // Stray opening brace removed from before this block
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
        if (tmp.isValid()) ui->spinBox_VideoSplitDuration->setValue(tmp.toInt());
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
        // if (tmp.isValid()) ui->checkBox_OutPath_KeepOriginalFileName->setChecked(tmp.toBool()); // Commented out: checkBox_OutPath_KeepOriginalFileName does not exist in UI
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

    // Load the master VFI setting for checkBox_EnableVFI_Home.
    // Its toggled() signal, connected in MainWindow constructor, will call on_checkBox_EnableVFI_Home_toggled().
    // That slot will then correctly set:
    // 1. ui->groupBox_FrameInterpolation->setChecked()
    // 2. ui->checkBox_FrameInterpolationOnly_Video->setEnabled()
    // 3. ui->checkBox_FrameInterpolationOnly_Video->setChecked() (if VFI is disabled)
    bool vfiEnabled = Settings_Read_value("/settings/EnableVFI_Home", false).toBool();

    // We still need to load the independent checked state of FrameInterpolationOnly_Video for when VFI *is* enabled.
    // The enabled state and the forced unchecked state (when VFI is off) are handled by the slot.
    if (vfiEnabled) { // Only restore its checked state if VFI is enabled, otherwise slot forces it false.
        QVariant tmpVfiOnly = Settings_Read_value("/settings/checkBox_FrameInterpolationOnly_Video", false).toBool();
        if (tmpVfiOnly.isValid()) {
            // Temporarily disable signals to prevent its toggled signal from firing if it has one,
            // or to avoid recursive calls if it were part of the same sync chain directly.
            // However, checkBox_FrameInterpolationOnly_Video does not have its own sync slot in this refactor.
        }
    }
    // Note: The state of groupBox_FrameInterpolation itself is no longer directly loaded from a separate setting.

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
    // Language settings removed; default to English
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
    return Settings_Read_value(Key, QVariant());
}

QVariant MainWindow::Settings_Read_value(const QString &Key,
                                         const QVariant &defaultValue)
{
    QString settings_ini_old = Current_Path + "/settings_old.ini";
    QString settings_ini_new = Current_Path + "/settings.ini";
    QSettings configIniRead_new(settings_ini_new, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    configIniRead_new.setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
    //====
    if (isReadOldSettings && QFile::exists(settings_ini_old))
    {
        QSettings configIniRead_old(settings_ini_old, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
        configIniRead_old.setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
        //====
        if (configIniRead_old.contains(Key))
        {
            return configIniRead_old.value(Key, defaultValue);
        }
    }
    return configIniRead_new.value(Key, defaultValue);
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
    if (!ui->checkBox_MultiGPU_RealesrganNcnnVulkan || !ui->checkBox_MultiGPU_RealesrganNcnnVulkan->isChecked() || !ui->comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan) {
        if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(false);
        if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(0);
        return;
    }

    QString selectedGPUID = ui->comboBox_GPUIDs_MultiGPU_RealesrganNcnnVulkan->currentText();
    if(selectedGPUID.isEmpty()) return;

    bool found = false;
    //for (const auto& gpuConfig : m_realesrgan_gpuJobConfig_temp) { // Error: m_realesrgan_gpuJobConfig_temp not declared
    //    if (gpuConfig.value("id") == selectedGPUID) {
    //        if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(gpuConfig.value("enabled", "false") == "true");
    //        if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(gpuConfig.value("tilesize", "0").toInt());
    //        found = true;
    //        break;
    //    }
    //}

    if (!found) {
        if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setChecked(false);
        if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan && ui->spinBox_TileSize_RealsrNCNNVulkan) {
            ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(ui->spinBox_TileSize_RealsrNCNNVulkan->value());
        } else if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan) {
            ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealesrganNcnnVulkan->setValue(0);
        }
    }
}