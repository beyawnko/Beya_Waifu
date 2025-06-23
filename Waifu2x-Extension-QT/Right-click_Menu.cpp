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

void MainWindow::Init_ActionsMenu_checkBox_DelOriginal()
{
    QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setText(tr("Move to Recycle Bin."));
    QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setCheckable(1);
    //===
    ui->checkBox_DelOriginal->addAction(QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal);
}

void MainWindow::Init_ActionsMenu_checkBox_ReplaceOriginalFile()
{
    QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->setText(tr("Move to Recycle Bin."));
    QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->setCheckable(1);
    //===
    ui->checkBox_ReplaceOriginalFile->addAction(QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile);
}

void MainWindow::Init_ActionsMenu_pushButton_RemoveItem()
{
    RemoveALL_image->setText(tr("Remove all Image files."));
    RemoveALL_gif->setText(tr("Remove all Animated Image files."));
    RemoveALL_video->setText(tr("Remove all Video files."));
    //==============
    RemoveALL_image->setIcon(QIcon(":/new/prefix1/icon/picture_Rmenu.png"));
    RemoveALL_gif->setIcon(QIcon(":/new/prefix1/icon/gif_Rmenu.png"));
    RemoveALL_video->setIcon(QIcon(":/new/prefix1/icon/video_Rmenu.png"));
    //==============
    connect(RemoveALL_image, SIGNAL(triggered()), this, SLOT(RemoveALL_image_slot()),Qt::UniqueConnection);
    connect(RemoveALL_gif, SIGNAL(triggered()), this, SLOT(RemoveALL_gif_slot()),Qt::UniqueConnection);
    connect(RemoveALL_video, SIGNAL(triggered()), this, SLOT(RemoveALL_video_slot()),Qt::UniqueConnection);
    //==============
    ui->pushButton_RemoveItem->addAction(RemoveALL_image);
    ui->pushButton_RemoveItem->addAction(RemoveALL_gif);
    ui->pushButton_RemoveItem->addAction(RemoveALL_video);
}

// Definitions for functions below are now stubs in mainwindow.cpp
// void MainWindow::RemoveALL_image_slot()
// void MainWindow::RemoveALL_gif_slot()
// void MainWindow::RemoveALL_video_slot()
// void MainWindow::OpenOutputFolder()
// void MainWindow::Apply_CustRes_QAction_FileList_slot()
// void MainWindow::Cancel_CustRes_QAction_FileList_slot()
// void MainWindow::OpenSelectedFile_FilesList()
// void MainWindow::OpenSelectedFilesFolder_FilesList()

void MainWindow::Init_ActionsMenu_lineEdit_outputPath()
{
    OpenFolder_lineEdit_outputPath->setText(tr("Open output path"));
    OpenFolder_lineEdit_outputPath->setIcon(QIcon(":/new/prefix1/icon/opne_folder.png"));
    connect(OpenFolder_lineEdit_outputPath, SIGNAL(triggered()), this, SLOT(OpenOutputFolder()),Qt::UniqueConnection);
    ui->lineEdit_outputPath->addAction(OpenFolder_lineEdit_outputPath);
}


void MainWindow::Init_ActionsMenu_FilesList()
{
    OpenFile_QAction_FileList->setText(tr("Open file"));
    OpenFile_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/view_file.png"));
    connect(OpenFile_QAction_FileList, SIGNAL(triggered()), this, SLOT(OpenSelectedFile_FilesList()),Qt::UniqueConnection);
    ui->tableView_image->addAction(OpenFile_QAction_FileList);
    ui->tableView_gif->addAction(OpenFile_QAction_FileList);
    ui->tableView_video->addAction(OpenFile_QAction_FileList);
    //===
    OpenFilesFolder_QAction_FileList->setText(tr("Open folder"));
    OpenFilesFolder_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/opne_folder.png"));
    connect(OpenFilesFolder_QAction_FileList, SIGNAL(triggered()), this, SLOT(OpenSelectedFilesFolder_FilesList()),Qt::UniqueConnection);
    ui->tableView_image->addAction(OpenFilesFolder_QAction_FileList);
    ui->tableView_gif->addAction(OpenFilesFolder_QAction_FileList);
    ui->tableView_video->addAction(OpenFilesFolder_QAction_FileList);
    //===
    RemoveFile_FilesList_QAction_FileList->setText(tr("Remove from list [Delete]"));
    RemoveFile_FilesList_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/RemoveFile.png"));
    connect(RemoveFile_FilesList_QAction_FileList, SIGNAL(triggered()), this, SLOT(on_pushButton_RemoveItem_clicked()),Qt::UniqueConnection);
    ui->tableView_image->addAction(RemoveFile_FilesList_QAction_FileList);
    ui->tableView_gif->addAction(RemoveFile_FilesList_QAction_FileList);
    ui->tableView_video->addAction(RemoveFile_FilesList_QAction_FileList);
    //===
    Apply_CustRes_QAction_FileList->setText(tr("Apply custom resolution [Ctrl+A]"));
    Apply_CustRes_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/Apply.png"));
    connect(Apply_CustRes_QAction_FileList, SIGNAL(triggered()), this, SLOT(Apply_CustRes_QAction_FileList_slot()),Qt::UniqueConnection);
    ui->tableView_image->addAction(Apply_CustRes_QAction_FileList);
    ui->tableView_gif->addAction(Apply_CustRes_QAction_FileList);
    ui->tableView_video->addAction(Apply_CustRes_QAction_FileList);
    //===
    Cancel_CustRes_QAction_FileList->setText(tr("Cancel custom resolution [Ctrl+C]"));
    Cancel_CustRes_QAction_FileList->setIcon(QIcon(":/new/prefix1/icon/cancel.png"));
    connect(Cancel_CustRes_QAction_FileList, SIGNAL(triggered()), this, SLOT(Cancel_CustRes_QAction_FileList_slot()),Qt::UniqueConnection);
    ui->tableView_image->addAction(Cancel_CustRes_QAction_FileList);
    ui->tableView_gif->addAction(Cancel_CustRes_QAction_FileList);
    ui->tableView_video->addAction(Cancel_CustRes_QAction_FileList);
}

void MainWindow::OpenSelectedFile_FailedWarning_FilesList()
{
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Error"));
    MSG->setText(tr("Target file(folder) doesn\'t exists!"));
    MSG->setIcon(QMessageBox::Warning);
    MSG->setModal(false);
    MSG->show();
}
