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
#include <QDirIterator>

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
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));
    //===================================================
    // Prepare lists of existing paths in the main thread (as dropEvent is a MainWindow method)
    QStringList existingImagePaths_copy = this->getImageFullPaths();
    QStringList existingGifPaths_copy;
    if (this->Table_model_gif) { // 'this' refers to MainWindow instance
        for (int i = 0; i < this->Table_model_gif->rowCount(); ++i) {
            if(this->Table_model_gif->item(i, 1))
                existingGifPaths_copy.append(this->Table_model_gif->item(i, 1)->text());
        }
    }
    QStringList existingVideoPaths_copy;
    if (this->Table_model_video) {
        for (int i = 0; i < this->Table_model_video->rowCount(); ++i) {
             if(this->Table_model_video->item(i, 1))
                existingVideoPaths_copy.append(this->Table_model_video->item(i, 1)->text());
        }
    }

    // Convert to QSet for efficient lookup in worker thread (capture by copy)
    QSet<QString> existingImagePaths_set = QSet<QString>::fromList(existingImagePaths_copy);
    QSet<QString> existingGifPaths_set = QSet<QString>::fromList(existingGifPaths_copy);
    QSet<QString> existingVideoPaths_set = QSet<QString>::fromList(existingVideoPaths_copy);

    (void)QtConcurrent::run([this, urls, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set]() {
        this->Read_urls(urls, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
    });
}
/*
Read urls (this is the implementation for the signature in mainwindow.h called by QtConcurrent::run)
*/
void MainWindow::Read_urls(QList<QUrl> urls,
                           const QSet<QString>& existingImagePaths_set,
                           const QSet<QString>& existingGifPaths_set,
                           const QSet<QString>& existingVideoPaths_set)
{
    // Data collection lists
    QList<QPair<QString, QString>> imagesToAdd;
    QList<QPair<QString, QString>> gifsToAdd;
    QList<QPair<QString, QString>> videosToAdd;
    bool localAddNewImage = false;
    bool localAddNewGif = false;
    bool localAddNewVideo = false;

    // Note: existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set are now passed in directly.

    emit Send_PrograssBar_Range_min_max_slots(0, urls.size()); // For scanning progress

    if(ui->checkBox_ScanSubFolders->isChecked())
    {
        for(const QUrl &url : urls) // Use const ref
        {
            Add_File_Folder_IncludeSubFolder(url.toLocalFile(),
                                             imagesToAdd, gifsToAdd, videosToAdd,
                                             localAddNewImage, localAddNewGif, localAddNewVideo,
                                             existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
            emit Send_progressbar_Add_slots(); // Progress for each URL scanned
        }
    }
    else
    {
        for(const QUrl &url : urls) // Use const ref
        {
            Add_File_Folder(url.toLocalFile(),
                            imagesToAdd, gifsToAdd, videosToAdd,
                            localAddNewImage, localAddNewGif, localAddNewVideo,
                            existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
            emit Send_progressbar_Add_slots(); // Progress for each URL scanned
        }
    }

    // After processing all URLs, emit one signal to update the tables in the main thread.
    emit Send_Batch_Table_Update(imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo);
}

/*
Read urls
Aftermath/Cleanup
*/
void MainWindow::Read_urls_finfished()
{
    // This function's primary responsibilities (re-enabling UI, showing messages, updating tables)
    // have been moved to Batch_Table_Update_slots, which is called in the main thread
    // after the worker thread (Read_urls) finishes collecting all file information.
    // The progress bar reset for the "loading files" phase is also handled there.
    // Global AddNew_image flags are also handled by the new slot based on info from worker.
    qDebug() << "Read_urls_finfished() called - its responsibilities are now in Batch_Table_Update_slots.";
}


/*
Add files & folders
*/
void MainWindow::Add_File_Folder(QString Full_Path,
                                 QList<QPair<QString, QString>>& imagesToAdd,
                                 QList<QPair<QString, QString>>& gifsToAdd,
                                 QList<QPair<QString, QString>>& videosToAdd,
                                 bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo,
                                 const QSet<QString>& existingImagePaths_set, // Changed to _set
                                 const QSet<QString>& existingGifPaths_set,   // Changed to _set
                                 const QSet<QString>& existingVideoPaths_set) // Changed to _set
{
    QFileInfo fileinfo(Full_Path);
    if(fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
    }
    else
    {
        QStringList FileNameList = file_getFileNames_in_Folder_nofilter(Full_Path); //Read legal file names
        QString Full_Path_File = "";
        if(!FileNameList.isEmpty())
        {
            for(const QString &tmp : FileNameList) // Use const ref and range-based for
            {
                Full_Path_File = Full_Path + "/" + tmp;
                FileList_Add(tmp, Full_Path_File, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
            }
        }
    }
}
/*
Add files & folders
Scan subfolders
*/
void MainWindow::Add_File_Folder_IncludeSubFolder(QString Full_Path,
                                                  QList<QPair<QString, QString>>& imagesToAdd,
                                                  QList<QPair<QString, QString>>& gifsToAdd,
                                                  QList<QPair<QString, QString>>& videosToAdd,
                                                  bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo,
                                                  const QSet<QString>& existingImagePaths_set, // Changed to _set
                                                  const QSet<QString>& existingGifPaths_set,   // Changed to _set
                                                  const QSet<QString>& existingVideoPaths_set) // Changed to _set
{
    QFileInfo fileinfo(Full_Path);
    if(fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
        return;
    }

    const QStringList filePaths = getFileNames_IncludeSubFolder(Full_Path);
    for (const QString &path : filePaths)
    {
        QFileInfo info(path);
        if (info.isFile() && QFile::exists(path)) // Ensure it's an existing file
        {
            FileList_Add(info.fileName(), path, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
        }
    }
}
/*
Read file names in folder (including subfolders
*/
QStringList MainWindow::getFileNames_IncludeSubFolder(QString path)
{
    QStringList results;
    QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        results << it.next();
    }
    return results;
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
// Modified FileList_Add to populate lists instead of emitting signals directly
int MainWindow::FileList_Add(QString fileName, QString SourceFile_fullPath,
                             QList<QPair<QString, QString>>& imagesToAdd,
                             QList<QPair<QString, QString>>& gifsToAdd,
                             QList<QPair<QString, QString>>& videosToAdd,
                             bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo,
                             const QSet<QString>& existingImagePaths_set, // Changed to _set
                             const QSet<QString>& existingGifPaths_set,   // Changed to _set
                             const QSet<QString>& existingVideoPaths_set) // Changed to _set
{
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix().toLower();

    // Deduplication using the passed-in sets
    if (Deduplicate_filelist_worker(SourceFile_fullPath, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set)) { // Use _set
        return 0; // Already exists in one of the lists (implicitly, as sets are based on current table content)
    }

    //============================  Determine if it is an image ===============================
    QString Ext_image_str = ui->Ext_image->text().toLower(); // Assuming ui elements are accessible if 'this' is MainWindow
    QStringList nameFilters_image = Ext_image_str.split(":");
    nameFilters_image.removeAll("gif");
    nameFilters_image.removeAll("apng");
    if (nameFilters_image.contains(file_ext))
    {
        imagesToAdd.append(qMakePair(fileName, SourceFile_fullPath));
        localAddNewImage = true;
        return 0;
    }
    //============================  Determine if it is a video ===============================
    QString Ext_video_str = ui->Ext_video->text().toLower();
    QStringList nameFilters_video = Ext_video_str.split(":");
    nameFilters_video.removeAll("gif");
    nameFilters_video.removeAll("apng");
    if (nameFilters_video.contains(file_ext))
    {
        videosToAdd.append(qMakePair(fileName, SourceFile_fullPath));
        localAddNewVideo = true;
        return 0;
    }
    //============================  Finally, it can only be gif & apng ===============================
    if(file_ext=="gif" || file_ext=="apng")
    {
        gifsToAdd.append(qMakePair(fileName, SourceFile_fullPath));
        localAddNewGif = true;
        return 0;
    }
    return 0;
}

/*
Determine if it is already in the file list (Worker thread version)
*/
bool MainWindow::Deduplicate_filelist_worker(const QString& SourceFile_fullPath,
                                             const QSet<QString>& existingImagePaths_set, // Changed to _set
                                             const QSet<QString>& existingGifPaths_set,   // Changed to _set
                                             const QSet<QString>& existingVideoPaths_set) // Changed to _set
{
    // This function is called from the worker thread.
    // It checks against copies of paths taken at the start of Read_urls.
    if (existingImagePaths_set.contains(SourceFile_fullPath) ||
        existingGifPaths_set.contains(SourceFile_fullPath) ||
        existingVideoPaths_set.contains(SourceFile_fullPath)) {
        return true;
    }
    return false;
}

// Original Deduplicate_filelist is kept for now if called from elsewhere,
// but drag-and-drop should use Deduplicate_filelist_worker.
bool MainWindow::Deduplicate_filelist(QString SourceFile_fullPath)
{
    // This version accesses Table_model directly, should only be called from main thread.
    // Kept for compatibility if other parts of the code use it.
    // The drag-and-drop path will use Deduplicate_filelist_worker.
    for ( int i = 0; i < Table_model_image->rowCount(); i++ )
    {
        // Assuming full path is in column 1 now based on Batch_Table_Update_slots and getter analysis
        QString fullpath_readRow =  Table_model_image->item(i,1)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    for ( int i = 0; i < Table_model_gif->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_gif->item(i,1)->text();
        if(fullpath_readRow == SourceFile_fullPath)
        {
            return true;
        }
    }
    for ( int i = 0; i < Table_model_video->rowCount(); i++ )
    {
        QString fullpath_readRow =  Table_model_video->item(i,1)->text();
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


