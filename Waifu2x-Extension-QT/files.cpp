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
#include <QEventLoop>
#include <QFileSystemWatcher>
#include <QTimer>

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    //If it is a file, drag and drop is supported
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}
/*
Drag and drop file event
*/
void MainWindow::dropEvent(QDropEvent *event)
{
    //Reset the judgment flag of whether a new file of a certain type has been added
    AddNew_gif=false;
    AddNew_image=false;
    AddNew_video=false;
    //================
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;
    //================== UI control ========================
    ui_tableViews_setUpdatesEnabled(false);
    //================
    ui->groupBox_Setting->setEnabled(0);
    ui->groupBox_FileList->setEnabled(0);
    ui->groupBox_InputExt->setEnabled(0);
    pushButton_Start_setEnabled_self(0);
    ui->checkBox_ScanSubFolders->setEnabled(0);
    this->setAcceptDrops(0);
    // ui->label_DropFile->setText(tr("Adding files, please wait."));
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));
    //===================================================
    (void)QtConcurrent::run([this, urls]() { this->Read_urls(urls); });
}
/*
Read urls
*/
void MainWindow::Read_urls(QList<QUrl> urls)
{
    emit Send_PrograssBar_Range_min_max(0, urls.size());
    if(ui->checkBox_ScanSubFolders->isChecked())
    {
        foreach(QUrl url, urls)
        {
            Add_File_Folder_IncludeSubFolder(url.toLocalFile());
            emit Send_progressbar_Add();
        }
    }
    else
    {
        foreach(QUrl url, urls)
        {
            Add_File_Folder(url.toLocalFile());
            emit Send_progressbar_Add();
        }
    }
    emit Send_Read_urls_finfished();
    return;
}
/*
Read urls
Aftermath/Cleanup
*/
void MainWindow::Read_urls_finfished()
{
    //================== Release UI control ========================
    ui_tableViews_setUpdatesEnabled(true);
    //===
    ui->groupBox_Setting->setEnabled(1);
    ui->groupBox_FileList->setEnabled(1);
    pushButton_Start_setEnabled_self(1);
    ui->groupBox_InputExt->setEnabled(1);
    ui->checkBox_ScanSubFolders->setEnabled(1);
    this->setAcceptDrops(1);
    // ui->label_DropFile->setText(tr("Drag and drop files or folders here\n(Image, GIF and Video)"));
    emit Send_TextBrowser_NewMessage(tr("Add file complete."));
    //===================================================
    emit Send_PrograssBar_Range_min_max(0, 0);
    //======================
    //If no files were added
    if(AddNew_gif==false&&AddNew_image==false&&AddNew_video==false)
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("The file format is not supported, please enter supported file format, or add more file extensions yourself."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        return;
    }
    if(AddNew_image)
    {
        // ui->label_DropFile->setVisible(0);//Hide file drop label
        ui->tableView_image->setVisible(1);
    }
    if(AddNew_gif)
    {
        // ui->label_DropFile->setVisible(0);//Hide file drop label
        ui->tableView_gif->setVisible(1);
    }
    if(AddNew_video)
    {
        // ui->label_DropFile->setVisible(0);//Hide file drop label
        ui->tableView_video->setVisible(1);
    }
    //===================
    ui->tableView_gif->scrollToBottom();
    ui->tableView_image->scrollToBottom();
    ui->tableView_video->scrollToBottom();
    QScrollBar *image_ScrBar = ui->tableView_image->horizontalScrollBar();
    image_ScrBar->setValue(0);
    QScrollBar *gif_ScrBar = ui->tableView_gif->horizontalScrollBar();
    gif_ScrBar->setValue(0);
    QScrollBar *video_ScrBar = ui->tableView_video->horizontalScrollBar();
    video_ScrBar->setValue(0);
    //==========
    AddNew_image=false;
    AddNew_gif=false;
    AddNew_video=false;
    //============
    Table_FileCount_reload();
}


