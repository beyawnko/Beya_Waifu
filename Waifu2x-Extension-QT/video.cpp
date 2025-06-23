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
#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#include <QTextCodec>
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "VideoProcessor.h"
#include "utils/ffprobe_helpers.h"

/*
Remove a video file from the custom resolution list by row number
*/
void MainWindow::video_RemoveFromCustResList(int RowNumber)
{
    QString SourceFile_fullPath = Table_model_video->item(RowNumber,2)->text();
    CustRes_remove(SourceFile_fullPath);
}

/*
If the video has no custom resolution and the scale ratio is double, compute one and add it to the custom list
*/
bool MainWindow::video_DoubleScaleRatioPrep(int RowNumber)
{
    QString SourceFile_fullPath = Table_model_video->item(RowNumber,2)->text();
    if(CustRes_isContained(SourceFile_fullPath) == true)
    {
        // If a custom resolution already exists or frame interpolation only is enabled
        return false;
    }
    //===================== Get resolution =============================
    QMap<QString,int> Map_OrgRes = video_get_Resolution(SourceFile_fullPath);
    //========= Calculate new height and width ==================
    double ScaleRatio_double = ui->doubleSpinBox_ScaleRatio_video->value();
    int Height_new = qRound(ScaleRatio_double * Map_OrgRes["height"]);
    int width_new = qRound(ScaleRatio_double * Map_OrgRes["width"]);
    if(Height_new<1 || width_new<1)
    {
        emit Send_TextBrowser_NewMessage("Warning! Unable to read the resolution of ["+SourceFile_fullPath+"]. This file will only be scaled to "+QString::number((int)ScaleRatio_double,10)+"X.");
        return false;
    }
    if(Height_new%2!=0)Height_new++;
    if(width_new%2!=0)width_new++;
    //======== Save to custom resolution list ============
    QMap<QString,QString> res_map;
    res_map["fullpath"] = SourceFile_fullPath;
    res_map["height"] = QString::number(Height_new,10);
    res_map["width"] = QString::number(width_new,10);
    Custom_resolution_list.append(res_map);
    //=========
    return true;
}
/*
Calculate number of digits
*/
int MainWindow::CalNumDigits(int input_num)
{
    if (input_num <= 0) return 1;
    int frameNumDigits=1+(int)log10(input_num);
    return frameNumDigits;
}

/*
Determine whether the video is variable frame rate
*/
bool MainWindow::video_isVFR(QString videoPath)
{
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe", videoPath);
    QJsonArray streams = doc.object().value("streams").toArray();
    if(!streams.isEmpty())
    {
        QJsonObject stream0 = streams.at(0).toObject();
        QString avg = stream0.value("avg_frame_rate").toString();
        QString r = stream0.value("r_frame_rate").toString();
        if(!avg.isEmpty() && !r.isEmpty())
            return (avg != r);
    }
    return true;
}
/*
Determine whether to skip based on resolution
true = skip
*/
bool MainWindow::Video_AutoSkip_CustRes(int rowNum)
{
    if(ui->checkBox_AutoSkip_CustomRes->isChecked()==false)return false;
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    if(CustRes_isContained(SourceFile_fullPath))
    {
        QMap<QString, QString> Res_map = CustRes_getResMap(SourceFile_fullPath);//res_map["fullpath"],["height"],["width"]
        int CustRes_height=Res_map["height"].toInt();
        int CustRes_width=Res_map["width"].toInt();
        //=========================
        QMap<QString,int> res_map = video_get_Resolution(SourceFile_fullPath);
        int original_height = res_map["height"];
        int original_width = res_map["width"];
        if(original_height<=0||original_width<=0)// check if reading failed
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
Directly obtain the video resolution
*/
QMap<QString,int> MainWindow::video_get_Resolution(QString VideoFileFullPath)
{
    emit Send_TextBrowser_NewMessage(tr("Get resolution of the video:[")+VideoFileFullPath+"]");
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe", VideoFileFullPath);
    QJsonArray streams = doc.object().value("streams").toArray();
    if(!streams.isEmpty())
    {
        QJsonObject s = streams.at(0).toObject();
        int width = s.value("width").toInt();
        int height = s.value("height").toInt();
        if(width>0 && height>0)
        {
            QMap<QString,int> res_map;
            res_map["height"] = height;
            res_map["width"] = width;
            return res_map;
        }
    }
    emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read the resolution of the video. [")+VideoFileFullPath+"]");
    QMap<QString,int> empty;
    empty["height"] = 0;
    empty["width"] = 0;
    return empty;
}

/*
Determine whether to process the video in segments based on its length
*/
bool MainWindow::video_isNeedProcessBySegment(int rowNum)
{
    if(ui->checkBox_ProcessVideoBySegment->isChecked()==false)return false;// if segment processing is disabled, return false
    QString VideoFile = Table_model_video->item(rowNum,2)->text();
    if(video_get_duration(VideoFile)>ui->spinBox_VideoSplitDuration->value())
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("This video is too short, so segment processing is automatically disabled.[")+VideoFile+"]");
        return false;
    }
}

