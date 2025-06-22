// file: srmdprocessor.cpp
#include "srmdprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

SrmdProcessor::SrmdProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    cleanup();

    connect(m_process, &QProcess::errorOccurred, this, &SrmdProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &SrmdProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &SrmdProcessor::onReadyReadStandardOutput);
}

SrmdProcessor::~SrmdProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void SrmdProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_destinationFile.clear();
}

void SrmdProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const SrmdSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage("[ERROR] SrmdProcessor is already busy.");
        return;
    }

    m_currentRowNum = rowNum;
    m_destinationFile = destinationFile;
    m_settings = settings;
    m_settings.programPath = QDir::toNativeSeparators(sourceFile); // Use sourceFile as input path for buildArguments

    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("SRMD: Starting job for %1...").arg(QFileInfo(sourceFile).fileName()));

    QStringList arguments = buildArguments();

    emit logMessage(QString("SRMD command: \"%1\" %2").arg(settings.programPath).arg(arguments.join(" ")));
    m_process->start(settings.programPath, arguments);
}

void SrmdProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QFileInfo checkFile(m_destinationFile);
    bool success = false;

    if (exitStatus == QProcess::NormalExit && exitCode == 0 && checkFile.exists() && checkFile.size() > 0) {
        success = true;
        emit logMessage(QString("SRMD: Successfully processed file."));
        emit statusChanged(m_currentRowNum, tr("Finished"));
    } else {
        success = false;
        QString errorOutput = m_process->readAllStandardError();
        emit logMessage(QString("SRMD: Error processing file. Exit code: %1").arg(exitCode));
        if (!errorOutput.isEmpty()) {
            emit logMessage("SRMD stderr: " + errorOutput.trimmed());
        }
        emit statusChanged(m_currentRowNum, tr("Error"));
    }

    emit processingFinished(m_currentRowNum, success);
    cleanup();
}

void SrmdProcessor::onProcessError(QProcess::ProcessError)
{
    emit logMessage(QString("[FATAL] SRMD process failed to start. Error: %1").arg(m_process->errorString()));
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void SrmdProcessor::onReadyReadStandardOutput()
{
    QString output = m_process->readAllStandardOutput();
    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        emit fileProgress(m_currentRowNum, match.captured(1).toInt());
    }
}

QStringList SrmdProcessor::buildArguments()
{
    QStringList arguments;
    arguments << "-i" << m_settings.programPath; // Input file was stored here
    arguments << "-o" << QDir::toNativeSeparators(m_destinationFile);
    arguments << "-s" << QString::number(m_settings.targetScale);
    arguments << "-n" << QString::number(m_settings.denoiseLevel);
    arguments << "-t" << QString::number(m_settings.tileSize);
    arguments << "-f" << m_settings.outputFormat;
    arguments << "-g" << m_settings.singleGpuId;

    if (m_settings.ttaEnabled) arguments << "-x";

    return arguments;
}