/*
Add files & folders
*/
void MainWindow::Add_File_Folder(QString Full_Path)
{
    QFileInfo fileinfo(Full_Path);
    if(fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path);
    }
    else
    {
        QStringList FileNameList = file_getFileNames_in_Folder_nofilter(Full_Path);//Read legal file names
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            QString tmp="";
            for(int i = 0; i < FileNameList.size(); i++)
            {
                tmp = FileNameList.at(i);
                Full_Path_File = Full_Path + "/" + tmp;
                FileList_Add(tmp, Full_Path_File);
            }
        }
    }
}
/*
Add files & folders
Scan subfolders
*/
void MainWindow::Add_File_Folder_IncludeSubFolder(QString Full_Path)
{
    QFileInfo fileinfo(Full_Path);
    if(fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path);
    }
    else
    {
        QStringList FileNameList = getFileNames_IncludeSubFolder(Full_Path);//Read legal file names
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            QString tmp="";
            for(int i = 0; i < FileNameList.size(); i++)
            {
                tmp = FileNameList.at(i);
                Full_Path_File = Full_Path + "/" + tmp;
                QFileInfo fileinfo_tmp(Full_Path_File);
                if(fileinfo_tmp.isFile())
                {
                    if(QFile::exists(Full_Path_File))FileList_Add(tmp, Full_Path_File);
                }
                else
                {
                    //if(QFile::exists(Full_Path_File))
                    if(file_isDirExist(Full_Path_File))Add_File_Folder_IncludeSubFolder(Full_Path_File);
                }
            }
        }
    }
}
/*
Read file names in folder (including subfolders
*/
QStringList MainWindow::getFileNames_IncludeSubFolder(QString path)
{
    QDir dir(path);
    QStringList files = dir.entryList(QDir::Dirs | QDir::Files | QDir::Writable, QDir::Name);
    QFileSystemWatcher watcher;
    watcher.addPath(path);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(100);
    QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged, [&](){
        files = dir.entryList(QDir::Dirs | QDir::Files | QDir::Writable, QDir::Name);
        timer.start();
    });
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
    files.removeAll("..");
    files.removeAll(".");
    return files;
}
/*
Scan file name list in folder (no filter
*/
QStringList MainWindow::file_getFileNames_in_Folder_nofilter(QString path)
{
    return fileManager.getFileNamesNoFilter(path);
}