/*
Generate a folder number for video clips
*/
QString MainWindow::video_getClipsFolderNo()
{
    QString current_date =QDateTime::currentDateTime().toString("yyMMddhhmmss");
    return current_date;
}
/*
Assemble video from MP4 segments
*/
void MainWindow::video_AssembleVideoClips(QString VideoClipsFolderPath,QString VideoClipsFolderName,QString video_mp4_scaled_fullpath,QString AudioPath)
{
    Q_UNUSED(VideoClipsFolderName);
    emit Send_TextBrowser_NewMessage(tr("Start assembling video with clips:[")+video_mp4_scaled_fullpath+"]");
    //=================
    QStringList VideoClips_Scan_list = file_getFileNames_in_Folder_nofilter(VideoClipsFolderPath);
    QStringList VideoClips_fileName_list;
    VideoClips_fileName_list.clear();
    QFileInfo vfinfo(video_mp4_scaled_fullpath);
    QString video_dir = file_getFolderPath(QFileInfo(video_mp4_scaled_fullpath));
    /*
    Generate a QStringList of full paths for video clip files
    */
    for (int VideoNameNo = 0; VideoNameNo<VideoClips_Scan_list.size(); VideoNameNo++)
    {
        QString VideoClip_FullPath_tmp = VideoClipsFolderPath+"/"+QString::number(VideoNameNo,10)+".mp4";
        if(QFile::exists(VideoClip_FullPath_tmp))
        {
            VideoClips_fileName_list.append(VideoClip_FullPath_tmp);
        }
    }
    // Get a valid full path for an mp4 clip
    QString Mp4Clip_forReadInfo = VideoClips_fileName_list.at(0);
    /*
    Generate file list QString
    */
    QString FFMpegFileList_QString = "";
    for(int CurrentVideoClipNo = 0; CurrentVideoClipNo < VideoClips_fileName_list.size(); CurrentVideoClipNo++)
    {
        QString VideoClip_fullPath = VideoClips_fileName_list.at(CurrentVideoClipNo);
        QString Line = "file \'"+VideoClip_fullPath+"\'\n";
        FFMpegFileList_QString.append(Line);
    }
    //================ Save the file list to disk ================
    QFileInfo videoFileInfo(video_mp4_scaled_fullpath);
    QString Path_FFMpegFileList = "";
    int FileNo = 0;
    do
    {
        FileNo++;
        Path_FFMpegFileList = video_dir+"/"+file_getBaseName(video_mp4_scaled_fullpath)+"_fileList_"+QString::number(FileNo,10)+"_Waifu2xEX.txt";
    }
    while(QFile::exists(Path_FFMpegFileList));
    //=========
    QFile FFMpegFileList(Path_FFMpegFileList);
    FFMpegFileList.remove();
    if (FFMpegFileList.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite supports read and write
    {
        QTextStream stream(&FFMpegFileList);
        stream << FFMpegFileList_QString;
    }
    FFMpegFileList.close();
    //========
    /*
    Assemble video
    */
    QString ffmpeg_path = Current_Path+"/ffmpeg/ffmpeg_waifu2xEX.exe";
    bool Del_DenoisedAudio = false;
    //=============== Audio denoise ========================
    if((ui->checkBox_AudioDenoise->isChecked())&&QFile::exists(AudioPath))
    {
        QString AudioPath_tmp = video_AudioDenoise(AudioPath);
        if(AudioPath_tmp!=AudioPath)
        {
            AudioPath = AudioPath_tmp;
            Del_DenoisedAudio = true;
        }
    }
    //================= Get bitrate =================
    QString bitrate_video_cmd="";
    if(ui->spinBox_bitrate_vid->value()>0 && ui->groupBox_video_settings->isChecked())
    {
        bitrate_video_cmd = " -b:v "+QString::number(ui->spinBox_bitrate_vid->value(),10)+"k ";
    }
    else
    {
        QString BitRate_str = video_get_bitrate(Mp4Clip_forReadInfo,false,true);
        if(BitRate_str!="")
        {
            bitrate_video_cmd = " -b:v "+BitRate_str+" ";
        }
        else
        {
            int BitRate = video_UseRes2CalculateBitrate(Mp4Clip_forReadInfo);
            if(BitRate!=0)bitrate_video_cmd = " -b:v "+QString::number(BitRate,10)+"k ";
        }
    }
    //================= Read video encoder settings ==============
    QString encoder_video_cmd="";
    if(ui->groupBox_video_settings->isChecked() && ui->lineEdit_encoder_vid->text().trimmed()!="")
    {
        encoder_video_cmd = " -c:v "+ui->lineEdit_encoder_vid->text().trimmed()+" ";//Video encoder
    }
    //=============== Read audio encoder settings ====================
    QString encoder_audio_cmd="";
    QString bitrate_audio_cmd="";
    if(ui->groupBox_video_settings->isChecked())
    {
        if(ui->lineEdit_encoder_audio->text().trimmed()!="")
            encoder_audio_cmd=" -c:a "+ui->lineEdit_encoder_audio->text().trimmed()+" ";
        if(ui->spinBox_bitrate_audio->value()>0)
            bitrate_audio_cmd=" -b:a "+QString::number(ui->spinBox_bitrate_audio->value(),10)+"k ";
    }
    //==================
    QString Extra_command = "";
    if(ui->groupBox_video_settings->isChecked() && ui->lineEdit_ExCommand_output->text().trimmed()!="")
    {
        Extra_command = " "+ui->lineEdit_ExCommand_output->text().trimmed()+" ";//Extra command
    }
    //================ Get fps =====================
    QString fps_video_cmd=" ";
    QString fps = video_get_fps(Mp4Clip_forReadInfo).trimmed();
    if(fps != "0.0")
    {
        fps_video_cmd = " -r "+fps+" ";
    }
    //================= Start processing =============================
    QString CMD = "";
    if(QFile::exists(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f concat -safe 0 "+fps_video_cmd+" -i \""+Path_FFMpegFileList+"\" "+fps_video_cmd+" -i \""+AudioPath+"\" "+fps_video_cmd+" "+bitrate_video_cmd+encoder_video_cmd+fps_video_cmd+encoder_audio_cmd+bitrate_audio_cmd+Extra_command+"\""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f concat -safe 0 "+fps_video_cmd+" -i \""+Path_FFMpegFileList+"\""+bitrate_video_cmd+encoder_video_cmd+fps_video_cmd+Extra_command+"\""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess AssembleVideo;
    bool ok = runProcess(&AssembleVideo, CMD);
    if(this->waifu2x_STOP || !ok)
    {
        AssembleVideo.close();
        QFile::remove(video_mp4_scaled_fullpath);
        QFile::remove(Path_FFMpegFileList);
        return;
    }
    //Check for errors
    if(QFile::exists(video_mp4_scaled_fullpath)==false)//Check whether video was generated successfully
    {
        MultiLine_ErrorOutput_QMutex.lock();
        emit Send_TextBrowser_NewMessage(tr("Error output for FFmpeg when processing:[")+video_mp4_scaled_fullpath+"]");
        emit Send_TextBrowser_NewMessage("\n--------------------------------------");
        //Standard output
        emit Send_TextBrowser_NewMessage(AssembleVideo.readAllStandardOutput());
        //Error output
        emit Send_TextBrowser_NewMessage(AssembleVideo.readAllStandardError());
        emit Send_TextBrowser_NewMessage("\n--------------------------------------");
        MultiLine_ErrorOutput_QMutex.unlock();
    }
    QFile::remove(Path_FFMpegFileList);//Delete file list
    //===================
    if(Del_DenoisedAudio)QFile::remove(AudioPath);
    //==============================
    emit Send_TextBrowser_NewMessage(tr("Finish assembling video with clips:[")+video_mp4_scaled_fullpath+"]");
}
/*
Split video into frames (by segment)
*/
void MainWindow::video_video2images_ProcessBySegment(QString VideoPath,QString FrameFolderPath,int StartTime,int SegmentDuration)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting video: [")+VideoPath+"]");
    //=================
    QString ffmpeg_path = Current_Path+"/ffmpeg/ffmpeg_waifu2xEX.exe";
    QString video_mp4_fullpath = VideoPath;
    //================ Get fps =====================
    QString fps_video_cmd=" ";
    QString fps = video_get_fps(video_mp4_fullpath).trimmed();
    if(fps != "0.0")
    {
        fps_video_cmd = " -r "+fps+" ";
    }
    //=====================
    int FrameNumDigits = video_get_frameNumDigits(video_mp4_fullpath);
    if(FrameNumDigits==0)return;
    QFile::remove(isPreVFIDone_MarkFilePath(VideoPath));
    //=====================
    QProcess video_splitFrame;
    QString splitCmd = "\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+video_mp4_fullpath+"\" -ss "+QString::number(StartTime,10)+" -t "+QString::number(SegmentDuration,10)+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\"";
    runProcess(&video_splitFrame, splitCmd);
    //============== Attempt commands that might work on Win7 ================================
    if(file_isDirEmpty(FrameFolderPath))
    {
        splitCmd = "\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+video_mp4_fullpath+"\" -ss "+QString::number(StartTime,10)+" -t "+QString::number(SegmentDuration,10)+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\"";
        runProcess(&video_splitFrame, splitCmd);
    }
    //======== Frame interpolation =========
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString VFI_FolderPath_tmp = video_dir+"/"+video_filename+"_PreVFI_W2xEX";
    if(ui->checkBox_VfiAfterScale_VFI->isChecked()==false && ui->groupBox_FrameInterpolation->isChecked()==true) //If frame interpolation is enabled
    {
        //If complete interpolated frame cache is detected
        if(file_isDirExist(VFI_FolderPath_tmp) && (file_getFileNames_in_Folder_nofilter(FrameFolderPath).size() * ui->spinBox_MultipleOfFPS_VFI->value() == file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()))
        {
            file_DelDir(FrameFolderPath);
            QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
            VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
            return;
        }
        else
        {
            if(FrameInterpolation(FrameFolderPath,VFI_FolderPath_tmp)==true)
            {
                //If frame interpolation succeeds
                file_DelDir(FrameFolderPath);
                QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
                VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
                file_generateMarkFile(isPreVFIDone_MarkFilePath(VideoPath),QString("%1").arg(ui->spinBox_MultipleOfFPS_VFI->value()));
                return;
            }
            else
            {
                file_DelDir(FrameFolderPath);
                file_mkDir(FrameFolderPath);
                file_DelDir(VFI_FolderPath_tmp);
                if(this->waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+"]");
                return;
            }
        }
    }
    //====================================
    emit Send_TextBrowser_NewMessage(tr("Finish splitting video: [")+VideoPath+"]");
}

/*
Extract audio from video
*/
void MainWindow::video_get_audio(QString VideoPath,QString AudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Extract audio from video: [")+VideoPath+"]");
    //==============================================
    QString ffmpeg_path = Current_Path+"/ffmpeg/ffmpeg_waifu2xEX.exe";
    QFile::remove(AudioPath);
    QProcess video_splitSound;
    QString sndCmd = "\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\" \""+AudioPath+"\"";
    runProcess(&video_splitSound, sndCmd);
    if(QFile::exists(AudioPath))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully extracted audio from video: [")+VideoPath+"]");
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Failed to extract audio from video: [")+VideoPath+tr("] This video might be a silent video, so will continue to process this video."));
    }
}
/*
Convert video to mp4
*/
QString MainWindow::video_To_CFRMp4(QString VideoPath)
{
    QFileInfo vfinfo(VideoPath);
    QString video_ext = vfinfo.suffix();
    bool isVFR = false;
    //==============
    if(video_ext.toLower()=="mp4")
    {
        if(video_isVFR(VideoPath)==false)
        {
            return VideoPath;
        }
        else
        {
            isVFR = true;
        }
    }
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString video_mp4_fullpath = video_dir+"/"+video_filename+"_"+video_ext+"_CfrMp4.mp4";
    if(QFile::exists(video_mp4_fullpath))return video_mp4_fullpath;
    QFile::remove(video_mp4_fullpath);
    //=================
    emit Send_TextBrowser_NewMessage(tr("Start converting video: [")+VideoPath+tr("] to CFR MP4."));
    QString ffmpeg_path = Current_Path+"/ffmpeg/ffmpeg_waifu2xEX.exe";
    QString vcodec_copy_cmd = "";
    QString acodec_copy_cmd = "";
    QString bitrate_vid_cmd = "";
    QString bitrate_audio_cmd = "";
    QString Extra_command = "";
    QString bitrate_FromOG = "";
    QString vsync_1 = " -vsync 1 ";
    if(ui->groupBox_video_settings->isChecked())
    {
        if(ui->checkBox_IgnoreFrameRateMode->isChecked()==true)
        {
            vsync_1 = "";
            isVFR = false;
        }
        Extra_command = ui->lineEdit_ExCommand_2mp4->text().trimmed();
        if(ui->checkBox_vcodec_copy_2mp4->isChecked()&&isVFR==false)
        {
            vcodec_copy_cmd = " -vcodec copy ";
        }
        else
        {
            if(ui->spinBox_bitrate_vid_2mp4->value()>0&&ui->spinBox_bitrate_audio_2mp4->value()>0)bitrate_vid_cmd = " -b:v "+QString::number(ui->spinBox_bitrate_vid_2mp4->value(),10)+"k ";
        }
        if(ui->checkBox_acodec_copy_2mp4->isChecked()&&isVFR==false)
        {
            acodec_copy_cmd = " -acodec copy ";
        }
        else
        {
            if(ui->spinBox_bitrate_vid_2mp4->value()>0&&ui->spinBox_bitrate_audio_2mp4->value()>0)bitrate_audio_cmd = " -b:a "+QString::number(ui->spinBox_bitrate_audio_2mp4->value(),10)+"k ";
        }
    }
    if((ui->groupBox_video_settings->isChecked()==false)||(ui->spinBox_bitrate_vid_2mp4->value()<=0||ui->spinBox_bitrate_audio_2mp4->value()<=0))
    {
        QString BitRate = video_get_bitrate(VideoPath,true,false);
        if(BitRate!="")
        {
            bitrate_FromOG = BitRate;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Warning! Unable to get the bitrate of the [")+VideoPath+tr("]. The bit rate automatically allocated by ffmpeg will be used."));
        }
    }
    //=====
    QProcess video_tomp4;
    QString mp4Cmd = "\""+ffmpeg_path+"\" -y -i \""+VideoPath+"\""+vsync_1+vcodec_copy_cmd+acodec_copy_cmd+bitrate_vid_cmd+bitrate_audio_cmd+bitrate_FromOG+" "+Extra_command+" \""+video_mp4_fullpath+"\"";
    runProcess(&video_tomp4, mp4Cmd);
    //======
    if(QFile::exists(video_mp4_fullpath))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully converted video: [")+VideoPath+tr("] to CFR MP4."));
        return video_mp4_fullpath;
    }
    return "null";
}
//===============
//Get duration (seconds)
//===============
int MainWindow::video_get_duration(QString videoPath)
{
    emit Send_TextBrowser_NewMessage(tr("Get duration of the video:[")+videoPath+"]");
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe", videoPath);
    double Duration_double = doc.object().value("format").toObject().value("duration").toString().toDouble();
    if(Duration_double <= 0)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to get the duration of the [")+videoPath+tr("]."));
        return 0;
    }
    int Duration_int = (int)Duration_double;
    //=====================
    return Duration_int;
}
/*
Audio denoise
*/
QString MainWindow::video_AudioDenoise(QString OriginalAudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Starting to denoise audio.[")+OriginalAudioPath+"]");
    //===========
    QFileInfo fileinfo(OriginalAudioPath);
    QString file_name = file_getBaseName(OriginalAudioPath);
    QString file_ext = fileinfo.suffix();
    QString file_path = file_getFolderPath(fileinfo);
    //================
    QString program = Current_Path+"/SoX/sox_waifu2xEX.exe";
    QString DenoiseProfile = file_path+"/"+file_name+"_DenoiseProfile.dp";
    QString DenoisedAudio = file_path+"/"+file_name+"_Denoised."+file_ext;
    double DenoiseLevel = ui->doubleSpinBox_AudioDenoiseLevel->value();
    //================
    QProcess vid;
    runProcess(&vid, "\""+program+"\" \""+OriginalAudioPath+"\" -n noiseprof \""+DenoiseProfile+"\"");
    //================
    runProcess(&vid, "\""+program+"\" \""+OriginalAudioPath+"\" \""+DenoisedAudio+"\" noisered \""+DenoiseProfile+"\" "+QString("%1").arg(DenoiseLevel));
    //================
    if(QFile::exists(DenoisedAudio))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully denoise audio.[")+OriginalAudioPath+"]");
        QFile::remove(DenoiseProfile);
        return DenoisedAudio;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Unable to denoise audio.[")+OriginalAudioPath+"]");
        return OriginalAudioPath;
    }
}
/*
Save progress
*/
void MainWindow::video_write_Progress_ProcessBySegment(QString VideoConfiguration_fullPath,int StartTime,bool isSplitComplete,bool isScaleComplete,int OLDSegmentDuration,int LastVideoClipNo)
{
    QSettings *configIniWrite = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
    //==================== Store progress ==================================
    configIniWrite->setValue("/Progress/StartTime", StartTime);
    configIniWrite->setValue("/Progress/isSplitComplete", isSplitComplete);
    configIniWrite->setValue("/Progress/isScaleComplete", isScaleComplete);
    configIniWrite->setValue("/Progress/OLDSegmentDuration", OLDSegmentDuration);
    configIniWrite->setValue("/Progress/LastVideoClipNo", LastVideoClipNo);
}
/*
Save video configuration
*/
void MainWindow::video_write_VideoConfiguration(QString VideoConfiguration_fullPath,int ScaleRatio,int DenoiseLevel,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,QString EngineName,bool isProcessBySegment,QString VideoClipsFolderPath,QString VideoClipsFolderName,bool isVideoFrameInterpolationEnabled,int MultipleOfFPS)
{
    QSettings *configIniWrite = new QSettings(VideoConfiguration_fullPath, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
#endif
    //================= Add warning =========================
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    //==================== Store video information ==================================
    configIniWrite->setValue("/VideoConfiguration/ScaleRatio", ScaleRatio);
    configIniWrite->setValue("/VideoConfiguration/DenoiseLevel", DenoiseLevel);
    configIniWrite->setValue("/VideoConfiguration/CustRes_isEnabled", CustRes_isEnabled);
    configIniWrite->setValue("/VideoConfiguration/CustRes_height", CustRes_height);
    configIniWrite->setValue("/VideoConfiguration/CustRes_width", CustRes_width);
    configIniWrite->setValue("/VideoConfiguration/EngineName", EngineName);
    configIniWrite->setValue("/VideoConfiguration/isProcessBySegment", isProcessBySegment);
    configIniWrite->setValue("/VideoConfiguration/VideoClipsFolderPath", VideoClipsFolderPath);
    configIniWrite->setValue("/VideoConfiguration/VideoClipsFolderName", VideoClipsFolderName);
    configIniWrite->setValue("/VideoConfiguration/isVideoFrameInterpolationEnabled", isVideoFrameInterpolationEnabled);
    configIniWrite->setValue("/VideoConfiguration/MultipleOfFPS", MultipleOfFPS);
    //==================== Store progress ==================================
    configIniWrite->setValue("/Progress/StartTime", 0);
    configIniWrite->setValue("/Progress/isSplitComplete", false);
    configIniWrite->setValue("/Progress/isScaleComplete", false);
    configIniWrite->setValue("/Progress/OLDSegmentDuration", -1);
    configIniWrite->setValue("/Progress/LastVideoClipNo", -1);
}

/*
Directly read video resolution and calculate ideal bitrate with a custom algorithm
Unit is k
*/
int MainWindow::video_UseRes2CalculateBitrate(QString VideoFileFullPath)
{
    QMap<QString,int> res_map = video_get_Resolution(VideoFileFullPath);
    int original_height = res_map["height"];
    int original_width = res_map["width"];
    if(original_height<=0||original_width<=0)
    {
        return 0;
    }
    //=========
    double MultipleOfBitrate_OGFR = 1;
    QString fps = video_get_fps(VideoFileFullPath).trimmed();
    if(fps != "0.0")
    {
        QStringList FPS_Nums = fps.split("/");
        double FPS_Nums_0 = FPS_Nums.at(0).toDouble();
        double FPS_Nums_1 = FPS_Nums.at(1).toDouble();
        if(FPS_Nums_0 >= FPS_Nums_1)
        {
            MultipleOfBitrate_OGFR = ((FPS_Nums_0/FPS_Nums_1)/24)*0.75;
            if(MultipleOfBitrate_OGFR<1)
            {
                MultipleOfBitrate_OGFR = 1;
            }
        }
    }
    //=========
    if(original_height<=original_width)
    {
        return qRound(original_height*6*MultipleOfBitrate_OGFR);
    }
    else
    {
        return qRound(original_width*6*MultipleOfBitrate_OGFR);
    }
}

QString MainWindow::video_get_bitrate_AccordingToRes_FrameFolder(QString ScaledFrameFolderPath,QString VideoPath)
{
    QStringList flist = file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath);
    QString Full_Path_File = "";
    if(!flist.isEmpty())
    {
        for(int i = 0; i < flist.size(); i++)
        {
            QString tmp = flist.at(i);
            Full_Path_File = ScaledFrameFolderPath + "/" + tmp;
            QFileInfo finfo(Full_Path_File);
            if(finfo.suffix()=="png")break;
        }
    }
    QImage qimage_original;
    qimage_original.load(Full_Path_File);
    int original_height = qimage_original.height();
    int original_width = qimage_original.width();
    if(original_height<=0||original_width<=0)
    {
        return "";
    }
    //=========
    double MultipleOfBitrate_OGFR = 1;
    QString fps = video_get_fps(VideoPath).trimmed();
    if(fps != "0.0")
    {
        QStringList FPS_Nums = fps.split("/");
        double FPS_Nums_0 = FPS_Nums.at(0).toDouble();
        double FPS_Nums_1 = FPS_Nums.at(1).toDouble();
        if(FPS_Nums_0 >= FPS_Nums_1)
        {
            MultipleOfBitrate_OGFR = ((FPS_Nums_0/FPS_Nums_1)/24)*0.75;
            if(MultipleOfBitrate_OGFR<1)
            {
                MultipleOfBitrate_OGFR = 1;
            }
        }
    }
    //=========
    double MultipleOfBitrate_MOF = 1;
    if(ui->groupBox_FrameInterpolation->isChecked()==true)
    {
        MultipleOfBitrate_MOF = ui->spinBox_MultipleOfFPS_VFI->value()*0.75;
    }
    //=========
    if(original_height<=original_width)
    {
        return QString::number(qRound(original_height*6*MultipleOfBitrate_MOF*MultipleOfBitrate_OGFR),10);
    }
    else
    {
        return QString::number(qRound(original_width*6*MultipleOfBitrate_MOF*MultipleOfBitrate_OGFR),10);
    }
}
/*
Get video bitrate
*/
QString MainWindow::video_get_bitrate(QString videoPath,bool isReturnFullCMD,bool isVidOnly)
{
    emit Send_TextBrowser_NewMessage(tr("Get bitrate of the video:[")+videoPath+"]");
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe", videoPath);
    QJsonObject root = doc.object();
    QJsonObject stream0 = root.value("streams").toArray().isEmpty() ? QJsonObject() : root.value("streams").toArray().at(0).toObject();
    QString BitRate = stream0.value("bit_rate").toString();
    QString BitRateCMD;
    if(BitRate.isEmpty() && !isVidOnly)
        BitRate = root.value("format").toObject().value("bit_rate").toString();
    if(isReturnFullCMD && !BitRate.isEmpty())
        BitRateCMD = QString(" %1 %2 ").arg(isVidOnly ? "-b:v" : "-b").arg(BitRate);
    //=======================
    if(BitRate.toUInt() == 0)
    {
        BitRate="";
    }
    //=======================
    if(isReturnFullCMD==true && BitRate!="" && BitRateCMD!="")
    {
        return BitRateCMD;
    }
    else
    {
        return BitRate;
    }
}
/*
Get video FPS
*/
QString MainWindow::video_get_fps(QString videoPath)
{
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe", videoPath);
    QString fpsDiv = doc.object().value("streams").toArray().isEmpty() ? QString() :
                     doc.object().value("streams").toArray().at(0).toObject().value("avg_frame_rate").toString();
    if(fpsDiv.isEmpty())
        return "0.0";
    QStringList nums = fpsDiv.split("/");
    if(nums.size()!=2)
        return "0.0";
    double n0 = nums.at(0).toDouble();
    double n1 = nums.at(1).toDouble();
    if(n0<=0 || n1<=0)
        return "0.0";
    return fpsDiv;
}

