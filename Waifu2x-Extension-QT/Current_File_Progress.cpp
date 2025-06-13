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
#include <memory>

/*
Start processing the current file
*/
void MainWindow::CurrentFileProgress_Start(QString FileName,int FrameNum)
{
    ui->groupBox_CurrentFile->setTitle(tr("Current File:")+" "+FileName);
    //=================================
    ui->progressBar_CurrentFile->setRange(0,FrameNum);
    ui->progressBar_CurrentFile->setValue(0);
    //=================================
    ui->label_FrameProgress_CurrentFile->setText("0/"+QString::number(FrameNum,10));
    ui->label_TimeRemain_CurrentFile->setText(tr("Time remaining:NULL"));
    ui->label_TimeCost_CurrentFile->setText(tr("Time taken:NULL"));
    ui->label_ETA_CurrentFile->setText(tr("ETA:NULL"));
    //=================================
    TimeCost_CurrentFile = 0;
    TaskNumTotal_CurrentFile=FrameNum;
    TaskNumFinished_CurrentFile=0;
    NewTaskFinished_CurrentFile=false;
    ETA_CurrentFile=0;
    //=================================
    ui->groupBox_CurrentFile->setVisible(1);
    //=================================
    isStart_CurrentFile = true;
    return;
}
/*
Stop processing the current file
*/
void MainWindow::CurrentFileProgress_Stop()
{
    ui->groupBox_CurrentFile->setVisible(0);
    isStart_CurrentFile = false;
    return;
}
/*
Progress bar +1
*/
void MainWindow::CurrentFileProgress_progressbar_Add()
{
    TaskNumFinished_CurrentFile++;
    NewTaskFinished_CurrentFile=true;
    ui->progressBar_CurrentFile->setValue(TaskNumFinished_CurrentFile);
    ui->label_FrameProgress_CurrentFile->setText(QString::number(TaskNumFinished_CurrentFile,10)+"/"+QString::number(TaskNumTotal_CurrentFile,10));
}
/*
Progress bar + segment duration
*/
void MainWindow::CurrentFileProgress_progressbar_Add_SegmentDuration(int SegmentDuration)
{
    TaskNumFinished_CurrentFile+=SegmentDuration;
    NewTaskFinished_CurrentFile=true;
    ui->progressBar_CurrentFile->setValue(TaskNumFinished_CurrentFile);
    ui->label_FrameProgress_CurrentFile->setText(QString::number(TaskNumFinished_CurrentFile,10)+"/"+QString::number(TaskNumTotal_CurrentFile,10));
}
/*
Progress bar + segment duration
*/
void MainWindow::CurrentFileProgress_progressbar_SetFinishedValue(int FinishedValue)
{
    TaskNumFinished_CurrentFile=FinishedValue;
    NewTaskFinished_CurrentFile=true;
    ui->progressBar_CurrentFile->setValue(TaskNumFinished_CurrentFile);
    ui->label_FrameProgress_CurrentFile->setText(QString::number(TaskNumFinished_CurrentFile,10)+"/"+QString::number(TaskNumTotal_CurrentFile,10));
}
/*
Monitor the number of files in the folder
*/
void MainWindow::CurrentFileProgress_WatchFolderFileNum(QString FolderPath)
{
    if(FileProgressWatcher==nullptr) return;
    FileProgressWatcher->removePaths(FileProgressWatcher->directories());
    FileProgressWatcher->addPath(FolderPath);

    auto updateFn = [=]() {
        if(!FileProgressWatch_isEnabled){
            FileProgressWatcher->removePaths(FileProgressWatcher->directories());
            return;
        }
        if(!file_isDirExist(FolderPath)){
            FileProgressWatcher->removePaths(FileProgressWatcher->directories());
            return;
        }
        QStringList FilesNameList = file_getFileNames_in_Folder_nofilter(FolderPath);
        emit Send_CurrentFileProgress_progressbar_SetFinishedValue(FilesNameList.size());
    };

    connect(FileProgressWatcher, &QFileSystemWatcher::directoryChanged,
            this, updateFn);

    if(FileProgressStopTimer==nullptr) return;
    FileProgressStopTimer->stop();
    FileProgressStopTimer->setInterval(1000);
    connect(FileProgressStopTimer, &QTimer::timeout, this, [=](){
        if(!FileProgressWatch_isEnabled){
            FileProgressWatcher->removePaths(FileProgressWatcher->directories());
        }
    });
    FileProgressStopTimer->start();

    updateFn();
}
/*
Monitor the number of files in the folder
*/
void MainWindow::CurrentFileProgress_WatchFolderFileNum_Textbrower(QString SourceFile_fullPath,QString FolderPath,int TotalFileNum)
{
    if(FileProgressWatcher_Text==nullptr) return;
    FileProgressWatcher_Text->removePaths(FileProgressWatcher_Text->directories());
    FileProgressWatcher_Text->addPath(FolderPath);

    auto countPtr = std::make_shared<int>(file_getFileNames_in_Folder_nofilter(FolderPath).size());
    auto updateFn = [=]() {
        if(!FileProgressWatch_isEnabled){
            FileProgressWatcher_Text->removePaths(FileProgressWatcher_Text->directories());
            return;
        }
        if(!file_isDirExist(FolderPath)){
            FileProgressWatcher_Text->removePaths(FileProgressWatcher_Text->directories());
            return;
        }
        int New_num = file_getFileNames_in_Folder_nofilter(FolderPath).size();
        if(New_num!=*countPtr){
            *countPtr = New_num;
            emit Send_TextBrowser_NewMessage(tr("File name:[")+SourceFile_fullPath+tr("]  Scale progress:[")+QString::number(New_num,10)+"/"+QString::number(TotalFileNum,10)+tr("] Frames"));
        }
    };

    connect(FileProgressWatcher_Text, &QFileSystemWatcher::directoryChanged,
            this, updateFn);

    if(FileProgressStopTimer_Text==nullptr) return;
    FileProgressStopTimer_Text->stop();
    FileProgressStopTimer_Text->setInterval(1000);
    connect(FileProgressStopTimer_Text, &QTimer::timeout, this, [=](){
        if(!FileProgressWatch_isEnabled){
            FileProgressWatcher_Text->removePaths(FileProgressWatcher_Text->directories());
        }
    });
    FileProgressStopTimer_Text->start();

    updateFn();
}
