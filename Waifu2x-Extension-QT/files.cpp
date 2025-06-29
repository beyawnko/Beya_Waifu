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
#include <QDesktopServices> // Added for QDesktopServices
#include <QUrl> // Added for QUrl

// Encapsulates enabling/disabling UI elements during drag-and-drop
void MainWindow::setUiEnabledState(bool enabled)
{
    ui->groupBox_Setting->setEnabled(enabled);
    ui->groupBox_FileList->setEnabled(enabled);
    ui->groupBox_InputExt->setEnabled(enabled);
    pushButton_Start_setEnabled_self(enabled);
    ui->checkBox_ScanSubFolders->setEnabled(enabled);
    this->setAcceptDrops(enabled);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // If it is a file, drag and drop is supported
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

/*
 * Drag and drop file event
 */
void MainWindow::dropEvent(QDropEvent *event)
{
    // Reset the judgment flag of whether a new file of a certain type has been added
    AddNew_gif = false;
    AddNew_image = false;
    AddNew_video = false;
    // ================
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    // ================== UI control ========================
    ui_tableViews_setUpdatesEnabled(false);
    setUiEnabledState(false);
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));
    // ===================================================
    // Prepare lists of existing paths in the main thread (as dropEvent is a MainWindow method)
    QStringList existingImagePaths_copy = this->getImageFullPaths();
    QStringList existingGifPaths_copy;
    if (this->Table_model_gif) { // 'this' refers to MainWindow instance
        for (int i = 0; i < this->Table_model_gif->rowCount(); ++i) {
            if (this->Table_model_gif->item(i, 1))
                existingGifPaths_copy.append(this->Table_model_gif->item(i, 1)->text());
        }
    }
    QStringList existingVideoPaths_copy;
    if (this->Table_model_video) {
        for (int i = 0; i < this->Table_model_video->rowCount(); ++i) {
             if (this->Table_model_video->item(i, 1))
                existingVideoPaths_copy.append(this->Table_model_video->item(i, 1)->text());
        }
    }

    // Convert to QSet for efficient lookup in worker thread (capture by copy)
    QSet<QString> existingImagePaths_set;
    for (const QString& path : existingImagePaths_copy) {
        existingImagePaths_set.insert(path);
    }
    QSet<QString> existingGifPaths_set;
    for (const QString& path : existingGifPaths_copy) {
        existingGifPaths_set.insert(path);
    }
    QSet<QString> existingVideoPaths_set;
    for (const QString& path : existingVideoPaths_copy) {
        existingVideoPaths_set.insert(path);
    }

    // Get UI-dependent values in the main thread before starting the worker
    const bool scanSubfolders = ui->checkBox_ScanSubFolders->isChecked();
    const QString imageExts = ui->Ext_image->text();
    const QString videoExts = ui->Ext_video->text();

    QFuture<ReadUrlsResult> future = QtConcurrent::run([this, urls, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set, scanSubfolders, imageExts, videoExts]() {
        return this->Read_urls_Worker(urls, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set, scanSubfolders, imageExts, videoExts);
    });

    future.then(this, [this](ReadUrlsResult result){
        // This continuation runs on the main thread (due to `this` context for .then)
        // Call the original slot that updates the tables.
        // The slot Batch_Table_Update_slots is already designed to be called from the main thread.
        this->Batch_Table_Update_slots(result.imagesToAdd, result.gifsToAdd, result.videosToAdd,
                                       result.newImageAdded, result.newGifAdded, result.newVideoAdded);

        // Re-enable UI elements that were disabled at the start of dropEvent
        setUiEnabledState(true);

        // Optionally, emit a signal or log that file adding is complete
        emit Send_TextBrowser_NewMessage(tr("Finished adding files."));
    });
}

/*
 * Read urls (this is the implementation for the signature in mainwindow.h called by QtConcurrent::run)
 */
