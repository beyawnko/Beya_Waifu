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
Remove gif file from custom resolution list by row number
*/
void MainWindow::Gif_RemoveFromCustResList(int RowNumber)
{
    QString SourceFile_fullPath = Table_model_gif->item(RowNumber,2)->text();
    CustRes_remove(SourceFile_fullPath);
}

/*
When gif has no custom resolution and the current magnification is double, calculate one and add it to the custom list
*/
bool MainWindow::Gif_DoubleScaleRatioPrep(int RowNumber)
{
    QString SourceFile_fullPath = Table_model_gif->item(RowNumber,2)->text();
    if(CustRes_isContained(SourceFile_fullPath) == true)
    {
        return false;
    }
    else
    {
        //===================== Get resolution =============================
        QMap<QString,int> Map_OrgRes = Image_Gif_Read_Resolution(SourceFile_fullPath);
        //========= Calculate new height and width ==================
        double ScaleRatio_double = ui->doubleSpinBox_ScaleRatio_gif->value();
        int Height_new = qRound(ScaleRatio_double * Map_OrgRes["height"]);
        int width_new = qRound(ScaleRatio_double * Map_OrgRes["width"]);
        if(Height_new<1 || width_new<1)
        {
            emit Send_TextBrowser_NewMessage("Warning! Unable to read the resolution of ["+SourceFile_fullPath+"]. This file will only be scaled to "+QString::number((int)ScaleRatio_double,10)+"X.");
            return false;
        }
        //======== Store in custom resolution list ============
        QMap<QString,QString> res_map;
        res_map["fullpath"] = SourceFile_fullPath;
        res_map["height"] = QString::number(Height_new,10);
        res_map["width"] = QString::number(width_new,10);
        Custom_resolution_list.append(res_map);
        //=========
        return true;
    }
}
/*
Get gif frame interval time
*/
int MainWindow::Gif_getDuration(QString gifPath)
{
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffprobe_waifu2xEX.exe", gifPath);
    QString FPS_Division = doc.object().value("streams").toArray().isEmpty() ? QString() :
                           doc.object().value("streams").toArray().at(0).toObject().value("avg_frame_rate").toString();
    //=======================
    int Duration = 0;
    if(!FPS_Division.isEmpty())
    {
        QStringList FPS_Nums = FPS_Division.split("/");
        if(FPS_Nums.size()==2)
        {
            double FPS_Num_0 = FPS_Nums.at(0).toDouble();
            double FPS_Num_1 = FPS_Nums.at(1).toDouble();
            if(FPS_Num_0>0&&FPS_Num_1>0)
            {
                double Duration_double = 100/(FPS_Num_0/FPS_Num_1);
                Duration = qRound(Duration_double);
            }
        }
    }
    if(Duration<=0)
    {
        QMovie movie(gifPath);
        movie.setSpeed(1);
        movie.start();
        movie.stop();
        Duration = ((movie.nextFrameDelay()/100)+1)/10;
    }
    return Duration;
}
/*
Get the number of digits of gif frame count
*/
int MainWindow::Gif_getFrameDigits(QString gifPath)
{
    QMovie movie(gifPath);
    int FrameCount=1+(int)log10(movie.frameCount());//Get number of frame digits
    return FrameCount;
}
/*
Split gif
*/
void MainWindow::Gif_splitGif(QString gifPath,QString SplitFramesFolderPath)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting GIF:[")+gifPath+"]");
    int FrameDigits = Gif_getFrameDigits(gifPath);
    //Delete and recreate frame folder
    file_DelDir(SplitFramesFolderPath);
    file_mkDir(SplitFramesFolderPath);
    //Start processing with convert
    QString program = Current_Path+"/convert_waifu2xEX.exe";
    QString cmd = "\"" + program + "\"" + " -coalesce " + "\"" + gifPath + "\"" + " " + "\"" + SplitFramesFolderPath + "/%0"+QString::number(FrameDigits,10)+"d.png\"";
    QProcess SplitGIF;
    runProcess(&SplitGIF, cmd);
    if(file_isDirEmpty(SplitFramesFolderPath))//If splitting fails, try win7 compatible command
    {
        QString cmd = "\"" + program + "\"" + " -coalesce " + "\"" + gifPath + "\"" + " " + "\"" + SplitFramesFolderPath + "/%%0"+QString::number(FrameDigits,10)+"d.png\"";
        QProcess SplitGIF;
        runProcess(&SplitGIF, cmd);
    }
    emit Send_TextBrowser_NewMessage(tr("Finish splitting GIF:[")+gifPath+"]");
}
/*
Assemble gif
*/
void MainWindow::Gif_assembleGif(QString ResGifPath,QString ScaledFramesPath,int Duration,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled,QString SourceGifFullPath)
{
    emit Send_TextBrowser_NewMessage(tr("Start to assemble GIF:[")+ResGifPath+"]");
    //===============================
    QString resize_cmd ="";
    QString program = Current_Path+"/convert_waifu2xEX.exe";
    if(ui->checkBox_DisableResize_gif->isChecked()==false)
    {
        if(CustRes_isEnabled || isOverScaled)
        {
            if(isOverScaled==true && CustRes_isEnabled==false)
            {
                QMap<QString,int> res_map = Image_Gif_Read_Resolution(SourceGifFullPath);
                int OriginalScaleRatio = ui->doubleSpinBox_ScaleRatio_gif->value();
                resize_cmd =" -resize "+QString::number(res_map["width"]*OriginalScaleRatio,10)+"x"+QString::number(res_map["height"]*OriginalScaleRatio,10)+"! ";
            }
            if(CustRes_AspectRatioMode==Qt::IgnoreAspectRatio && CustRes_isEnabled==true)
            {
                resize_cmd =" -resize "+QString::number(CustRes_width,10)+"x"+QString::number(CustRes_height,10)+"! ";
            }
            if(CustRes_AspectRatioMode==Qt::KeepAspectRatio && CustRes_isEnabled==true)
            {
                resize_cmd =" -resize "+QString::number(CustRes_width,10)+"x"+QString::number(CustRes_height,10)+" ";
            }
            if(CustRes_AspectRatioMode==Qt::KeepAspectRatioByExpanding && CustRes_isEnabled==true)
            {
                if(CustRes_width>CustRes_height)
                {
                    resize_cmd =" -resize "+QString::number(CustRes_width,10)+" ";
                }
                else
                {
                    resize_cmd =" -resize x"+QString::number(CustRes_height,10)+" ";
                }
            }
        }
        QString cmd = "\"" + program + "\" "+resize_cmd+" -delay " + QString::number(Duration, 10) + " -loop 0 \"" + ScaledFramesPath + "/*png\" \""+ResGifPath+"\"";
        QProcess AssembleGIF;
        runProcess(&AssembleGIF, cmd);
        //======= Correct file name error (when there is a % symbol in the result gif file path) ======
        if(QFile::exists(ResGifPath)==false)
        {
            QFileInfo fileinfo(ResGifPath);
            QString file_name = file_getBaseName(ResGifPath);
            QString file_ext = fileinfo.suffix();
            QString file_path = file_getFolderPath(fileinfo);
            QString ActualResGifPath = file_path + "/" + file_name + "-0." + file_ext;
            if(QFile::exists(ActualResGifPath)==true)
            {
                QFile::rename(ActualResGifPath,ResGifPath);
            }
        }
        if(QFile::exists(ResGifPath))
        {
            emit Send_TextBrowser_NewMessage(tr("Finish assembling GIF:[")+ResGifPath+"]");
            return;
        }
        AssembleGIF->kill();
    }
    //Adjust image size yourself and then assemble
    if(CustRes_isEnabled || isOverScaled)
    {
        int New_width=0;
        int New_height=0;
        if(isOverScaled==true && CustRes_isEnabled==false)
        {
            QMap<QString,int> res_map = Image_Gif_Read_Resolution(SourceGifFullPath);
            int OriginalScaleRatio = ui->doubleSpinBox_ScaleRatio_gif->value();
            New_width = res_map["width"]*OriginalScaleRatio;
            New_height = res_map["height"]*OriginalScaleRatio;
        }
        if(CustRes_isEnabled==true)
        {
            New_width = CustRes_width;
            New_height = CustRes_height;
        }
        ImagesResize_Folder_MultiThread(New_width,New_height,ScaledFramesPath);
    }
    QString cmd = "\"" + program + "\" \"" + ScaledFramesPath + "/*png\" -delay " + QString::number(Duration, 10) + " -loop 0 \""+ResGifPath+"\"";
    QProcess AssembleGIF_1;
    runProcess(&AssembleGIF_1, cmd);
    //======= Correct file name error (when there is a % symbol in the result gif file path) ======
    if(QFile::exists(ResGifPath)==false)
    {
        QFileInfo fileinfo(ResGifPath);
        QString file_name = file_getBaseName(ResGifPath);
        QString file_ext = fileinfo.suffix();
        QString file_path = file_getFolderPath(fileinfo);
        QString ActualResGifPath = file_path + "/" + file_name + "-0." + file_ext;
        if(QFile::exists(ActualResGifPath)==true)
        {
            QFile::rename(ActualResGifPath,ResGifPath);
        }
    }
    //=====================
    if(QFile::exists(ResGifPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Finish assembling GIF:[")+ResGifPath+"]");
        if(ui->checkBox_DisableResize_gif->isChecked()==false)emit Send_Set_checkBox_DisableResize_gif_Checked();
    }
    return;
}
/*
Compress gif
*/
QString MainWindow::Gif_compressGif(QString gifPath,QString gifPath_compressd)
{
    emit Send_TextBrowser_NewMessage(tr("Starting to optimize GIF:[")+gifPath+"]");
    //=====
    QString program = Current_Path+"/gifsicle_waifu2xEX.exe";
    QString cmd = "\"" + program + "\"" + " -O3 -i \""+gifPath+"\" -o \""+gifPath_compressd+"\"";
    QProcess CompressGIF;
    runProcess(&CompressGIF, cmd);
    //======
    //Determine if a compressed gif was generated
    if(QFile::exists(gifPath_compressd) == false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+gifPath+tr("]. Error: [Can't optimize gif.]"));
        return gifPath;//Return source file path
    }
    //======
    //Compare file sizes to determine if compression is effective
    QFileInfo gifPath_QFileInfo(gifPath);
    QFileInfo gifPath_compressd_QFileInfo(gifPath_compressd);
    if(gifPath_compressd_QFileInfo.size() >= gifPath_QFileInfo.size())
    {
        emit Send_TextBrowser_NewMessage(tr("Failed to optimize gif [")+gifPath+tr("] to reduce storage usage, the optimized gif file will be deleted."));
        QFile::remove(gifPath_compressd);
        return gifPath;//Return source file path
    }
    //======
    QFile::remove(gifPath);
    emit Send_TextBrowser_NewMessage(tr("Finish optimizing GIF:[")+gifPath+"]");
    return gifPath_compressd;//Return the path of the processed file
}