/*
Add files to file list and table
*/
int MainWindow::FileList_Add(QString fileName, QString SourceFile_fullPath)
{
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix().toLower();
    //============================  Determine if it is an image ===============================
    QString Ext_image_str = ui->Ext_image->text().toLower();
    QStringList nameFilters_image = Ext_image_str.split(":");
    nameFilters_image.removeAll("gif");
    nameFilters_image.removeAll("apng");
    if (nameFilters_image.contains(file_ext))
    {
        AddNew_image=true;
        int rowNum = Table_image_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        if(!Deduplicate_filelist(SourceFile_fullPath))
        {
            mutex_Table_insert_finished.lock();
            Table_insert_finished=false;
            mutex_Table_insert_finished.unlock();
            emit Send_Table_image_insert_fileName_fullPath(fileName, SourceFile_fullPath);
            while(!Table_insert_finished)
            {
                Delay_msec_sleep(10);
            }
        }
        return 0;
    }
    //============================  Determine if it is a video ===============================
    QString Ext_video_str = ui->Ext_video->text().toLower();
    QStringList nameFilters_video = Ext_video_str.split(":");
    nameFilters_video.removeAll("gif");
    nameFilters_video.removeAll("apng");
    if (nameFilters_video.contains(file_ext))
    {
        AddNew_video=true;
        int rowNum = Table_video_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        if(!Deduplicate_filelist(SourceFile_fullPath))
        {
            mutex_Table_insert_finished.lock();
            Table_insert_finished=false;
            mutex_Table_insert_finished.unlock();
            emit Send_Table_video_insert_fileName_fullPath(fileName, SourceFile_fullPath);
            while(!Table_insert_finished)
            {
                Delay_msec_sleep(10);
            }
        }
        return 0;
    }
    //============================  Finally, it can only be gif & apng ===============================
    if(file_ext=="gif" || file_ext=="apng")
    {
        int rowNum = Table_gif_get_rowNum();
        QMap<QString, QString> map;
        map["SourceFile_fullPath"] = SourceFile_fullPath;
        map["rowNum"] = QString::number(rowNum, 10);
        AddNew_gif=true;
        if(!Deduplicate_filelist(SourceFile_fullPath))
        {
            mutex_Table_insert_finished.lock();
            Table_insert_finished=false;
            mutex_Table_insert_finished.unlock();
            emit Send_Table_gif_insert_fileName_fullPath(fileName, SourceFile_fullPath);
            while(!Table_insert_finished)
            {
                Delay_msec_sleep(10);
            }
        }
        return 0;
    }
    return 0;
}
/*
Determine if it is already in the file list
*/
bool MainWindow::Deduplicate_filelist(QString SourceFile_fullPath)
{
    for ( int i = 0; i < Table_model_image->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_image->item(i,2)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    for ( int i = 0; i < Table_model_gif->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_gif->item(i,2)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    for ( int i = 0; i < Table_model_video->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_video->item(i,2)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    return false;
}
/*
Does the folder exist
*/
bool MainWindow::file_isDirExist(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath.trimmed());
    return dir.exists();
}
/*
Create folder
*/
void MainWindow::file_mkDir(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath);
    if(dir.exists() == false)
    {
        dir.mkdir(SourceFile_fullPath);
    }
}
/*
Is the folder empty
*/
bool MainWindow::file_isDirEmpty(QString FolderPath)
{
    QDir dir(FolderPath);
    return dir.isEmpty();
}

/*
Delete folder
*/
bool MainWindow::file_DelDir(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }
    QDir dir(path);
    if(!dir.exists())
    {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //Set filter
    QFileInfoList fileList = dir.entryInfoList(); // Get all file information
    foreach (QFileInfo file, fileList)  //Traverse file information
    {
        if (file.isFile())  // Is a file, delete
        {
            file.dir().remove(file.fileName());
        }
        else   // Recursively delete
        {
            file_DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // Delete folder
}
/*
Overridden get basename function
*/
QString MainWindow::file_getBaseName(QString path)
{
    QFileInfo fileinfo(path);
    QString file_fullname = fileinfo.fileName();
    QStringList parts = file_fullname.split(".");
    QString file_basename="";
    for(int i=0; i<(parts.size()-1); i++)
    {
        file_basename+=parts.at(i);
        file_basename+=".";
    }
    if(file_basename.right(1)==".")
    {
        file_basename = file_basename.left(file_basename.length() - 1);
    }
    return file_basename;
}
/*
Move file to recycle bin
*/
void MainWindow::file_MoveToTrash( QString file )
{
#ifdef Q_OS_WIN
    QFileInfo fileinfo( file );
    if( !fileinfo.exists() )
        return;
    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));
    int l = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= l && l < MAX_PATH );
    from[ l ] = '\0';
    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation( &fileop );
    if( 0 != rv )
    {
        // SHFileOperation failed, could log an error or try another method
        // For now, just returning, but ideally, indicate failure.
        qWarning() << "SHFileOperation failed for" << file << "Error code:" << rv;
        // Fallback or alternative: QFile::remove(file);
        // However, that doesn't move to trash.
        return;
    }
#else
    // Use Qt's cross-platform way to move to trash for non-Windows
    if (!QFile::moveToTrash(file)) {
        qWarning() << "QFile::moveToTrash failed for" << file << "- attempting direct removal.";
        // Fallback: If moveToTrash fails or is not supported for some reason,
        // you might just remove the file directly, though this isn't "trash".
        // QFile::remove(file);
    }
#endif
}

