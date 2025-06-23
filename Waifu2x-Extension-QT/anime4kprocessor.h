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

// file: anime4kprocessor.h
#ifndef ANIME4KPROCESSOR_H
#define ANIME4KPROCESSOR_H

#include <QObject>
#include <QProcess> // <-- Add include
#include "anime4k_settings.h"

class Anime4KProcessor : public QObject
{
    Q_OBJECT
public:
    explicit Anime4KProcessor(QObject *parent = nullptr);
    ~Anime4KProcessor(); // Add destructor

    // Public processing methods
    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Anime4kSettings &settings);
    void processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const Anime4kSettings &settings);

signals:
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);

private slots: // <-- Add private slots
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QStringList buildArguments(const QString &inputFile, const QString &outputFile, const Anime4kSettings &settings);

    QProcess *m_process; // <-- Process is now a member pointer
    // --- Member variables to store context for the async operation ---
    int m_currentRowNum;
    QString m_destinationFile;
};

#endif // ANIME4KPROCESSOR_H