long long MainWindow::video_get_frameNum(QString videoPath)
{
    QJsonDocument doc = parseFfprobeJson(Current_Path+"/ffmpeg/ffprobe_waifu2xEX.exe", videoPath);
    long long FrameNum = doc.object().value("streams").toArray().isEmpty() ? 0 :
                   doc.object().value("streams").toArray().at(0).toObject().value("nb_frames").toString().toLongLong();
    if(FrameNum < 1)
    {
        emit Send_TextBrowser_NewMessage(tr("ERROR! Unable to read the number of frames of the video: [")+videoPath+"]");
        return 0;
    }
    return FrameNum;
}

int MainWindow::video_get_frameNumDigits(QString videoPath)
{
    int frameNumDigits=1+(int)log10(video_get_frameNum(videoPath));
    return frameNumDigits;
}

void MainWindow::video_video2images(QString VideoPath,QString FrameFolderPath,QString AudioPath)
{
    emit Send_TextBrowser_NewMessage(tr("Start splitting video: [")+VideoPath+"]");
    //=================
    QString ffmpeg_path = Current_Path+"/ffmpeg/ffmpeg_waifu2xEX.exe";
    //================ Get fps =====================
    QString fps_video_cmd=" ";
    QString fps = video_get_fps(VideoPath).trimmed();
    if(fps != "0.0")
    {
        fps_video_cmd = " -r "+fps+" ";
    }
    //=====================
    int FrameNumDigits = video_get_frameNumDigits(VideoPath);
    if(FrameNumDigits==0)return;
    QFile::remove(isPreVFIDone_MarkFilePath(VideoPath));
    //=====================
    QProcess video_splitFrame;
    QString splitCmd = "\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+VideoPath+"\" "+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\"";
    runProcess(&video_splitFrame, splitCmd);
    //============== Attempt commands that might work on Win7 ================================
    if(file_isDirEmpty(FrameFolderPath))
    {
        splitCmd = "\""+ffmpeg_path+"\" -y"+fps_video_cmd+"-i \""+VideoPath+"\" "+fps_video_cmd+" \""+FrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\"";
        runProcess(&video_splitFrame, splitCmd);
    }
    video_get_audio(VideoPath,AudioPath);//Extract audio
    //======== Frame interpolation =========
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString VFI_FolderPath_tmp = video_dir+"/"+video_filename+"_PreVFI_W2xEX";
    if(ui->checkBox_VfiAfterScale_VFI->isChecked()==false && ui->groupBox_FrameInterpolation->isChecked()==true) //If frame interpolation is enabled
    {
        //If complete interpolated frame cache is detected
        if(file_isDirExist(VFI_FolderPath_tmp) && (file_getFileNames_in_Folder_nofilter(FrameFolderPath).size() * ui->spinBox_MultipleOfFPS_VFI->value() == file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()))
        {
            file_DelDir(FrameFolderPath);
            QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
            VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
            return;
        }
        else
        {
            if(FrameInterpolation(FrameFolderPath,VFI_FolderPath_tmp)==true)
            {
                //If frame interpolation succeeds
                file_DelDir(FrameFolderPath);
                QDir VFI_FolderPath_tmp_qdir(VFI_FolderPath_tmp);
                VFI_FolderPath_tmp_qdir.rename(VFI_FolderPath_tmp,FrameFolderPath);
                file_generateMarkFile(isPreVFIDone_MarkFilePath(VideoPath),QString("%1").arg(ui->spinBox_MultipleOfFPS_VFI->value()));
                return;
            }
            else
            {
                //If interpolation fails but upscaling already done
                file_DelDir(VFI_FolderPath_tmp);
                if(this->waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+tr("]. Gonna generate a video without frame Interpolation."));
            }
        }
    }
    //====================================
    emit Send_TextBrowser_NewMessage(tr("Finish splitting video: [")+VideoPath+"]");
}

