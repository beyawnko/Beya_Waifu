/*
    FileManager.cpp - utilities for handling filesystem operations
    across platforms using Qt.

    Copyright (C) 2025  beyawnko
*/
#include "FileManager.h"
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QFileSystemWatcher>
#include <QEventLoop>
#include <QTimer>
#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#endif

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

bool FileManager::moveToTrash(const QString &file)
{
#ifdef Q_OS_WIN
    // This function is Windows-specific.
    QFileInfo fileinfo(file);
    if (!fileinfo.exists())
        return false;
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
    return SHFileOperation(&fileop) == 0;
#else
    // On non-Windows systems, this function does nothing.
    qWarning("file_MoveToTrash is only implemented on Windows.");
    QFileInfo fileInfo(file);
    if (!fileInfo.exists())
        return false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (QDesktopServices::moveToTrash(QUrl::fromLocalFile(fileInfo.absoluteFilePath())))
        return true;
#elif QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    if (QFile::moveToTrash(fileInfo.absoluteFilePath()))
        return true;
#endif
    QString absPath = fileInfo.absoluteFilePath();
    if (QProcess::startDetached("gio", QStringList() << "trash" << absPath))
        return true;
    return QProcess::startDetached("xdg-trash", QStringList() << absPath);
#endif
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
        QString path = QDir::toNativeSeparators(folderPath);
#ifdef Q_OS_WIN
        if (!QProcess::startDetached("explorer", QStringList() << path))
            qWarning() << "Failed to open" << path;
#else
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
        {
            if (!QProcess::startDetached("xdg-open", QStringList() << path))
                qWarning() << "Failed to open" << path;
        }
#endif
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
        QString path = QDir::toNativeSeparators(filePath);
#ifdef Q_OS_WIN
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
        {
            if (!QProcess::startDetached("cmd",
                                       QStringList() << "/c" << "start" << path))
                qWarning() << "Failed to open" << path;
        }
#else
        if (!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))
        {
            if (!QProcess::startDetached("xdg-open", QStringList() << path))
                qWarning() << "Failed to open" << path;
        }
#endif
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