ReadUrlsResult MainWindow::Read_urls_Worker(QList<QUrl> urls,
                                           const QSet<QString>& existingImagePaths_set,
                                           const QSet<QString>& existingGifPaths_set,
                                           const QSet<QString>& existingVideoPaths_set,
                                           bool scanSubfolders,
                                           const QString& imageExts,
                                           const QString& videoExts)
{
    ReadUrlsResult result; // Initialize the result struct

    // Note: existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set are now passed in directly.
    const QString imageExtsLower = imageExts.toLower();
    const QString videoExtsLower = videoExts.toLower();

    // Emit progress signals directly from the worker thread
    // Ensure these signals are connected via Qt::QueuedConnection or safe for direct call
    QMetaObject::invokeMethod(this, "Send_PrograssBar_Range_min_max_slots", Qt::QueuedConnection,
                              Q_ARG(int, 0), Q_ARG(int, urls.size()));


    if (scanSubfolders)
    {
        for (const QUrl &url : urls) // Use const ref
        {
            Add_File_Folder_IncludeSubFolder(url.toLocalFile(),
                                             imageExtsLower, videoExtsLower,
                                             result.imagesToAdd, result.gifsToAdd, result.videosToAdd,
                                             result.newImageAdded, result.newGifAdded, result.newVideoAdded,
                                             existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
             QMetaObject::invokeMethod(this, "Send_progressbar_Add_slots", Qt::QueuedConnection); // Progress for each URL scanned
        }
    }
    else
    {
        for (const QUrl &url : urls) // Use const ref
        {
            Add_File_Folder(url.toLocalFile(),
                            imageExtsLower, videoExtsLower,
                            result.imagesToAdd, result.gifsToAdd, result.videosToAdd,
                            result.newImageAdded, result.newGifAdded, result.newVideoAdded,
                            existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
            QMetaObject::invokeMethod(this, "Send_progressbar_Add_slots", Qt::QueuedConnection); // Progress for each URL scanned
        }
    }

    // The data is returned in the 'result' object.
    return result;
}

/*
 * Read urls
 * Aftermath/Cleanup
 */
// void MainWindow::Read_urls_finfished() -> Definition is now a stub in mainwindow.cpp


/*
 * Add files & folders
 */
void MainWindow::Add_File_Folder(QString Full_Path,
                                 const QString& imageExts, const QString& videoExts,
                                 QList<FileLoadInfo>& imagesToAdd,
                                 QList<FileLoadInfo>& gifsToAdd,
                                 QList<FileLoadInfo>& videosToAdd,
                                 bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo,
                                 const QSet<QString>& existingImagePaths_set,
                                 const QSet<QString>& existingGifPaths_set,
                                 const QSet<QString>& existingVideoPaths_set)
{
    QFileInfo fileinfo(Full_Path);
    if (fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path, imageExts, videoExts, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
    }
    else
    {
        QStringList FileNameList = file_getFileNames_in_Folder_nofilter(Full_Path); // Read legal file names
        QString Full_Path_File = "";
        if (!FileNameList.isEmpty())
        {
            for (const QString &tmp : FileNameList) // Use const ref and range-based for
            {
                Full_Path_File = Full_Path + "/" + tmp;
                FileList_Add(tmp, Full_Path_File, imageExts, videoExts, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
            }
        }
    }
}

/*
 * Add files & folders
 * Scan subfolders
 */
void MainWindow::Add_File_Folder_IncludeSubFolder(QString Full_Path,
                                                  const QString& imageExts, const QString& videoExts,
                                                  QList<FileLoadInfo>& imagesToAdd,
                                                  QList<FileLoadInfo>& gifsToAdd,
                                                  QList<FileLoadInfo>& videosToAdd,
                                                  bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo,
                                                  const QSet<QString>& existingImagePaths_set,
                                                  const QSet<QString>& existingGifPaths_set,
                                                  const QSet<QString>& existingVideoPaths_set)
{
    QFileInfo fileinfo(Full_Path);
    if (fileinfo.isFile())
    {
        QString file_name = fileinfo.fileName();
        FileList_Add(file_name, Full_Path, imageExts, videoExts, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
        return;
    }

    const QStringList filePaths = getFileNames_IncludeSubFolder(Full_Path);
    for (const QString &path : filePaths)
    {
        QFileInfo info(path);
        if (info.isFile() && QFile::exists(path)) // Ensure it's an existing file
        {
            FileList_Add(info.fileName(), path, imageExts, videoExts, imagesToAdd, gifsToAdd, videosToAdd, localAddNewImage, localAddNewGif, localAddNewVideo, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set);
        }
    }
}

/*
 * Read file names in folder (including subfolders)
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
 * Scan file name list in folder (no filter)
 */
QStringList MainWindow::file_getFileNames_in_Folder_nofilter(QString path)
{
    return fileManager.getFileNamesNoFilter(path);
}

/*
 * Add files to file list and table
 */
int MainWindow::FileList_Add(QString fileName, QString SourceFile_fullPath,
                             const QString& imageExts, const QString& videoExts,
                             QList<FileLoadInfo>& imagesToAdd,
                             QList<FileLoadInfo>& gifsToAdd,
                             QList<FileLoadInfo>& videosToAdd,
                             bool& localAddNewImage, bool& localAddNewGif, bool& localAddNewVideo,
                             const QSet<QString>& existingImagePaths_set,
                             const QSet<QString>& existingGifPaths_set,
                             const QSet<QString>& existingVideoPaths_set)
{
    QFileInfo fileinfo(SourceFile_fullPath);
    QString file_ext = fileinfo.suffix().toLower();

    // Deduplication using the passed-in sets
    if (Deduplicate_filelist_worker(SourceFile_fullPath, existingImagePaths_set, existingGifPaths_set, existingVideoPaths_set)) {
        return 0; // Already exists
    }

    FileLoadInfo info;
    info.fileName = fileName;
    info.fullPath = SourceFile_fullPath;
    info.status = "Waiting";
    info.customResolutionWidth = "";
    info.customResolutionHeight = "";

    // ============================  Determine if it is an image ===============================
    QStringList nameFilters_image = imageExts.split(":");
    nameFilters_image.removeAll("gif");
    nameFilters_image.removeAll("apng");
    if (nameFilters_image.contains(file_ext))
    {
        imagesToAdd.append(info);
        localAddNewImage = true;
        return 0;
    }
    // ============================  Determine if it is a video ===============================
    QStringList nameFilters_video = videoExts.split(":");
    nameFilters_video.removeAll("gif");
    nameFilters_video.removeAll("apng");
    if (nameFilters_video.contains(file_ext))
    {
        videosToAdd.append(info);
        localAddNewVideo = true;
        return 0;
    }
    // ============================  Finally, it can only be gif & apng ===============================
    if (file_ext == "gif" || file_ext == "apng")
    {
        gifsToAdd.append(info);
        localAddNewGif = true;
        return 0;
    }
    return 0;
}

/*
 * Determine if it is already in the file list (Worker thread version)
 */
bool MainWindow::Deduplicate_filelist_worker(const QString& SourceFile_fullPath,
                                             const QSet<QString>& existingImagePaths_set,
                                             const QSet<QString>& existingGifPaths_set,
                                             const QSet<QString>& existingVideoPaths_set)
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
 * Does the folder exist
 */
bool MainWindow::file_isDirExist(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath.trimmed());
    return dir.exists();
}

/*
 * Create folder
 */
void MainWindow::file_mkDir(QString SourceFile_fullPath)
{
    QDir dir(SourceFile_fullPath);
    if(!dir.exists())
    {
        dir.mkdir(SourceFile_fullPath);
    }
}

/*
 * Is the folder empty
 */
bool MainWindow::file_isDirEmpty(QString FolderPath)
{
    QDir dir(FolderPath);
    return dir.isEmpty();
}

/*
 * Delete folder
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
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); // Set filter
    QFileInfoList fileList = dir.entryInfoList(); // Get all file information
    foreach (QFileInfo file, fileList)  // Traverse file information
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
 * Get base name using QFileInfo
 */
QString MainWindow::file_getBaseName(QString path)
{
    QFileInfo fileinfo(path);
    return fileinfo.baseName();
}

/*
 * Move file to recycle bin
 */
void MainWindow::file_MoveToTrash( QString file )
{
    if (!QFile::moveToTrash(file)) {
        qWarning() << "QFile::moveToTrash failed for" << file;
        // Optionally emit an error message to the UI
        emit Send_TextBrowser_NewMessage(tr("Failed to move file to trash: ") + file);
    }
}

/*
 * Get folder path (remove trailing "/")
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
 * Check if folder is writable
 */
bool MainWindow::file_isDirWritable(QString DirPath)
{
    QDir dir(DirPath.trimmed());
    if (!dir.exists()) {
        return false;
    }

    QString testFilePath = dir.filePath("w2xex_write_test.tmp");
    QFile testFile(testFilePath);

    if (testFile.open(QIODevice::WriteOnly)) {
        testFile.close();
        testFile.remove();
        return true;
    }

    return false;
}

/*
 * Determine if the folder where the currently processed file is located can be written
 */
bool MainWindow::file_isFilesFolderWritable_row_image(int rowNum)
{
    // Standardized column index to 1 for full path
    QString SourceFile_fullPath = Table_model_image->item(rowNum,1)->text();
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
    // Standardized column index to 1 for full path
    QString SourceFile_fullPath = Table_model_gif->item(rowNum,1)->text();
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
    // Standardized column index to 1 for full path
    QString SourceFile_fullPath = Table_model_video->item(rowNum,1)->text();
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
        // Use QDesktopServices::openUrl for cross-platform compatibility
        bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(FolderPath));
        if (!success) {
            qWarning() << "Failed to open folder:" << FolderPath;
            // Optionally emit an error message to the UI
            emit Send_TextBrowser_NewMessage(tr("Failed to open folder: ") + FolderPath);
        }
        return success;
    }
    else
    {
        qWarning() << "Folder does not exist:" << FolderPath;
        // Optionally emit an error message to the UI
        emit Send_TextBrowser_NewMessage(tr("Folder does not exist: ") + FolderPath);
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
        // Rely solely on QDesktopServices::openUrl
        bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(FilePath));
        if (!success) {
             qWarning() << "Failed to open file:" << FilePath;
             // Optionally emit an error message to the UI
             emit Send_TextBrowser_NewMessage(tr("Failed to open file: ") + FilePath);
        }
        return success;
    }
    else
    {
        qWarning() << "File does not exist:" << FilePath;
        // Optionally emit an error message to the UI
        emit Send_TextBrowser_NewMessage(tr("File does not exist: ") + FilePath);
        return false;
    }
}

bool MainWindow::file_generateMarkFile(QString FileFullPath,QString Msg)
{
    QFile file(FileFullPath);
    file.remove();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) // Use WriteOnly as we are overwriting
    {
        QTextStream stream(&file);
        if(Msg.trimmed().isEmpty()) // Use isEmpty() for clarity
        {
            stream << "Beya_Waifu\nDo NOT delete this file!!";
        }
        else
        {
            stream << Msg;
        }
        file.close(); // Close the file after writing
        return true; // File was opened and written to
    }
    else
    {
        qWarning() << "Failed to create mark file:" << FileFullPath << file.errorString();
        // Optionally emit an error message to the UI
        emit Send_TextBrowser_NewMessage(tr("Failed to create mark file: ") + FileFullPath + tr(" Error: ") + file.errorString());
        return false; // File could not be opened/written
    }
}