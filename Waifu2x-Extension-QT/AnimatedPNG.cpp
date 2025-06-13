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
#include "utils/ffprobe_helpers.h"
#include "ui_mainwindow.h"
/*
Main function for processing APNG files
*/
void MainWindow::APNG_Main(int rowNum,bool isFromImageList)
{
    // Start processing
    bool isNeedRemoveFromCustResList = false;
    QString sourceFileFullPath="";
    if(isFromImageList==false)
    {
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
        sourceFileFullPath = Table_model_gif->item(rowNum,2)->text();
    }
    else
    {
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Processing");
        sourceFileFullPath = Table_model_image->item(rowNum,2)->text();
        emit Send_TextBrowser_NewMessage("It is detected that this PNG is actually an APNG(Animated PNG), so it will be processed as an APNG. ["+sourceFileFullPath+"]");
    }
    //===============================
    // Check if we should auto add to the custom resolution list
    double double_ScaleRatio_gif = ui->doubleSpinBox_ScaleRatio_gif->value();
    // If no custom resolution and scale ratio is fractional
    if((CustRes_isContained(sourceFileFullPath) == false) && (double_ScaleRatio_gif != qRound(double_ScaleRatio_gif)))
    {
        //===================== Get resolution =============================
        QMap<QString,int> Map_OrgRes = Image_Gif_Read_Resolution(sourceFileFullPath);
        //========= Calculate new height and width ==================
        double ScaleRatio_double = ui->doubleSpinBox_ScaleRatio_gif->value();
        int Height_new = qRound(ScaleRatio_double * Map_OrgRes["height"]);
        int width_new = qRound(ScaleRatio_double * Map_OrgRes["width"]);
        if(Height_new<1 || width_new<1)
        {
            emit Send_TextBrowser_NewMessage("Warning! Unable to read the resolution of ["+sourceFileFullPath+"]. This file will only be scaled to "+QString::number((int)ScaleRatio_double,10)+"X.");
        }
        //======== Store into custom resolution list ============
        QMap<QString,QString> res_map;
        res_map["fullpath"] = sourceFileFullPath;
        res_map["height"] = QString::number(Height_new,10);
        res_map["width"] = QString::number(width_new,10);
        Custom_resolution_list.append(res_map);
        isNeedRemoveFromCustResList = true;
    }
    //======================
    // Read source file info
    QFileInfo fileinfo_sourceFileFullPath(sourceFileFullPath);
    QString sourceFileFullPath_baseName = file_getBaseName(sourceFileFullPath);
    QString sourceFileFullPath_fileExt = fileinfo_sourceFileFullPath.suffix();
    QString sourceFileFullPath_folderPath = file_getFolderPath(fileinfo_sourceFileFullPath);
    // Generate various paths
    QString splitFramesFolder = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+sourceFileFullPath_fileExt+"_splitFramesFolder_W2xEX";
    QString scaledFramesFolder = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+sourceFileFullPath_fileExt+"_scaledFramesFolder_W2xEX";
    QString resultFileFullPath="";
    if(CustRes_isContained(sourceFileFullPath) && isNeedRemoveFromCustResList==false)
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(sourceFileFullPath);//res_map["fullpath"],["height"],["width"]
        resultFileFullPath = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+QString::number(Res_map["width"].toInt(), 10)+"x"+QString::number(Res_map["height"].toInt(),10)+"_"+QString("%1").arg(ui->spinBox_DenoiseLevel_gif->value())+"n_W2xEX"+"."+sourceFileFullPath_fileExt;
    }
    else
    {
        resultFileFullPath = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+QString("%1").arg(ui->doubleSpinBox_ScaleRatio_gif->value())+"x_"+QString("%1").arg(ui->spinBox_DenoiseLevel_gif->value())+"n_W2xEX"+"."+sourceFileFullPath_fileExt;
    }
    //=======================
    // Start splitting
    APNG_Split2Frames(sourceFileFullPath,splitFramesFolder);
    // Was the process paused?
    if(waifu2x_STOP)
    {
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
        }
        file_DelDir(splitFramesFolder);
        file_DelDir(scaledFramesFolder);
        if(isNeedRemoveFromCustResList)CustRes_remove(sourceFileFullPath);
        return;
    }
    // Check whether splitting succeeded
    // Get the list of split frame files
    QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if(framesFileName_qStrList.isEmpty())//Check if GIF was split successfully
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+sourceFileFullPath+tr("]. Error: [Can't split GIF into frames.]"));
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        }
        file_DelDir(splitFramesFolder);
        file_DelDir(scaledFramesFolder);
        if(isNeedRemoveFromCustResList)CustRes_remove(sourceFileFullPath);
        emit Send_progressbar_Add();
        return;
    }
    //=======================
    // Start scaling & assembling
    bool isSuccessfullyScaled = false;
    int engineIndex = ui->comboBox_Engine_GIF->currentIndex(); // Use engine from GIF tab for APNG
    switch(engineIndex)
    {
        case 0:
            {
                isSuccessfullyScaled = APNG_Waifu2xNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 1:
            {
                isSuccessfullyScaled = APNG_Waifu2xConverter(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 2:
            {
                isSuccessfullyScaled = APNG_SrmdNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 3:
            {
                isSuccessfullyScaled = APNG_Anime4k(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 4:
            {
                isSuccessfullyScaled = APNG_Waifu2xCaffe(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 5:
            {
                isSuccessfullyScaled = APNG_RealsrNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 6:
            {
                isSuccessfullyScaled = APNG_SrmdCUDA(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 7: // RealCUGAN-ncnn-Vulkan
            {
                isSuccessfullyScaled = APNG_RealcuganNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
        case 8: // RealESRGAN-ncnn-Vulkan
            {
                isSuccessfullyScaled = APNG_RealESRGANNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
                break;
            }
    }
    //============
    // Delete cache
    file_DelDir(splitFramesFolder);
    file_DelDir(scaledFramesFolder);
    if(isNeedRemoveFromCustResList)CustRes_remove(sourceFileFullPath);
    //============
    // Failure or pause during scaling
    if(waifu2x_STOP)
    {
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted");
        }
        return;
    }
    if(isSuccessfullyScaled==false)
    {
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        }
        emit Send_progressbar_Add();
        return;
    }
    // Check whether result file exists
    if(QFile::exists(resultFileFullPath)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+sourceFileFullPath+tr("]. Error: [Unable to assemble APNG.]"));
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        }
        emit Send_progressbar_Add();
        return;
    }
    //===================
    // Delete original file and update table
    if(ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked())
    {
        if(ReplaceOriginalFile(sourceFileFullPath,resultFileFullPath)==false)
        {
            if(QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->isChecked())
            {
                file_MoveToTrash(sourceFileFullPath);
            }
            else
            {
                QFile::remove(sourceFileFullPath);
            }
        }
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted");
        }
    }
    else
    {
        if(isFromImageList)
        {
            emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
        }
        else
        {
            emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished");
        }
    }
    // Move to output path
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(resultFileFullPath,sourceFileFullPath);
    }
    // Update progress bar
    emit Send_progressbar_Add();
    //=====
    return;
}
/*
Split APNG into frames
*/
void MainWindow::APNG_Split2Frames(QString sourceFileFullPath,QString splitFramesFolder)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting APNG:[")+sourceFileFullPath+"]");
    //========================
    file_DelDir(splitFramesFolder);
    file_mkDir(splitFramesFolder);
    //========================
    QString sourceFileFullPath_baseName = file_getBaseName(sourceFileFullPath);
    QString splitCopy = splitFramesFolder+"/W2xEX_"+sourceFileFullPath_baseName+".apng";
    QFile::copy(sourceFileFullPath,splitCopy);
    //========================
    QString program = Current_Path+"/apngdis_waifu2xEX.exe";
    QString cmd = "\""+program+"\" \""+splitCopy+"\" \"0\"";
    QProcess SplitAPNG;
    bool ok = runProcess(&SplitAPNG, cmd);
    if(waifu2x_STOP || !ok)
    {
        SplitAPNG.close();
        return;
    }
    //========================
    QFile::remove(splitCopy);
    QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    foreach (QString fname, framesFileName_qStrList)
    {
        if(fname.toLower().contains(".txt"))QFile::remove(splitFramesFolder+"/"+fname);
    }
    //========================
    emit Send_TextBrowser_NewMessage(tr("Finish splitting APNG:[")+sourceFileFullPath+"]");
    return;
}
/*
Assemble frames into an APNG
*/
void MainWindow::APNG_Frames2APNG(QString sourceFileFullPath,QString scaledFramesFolder,QString resultFileFullPath,bool isOverScaled)
{
    emit Send_TextBrowser_NewMessage(tr("Start assembling APNG:[")+sourceFileFullPath+"]");
    // Manually resize frames if needed
    bool CustRes_isEnabled = CustRes_isContained(sourceFileFullPath);
    if(CustRes_isEnabled || isOverScaled)
    {
        int New_width=0;
        int New_height=0;
        if(isOverScaled==true && CustRes_isEnabled==false)
        {
            QMap<QString,int> res_map = Image_Gif_Read_Resolution(sourceFileFullPath);
            int OriginalScaleRatio = ui->doubleSpinBox_ScaleRatio_gif->value();
            New_width = res_map["width"]*OriginalScaleRatio;
            New_height = res_map["height"]*OriginalScaleRatio;
        }
        if(CustRes_isEnabled==true)
        {
            QMap<QString, QString> Res_map = CustRes_getResMap(sourceFileFullPath);//res_map["fullpath"],["height"],["width"]
            New_width = Res_map["width"].toInt();
            New_height = Res_map["height"].toInt();
        }
        ImagesResize_Folder_MultiThread(New_width,New_height,scaledFramesFolder);
    }
    //========================= Use ffprobe to read APNG info ======================
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffprobe_waifu2xEX.exe", sourceFileFullPath);
    QString FPS_Division = doc.object().value("streams").toArray().isEmpty() ? QString() :
                           doc.object().value("streams").toArray().at(0).toObject().value("avg_frame_rate").toString();
    //=======================
    int fps = 0;
    if(!FPS_Division.isEmpty())
    {
        QStringList FPS_Nums = FPS_Division.split("/");
        if(FPS_Nums.size()==2)
        {
            double FPS_Num_0 = FPS_Nums.at(0).toDouble();
            double FPS_Num_1 = FPS_Nums.at(1).toDouble();
            if(FPS_Num_0>0&&FPS_Num_1>0)
            {
                double fps_double = FPS_Num_0/FPS_Num_1;
                fps = qRound(fps_double);
            }
        }
    }
    //==========
    if(fps<=0)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read the FPS of this APNG. Failed to assemble APNG:[")+sourceFileFullPath+"]");
        return;
    }
    //========================
    // Delete existing result file
    QFile::remove(resultFileFullPath);
    //========================
    QString program = Current_Path+"/apngasm_waifu2xEX.exe";
    QString cmd ="\""+program+"\" \""+resultFileFullPath+"\" \""+scaledFramesFolder.replace("%","%%")+"/*.png\" -kp -kc -z1 1 "+QString::number(fps,10)+" -l0";
    QProcess AssembleAPNG;
    bool ok = runProcess(&AssembleAPNG, cmd);
    if(waifu2x_STOP || !ok)
    {
        AssembleAPNG.close();
        QFile::remove(resultFileFullPath);
        return;
    }
    //========================
    emit Send_TextBrowser_NewMessage(tr("Finish assembling APNG:[")+sourceFileFullPath+"]");
}
/*
Detect whether a PNG file is animated
*/
bool MainWindow::APNG_isAnimatedPNG(int rowNum)
{
    QString sourceFileFullPath = Table_model_image->item(rowNum,2)->text();
    //========================= Use ffprobe to read APNG info ======================
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffprobe_waifu2xEX.exe", sourceFileFullPath);
    QJsonArray streams = doc.object().value("streams").toArray();
    if(streams.isEmpty())
        return false;
    QString codec = streams.at(0).toObject().value("codec_name").toString().trimmed().toLower();
    return codec == "apng";
}
