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

// file: anime4kprocessor.cpp
#include "anime4kprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug> // For qInfo, qWarning, and tr (though tr might need QCoreApplication for non-widgets)
#include <QCoreApplication> // For tr, if this class doesn't inherit from QWidget

Anime4KProcessor::Anime4KProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(); // No parent
    m_currentRowNum = -1;

    // Connect the process signals to our new slots
    connect(m_process, &QProcess::errorOccurred, this, &Anime4KProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Anime4KProcessor::onProcessFinished);
}

Anime4KProcessor::~Anime4KProcessor()
{
    // Clean up the process if it's running
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000); // Wait a bit for it to die
    }
    // m_process is a child of this QObject, so it will be deleted automatically.
}

QStringList Anime4KProcessor::buildArguments(const QString &inputFile, const QString &outputFile, const Anime4kSettings &settings)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

    // Logic ported and corrected from the original MainWindow::Anime4k_ReadSettings
    // and user's latest instructions for Anime4KProcessor
    if (settings.preserveAlpha) arguments << "-k"; // Use -k for alpha preservation as per docs

    if (settings.preProcessing) {
        arguments << "-b"; // Enable pre-processing block
        if (!settings.preFilters.isEmpty()) {
            arguments << "-L" << settings.preFilters;
        }
    }
    if (settings.postProcessing) {
        // Anime4KCPP v2.x uses -P for post-processing filters.
        // The -a flag is separate for enabling the post-processing block if preserveAlpha isn't already enabling it.
        // However, the common way is just to specify post-filters with -P.
        // Let's assume if postProcessing is true, we enable it and add filters.
        // The flag to enable post-processing itself seems to be just providing filters with -P.
        if (!settings.postFilters.isEmpty()) {
             arguments << "-P" << settings.postFilters;
        }
    }

    if (settings.acNet) {
        arguments << "-A"; // Enable ACNet
        if (settings.hdnMode) arguments << "-H"; // Enable HDN mode (implies ACNet)
    }

    if (settings.fastMode) arguments << "-F";

    arguments << "-p" << QString::number(settings.passes);
    arguments << "-c" << QString::number(settings.pushColorCount);
    arguments << "-s" << QString::number(settings.pushColorStrength);
    arguments << "-g" << QString::number(settings.pushGradientStrength);

    if (settings.gpuMode) {
        arguments << "-q"; // Enable GPU acceleration
        arguments << "-Y" << settings.gpgpuModel; // Specify GPGPU model (OpenCL/CUDA)
        if (settings.specifyGpu && !settings.gpuString.isEmpty()) {
            arguments << "-D" << settings.gpuString; // Specify PlatformID and DeviceID
        }
        arguments << "-C" << QString::number(settings.commandQueues); // Number of command queues
        if (settings.parallelIo) {
            arguments << "-I"; // Enable OpenCL parallel IO command queue
        }
    }
    return arguments;
}

void Anime4KProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Anime4kSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage(tr("[ERROR] Anime4KProcessor is already busy. Task for row %1 (%2) cannot be started.").arg(rowNum).arg(QFileInfo(sourceFile).fileName()));
        // Optionally emit processingFinished(rowNum, false) or let MainWindow handle timeout/retry for busy state
        return;
    }

    m_currentRowNum = rowNum;
    // If destinationFile is empty, create one in a temp location or based on source.
    // For now, assuming a valid destinationFile is constructed by the caller or this needs more logic.
    // The user prompt for MainWindow suggests Generate_Output_Path. If destFile is empty, we might use that.
    if (destinationFile.isEmpty()) {
        QFileInfo sourceInfo(sourceFile);
        QString tempPath = QDir::tempPath() + "/" + sourceInfo.fileName() + "_anime4k." + sourceInfo.suffix();
        m_destinationFile = tempPath;
        emit logMessage(QString("Anime4K: Destination file empty, using temp: %1").arg(m_destinationFile));
    } else {
        m_destinationFile = destinationFile;
    }


    emit statusChanged(rowNum, tr("Processing..."));
    emit logMessage(QString("Anime4K: Starting processing for %1").arg(QFileInfo(sourceFile).fileName()));

    QStringList arguments = buildArguments(sourceFile, m_destinationFile, settings);

    emit logMessage(QString("Anime4K command: \"%1\" %2").arg(settings.programPath).arg(arguments.join(" ")));

    m_process->start(settings.programPath, arguments);
}

void Anime4KProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QFileInfo checkFile(m_destinationFile);
    bool success = false;

    if (exitStatus == QProcess::NormalExit && exitCode == 0 && checkFile.exists() && checkFile.size() > 0) {
        success = true;
        emit logMessage(QString("Anime4K: Successfully processed %1").arg(QFileInfo(m_destinationFile).fileName()));
        emit statusChanged(m_currentRowNum, tr("Finished"));
    } else {
        success = false;
        QString errorOutput = m_process->readAllStandardError();
        QString stdOutput = m_process->readAllStandardOutput(); // Sometimes errors go to stdout
        emit logMessage(QString("Anime4K: Error processing file for row %1. Source: %2, Dest: %3. Exit code: %4, Exit status: %5")
                        .arg(m_currentRowNum)
                        .arg(m_process->arguments().value(1)) // Attempt to get input file from args
                        .arg(m_destinationFile)
                        .arg(exitCode)
                        .arg(exitStatus));
        if (!errorOutput.isEmpty()) {
            emit logMessage("Anime4K stderr: " + errorOutput.trimmed());
        }
        if (!stdOutput.isEmpty()){
            emit logMessage("Anime4K stdout: " + stdOutput.trimmed());
        }
        emit statusChanged(m_currentRowNum, tr("Error"));
    }

    emit processingFinished(m_currentRowNum, success);

    m_currentRowNum = -1;
    m_destinationFile.clear();
}

void Anime4KProcessor::onProcessError(QProcess::ProcessError error)
{
    if (m_currentRowNum != -1) { // Ensure we have a context
        emit logMessage(QString("[FATAL] Anime4K process failed to start for row %1. Error: %2 (%3)")
                        .arg(m_currentRowNum)
                        .arg(m_process->errorString())
                        .arg(error));
        emit statusChanged(m_currentRowNum, tr("Fatal Error"));
        emit processingFinished(m_currentRowNum, false);

        m_currentRowNum = -1;
        m_destinationFile.clear();
    } else {
        // Error occurred outside of a specific job context
        emit logMessage(QString("[FATAL] Anime4K process encountered an error: %1 (%2)")
                        .arg(m_process->errorString())
                        .arg(error));
    }
}

void Anime4KProcessor::processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const Anime4kSettings &settings)
{
    Q_UNUSED(sourceFile);
    Q_UNUSED(destinationFile);
    Q_UNUSED(settings);
    emit logMessage(tr("[INFO] Anime4K video processing is not yet implemented in the async processor. Row: %1").arg(rowNum));
    emit statusChanged(rowNum, tr("Skipped (Video N/A)"));
    emit processingFinished(rowNum, false);
}
