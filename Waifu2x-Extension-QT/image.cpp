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
/*
Multi-threaded adjustment of image size in folder
*/
void MainWindow::ImagesResize_Folder_MultiThread(int New_width,int New_height,QString ImagesFolderPath)
{
    if(file_isDirExist(ImagesFolderPath)==false)return;
    //====
    QMutex_ResizeImage_MultiThread.lock();
    //=====
    TotalNumOfThreads_ImagesResize_Folder_MultiThread = QThread::idealThreadCount()/2;
    if(TotalNumOfThreads_ImagesResize_Folder_MultiThread>32)TotalNumOfThreads_ImagesResize_Folder_MultiThread=32;
    if(TotalNumOfThreads_ImagesResize_Folder_MultiThread<1)TotalNumOfThreads_ImagesResize_Folder_MultiThread=1;
    RunningNumOfThreads_ImagesResize_Folder_MultiThread=0;
    //=====
    QMutex_ResizeImage_MultiThread.unlock();
    //================
    QStringList Frames_QStringList = file_getFileNames_in_Folder_nofilter(ImagesFolderPath);
    if(Frames_QStringList.isEmpty())return;
    for(int i=0; i<Frames_QStringList.size(); i++)
    {
        QString OutPut_Path = ImagesFolderPath+"/"+Frames_QStringList.at(i);
        //====
        QMutex_ResizeImage_MultiThread.lock();
        RunningNumOfThreads_ImagesResize_Folder_MultiThread++;
        QMutex_ResizeImage_MultiThread.unlock();
        //====
        QtConcurrent::run(this,&MainWindow::ResizeImage_MultiThread,New_width,New_height,OutPut_Path);
        while(RunningNumOfThreads_ImagesResize_Folder_MultiThread>=TotalNumOfThreads_ImagesResize_Folder_MultiThread)
        {
            Delay_msec_sleep(300);
        }
    }
    while(RunningNumOfThreads_ImagesResize_Folder_MultiThread>0)
    {
        Delay_msec_sleep(300);
    }
    return;
}

void MainWindow::ResizeImage_MultiThread(int New_width,int New_height,QString ImagesPath)
{
    QImage qimage_adj(ImagesPath);
    QImage qimage_adj_scaled = qimage_adj.scaled(New_width,New_height,CustRes_AspectRatioMode,Qt::SmoothTransformation);
    QImageWriter qimageW_adj;
    qimageW_adj.setFormat("png");
    qimageW_adj.setFileName(ImagesPath);
    if(qimageW_adj.canWrite())
    {
        qimageW_adj.write(qimage_adj_scaled);
    }
    QMutex_ResizeImage_MultiThread.lock();
    RunningNumOfThreads_ImagesResize_Folder_MultiThread--;
    QMutex_ResizeImage_MultiThread.unlock();
    return;
}