/*
Get folder path (remove trailing "/")
*/
QString MainWindow::file_getFolderPath(QFileInfo fileInfo)
{
    QString folder_path = fileInfo.path();
    if(folder_path.right(1)=="/")
    {
        folder_path = folder_path.left(folder_path.length() - 1);
    }
    return folder_path;
}
/*
Check if folder is writable
*/
bool MainWindow::file_isDirWritable(QString DirPath)
{
    if(DirPath.right(1)=="/")
    {
        DirPath = DirPath.left(DirPath.length() - 1);
    }
    QString TestTemp = DirPath+"/RWTest_W2xEX.tmp";
    QFile file_TestTemp(TestTemp);
    file_TestTemp.remove();
    if (file_TestTemp.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite supports read and write
    {
        QTextStream stream(&file_TestTemp);
        stream << "W2xEX";
    }
    if(QFile::exists(TestTemp))
    {
        file_TestTemp.remove();
        return true;
    }
    else
    {
        file_TestTemp.remove();
        return false;
    }
}
/*
Determine if the folder where the currently processed file is located can be written
*/
bool MainWindow::file_isFilesFolderWritable_row_image(int rowNum)
{
    QString SourceFile_fullPath = Table_model_image->item(rowNum,2)->text();
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_FolderPath = file_getFolderPath(fileinfo);
    if(file_isDirWritable(file_FolderPath))
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Insufficient permissions, administrator permissions is needed.]"));
        QString status = "Failed";
        emit Send_Table_image_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        return false;
    }
}

bool MainWindow::file_isFilesFolderWritable_row_gif(int rowNum)
{
    QString SourceFile_fullPath = Table_model_gif->item(rowNum,2)->text();
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_FolderPath = file_getFolderPath(fileinfo);
    if(file_isDirWritable(file_FolderPath))
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Insufficient permissions, administrator permissions is needed.]"));
        QString status = "Failed";
        emit Send_Table_gif_ChangeStatus_rowNumInt_statusQString(rowNum, status);
        return false;
    }
}

bool MainWindow::file_isFilesFolderWritable_row_video(int rowNum)
{
    QString SourceFile_fullPath = Table_model_video->item(rowNum,2)->text();
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_FolderPath = file_getFolderPath(fileinfo);
    if(file_isDirWritable(file_FolderPath))
    {
        return true;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Error occured when processing [")+SourceFile_fullPath+tr("]. Error: [Insufficient permissions, administrator permissions is needed.]"));
        emit Send_Table_video_ChangeStatus_rowNumInt_statusQString(rowNum, "Failed");
        return false;
    }
}

bool MainWindow::file_OpenFolder(QString FolderPath)
{
    if(file_isDirExist(FolderPath))
    {
        FolderPath= FolderPath.replace("/","\\");
        QProcess::execute("explorer \""+FolderPath+"\"");
        return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow::file_OpenFilesFolder(QString FilePath)
{
    QFileInfo finfo = QFileInfo(FilePath);
    return file_OpenFolder(file_getFolderPath(finfo));
}

bool MainWindow::file_OpenFile(QString FilePath)
{
    if(QFile::exists(FilePath))
    {
        if(QDesktopServices::openUrl(QUrl("file:"+QUrl::toPercentEncoding(FilePath),QUrl::TolerantMode))==false)
        {
            ExecuteCMD_batFile("start \"\" \""+FilePath.replace("%","%%")+"\"",false);
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool MainWindow::file_generateMarkFile(QString FileFullPath,QString Msg)
{
    QFile file(FileFullPath);
    file.remove();
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite supports read and write
    {
        QTextStream stream(&file);
        if(Msg.trimmed() == "")
        {
            stream << "Beya_Waifu\nDo NOT delete this file!!";
        }
        else
        {
            stream << Msg;
        }
    }
    return file.exists();
}


