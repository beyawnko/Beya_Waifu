/*
    Copyright (C) 2025  beyawnko
*/
#pragma once

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QMutex>

/**
 * @brief Handles common file management operations.
 */
class FileManager
{
public:
    FileManager() = default;

    bool isDirExist(const QString &path);
    void makeDir(const QString &path);
    bool isDirEmpty(const QString &path);
    QStringList getFileNamesNoFilter(const QString &path);
    bool deleteDir(const QString &path);
    QString getBaseName(const QString &path);
    /**
     * @brief Move the specified file to the system trash.
     * @param file Absolute file path to remove.
     * @return True if the file was successfully moved.
     */
    bool moveToTrash(const QString &file);
    QString getFolderPath(const QFileInfo &fileInfo);
    bool isDirWritable(const QString &dirPath);
    bool openFolder(const QString &folderPath);
    bool openFilesFolder(const QString &filePath);

    /**
     * @brief Open the given file using the system's default application.
     * @param filePath Path to the file.
     * @return True if the launch command was issued.
     */
    bool openFile(const QString &filePath);
    bool generateMarkFile(const QString &fileFullPath, const QString &msg);

private:
    QMutex moveFileMutex;
};