/*
Determine whether to skip based on resolution
true = skip
*/
bool MainWindow::Image_Gif_AutoSkip_CustRes(int rowNum,bool isGif)
{
    if(ui->checkBox_AutoSkip_CustomRes->isChecked()==false)return false;
    QString SourceFile_fullPath = "";
    if(isGif)
    {
        SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    }
    else
    {
        SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    }
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        int CustRes_height=Res_map["height"].toInt();
        int CustRes_width=Res_map["width"].toInt();
        //=========================
        QMap<QString,int> res_map = Image_Gif_Read_Resolution(SourceFile_fullPath);
        int original_height = res_map["height"];
        int original_width = res_map["width"];
        if(original_height<=0||original_width<=0)//Judge whether reading failed
        {
            return false;
        }
        //==========================
        if((CustRes_height*CustRes_width) <= (original_height*original_width))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
/*
Modify the format and image quality of the image
Return the path of the file after modification is complete
*/
QString MainWindow::SaveImageAs_FormatAndQuality(QString OriginalSourceImage_fullPath,QString ScaledImage_fullPath,bool isDenoiseLevelEnabled,int DenoiseLevel)
{
    QString FinalFile_FullName = "";
    QString FinalFile_Ext = "";
    QString FinalFile_Folder = "";
    QString FinalFile_FullPath = "";
    int ImageQualityLevel = ui->spinBox_ImageQualityLevel->value();
    //=========== Determine extension name ===================
    FinalFile_Ext = ui->comboBox_ImageSaveFormat->currentText();
    QImage QImage_OriginalSourceImage_fullPath(OriginalSourceImage_fullPath);
    if(QImage_OriginalSourceImage_fullPath.hasAlphaChannel()&&ui->checkBox_AutoDetectAlphaChannel->isChecked())
    {
        if(FinalFile_Ext=="jpg" || FinalFile_Ext=="tga")
        {
            FinalFile_Ext = "png";
        }
    }
    //================ Determine whether to continue ==================
    //If the extension name remains unchanged and the image quality is maxed out, return the original image path directly
    QFileInfo ScaledImage_fullPath_fileinfo(ScaledImage_fullPath);
    if((FinalFile_Ext==ScaledImage_fullPath_fileinfo.suffix())&&(ImageQualityLevel==100))
    {
        return ScaledImage_fullPath;
    }
    //==========================
    QFileInfo OriginalSourceImage_fileinfo(OriginalSourceImage_fullPath);
    QString OriginalSourceImage_file_name = file_getBaseName(OriginalSourceImage_fullPath);
    QString OriginalSourceImage_file_ext = OriginalSourceImage_fileinfo.suffix();
    //============ Determine folder ==============
    FinalFile_Folder = file_getFolderPath(OriginalSourceImage_fileinfo);
    //============== Determine file name ============
    QString Compressed_str = "";
    if(ImageQualityLevel<100)
    {
        Compressed_str = "_compressed";
    }
    QString OriginalExt_str = "";
    if(OriginalSourceImage_file_ext!=FinalFile_Ext)
    {
        OriginalExt_str="_"+OriginalSourceImage_file_ext;
    }
    QString DenoiseLevel_str = "";
    if(isDenoiseLevelEnabled)
    {
        DenoiseLevel_str="_"+QString::number(DenoiseLevel,10)+"n";
    }
    //===
    if(CustRes_isContained(OriginalSourceImage_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(OriginalSourceImage_fullPath);//res_map["fullpath"],["height"],["width"]
        int CustRes_height=Res_map["height"].toInt();
        int CustRes_width=Res_map["width"].toInt();
        FinalFile_FullName = OriginalSourceImage_file_name + "_waifu2x_"+QString::number(CustRes_width, 10)+"x"+QString::number(CustRes_height, 10)+DenoiseLevel_str+Compressed_str+OriginalExt_str+"."+FinalFile_Ext;
    }
    else
    {
        FinalFile_FullName = OriginalSourceImage_file_name + "_waifu2x_"+QString("%1").arg(ui->doubleSpinBox_ScaleRatio_image->value())+"x"+DenoiseLevel_str+Compressed_str+OriginalExt_str+"."+FinalFile_Ext;
    }
    //============ Assemble full path ==============
    FinalFile_FullPath = FinalFile_Folder+"/"+FinalFile_FullName;
    //==========================
    QString program = Current_Path+"/convert_waifu2xEX.exe";
    QFile::remove(FinalFile_FullPath);
    QProcess SaveImageAs_QProcess;
    QString saveCmd = "\""+program+"\" \""+ScaledImage_fullPath+"\" -quality "+QString::number(ImageQualityLevel,10)+" \""+FinalFile_FullPath+"\"";
    runProcess(&SaveImageAs_QProcess, saveCmd);
    //======
    QFileInfo FinalFile_FullPath_QFileInfo(FinalFile_FullPath);
    if((QFile::exists(FinalFile_FullPath)==false) || (FinalFile_FullPath_QFileInfo.size()<1))
    {
        QFile::remove(FinalFile_FullPath);
        emit Send_TextBrowser_NewMessage(tr("Error: Can\'t convert [")+ScaledImage_fullPath+tr("] to ")+FinalFile_Ext);
        return ScaledImage_fullPath;
    }
    QFile::remove(ScaledImage_fullPath);
    return FinalFile_FullPath;
}
/*
Determine whether to enable quality level adjustment based on the saved format
*/
void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int index)
{
    if(ui->comboBox_ImageSaveFormat->currentIndex()>2)
    {
        ui->spinBox_ImageQualityLevel->setEnabled(0);
        ui->spinBox_ImageQualityLevel->setValue(100);
    }
    else
    {
        ui->spinBox_ImageQualityLevel->setEnabled(1);
    }
}
MainWindow::AlphaInfo MainWindow::PrepareAlpha(const QString &inputImagePath)
{
    AlphaInfo info;
    info.rgbPath = inputImagePath;

    QImage src(inputImagePath);
    if(src.isNull())
        return info;

    if(src.hasAlphaChannel())
    {
        info.hasAlpha = true;
        info.is16Bit = src.depth() > 32;
        info.tempDir = QDir::tempPath() + "/W2xEX_alpha_" + QString::number(QRandomGenerator::global()->generate());
        QDir().mkpath(info.tempDir);
        info.rgbPath = QDir(info.tempDir).filePath("rgb.png");
        info.alphaPath = QDir(info.tempDir).filePath("alpha.png");

        QByteArray rgbData;
        QBuffer rgbBuf(&rgbData);
        rgbBuf.open(QIODevice::WriteOnly);
        QImage rgb = info.is16Bit ? src.convertToFormat(QImage::Format_RGBX64) : src.convertToFormat(QImage::Format_RGB888);
        rgb.save(&rgbBuf, "PNG");
        rgbBuf.close();
        QImage::fromData(rgbData, "PNG").save(info.rgbPath);
        QImage alpha = src.alphaChannel();
        if(info.is16Bit)
            alpha = alpha.convertToFormat(QImage::Format_Grayscale16);
        QByteArray alphaData;
        QBuffer alphaBuf(&alphaData);
        alphaBuf.open(QIODevice::WriteOnly);
        alpha.save(&alphaBuf, "PNG");
        alphaBuf.close();
        QImage::fromData(alphaData, "PNG").save(info.alphaPath);
    }

    return info;
}

void MainWindow::RestoreAlpha(const AlphaInfo &info, const QString &processedRgbPath, const QString &finalOutputPath)
{
    if(!info.hasAlpha)
    {
        if(processedRgbPath != finalOutputPath)
        {
            QFile::remove(finalOutputPath);
            QFile::rename(processedRgbPath, finalOutputPath);
        }
        return;
    }

    QImage rgb(processedRgbPath);
    QImage alpha(info.alphaPath);

    if(info.is16Bit)
    {
        rgb = rgb.convertToFormat(QImage::Format_RGBA64);
        alpha = alpha.convertToFormat(QImage::Format_Grayscale16);
    }
    else
    {
        rgb = rgb.convertToFormat(QImage::Format_ARGB32);
    }

    if (info.hasAlpha && alpha.size() != rgb.size()) {
        qDebug() << "Scaling alpha channel from" << alpha.size() << "to" << rgb.size();
        alpha = alpha.scaled(rgb.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    rgb.setAlphaChannel(alpha);
    QByteArray outData;
    QBuffer outBuf(&outData);
    outBuf.open(QIODevice::WriteOnly);
    rgb.save(&outBuf, "PNG");
    outBuf.close();
    QImage::fromData(outData, "PNG").save(finalOutputPath);

    QFile::remove(processedRgbPath);
    QFile::remove(info.alphaPath);
    if(!info.tempDir.isEmpty())
        QDir(info.tempDir).removeRecursively();
}
