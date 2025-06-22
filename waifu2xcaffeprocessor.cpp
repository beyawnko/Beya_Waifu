// file: waifu2xcaffeprocessor.cpp
#include "waifu2xcaffeprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>

Waifu2xCaffeProcessor::Waifu2xCaffeProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    cleanup();

    connect(m_process, &QProcess::errorOccurred, this, &Waifu2xCaffeProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Waifu2xCaffeProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Waifu2xCaffeProcessor::onReadyReadStandardOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &Waifu2xCaffeProcessor::onReadyReadStandardError);
}

Waifu2xCaffeProcessor::~Waifu2xCaffeProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void Waifu2xCaffeProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_destinationFile.clear();
}

void Waifu2xCaffeProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Waifu2xCaffeSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage("[ERROR] Waifu2xCaffeProcessor is already busy.");
        return;
    }

    m_currentRowNum = rowNum;
    m_destinationFile = destinationFile;
    m_settings = settings;

    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("Waifu2x-Caffe: Starting job for %1...").arg(QFileInfo(sourceFile).fileName()));

    QStringList arguments = buildArguments(sourceFile, destinationFile);

    emit logMessage(QString("Waifu2x-Caffe command: \"%1\" %2").arg(m_settings.programPath).arg(arguments.join(" ")));
    m_process->start(m_settings.programPath, arguments);
}

QStringList Waifu2xCaffeProcessor::buildArguments(const QString &sourceFile, const QString &destinationFile)
{
    QStringList arguments;
    arguments << "-m" << "noise_scale";
    arguments << "-i" << QDir::toNativeSeparators(sourceFile);
    arguments << "-o" << QDir::toNativeSeparators(destinationFile);
    arguments << "-p" << m_settings.processMode.toLower();
    arguments << "--model_dir" << QDir::toNativeSeparators(m_settings.model);
    arguments << "--batch_size" << QString::number(m_settings.batchSize);
    arguments << "--crop_size" << QString::number(m_settings.splitSize);

    if (m_settings.ttaEnabled) arguments << "--tta" << "1";

    if (m_settings.multiGpuEnabled) {
        arguments << "--gpu" << m_settings.multiGpuConfig;
    } else {
        arguments << "-g" << QString::number(m_settings.gpuId);
    }

    return arguments;
}

void Waifu2xCaffeProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QFileInfo checkFile(m_destinationFile);
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0 && checkFile.exists() && checkFile.size() > 0);

    if (success) {
        emit logMessage(QString("Waifu2x-Caffe: Successfully processed file."));
        emit statusChanged(m_currentRowNum, tr("Finished"));
    } else {
        emit logMessage(QString("Waifu2x-Caffe: Error processing file. Exit code: %1").arg(exitCode));
        emit statusChanged(m_currentRowNum, tr("Error"));
    }

    emit processingFinished(m_currentRowNum, success);
    cleanup();
}

void Waifu2xCaffeProcessor::onProcessError(QProcess::ProcessError)
{
    emit logMessage(QString("[FATAL] Waifu2x-Caffe process failed to start. Error: %1").arg(m_process->errorString()));
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void Waifu2xCaffeProcessor::onReadyReadStandardOutput() { /* Optional: Log stdout if needed for debugging */ }
void Waifu2xCaffeProcessor::onReadyReadStandardError() {
    QString errorOutput = m_process->readAllStandardError();
    if (!errorOutput.trimmed().isEmpty()) {
        emit logMessage("Waifu2x-Caffe stderr: " + errorOutput.trimmed());
    }
}
