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
// #include "utils/ffprobe_helpers.h" // Not needed anymore due to getOrFetchMetadata
#include "ui_mainwindow.h"

/*
Main function for processing APNG files

NOTE:
The statements like `emit Send_TextBrowser_NewMessage(...)` call Qt signals
defined on MainWindow. When preprocessed, `emit` expands to nothing which can
confuse static analyzers such as cppcheck and lead to false shadowFunction
warnings. These identifiers are not local variables.
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

    if((CustRes_isContained(sourceFileFullPath) == false) && (double_ScaleRatio_gif != qRound(double_ScaleRatio_gif)))
    {
        FileMetadataCache metadata = getOrFetchMetadata(sourceFileFullPath);
        if (!metadata.isValid || metadata.width == 0 || metadata.height == 0) {
            emit Send_TextBrowser_NewMessage(tr("Warning! Unable to read the resolution of [") + sourceFileFullPath + tr("]. This file will only be scaled to ") + QString::number((int)double_ScaleRatio_gif,10) + "X.");
            if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
            else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
            emit Send_progressbar_Add();
            return;
        }
        int origWidth = metadata.width;
        int origHeight = metadata.height;

        int newHeight = qRound(double_ScaleRatio_gif * origHeight);
        int newWidth = qRound(double_ScaleRatio_gif * origWidth);

        if(newHeight<1 || newWidth<1)
        {
            emit Send_TextBrowser_NewMessage("Warning! Invalid new dimensions calculated for ["+sourceFileFullPath+"]. This file will only be scaled to "+QString::number((int)double_ScaleRatio_gif,10)+"X.");
             // If fractional scaling cannot determine original size, it's an error for this path.
            if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
            else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
            emit Send_progressbar_Add();
            return;
        }

        QMap<QString,QString> resMap;
        resMap["fullpath"] = sourceFileFullPath;
        resMap["height"] = QString::number(newHeight,10);
        resMap["width"] = QString::number(newWidth,10);
        Custom_resolution_list.append(resMap);
        isNeedRemoveFromCustResList = true;
    }
    //======================
    QFileInfo fileinfo_sourceFileFullPath(sourceFileFullPath);
    QString sourceFileFullPath_baseName = file_getBaseName(sourceFileFullPath);
    QString sourceFileFullPath_fileExt = fileinfo_sourceFileFullPath.suffix();
    QString sourceFileFullPath_folderPath = file_getFolderPath(fileinfo_sourceFileFullPath);

    QString splitFramesFolder = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+sourceFileFullPath_fileExt+"_splitFramesFolder_W2xEX";
    QString scaledFramesFolder = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+sourceFileFullPath_fileExt+"_scaledFramesFolder_W2xEX";
    QString resultFileFullPath="";

    if(CustRes_isContained(sourceFileFullPath) && isNeedRemoveFromCustResList==false)
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(sourceFileFullPath);
        resultFileFullPath = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+QString::number(Res_map["width"].toInt(), 10)+"x"+QString::number(Res_map["height"].toInt(),10)+"_"+QString("%1").arg(ui->spinBox_DenoiseLevel_gif->value())+"n_W2xEX"+"."+sourceFileFullPath_fileExt;
    }
    else
    {
        resultFileFullPath = sourceFileFullPath_folderPath+"/"+sourceFileFullPath_baseName+"_"+QString("%1").arg(ui->doubleSpinBox_ScaleRatio_gif->value())+"x_"+QString("%1").arg(ui->spinBox_DenoiseLevel_gif->value())+"n_W2xEX"+"."+sourceFileFullPath_fileExt;
    }

    APNG_Split2Frames(sourceFileFullPath,splitFramesFolder);

    if(waifu2x_STOP)
    {
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted"); }
        file_DelDir(splitFramesFolder);
        file_DelDir(scaledFramesFolder);
        if(isNeedRemoveFromCustResList)CustRes_remove(sourceFileFullPath);
        return;
    }

    QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if(framesFileName_qStrList.isEmpty())
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+sourceFileFullPath+tr("]. Error: [Can't split APNG into frames.]"));
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
        file_DelDir(splitFramesFolder);
        file_DelDir(scaledFramesFolder);
        if(isNeedRemoveFromCustResList)CustRes_remove(sourceFileFullPath);
        emit Send_progressbar_Add();
        return;
    }

    bool isSuccessfullyScaled = false;
    int engineIndex = ui->comboBox_Engine_GIF->currentIndex();
    switch(engineIndex)
    {
        case 0: isSuccessfullyScaled = APNG_Waifu2xNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 1: isSuccessfullyScaled = APNG_Waifu2xConverter(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 2: isSuccessfullyScaled = APNG_SrmdNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 3: isSuccessfullyScaled = APNG_Anime4k(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 4: isSuccessfullyScaled = APNG_Waifu2xCaffe(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 5: isSuccessfullyScaled = APNG_RealsrNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 6: isSuccessfullyScaled = APNG_SrmdCUDA(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 7: isSuccessfullyScaled = APNG_RealcuganNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
        case 8: isSuccessfullyScaled = APNG_RealESRGANNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
    }

    file_DelDir(splitFramesFolder);
    file_DelDir(scaledFramesFolder);
    if(isNeedRemoveFromCustResList)CustRes_remove(sourceFileFullPath);

    if(waifu2x_STOP)
    {
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Interrupted"); }
        return;
    }
    if(isSuccessfullyScaled==false)
    {
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
        emit Send_progressbar_Add();
        return;
    }

    if(QFile::exists(resultFileFullPath)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+sourceFileFullPath+tr("]. Error: [Unable to assemble APNG.]"));
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed"); }
        emit Send_progressbar_Add();
        return;
    }

    if(ui->checkBox_DelOriginal->isChecked()||ui->checkBox_ReplaceOriginalFile->isChecked())
    {
        if(ReplaceOriginalFile(sourceFileFullPath,resultFileFullPath)==false)
        {
            if(QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->isChecked()) { file_MoveToTrash(sourceFileFullPath); }
            else { QFile::remove(sourceFileFullPath); }
        }
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished, original file deleted"); }
    }
    else
    {
        if(isFromImageList) { emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished"); }
        else { emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, "Finished"); }
    }

    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        MoveFileToOutputPath(resultFileFullPath,sourceFileFullPath);
    }
    emit Send_progressbar_Add();
    return;
}

void MainWindow::APNG_Split2Frames(QString sourceFileFullPath,QString splitFramesFolder)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting APNG:[")+sourceFileFullPath+"]");
    file_DelDir(splitFramesFolder);
    file_mkDir(splitFramesFolder);
    QString sourceFileFullPath_baseName = file_getBaseName(sourceFileFullPath);
    QString splitCopy = splitFramesFolder+"/W2xEX_"+sourceFileFullPath_baseName+".apng";
    QFile::copy(sourceFileFullPath,splitCopy);
    QString program = Current_Path+"/apngdis_waifu2xEX.exe";
    QString cmd = "\""+program+"\" \""+splitCopy+"\" \"0\"";
    QProcess SplitAPNG;
    bool ok = runProcess(&SplitAPNG, cmd);
    if(waifu2x_STOP || !ok)
    {
        SplitAPNG.close();
        return;
    }
    QFile::remove(splitCopy);
    QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    foreach (QString fname, framesFileName_qStrList)
    {
        if(fname.toLower().contains(".txt"))QFile::remove(splitFramesFolder+"/"+fname);
    }
    emit Send_TextBrowser_NewMessage(tr("Finish splitting APNG:[")+sourceFileFullPath+"]");
    return;
}

void MainWindow::APNG_Frames2APNG(QString sourceFileFullPath,QString scaledFramesFolder,QString resultFileFullPath,bool isOverScaled)
{
    emit Send_TextBrowser_NewMessage(tr("Start assembling APNG:[")+sourceFileFullPath+"]");

    FileMetadataCache metadata = getOrFetchMetadata(sourceFileFullPath);
    if(!metadata.isValid)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read metadata for APNG: [") + sourceFileFullPath + tr("]. Failed to assemble APNG.") );
        return;
    }

    bool CustRes_isEnabled = CustRes_isContained(sourceFileFullPath);
    if(CustRes_isEnabled || isOverScaled)
    {
        int New_width=0;
        int New_height=0;
        if(isOverScaled==true && CustRes_isEnabled==false)
        {
            int OriginalScaleRatio = ui->doubleSpinBox_ScaleRatio_gif->value();
            New_width = metadata.width * OriginalScaleRatio;
            New_height = metadata.height * OriginalScaleRatio;
        }
        if(CustRes_isEnabled==true)
        {
            QMap<QString, QString> Res_map = CustRes_getResMap(sourceFileFullPath);
            New_width = Res_map["width"].toInt();
            New_height = Res_map["height"].toInt();
        }
        if (New_width > 0 && New_height > 0) {
             ImagesResize_Folder_MultiThread(New_width,New_height,scaledFramesFolder);
        } else {
            emit Send_TextBrowser_NewMessage(tr("ERROR! Invalid dimensions for resizing APNG frames: [") + sourceFileFullPath + "]");
            return;
        }
    }

    double fps_double = 0.0;
    if (!metadata.fps.isEmpty()) {
        QStringList fpsParts = metadata.fps.split('/');
        if (fpsParts.size() == 2) {
            double num = fpsParts[0].toDouble();
            double den = fpsParts[1].toDouble();
            if (den > 0) fps_double = num / den;
        } else if (fpsParts.size() == 1 && !fpsParts[0].isEmpty()) {
            fps_double = fpsParts[0].toDouble();
        }
    }

    int fps_int = qRound(fps_double);
    // Fallback to identifyOutput if FPS is still invalid and identifyOutput has info
    if(fps_int <=0 && metadata.isValid && !metadata.identifyOutput.isEmpty()){
        QStringList lines = metadata.identifyOutput.split('\n');
        if(!lines.isEmpty()){
            QStringList parts = lines[0].trimmed().split(" ");
            if(parts.size() >= 4){ // %w %h %n %T
                double frameDelayHundredths = parts[3].toDouble();
                if(frameDelayHundredths > 0){
                    fps_double = 100.0 / frameDelayHundredths;
                    fps_int = qRound(fps_double);
                }
            }
        }
    }

    if(fps_int <= 0)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read or parse FPS of this APNG. FPS value: ") + metadata.fps + tr(". Failed to assemble APNG:[") + sourceFileFullPath + "]");
        return;
    }

    QFile::remove(resultFileFullPath);
    QString program = Current_Path+"/apngasm_waifu2xEX.exe";
    QString cmd ="\""+program+"\" \""+resultFileFullPath+"\" \""+scaledFramesFolder.replace("%","%%")+"/*.png\" -kp -kc -z1 1 "+QString::number(fps_int,10)+" -l0";
    QProcess AssembleAPNG;
    bool ok = runProcess(&AssembleAPNG, cmd);
    if(waifu2x_STOP || !ok)
    {
        AssembleAPNG.close();
        QFile::remove(resultFileFullPath);
        return;
    }
    emit Send_TextBrowser_NewMessage(tr("Finish assembling APNG:[")+sourceFileFullPath+"]");
}

bool MainWindow::APNG_isAnimatedPNG(int rowNum)
{
    QString sourceFileFullPath = Table_model_image->item(rowNum,2)->text();
    FileMetadataCache metadata = getOrFetchMetadata(sourceFileFullPath);
    return metadata.isValid && metadata.isAnimated && metadata.fileFormat == "apng";
}

/*
Placeholder implementations for APNG processing functions that are not yet
ported from the original project. They simply emit a message and return failure
so the application can compile without providing the actual functionality.
*/

