// file: realesrganprocessor.cpp
#include "realesrganprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

RealEsrganProcessor::RealEsrganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    cleanup();

    connect(m_process, &QProcess::errorOccurred, this, &RealEsrganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealEsrganProcessor::onReadyReadStandardOutput);
}

RealEsrganProcessor::~RealEsrganProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void RealEsrganProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_scaleSequence.clear();

    if (!m_currentPassInputFile.isEmpty() && m_currentPassInputFile != m_originalSourceFile) {
        QFile::remove(m_currentPassInputFile);
    }

    m_originalSourceFile.clear();
    m_finalDestinationFile.clear();
    m_currentPassInputFile.clear();
    m_currentPassOutputFile.clear();
}

void RealEsrganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage("[ERROR] RealEsrganProcessor is already busy.");
        return;
    }

    m_currentRowNum = rowNum;
    m_settings = settings;
    m_originalSourceFile = sourceFile;
    m_finalDestinationFile = destinationFile;
    m_currentPassInputFile = sourceFile;

    // --- Determine scaling sequence ---
    int remainingScale = m_settings.targetScale;
    int nativeScale = m_settings.modelNativeScale > 1 ? m_settings.modelNativeScale : 2; // Safeguard
    while (remainingScale >= nativeScale) {
        m_scaleSequence.enqueue(nativeScale);
        remainingScale /= nativeScale;
    }
    if (m_scaleSequence.isEmpty()) m_scaleSequence.enqueue(m_settings.targetScale);

    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("Real-ESRGAN: Starting job for %1...").arg(QFileInfo(sourceFile).fileName()));
    startNextPass();
}

void RealEsrganProcessor::startNextPass()
{
    if (m_scaleSequence.isEmpty()) {
        onProcessFinished(0, QProcess::NormalExit); // Should trigger final cleanup and signals
        return;
    }

    m_scaleSequence.dequeue(); // Dequeue the scale for the *current* pass

    // Determine output file for this pass
    // If this is the last pass, output to final destination. Otherwise, create a temporary file.
    m_currentPassOutputFile = m_scaleSequence.isEmpty()
        ? m_finalDestinationFile
        : QDir(QFileInfo(m_finalDestinationFile).path()).filePath(QString("%1_pass_tmp.%2")
          .arg(QFileInfo(m_finalDestinationFile).completeBaseName())
          .arg(m_settings.outputFormat));

    emit logMessage(QString("Real-ESRGAN: Starting scaling pass... Input: %1, Output: %2").arg(m_currentPassInputFile).arg(m_currentPassOutputFile));
    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile);

    // Ensure program path and arguments are logged for debugging
    // qDebug() << "Starting Real-ESRGAN process:";
    // qDebug() << "Program:" << m_settings.programPath;
    // qDebug() << "Arguments:" << arguments.join(" ");

    m_process->start(m_settings.programPath, arguments);
}

void RealEsrganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    // Log raw output for debugging if not NormalExit or exitCode != 0
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stdErr = m_process->readAllStandardError();
        QString stdOut = m_process->readAllStandardOutput(); // It might have been read partially by onReadyReadStandardOutput
        emit logMessage(QString("Real-ESRGAN Pass STDOUT:\n%1").arg(stdOut));
        emit logMessage(QString("Real-ESRGAN Pass STDERR:\n%1").arg(stdErr));
        emit logMessage(QString("Real-ESRGAN: A pass failed for %1. Exit code: %2, Exit status: %3")
                        .arg(QFileInfo(m_originalSourceFile).fileName())
                        .arg(exitCode)
                        .arg(exitStatus));
        emit statusChanged(m_currentRowNum, tr("Error"));
        emit processingFinished(m_currentRowNum, false);

        // If a pass fails, remove its output if it's a temp file
        if (m_currentPassOutputFile != m_finalDestinationFile) {
            QFile::remove(m_currentPassOutputFile);
        }
        cleanup(); // Full cleanup
        return;
    }

    // Successful pass
    // Delete previous pass's input if it was a temporary file
    if (m_currentPassInputFile != m_originalSourceFile) {
        QFile::remove(m_currentPassInputFile);
    }

    m_currentPassInputFile = m_currentPassOutputFile; // Output of this pass is input for the next

    if (m_scaleSequence.isEmpty()) { // All passes completed successfully
        emit logMessage(QString("Real-ESRGAN: Successfully processed %1. Output: %2")
                        .arg(QFileInfo(m_originalSourceFile).fileName())
                        .arg(m_finalDestinationFile));
        emit statusChanged(m_currentRowNum, tr("Finished"));
        emit fileProgress(m_currentRowNum, 100); // Ensure progress is 100%
        emit processingFinished(m_currentRowNum, true);
        cleanup(); // Final cleanup (removes last temp input if it was one, though it shouldn't be if logic is correct)
    } else {
        // More passes to go
        emit logMessage(QString("Real-ESRGAN: Pass completed for %1. Next pass...").arg(QFileInfo(m_originalSourceFile).fileName()));
        startNextPass();
    }
}

void RealEsrganProcessor::onProcessError(QProcess::ProcessError error)
{
    // This slot is for errors that prevent the process from starting, like "command not found".
    // Runtime errors of the started process are handled in onProcessFinished.
    emit logMessage(QString("[FATAL] Real-ESRGAN process failed to start for %1. Error: %2 (Code: %3)")
                    .arg(QFileInfo(m_originalSourceFile).fileName())
                    .arg(m_process->errorString())
                    .arg(error));
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void RealEsrganProcessor::onReadyReadStandardOutput()
{
    QString output = m_process->readAllStandardOutput();
    // emit logMessage(QString("Real-ESRGAN STDOUT chunk: %1").arg(output)); // Optional: for debugging live output

    // Real-ESRGAN-NCNN-Vulkan progress is typically like "  0%  |..." or "100%  |..."
    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatchIterator i = re.globalMatch(output);
    int lastProgress = -1;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        lastProgress = match.captured(1).toInt();
    }

    if (lastProgress != -1) {
        emit fileProgress(m_currentRowNum, lastProgress);
        // emit logMessage(QString("Real-ESRGAN Progress: %1%").arg(lastProgress)); // For debugging progress parsing
    }
}

QStringList RealEsrganProcessor::buildArguments(const QString &inputFile, const QString &outputFile)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);
    arguments << "-s" << QString::number(m_settings.modelNativeScale); // Use the model's native scale for this pass
    arguments << "-m" << m_settings.modelName;
    arguments << "-t" << QString::number(m_settings.tileSize);
    arguments << "-f" << m_settings.outputFormat;

    if (!m_settings.singleGpuId.trimmed().isEmpty() && m_settings.singleGpuId.toLower() != "auto") {
        arguments << "-g" << m_settings.singleGpuId;
    }

    if (m_settings.ttaEnabled) {
        arguments << "-x";
    }

    return arguments;
}
