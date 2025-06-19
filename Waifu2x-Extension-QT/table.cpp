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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
/*
Initialize table view
*/
void MainWindow::Init_Table()
{
    Table_model_image->setColumnCount(6); // Changed from 4 to 6
    Table_model_image->setHeaderData(0, Qt::Horizontal, tr("Image File Name"));
    Table_model_image->setHeaderData(1, Qt::Horizontal, tr("Status"));
    Table_model_image->setHeaderData(2, Qt::Horizontal, tr("Full Path"));
    Table_model_image->setHeaderData(3, Qt::Horizontal, tr("Resolution")); // Changed from "Custom resolution(Width x Height)"
    Table_model_image->setHeaderData(4, Qt::Horizontal, tr("Output Path"));   // New
    Table_model_image->setHeaderData(5, Qt::Horizontal, tr("Engine Settings"));// New
    ui->tableView_image->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_image->setModel(Table_model_image);
    //=================================
    Table_model_gif->setColumnCount(6); // Changed from 4 to 6
    Table_model_gif->setHeaderData(0, Qt::Horizontal, tr("Animated Image File Name"));
    Table_model_gif->setHeaderData(1, Qt::Horizontal, tr("Status"));
    Table_model_gif->setHeaderData(2, Qt::Horizontal, tr("Full Path"));
    Table_model_gif->setHeaderData(3, Qt::Horizontal, tr("Resolution")); // Changed from "Custom resolution(Width x Height)"
    Table_model_gif->setHeaderData(4, Qt::Horizontal, tr("Output Path"));   // New
    Table_model_gif->setHeaderData(5, Qt::Horizontal, tr("Engine Settings"));// New
    ui->tableView_gif->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_gif->setModel(Table_model_gif);
    //=================================
    Table_model_video->setColumnCount(8); // Changed from 4 to 8
    Table_model_video->setHeaderData(0, Qt::Horizontal, tr("Video File Name"));
    Table_model_video->setHeaderData(1, Qt::Horizontal, tr("Status"));
    Table_model_video->setHeaderData(2, Qt::Horizontal, tr("Full Path"));
    Table_model_video->setHeaderData(3, Qt::Horizontal, tr("Resolution"));      // Changed from "Custom resolution(Width x Height)"
    Table_model_video->setHeaderData(4, Qt::Horizontal, tr("FPS"));             // New
    Table_model_video->setHeaderData(5, Qt::Horizontal, tr("Duration"));        // New
    Table_model_video->setHeaderData(6, Qt::Horizontal, tr("Output Path"));     // New
    Table_model_video->setHeaderData(7, Qt::Horizontal, tr("Engine Settings")); // New
    ui->tableView_video->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableView_video->setModel(Table_model_video);
    //Set horizontal header visible
    ui->tableView_video->horizontalHeader()->setVisible(1);
    ui->tableView_gif->horizontalHeader()->setVisible(1);
    ui->tableView_image->horizontalHeader()->setVisible(1);
    //=============================================
    emit Send_Table_EnableSorting(1);//Enable sorting for file lists
}
/*
Refresh file count under TableView
*/
int MainWindow::Table_FileCount_reload()
{
    long int filecount_image=Table_model_image->rowCount();
    long int filecount_gif=Table_model_gif->rowCount();
    long int filecount_video=Table_model_video->rowCount();
    long int filecount_total=filecount_image+filecount_gif+filecount_video;
    //====================
    // Files exist in the list
    //====================
    if(filecount_total>0)
    {
        //===========
        //Show file count
        //===========
        ui->label_FileCount->setVisible(1);
        ui->label_FileCount->setText(QString(tr("File count: %1")).arg(filecount_total));
        ui->label_FileCount->setToolTip(QString(tr("Image: %1\nAnimated Image: %2\nVideo: %3")).arg(filecount_image).arg(filecount_gif).arg(filecount_video));
        //=================
        //Enable control buttons
        //=================
        ui->pushButton_ClearList->setEnabled(1);
        ui->pushButton_RemoveItem->setEnabled(1);
        //===============================================
        //Show or hide each list based on its file count
        //===============================================
        int TableView_VisibleCount = 0;
        if(filecount_image>0)
        {
            ui->tableView_image->setVisible(1);
            TableView_VisibleCount++;
        }
        else
        {
            curRow_image = -1;
            ui->tableView_image->clearSelection();
            ui->tableView_image->setVisible(0);
        }
        //===
        if(filecount_gif>0)
        {
            ui->tableView_gif->setVisible(1);
            TableView_VisibleCount++;
        }
        else
        {
            curRow_gif = -1;
            ui->tableView_gif->clearSelection();
            ui->tableView_gif->setVisible(0);
        }
        //===
        if(filecount_video>0)
        {
            ui->tableView_video->setVisible(1);
            TableView_VisibleCount++;
        }
        else
        {
            curRow_video = -1;
            ui->tableView_video->clearSelection();
            ui->tableView_video->setVisible(0);
        }
        //========================
        ui->pushButton_ResizeFilesListSplitter->setEnabled(TableView_VisibleCount>1);//Enable reset splitter when more than one list is visible
        //========================
        ui->pushButton_SaveFileList->setEnabled(1);//Enable save list button when files exist
    }
    //====================
    // No files in the list
    //====================
    else
    {
        ui->label_FileCount->setVisible(0);//Hide file count label
        //===================
        // Disable file list control buttons
        //===================
        ui->pushButton_ClearList->setEnabled(0);
        ui->pushButton_RemoveItem->setEnabled(0);
        ui->pushButton_ResizeFilesListSplitter->setEnabled(0);
        //====================
        //Hide file lists and clear selection
        //====================
        curRow_image = -1;
        ui->tableView_image->clearSelection();
        ui->tableView_image->setVisible(0);
        curRow_gif = -1;
        ui->tableView_gif->clearSelection();
        ui->tableView_gif->setVisible(0);
        curRow_video = -1;
        ui->tableView_video->clearSelection();
        ui->tableView_video->setVisible(0);
        //========================
        ui->pushButton_SaveFileList->setEnabled(0);//Disable save list button when no files exist
    }
    return 0;
}
//============================
// Insert files (implemented in mainwindow.cpp)
//============================

void MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status)
{
    ui->tableView_image->setUpdatesEnabled(false);
    Table_model_image->setItem(rowNum, 1, new QStandardItem(status));
    if(ui->checkBox_FileListAutoSlide->isChecked())
    {
        QAbstractItemModel *modessl = Table_model_image;
        QModelIndex indextemp = modessl->index(rowNum, 1);
        ui->tableView_image->scrollTo(indextemp,QAbstractItemView::PositionAtCenter);
    }
    ui->tableView_image->setUpdatesEnabled(true);
}

void MainWindow::Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status)
{
    ui->tableView_gif->setUpdatesEnabled(false);
    Table_model_gif->setItem(rowNum, 1, new QStandardItem(status));
    if(ui->checkBox_FileListAutoSlide->isChecked())
    {
        QAbstractItemModel *modessl = Table_model_gif;
        QModelIndex indextemp = modessl->index(rowNum, 1);
        ui->tableView_gif->scrollTo(indextemp,QAbstractItemView::PositionAtCenter);
    }
    ui->tableView_gif->setUpdatesEnabled(true);
}

void MainWindow::Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status)
{
    ui->tableView_video->setUpdatesEnabled(false);
    Table_model_video->setItem(rowNum, 1, new QStandardItem(status));
    if(ui->checkBox_FileListAutoSlide->isChecked())
    {
        QAbstractItemModel *modessl = Table_model_video;
        QModelIndex indextemp = modessl->index(rowNum, 1);
        ui->tableView_video->scrollTo(indextemp,QAbstractItemView::PositionAtCenter);
    }
    ui->tableView_video->setUpdatesEnabled(true);
}

void MainWindow::Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width)
{
    QString ResStr = width+"x"+height;
    ui->tableView_image->setUpdatesEnabled(false);
    Table_model_image->setItem(rowNum, 3, new QStandardItem(ResStr));
    ui->tableView_image->setUpdatesEnabled(true);
}

void MainWindow::Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width)
{
    QString ResStr = width+"x"+height;
    ui->tableView_gif->setUpdatesEnabled(false);
    Table_model_gif->setItem(rowNum, 3, new QStandardItem(ResStr));
    ui->tableView_gif->setUpdatesEnabled(true);
}

void MainWindow::Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width)
{
    QString ResStr = width+"x"+height;
    ui->tableView_video->setUpdatesEnabled(false);
    Table_model_video->setItem(rowNum, 3, new QStandardItem(ResStr));
    ui->tableView_video->setUpdatesEnabled(true);
}

void MainWindow::Table_image_CustRes_Cancel_rowNumInt(int rowNum)
{
    ui->tableView_image->setUpdatesEnabled(false);
    Table_model_image->setItem(rowNum, 3, new QStandardItem(""));
    ui->tableView_image->setUpdatesEnabled(true);
}

void MainWindow::Table_gif_CustRes_Cancel_rowNumInt(int rowNum)
{
    ui->tableView_gif->setUpdatesEnabled(false);
    Table_model_gif->setItem(rowNum, 3, new QStandardItem(""));
    ui->tableView_gif->setUpdatesEnabled(true);
}

void MainWindow::Table_video_CustRes_Cancel_rowNumInt(int rowNum)
{
    ui->tableView_video->setUpdatesEnabled(false);
    Table_model_video->setItem(rowNum, 3, new QStandardItem(""));
    ui->tableView_video->setUpdatesEnabled(true);
}