bool MainWindow::APNG_Waifu2xNCNNVulkan(QString, QString, QString,
                                        QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_Waifu2xNCNNVulkan is not available."));
    return false;
}

bool MainWindow::APNG_Waifu2xConverter(QString, QString, QString,
                                       QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_Waifu2xConverter is not available."));
    return false;
}

int MainWindow::Waifu2x_Converter_APNG_scale(QMap<QString, QString>, int *, bool *)
{
    return 0;
}

bool MainWindow::APNG_SrmdNCNNVulkan(QString, QString, QString,
                                     QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_SrmdNCNNVulkan is not available."));
    return false;
}

bool MainWindow::APNG_Anime4k(QString, QString, QString, QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_Anime4k is not available."));
    return false;
}

int MainWindow::Anime4k_APNG_scale(QMap<QString, QString>, int *, bool *)
{
    return 0;
}

bool MainWindow::APNG_SrmdCUDA(QString, QString, QString, QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_SrmdCUDA is not available."));
    return false;
}

bool MainWindow::APNG_Waifu2xCaffe(QString, QString, QString,
                                   QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_Waifu2xCaffe is not available."));
    return false;
}

int MainWindow::Waifu2x_Caffe_APNG_scale(QMap<QString, QString>, int *, bool *)
{
    return 0;
}

bool MainWindow::APNG_RealsrNCNNVulkan(QString, QString, QString,
                                       QStringList, QString)
{
    emit Send_TextBrowser_NewMessage(tr("APNG_RealsrNCNNVulkan is not available."));
    return false;
}

