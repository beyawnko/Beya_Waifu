/*
    Copyright (C) 2025  beyawnko
*/
#include "FileManager.h"
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <windows.h>
#include <shellapi.h>

bool FileManager::isDirExist(const QString &path)
{
    QDir dir(path.trimmed());
    return dir.exists();
}

void FileManager::makeDir(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkdir(path);
    }
}

bool FileManager::isDirEmpty(const QString &path)
{
    QDir dir(path);
    return dir.isEmpty();
}

QStringList FileManager::getFileNamesNoFilter(const QString &path)
{
    QDir dir(path);
    QStringList files = dir.entryList(QDir::Files | QDir::Writable, QDir::Name);
    QFileSystemWatcher watcher;
    watcher.addPath(path);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(100);
    QObject::connect(&watcher, &QFileSystemWatcher::directoryChanged, [&]() {
        files = dir.entryList(QDir::Files | QDir::Writable, QDir::Name);
        timer.start();
    });
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
    files.removeAll("..");
    files.removeAll(".");
    return files;
}

bool FileManager::deleteDir(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }
    QDir dir(path);
    if (!dir.exists())
    {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo file, fileList)
    {
        if (file.isFile())
        {
            file.dir().remove(file.fileName());
        }
        else
        {
            deleteDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

QString FileManager::getBaseName(const QString &path)
{
    QFileInfo fileinfo(path);
    QString fileFullname = fileinfo.fileName();
    QStringList parts = fileFullname.split('.');
    QString fileBasename;
    for (int i = 0; i < parts.size() - 1; i++)
    {
        fileBasename += parts.at(i);
        fileBasename += '.';
    }
    if (fileBasename.endsWith('.'))
    {
        fileBasename.chop(1);
    }
    return fileBasename;
}

void FileManager::moveToTrash(const QString &file)
{
    QFileInfo fileinfo(file);
    if (!fileinfo.exists())
        return;
    WCHAR from[MAX_PATH];
    memset(from, 0, sizeof(from));
    int l = fileinfo.absoluteFilePath().toWCharArray(from);
    Q_ASSERT(0 <= l && l < MAX_PATH);
    from[l] = '\0';
    SHFILEOPSTRUCT fileop;
    memset(&fileop, 0, sizeof(fileop));
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    SHFileOperation(&fileop);
}

QString FileManager::getFolderPath(const QFileInfo &fileInfo)
{
    QString folderPath = fileInfo.path();
    if (folderPath.endsWith('/'))
    {
        folderPath.chop(1);
    }
    return folderPath;
}

bool FileManager::isDirWritable(const QString &dirPath)
{
    QString sanitized = dirPath;
    if (sanitized.endsWith('/'))
    {
        sanitized.chop(1);
    }
    QString testPath = sanitized + "/RWTest_W2xEX.tmp";
    QFile fileTest(testPath);
    fileTest.remove();
    if (fileTest.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&fileTest);
        stream << "W2xEX";
    }
    bool ok = QFile::exists(testPath);
    fileTest.remove();
    return ok;
}

bool FileManager::openFolder(const QString &folderPath)
{
    if (isDirExist(folderPath))
    {
        QString path = folderPath;
        path.replace('/', "\\");
        QProcess::execute("explorer \"" + path + "\"");
        return true;
    }
    return false;
}

bool FileManager::openFilesFolder(const QString &filePath)
{
    QFileInfo finfo(filePath);
    return openFolder(getFolderPath(finfo));
}

bool FileManager::openFile(const QString &filePath)
{
    if (QFile::exists(filePath))
    {
        if (!QDesktopServices::openUrl(QUrl("file:" + QUrl::toPercentEncoding(filePath), QUrl::TolerantMode)))
        {
            QProcess::execute("start \"\" \"" + filePath.replace('%', "%%") + "\"");
        }
        return true;
    }
    return false;
}

bool FileManager::generateMarkFile(const QString &fileFullPath, const QString &msg)
{
    QFile file(fileFullPath);
    file.remove();
    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        if (msg.trimmed().isEmpty())
        {
            stream << "Beya_Waifu\nDo NOT delete this file!!";
        }
        else
        {
            stream << msg;
        }
    }
    return file.exists();
}

