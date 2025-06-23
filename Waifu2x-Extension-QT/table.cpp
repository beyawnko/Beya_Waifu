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
// int MainWindow::Table_FileCount_reload() -> Definition is now a stub in mainwindow.cpp
//============================
// Insert files (implemented in mainwindow.cpp)
//============================

// void MainWindow::Table_image_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status) -> Definition is now a stub in mainwindow.cpp

// void MainWindow::Table_gif_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status) -> Definition is now a stub in mainwindow.cpp

// void MainWindow::Table_video_ChangeStatus_rowNumInt_statusQString(int rowNum, QString status) -> Definition is now a stub in mainwindow.cpp

// void MainWindow::Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width) -> Definition is now a stub in mainwindow.cpp

// void MainWindow::Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width) -> Definition is now a stub in mainwindow.cpp

// void MainWindow::Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int rowNum, QString height, QString width) -> Definition is now a stub in mainwindow.cpp

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
// void MainWindow::on_pushButton_SaveFileList_clicked() -> Definition is now a stub in mainwindow.cpp
// int MainWindow::Table_Save_Current_Table_Filelist_Watchdog(QString Table_FileList_ini) -> Helper, effectively stubbed
// int MainWindow::Table_Save_Current_Table_Filelist_Finished() -> Definition is now a stub in mainwindow.cpp

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

// int MainWindow::Table_Read_Saved_Table_Filelist_Finished(QString Table_FileList_ini) -> Definition is now a stub in mainwindow.cpp
// void MainWindow::on_tableView_image_doubleClicked(const QModelIndex &index) -> Definition is now a stub in mainwindow.cpp
// void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex &index) -> Definition is now a stub in mainwindow.cpp
// void MainWindow::on_tableView_video_doubleClicked(const QModelIndex &index) -> Definition is now a stub in mainwindow.cpp
// void MainWindow::on_tableView_image_pressed(const QModelIndex &index) -> Definition is now a stub in mainwindow.cpp
// void MainWindow::on_tableView_gif_pressed(const QModelIndex &index) -> Definition is now a stub in mainwindow.cpp
// void MainWindow::on_tableView_video_pressed(const QModelIndex &index) -> Definition is now a stub in mainwindow.cpp
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

// void MainWindow::on_pushButton_ClearList_clicked() -> Definition is now a stub in mainwindow.cpp