void MainWindow::Table_ChangeAllStatusToWaiting()
{
    QMutex_Table_ChangeAllStatusToWaiting.lock();
    ui_tableViews_setUpdatesEnabled(false);
    bool ReProcFinFiles = ui->checkBox_ReProcFinFiles->isChecked();
    int rowNum = Table_model_image->rowCount();
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model_image;
        QModelIndex indextemp = modessl->index(i, 1);
        QVariant datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        if(ReProcFinFiles)
        {
            if(status != "Finished, original file deleted")
            {
                Table_model_image->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
        else
        {
            if(status != "Finished, original file deleted" && status != "Finished")
            {
                Table_model_image->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
    }
    rowNum = Table_model_gif->rowCount();
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model_gif;
        QModelIndex indextemp = modessl->index(i, 1);
        QVariant datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        if(ReProcFinFiles)
        {
            if(status != "Finished, original file deleted")
            {
                Table_model_gif->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
        else
        {
            if(status != "Finished, original file deleted" && status != "Finished")
            {
                Table_model_gif->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
    }
    rowNum = Table_model_video->rowCount();
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model_video;
        QModelIndex indextemp = modessl->index(i, 1);
        QVariant datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        if(ReProcFinFiles)
        {
            if(status != "Finished, original file deleted")
            {
                Table_model_video->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
        else
        {
            if(status != "Finished, original file deleted" && status != "Finished")
            {
                Table_model_video->setItem(i, 1, new QStandardItem("Waiting"));
            }
        }
    }
    ui_tableViews_setUpdatesEnabled(true);
    QMutex_Table_ChangeAllStatusToWaiting.unlock();
}

void MainWindow::Table_Clear()
{
    ui_tableViews_setUpdatesEnabled(false);
    //=====
    Table_model_image->clear();
    Table_model_gif->clear();
    Table_model_video->clear();
    Init_Table();
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = -1;
    //=====
    ui_tableViews_setUpdatesEnabled(true);
}

int MainWindow::Table_image_get_rowNum()
{
    int rowNum = Table_model_image->rowCount();
    return rowNum;
}

int MainWindow::Table_gif_get_rowNum()
{
    int rowNum = Table_model_gif->rowCount();
    return rowNum;
}

int MainWindow::Table_video_get_rowNum()
{
    int rowNum = Table_model_video->rowCount();
    return rowNum;
}
/*
Read status and full path from table
[fullpath]=status;
*/
QMap<QString, QString> MainWindow::Table_Read_status_fullpath(QStandardItemModel *Table_model)
{
    int rowNum = Table_model->rowCount();
    QMap<QString, QString> Map_fullPath_status;
    for (int i = 0; i < rowNum; i++)
    {
        QAbstractItemModel *modessl = Table_model;
        QModelIndex indextemp_status = modessl->index(i, 1);
        QVariant datatemp_status = modessl->data(indextemp_status);
        QString status = datatemp_status.toString();
        QModelIndex indextemp_fullpath = modessl->index(i, 2);
        QVariant datatemp_fullpath = modessl->data(indextemp_fullpath);
        QString fullpath = datatemp_fullpath.toString();
        Map_fullPath_status[fullpath]=status;
    }
    return Map_fullPath_status;
}

//Save current file list
int MainWindow::Table_Save_Current_Table_Filelist(QString Table_FileList_ini)
{
    QFile::remove(Table_FileList_ini);
    //=================
    QSettings *configIniWrite = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    //================= Add warning =========================
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    //================= Store table_image =========================
    configIniWrite->setValue("/table_image/rowCount", Table_model_image->rowCount());
    for(int i=0; i<Table_model_image->rowCount(); i++)
    {
        QAbstractItemModel *modessl = Table_model_image;
        //===
        QModelIndex indextemp = modessl->index(i, 0);
        QVariant datatemp = modessl->data(indextemp);
        QString FileName = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_FileName", FileName);
        //===
        indextemp = modessl->index(i, 1);
        datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_status", status);
        //===
        indextemp = modessl->index(i, 2);
        datatemp = modessl->data(indextemp);
        QString fullPath = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_fullPath", fullPath);
        //===
        indextemp = modessl->index(i, 3);
        datatemp = modessl->data(indextemp);
        QString CustRes_str = datatemp.toString();
        configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_CustRes_str", CustRes_str);
        if(CustRes_str!="")
        {
            QMap<QString, QString> ResMap = CustRes_getResMap(fullPath);
            if(!ResMap.isEmpty())
            {
                configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_CustRes_height", ResMap["height"]);
                configIniWrite->setValue("/table_image/"+QString::number(i,10)+"_CustRes_width", ResMap["width"]);
            }
        }
    }
    //================= Store table_gif =========================
    configIniWrite->setValue("/table_gif/rowCount", Table_model_gif->rowCount());
    for(int i=0; i<Table_model_gif->rowCount(); i++)
    {
        QAbstractItemModel *modessl = Table_model_gif;
        //===
        QModelIndex indextemp = modessl->index(i, 0);
        QVariant datatemp = modessl->data(indextemp);
        QString FileName = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_FileName", FileName);
        //===
        indextemp = modessl->index(i, 1);
        datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_status", status);
        //===
        indextemp = modessl->index(i, 2);
        datatemp = modessl->data(indextemp);
        QString fullPath = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_fullPath", fullPath);
        //===
        indextemp = modessl->index(i, 3);
        datatemp = modessl->data(indextemp);
        QString CustRes_str = datatemp.toString();
        configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_CustRes_str", CustRes_str);
        if(CustRes_str!="")
        {
            QMap<QString, QString> ResMap = CustRes_getResMap(fullPath);
            if(!ResMap.isEmpty())
            {
                configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_CustRes_height", ResMap["height"]);
                configIniWrite->setValue("/table_gif/"+QString::number(i,10)+"_CustRes_width", ResMap["width"]);
            }
        }
    }
    //================= Store table_video =========================
    configIniWrite->setValue("/table_video/rowCount", Table_model_video->rowCount());
    for(int i=0; i<Table_model_video->rowCount(); i++)
    {
        QAbstractItemModel *modessl = Table_model_video;
        //===
        QModelIndex indextemp = modessl->index(i, 0);
        QVariant datatemp = modessl->data(indextemp);
        QString FileName = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_FileName", FileName);
        //===
        indextemp = modessl->index(i, 1);
        datatemp = modessl->data(indextemp);
        QString status = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_status", status);
        //===
        indextemp = modessl->index(i, 2);
        datatemp = modessl->data(indextemp);
        QString fullPath = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_fullPath", fullPath);
        //===
        indextemp = modessl->index(i, 3);
        datatemp = modessl->data(indextemp);
        QString CustRes_str = datatemp.toString();
        configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_CustRes_str", CustRes_str);
        if(CustRes_str!="")
        {
            QMap<QString, QString> ResMap = CustRes_getResMap(fullPath);
            if(!ResMap.isEmpty())
            {
                configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_CustRes_height", ResMap["height"]);
                configIniWrite->setValue("/table_video/"+QString::number(i,10)+"_CustRes_width", ResMap["width"]);
            }
        }
    }
    return 0;
}
void MainWindow::on_pushButton_SaveFileList_clicked()
{
    if(Table_model_video->rowCount()<=0&&Table_model_image->rowCount()<=0&&Table_model_gif->rowCount()<=0)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("File list is empty!"));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        return;
    }
    //====================== Select save location ==========================
    //Create default folder for saved files
    file_mkDir(Current_Path+"/FilesList_W2xEX");
    //Save file dialog
    QString FilesListFullPath = QFileDialog::getSaveFileName(this, tr("Save files list @Beya_Waifu"),
                                Current_Path+"/FilesList_W2xEX/FilesList_W2xEX_"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss")+".ini",
                                "*.ini");
    if(FilesListFullPath.trimmed()=="")return;
    //Check if folder exists and is writable
    QFileInfo FilesListFullPath_fileinfo(FilesListFullPath);
    QString FilesListFullPath_FolderPath = file_getFolderPath(FilesListFullPath_fileinfo);
    if(file_isDirExist(FilesListFullPath_FolderPath)==false || file_isDirWritable(FilesListFullPath_FolderPath)==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Error"));
        MSG->setText(tr("Target folder doesn't exist or unable to write to the target folder."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(false);
        MSG->show();
        return;
    }
    //======================= Start saving ========================
    this->setAcceptDrops(0);//Disable drop file
    pushButton_Start_setEnabled_self(0);//Disable start button
    ui->pushButton_ClearList->setEnabled(0);
    ui->pushButton_RemoveItem->setEnabled(0);
    ui->pushButton_CustRes_cancel->setEnabled(0);
    ui->pushButton_CustRes_apply->setEnabled(0);
    ui->pushButton_ReadFileList->setEnabled(0);
    ui->pushButton_SaveFileList->setEnabled(0);
    ui->pushButton_BrowserFile->setEnabled(0);
    emit Send_TextBrowser_NewMessage(tr("Write to the file, please wait."));
    Table_Save_Current_Table_Filelist(FilesListFullPath);
    (void)QtConcurrent::run([this, FilesListFullPath] { this->Table_Save_Current_Table_Filelist_Watchdog(FilesListFullPath); });
}
int MainWindow::Table_Save_Current_Table_Filelist_Watchdog(QString Table_FileList_ini)
{
    while(!QFile::exists(Table_FileList_ini))
    {
        Delay_msec_sleep(100);
    }
    emit Send_Table_Save_Current_Table_Filelist_Finished();
    return 0;
}
int MainWindow::Table_Save_Current_Table_Filelist_Finished()
{
    if(Waifu2xMain.isRunning()==false)
    {
        this->setAcceptDrops(1);//Enable drop file
        ui->pushButton_ClearList->setEnabled(1);
        ui->pushButton_RemoveItem->setEnabled(1);
        ui->pushButton_ReadFileList->setEnabled(1);
        ui->pushButton_BrowserFile->setEnabled(1);
    }
    ui->pushButton_CustRes_cancel->setEnabled(1);
    ui->pushButton_CustRes_apply->setEnabled(1);
    pushButton_Start_setEnabled_self(1);//Enable start button
    ui->pushButton_SaveFileList->setEnabled(1);
    emit Send_TextBrowser_NewMessage(tr("File list saved successfully!"));
    //===
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("File list saved successfully!"));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
    //===
    return 0;
}
int MainWindow::Table_Read_Saved_Table_Filelist(QString Table_FileList_ini)
{
    if(!QFile::exists(Table_FileList_ini))
    {
        emit Send_TextBrowser_NewMessage(tr("Cannot find the saved Files List!"));
        emit Send_Table_Read_Saved_Table_Filelist_Finished(Table_FileList_ini);
        return 0;
    }
    //=================
    QSettings *configIniRead = new QSettings(Table_FileList_ini, QSettings::IniFormat);
    //====================
    int rowCount_image = configIniRead->value("/table_image/rowCount").toInt();
    int rowCount_gif = configIniRead->value("/table_gif/rowCount").toInt();
    int rowCount_video = configIniRead->value("/table_video/rowCount").toInt();
    //==========
    // Progressbar_MaxVal and Progressbar_CurrentVal are member variables that are being removed.
    // We'll use a local variable for the total count in this function.
    int totalFilesToLoad = rowCount_image + rowCount_gif + rowCount_video;
    emit Send_PrograssBar_Range_min_max_slots(0, totalFilesToLoad); // Use new signal
    //========= Load image ========
    for(int i=0; i<rowCount_image; i++)
    {
        //===========
        QString FileName =configIniRead->value("/table_image/"+QString::number(i,10)+"_FileName").toString();
        QString status =configIniRead->value("/table_image/"+QString::number(i,10)+"_status").toString();
        QString fullPath =configIniRead->value("/table_image/"+QString::number(i,10)+"_fullPath").toString();
        QString CustRes_str =configIniRead->value("/table_image/"+QString::number(i,10)+"_CustRes_str").toString();
        //================
        // mutex_Table_insert_finished.lock(); // Obsolete
        // Table_insert_finished=false; // Obsolete
        // mutex_Table_insert_finished.unlock(); // Obsolete
        emit Send_Table_image_insert_fileName_fullPath(FileName,fullPath);
        // while(!Table_insert_finished) // Obsolete
        // {
        //     Delay_msec_sleep(10);
        // }
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(i,status);
        if(status=="Failed")
        {
            emit Send_TextBrowser_NewMessage(tr("[Failed]--[")+fullPath+"]");
        }
        //===============
        if(CustRes_str!="")
        {
            QString CustRes_height =configIniRead->value("/table_image/"+QString::number(i,10)+"_CustRes_height").toString();
            QString CustRes_width =configIniRead->value("/table_image/"+QString::number(i,10)+"_CustRes_width").toString();
            emit Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(i,CustRes_height,CustRes_width);
            QMap<QString, QString> res_map;
            res_map["fullpath"] = fullPath;
            res_map["height"] = CustRes_height;
            res_map["width"] = CustRes_width;
            Custom_resolution_list.append(res_map);
        }
        emit Send_progressbar_Add_slots(); // Use new signal
        //Delay_msec_sleep(100);
    }
    //========= Load gif ========
    for(int i=0; i<rowCount_gif; i++)
    {
        //===========
        QString FileName =configIniRead->value("/table_gif/"+QString::number(i,10)+"_FileName").toString();
        QString status =configIniRead->value("/table_gif/"+QString::number(i,10)+"_status").toString();
        QString fullPath =configIniRead->value("/table_gif/"+QString::number(i,10)+"_fullPath").toString();
        QString CustRes_str =configIniRead->value("/table_gif/"+QString::number(i,10)+"_CustRes_str").toString();
        //================
        // mutex_Table_insert_finished.lock(); // Obsolete
        // Table_insert_finished=false; // Obsolete
        // mutex_Table_insert_finished.unlock(); // Obsolete
        emit Send_Table_gif_insert_fileName_fullPath(FileName,fullPath);
        // while(!Table_insert_finished) // Obsolete
        // {
        //     Delay_msec_sleep(10);
        // }
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(i,status);
        if(status=="Failed")
        {
            emit Send_TextBrowser_NewMessage(tr("[Failed]--[")+fullPath+"]");
        }
        //===============
        if(CustRes_str!="")
        {
            QString CustRes_height =configIniRead->value("/table_gif/"+QString::number(i,10)+"_CustRes_height").toString();
            QString CustRes_width =configIniRead->value("/table_gif/"+QString::number(i,10)+"_CustRes_width").toString();
            emit Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(i,CustRes_height,CustRes_width);
            QMap<QString, QString> res_map;
            res_map["fullpath"] = fullPath;
            res_map["height"] = CustRes_height;
            res_map["width"] = CustRes_width;
            Custom_resolution_list.append(res_map);
        }
        emit Send_progressbar_Add_slots(); // Use new signal (corrected: only one call)
        //Delay_msec_sleep(100);
    }
    //========= Load video ========
    for(int i=0; i<rowCount_video; i++)
    {
        //===========
        QString FileName =configIniRead->value("/table_video/"+QString::number(i,10)+"_FileName").toString();
        QString status =configIniRead->value("/table_video/"+QString::number(i,10)+"_status").toString();
        QString fullPath =configIniRead->value("/table_video/"+QString::number(i,10)+"_fullPath").toString();
        QString CustRes_str =configIniRead->value("/table_video/"+QString::number(i,10)+"_CustRes_str").toString();
        //================
        // mutex_Table_insert_finished.lock(); // Obsolete
        // Table_insert_finished=false; // Obsolete
        // mutex_Table_insert_finished.unlock(); // Obsolete
        emit Send_Table_video_insert_fileName_fullPath(FileName,fullPath);
        // while(!Table_insert_finished) // Obsolete
        // {
        //     Delay_msec_sleep(10);
        // }
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(i,status);
        if(status=="Failed")
        {
            emit Send_TextBrowser_NewMessage(tr("[Failed]--[")+fullPath+"]");
        }
        //===============
        if(CustRes_str!="")
        {
            QString CustRes_height =configIniRead->value("/table_video/"+QString::number(i,10)+"_CustRes_height").toString();
            QString CustRes_width =configIniRead->value("/table_video/"+QString::number(i,10)+"_CustRes_width").toString();
            emit Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(i,CustRes_height,CustRes_width);
            QMap<QString, QString> res_map;
            res_map["fullpath"] = fullPath;
            res_map["height"] = CustRes_height;
            res_map["width"] = CustRes_width;
            Custom_resolution_list.append(res_map);
        }
        emit Send_progressbar_Add_slots();
        //Delay_msec_sleep(100);
    }
    //====================
    // The main logic has been moved to ProcessFileListWorker.
    // This function is kept to avoid breaking potential (though unlikely) direct internal calls,
    // but it should no longer be the primary entry point for loading saved lists.
    // on_pushButton_ReadFileList_clicked now calls ProcessFileListWorker directly via QtConcurrent.
    // emit Send_Table_Read_Saved_Table_Filelist_Finished(Table_FileList_ini); // This is now emitted by the worker.
    return 0;
}

void MainWindow::ProcessFileListWorker(QString file_list_Path, const QSet<QString>& existingImagePaths, const QSet<QString>& existingGifPaths, const QSet<QString>& existingVideoPaths)
{
    if (!QFile::exists(file_list_Path)) {
        emit Send_TextBrowser_NewMessage(tr("Cannot find the saved Files List!"));
        // Ensure UI is re-enabled by emitting the finished signal
        emit Send_Table_Read_Saved_Table_Filelist_Finished(file_list_Path);
        return;
    }

    QSettings *configIniRead = new QSettings(file_list_Path, QSettings::IniFormat);

    int rowCount_image = configIniRead->value("/table_image/rowCount", 0).toInt();
    int rowCount_gif = configIniRead->value("/table_gif/rowCount", 0).toInt();
    int rowCount_video = configIniRead->value("/table_video/rowCount", 0).toInt();
    int totalFilesToLoad = rowCount_image + rowCount_gif + rowCount_video;

    if (totalFilesToLoad == 0) {
        emit Send_TextBrowser_NewMessage(tr("The file list is empty."));
        emit Send_Table_Read_Saved_Table_Filelist_Finished(file_list_Path);
        delete configIniRead;
        return;
    }

    emit Send_PrograssBar_Range_min_max_slots(0, totalFilesToLoad);

    QList<FileLoadInfo> imagesToLoad;
    QList<FileLoadInfo> gifsToLoad;
    QList<FileLoadInfo> videosToLoad;
    bool localAddNewImage = false;
    bool localAddNewGif = false;
    bool localAddNewVideo = false;

    // Read extensions (ideally pass these as args to worker, but using ui for now)
    QString Ext_image_str = ui->Ext_image->text().toLower();
    QStringList nameFilters_image = Ext_image_str.split(":");
    nameFilters_image.removeAll("gif");
    nameFilters_image.removeAll("apng");

    QString Ext_video_str = ui->Ext_video->text().toLower();
    QStringList nameFilters_video = Ext_video_str.split(":");
    nameFilters_video.removeAll("gif");
    nameFilters_video.removeAll("apng");

    // Load Images
    for (int i = 0; i < rowCount_image; i++) {
        FileLoadInfo info;
        info.fileName = configIniRead->value("/table_image/" + QString::number(i) + "_FileName").toString();
        info.status = configIniRead->value("/table_image/" + QString::number(i) + "_status").toString();
        info.fullPath = configIniRead->value("/table_image/" + QString::number(i) + "_fullPath").toString();
        QString custResStr = configIniRead->value("/table_image/" + QString::number(i) + "_CustRes_str").toString();
        if (!custResStr.isEmpty()) {
            info.customResolutionHeight = configIniRead->value("/table_image/" + QString::number(i) + "_CustRes_height").toString();
            info.customResolutionWidth = configIniRead->value("/table_image/" + QString::number(i) + "_CustRes_width").toString();
        }

        if (!Deduplicate_filelist_worker(info.fullPath, existingImagePaths, existingGifPaths, existingVideoPaths)) {
            imagesToLoad.append(info);
            localAddNewImage = true;
        }
        emit Send_progressbar_Add_slots();
    }

    // Load GIFs
    for (int i = 0; i < rowCount_gif; i++) {
        FileLoadInfo info;
        info.fileName = configIniRead->value("/table_gif/" + QString::number(i) + "_FileName").toString();
        info.status = configIniRead->value("/table_gif/" + QString::number(i) + "_status").toString();
        info.fullPath = configIniRead->value("/table_gif/" + QString::number(i) + "_fullPath").toString();
        QString custResStr = configIniRead->value("/table_gif/" + QString::number(i) + "_CustRes_str").toString();
        if (!custResStr.isEmpty()) {
            info.customResolutionHeight = configIniRead->value("/table_gif/" + QString::number(i) + "_CustRes_height").toString();
            info.customResolutionWidth = configIniRead->value("/table_gif/" + QString::number(i) + "_CustRes_width").toString();
        }

        if (!Deduplicate_filelist_worker(info.fullPath, existingImagePaths, existingGifPaths, existingVideoPaths)) {
            // Double check file type based on extension, as INI might be old/mixed
            QFileInfo qFileInfo(info.fullPath);
            QString file_ext = qFileInfo.suffix().toLower();
            if (file_ext == "gif" || file_ext == "apng") {
                 gifsToLoad.append(info);
                 localAddNewGif = true;
            } else if (nameFilters_image.contains(file_ext)) {
                 imagesToLoad.append(info); // Re-classify as image if needed
                 localAddNewImage = true;
            } else {
                // Could add to a generic "unknown" list or log if necessary
            }
        }
        emit Send_progressbar_Add_slots();
    }

    // Load Videos
    for (int i = 0; i < rowCount_video; i++) {
        FileLoadInfo info;
        info.fileName = configIniRead->value("/table_video/" + QString::number(i) + "_FileName").toString();
        info.status = configIniRead->value("/table_video/" + QString::number(i) + "_status").toString();
        info.fullPath = configIniRead->value("/table_video/" + QString::number(i) + "_fullPath").toString();
        QString custResStr = configIniRead->value("/table_video/" + QString::number(i) + "_CustRes_str").toString();
        if (!custResStr.isEmpty()) {
            info.customResolutionHeight = configIniRead->value("/table_video/" + QString::number(i) + "_CustRes_height").toString();
            info.customResolutionWidth = configIniRead->value("/table_video/" + QString::number(i) + "_CustRes_width").toString();
        }

        if (!Deduplicate_filelist_worker(info.fullPath, existingImagePaths, existingGifPaths, existingVideoPaths)) {
            // Double check file type
            QFileInfo qFileInfo(info.fullPath);
            QString file_ext = qFileInfo.suffix().toLower();
            if (nameFilters_video.contains(file_ext)) {
                videosToLoad.append(info);
                localAddNewVideo = true;
            } else {
                // Could add to a generic "unknown" list or log
            }
        }
        emit Send_progressbar_Add_slots();
    }

    delete configIniRead;

    emit Send_Batch_Table_Update(imagesToLoad, gifsToLoad, videosToLoad, localAddNewImage, localAddNewGif, localAddNewVideo);
    emit Send_Table_Read_Saved_Table_Filelist_Finished(file_list_Path); // Signal completion
}

int MainWindow::Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini)
{
    // This function is now primarily for UI re-enablement and final messages.
    // The actual table population is done by Batch_Table_Update_slots.
    // Progressbar_MaxVal = 0; // Obsolete
    // Progressbar_CurrentVal = 0; // Obsolete
    // progressbar_clear(); // This resets m_TotalNumProc, which might be undesirable if it was set by the worker.
                         // Batch_Table_Update_slots handles the progress bar finalization for loading.

    ui_tableViews_setUpdatesEnabled(true); // Ensure updates are on
    this->setAcceptDrops(1);
    pushButton_Start_setEnabled_self(1);
    ui->pushButton_CustRes_cancel->setEnabled(1);
    ui->pushButton_CustRes_apply->setEnabled(1);
    ui->pushButton_ReadFileList->setEnabled(1);
    ui->pushButton_SaveFileList->setEnabled(1);
    ui->pushButton_BrowserFile->setEnabled(1);
    // Re-enable other UI elements disabled by on_pushButton_ReadFileList_clicked
    ui->groupBox_Setting->setEnabled(1);
    ui->groupBox_FileList->setEnabled(1);
    ui->groupBox_InputExt->setEnabled(1);
    ui->checkBox_ScanSubFolders->setEnabled(1);


    if(!QFile::exists(Table_FileList_ini)) // Should not happen if worker checked first
    {
        return 0;
    }

    // The visibility and scrolling logic is now handled by Batch_Table_Update_slots
    // So, no need to re-read rowCounts here for that purpose.

    Table_FileCount_reload(); // Ensure file counts are accurate based on what was actually added

    // Check if anything was actually added (Batch_Table_Update_slots also has a similar check)
    bool anyAdded = false;
    if (Table_model_image->rowCount() > 0 || Table_model_gif->rowCount() > 0 || Table_model_video->rowCount() > 0) {
        anyAdded = true; // A rough check, better if Batch_Table_Update_slots communicated this
    }

    if(!anyAdded && QFile::exists(Table_FileList_ini)) { // If the file existed but nothing got added (e.g. all duplicates or empty sections)
         QSettings settings(Table_FileList_ini, QSettings::IniFormat);
         if (settings.childGroups().isEmpty() ||
            (settings.value("/table_image/rowCount", 0).toInt() == 0 &&
             settings.value("/table_gif/rowCount", 0).toInt() == 0 &&
             settings.value("/table_video/rowCount", 0).toInt() == 0))
         {
            emit Send_TextBrowser_NewMessage(tr("The file list saved last time is empty."));
         }
         // The problematic 'else if' block is removed.
    }
    emit Send_TextBrowser_NewMessage(tr("Finished loading file list.")); // General completion message
    return 0;
}
void MainWindow::on_tableView_image_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    if(curRow_image==-1)return;
    QModelIndex a;
    on_tableView_image_pressed(a);
}
void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    if(curRow_gif==-1)return;
    QModelIndex a;
    on_tableView_gif_pressed(a);
}
void MainWindow::on_tableView_video_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    if(curRow_video==-1)return;
    QModelIndex a;
    on_tableView_video_pressed(a);
}
void MainWindow::on_tableView_image_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    curRow_image = ui->tableView_image->currentIndex().row();
    curRow_gif = -1;
    curRow_video = -1;
    ui->tableView_gif->clearSelection();
    ui->tableView_video->clearSelection();
}
void MainWindow::on_tableView_gif_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    curRow_gif = ui->tableView_gif->currentIndex().row();
    curRow_image = -1;
    curRow_video = -1;
    ui->tableView_image->clearSelection();
    ui->tableView_video->clearSelection();
}
void MainWindow::on_tableView_video_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    curRow_video = ui->tableView_video->currentIndex().row();
    curRow_image = -1;
    curRow_gif = -1;
    ui->tableView_image->clearSelection();
    ui->tableView_gif->clearSelection();
}
/*
Enable or disable sorting for the three file lists
*/
void MainWindow::Table_EnableSorting(bool EnableSorting)
{
    ui->tableView_video->horizontalHeader()->setSortIndicatorShown(EnableSorting);
    ui->tableView_gif->horizontalHeader()->setSortIndicatorShown(EnableSorting);
    ui->tableView_image->horizontalHeader()->setSortIndicatorShown(EnableSorting);
    //=====
    ui->tableView_image->setSortingEnabled(EnableSorting);
    ui->tableView_gif->setSortingEnabled(EnableSorting);
    ui->tableView_video->setSortingEnabled(EnableSorting);
}
/*
Enable/disable file list table UI updates
*/
void MainWindow::ui_tableViews_setUpdatesEnabled(bool isEnabled)
{
    ui->tableView_video->setUpdatesEnabled(isEnabled);
    ui->tableView_gif->setUpdatesEnabled(isEnabled);
    ui->tableView_image->setUpdatesEnabled(isEnabled);
}

void MainWindow::on_pushButton_ClearList_clicked()
{
    Table_Clear();
    Table_FileCount_reload();
}