int MainWindow::video_images2video(QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled)
{
    bool Del_DenoisedAudio = false;
    //=================
    QString bitrate_video_cmd="";
    //=======
    if(ui->groupBox_video_settings->isChecked()&&(ui->spinBox_bitrate_vid->value()>0))
    {
        bitrate_video_cmd=" -b:v "+QString::number(ui->spinBox_bitrate_vid->value(),10)+"k ";
    }
    else
    {
        QString BitRate = video_get_bitrate_AccordingToRes_FrameFolder(ScaledFrameFolderPath,VideoPath);
        if(BitRate!="")bitrate_video_cmd=" -b:v "+BitRate+"k ";
    }
    //================ Custom resolution ======================
    QString resize_cmd ="";
    if(CustRes_isEnabled || isOverScaled)
    {
        if(isOverScaled==true && CustRes_isEnabled==false)
        {
            QMap<QString,int> res_map = video_get_Resolution(VideoPath);
            int scaleratio_orginal = ui->doubleSpinBox_ScaleRatio_video->value();
            resize_cmd =" -vf scale="+QString::number(res_map["width"]*scaleratio_orginal,10)+":"+QString::number(res_map["height"]*scaleratio_orginal,10)+" ";
        }
        //============= If no custom video parameters, recalculate bitrate based on custom resolution ==========
        if(ui->groupBox_video_settings->isChecked()==false || (ui->spinBox_bitrate_vid->value()<1))
        {
            int small_res =0;
            if(CustRes_width<=CustRes_height)
            {
                small_res = CustRes_width;
            }
            else
            {
                small_res = CustRes_height;
            }
            bitrate_video_cmd=" -b:v "+QString::number(small_res*6,10)+"k ";
        }
        //=================================================================
        if(CustRes_AspectRatioMode==Qt::IgnoreAspectRatio && CustRes_isEnabled==true)
        {
            resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":"+QString::number(CustRes_height,10)+" ";
        }
        if(CustRes_AspectRatioMode==Qt::KeepAspectRatio && CustRes_isEnabled==true)
        {
            if(CustRes_width>=CustRes_height)
            {
                resize_cmd =" -vf scale=-2:"+QString::number(CustRes_height,10)+" ";
            }
            else
            {
                resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":-2 ";
            }
        }
        if(CustRes_AspectRatioMode==Qt::KeepAspectRatioByExpanding && CustRes_isEnabled==true)
        {
            if(CustRes_width>=CustRes_height)
            {
                resize_cmd =" -vf scale="+QString::number(CustRes_width,10)+":-2 ";
            }
            else
            {
                resize_cmd =" -vf scale=-2:"+QString::number(CustRes_height,10)+" ";
            }
        }
        if(!resize_cmd.isEmpty())
        {
            resize_cmd.append("-sws_flags lanczos ");
        }
    }
    QString ffmpeg_path = Current_Path+"/ffmpeg/ffmpeg_waifu2xEX.exe";
    int FrameNumDigits = video_get_frameNumDigits(VideoPath);
    if(FrameNumDigits==0)return 0;
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    QString video_ext = vfinfo.suffix();
    //=========== Get fps ===========
    QString fps = video_get_fps(VideoPath).trimmed();
    if(fps == "0.0")
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+VideoPath+tr("]. Error: [Unable to get video frame rate.]"));
        return 0;
    }
    //=============== Frame interpolation ===============
    QString VFI_FolderPath_tmp = video_dir+"/"+video_filename+"_PostVFI_W2xEX";
    //If frame interpolation is enabled
    if(ui->groupBox_FrameInterpolation->isChecked()==true)
    {
        bool isPreVFIDone = QFile::exists(isPreVFIDone_MarkFilePath(VideoPath));
        if((ui->checkBox_VfiAfterScale_VFI->isChecked()==false) || isPreVFIDone==true)
        {
            if(isPreVFIDone==true)
            {
                FrameNumDigits = CalNumDigits((file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath).size()));
                //==========
                int Old_MultipleOfFPS = ui->spinBox_MultipleOfFPS_VFI->value();
                QFile f(isPreVFIDone_MarkFilePath(VideoPath));
                if(f.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QTextStream txtInput(&f);
                    QString lineStr;
                    lineStr = txtInput.readAll();
                    int tmp_mof = lineStr.trimmed().toInt();
                    if(tmp_mof>1)
                    {
                        Old_MultipleOfFPS = tmp_mof;
                    }
                }
                //==========
                QStringList FPS_Nums = fps.split("/");
                fps = QString("%1/%2").arg(FPS_Nums.at(0).toDouble() * Old_MultipleOfFPS).arg(FPS_Nums.at(1).toDouble());
            }
        }
        else
        {
            //If complete interpolated frame cache is detected
            if(file_isDirExist(VFI_FolderPath_tmp) && (file_getFileNames_in_Folder_nofilter(ScaledFrameFolderPath).size() * ui->spinBox_MultipleOfFPS_VFI->value() == file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()))
            {
                FrameNumDigits = CalNumDigits((file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()));
                ScaledFrameFolderPath = VFI_FolderPath_tmp;
                QStringList FPS_Nums = fps.split("/");
                fps = QString("%1/%2").arg(FPS_Nums.at(0).toDouble() * ui->spinBox_MultipleOfFPS_VFI->value()).arg(FPS_Nums.at(1).toDouble());
            }
            else
            {
                //If frame interpolation succeeds
                if(FrameInterpolation(ScaledFrameFolderPath,VFI_FolderPath_tmp)==true)
                {
                    FrameNumDigits = CalNumDigits((file_getFileNames_in_Folder_nofilter(VFI_FolderPath_tmp).size()));
                    ScaledFrameFolderPath = VFI_FolderPath_tmp;
                    QStringList FPS_Nums = fps.split("/");
                    fps = QString("%1/%2").arg(FPS_Nums.at(0).toDouble() * ui->spinBox_MultipleOfFPS_VFI->value()).arg(FPS_Nums.at(1).toDouble());
                }
                else
                {
                    //If interpolation fails and segmented processing or frame-interpolation-only mode is enabled
                    if(ui->checkBox_ProcessVideoBySegment->isChecked()==true)
                    {
                        file_DelDir(VFI_FolderPath_tmp);
                        if(this->waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+"]");
                        return 0;
                    }
                    //If interpolation fails but upscaling was done and not segmented
                    else
                    {
                        file_DelDir(VFI_FolderPath_tmp);
                        if(this->waifu2x_STOP==false)emit Send_TextBrowser_NewMessage(tr("Failed to interpolate frames of video:[")+VideoPath+tr("]. Gonna generate a video without frame Interpolation."));
                    }
                }
            }
        }
    }
    //=============== Audio denoise ========================
    if((ui->checkBox_AudioDenoise->isChecked())&&QFile::exists(AudioPath))
    {
        QString AudioPath_tmp = video_AudioDenoise(AudioPath);
        if(AudioPath_tmp!=AudioPath)
        {
            AudioPath = AudioPath_tmp;
            Del_DenoisedAudio = true;
        }
    }
    //================= Start processing =============================
    emit Send_TextBrowser_NewMessage(tr("Start assembling video:[")+VideoPath+"]");
    //======
    QString CMD = "";
    if(QFile::exists(AudioPath))
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -i \""+AudioPath+"\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
    }
    else
    {
        CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%0"+QString::number(FrameNumDigits,10)+"d.png\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
    }
    QProcess images2video;
    QFile::remove(video_mp4_scaled_fullpath);//Delete old file
    runProcess(&images2video, CMD);
    if(this->waifu2x_STOP)
    {
        images2video.close();
        QFile::remove(video_mp4_scaled_fullpath);
        file_DelDir(VFI_FolderPath_tmp);
        if(Del_DenoisedAudio)QFile::remove(AudioPath);
        return 0;
    }
    //============== Attempt commands that might work on Win7 ================================
    if(QFile::exists(video_mp4_scaled_fullpath)==false)
    {
        if(QFile::exists(AudioPath))
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\" -i \""+AudioPath+"\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
        }
        else
        {
            CMD = "\""+ffmpeg_path+"\" -y -f image2 -framerate "+fps+" -r "+fps+" -i \""+ScaledFrameFolderPath.replace("%","%%")+"/%%0"+QString::number(FrameNumDigits,10)+"d.png\" -r "+fps+bitrate_video_cmd+resize_cmd+video_ReadSettings_OutputVid(AudioPath)+" -r "+fps+" \""+video_mp4_scaled_fullpath+"\"";
        }
        QProcess images2video_retry;
        runProcess(&images2video_retry, CMD);
        if(this->waifu2x_STOP)
        {
            images2video_retry.close();
            QFile::remove(video_mp4_scaled_fullpath);
            file_DelDir(VFI_FolderPath_tmp);
            if(Del_DenoisedAudio)QFile::remove(AudioPath);
            return 0;
        }
    }
    //===================
    if(Del_DenoisedAudio)QFile::remove(AudioPath);
    if(ui->checkBox_KeepVideoCache->isChecked() == false || ui->checkBox_ProcessVideoBySegment->isChecked())file_DelDir(VFI_FolderPath_tmp);
    if(ui->checkBox_KeepVideoCache->isChecked() == false)QFile::remove(isPreVFIDone_MarkFilePath(VideoPath));
    //==============================
    emit Send_TextBrowser_NewMessage(tr("Finish assembling video:[")+VideoPath+"]");
    return 0;
}

QString MainWindow::video_ReadSettings_OutputVid(QString AudioPath)
{
    return videoProcessor->buildOutputArguments(
                AudioPath,
                ui->groupBox_video_settings->isChecked(),
                ui->lineEdit_encoder_vid->text(),
                ui->lineEdit_encoder_audio->text(),
                ui->spinBox_bitrate_audio->value(),
                ui->lineEdit_pixformat->text(),
                ui->lineEdit_ExCommand_output->text());
}

void MainWindow::DelVfiDir(QString VideoPath)
{
    QFileInfo vfinfo(VideoPath);
    QString video_dir = file_getFolderPath(vfinfo);
    QString video_filename = file_getBaseName(VideoPath);
    file_DelDir(video_dir+"/"+video_filename+"_PreVFI_W2xEX");
    file_DelDir(video_dir+"/"+video_filename+"_PostVFI_W2xEX");
}