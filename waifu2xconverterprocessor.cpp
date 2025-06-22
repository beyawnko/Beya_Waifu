// file: waifu2xconverterprocessor.cpp
#include "waifu2xconverterprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

Waifu2xConverterProcessor::Waifu2xConverterProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    cleanup();

    connect(m_process, &QProcess::errorOccurred, this, &Waifu2xConverterProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Waifu2xConverterProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Waifu2xConverterProcessor::onReadyReadStandardOutput);
}

Waifu2xConverterProcessor::~Waifu2xConverterProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void Waifu2xConverterProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_destinationFile.clear();
}

void Waifu2xConverterProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Waifu2xConverterSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage("[ERROR] Waifu2xConverterProcessor is already busy.");
        return;
    }

    m_currentRowNum = rowNum;
    m_destinationFile = destinationFile;
    m_settings = settings;

    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("Waifu2x-Converter: Starting job for %1...").arg(QFileInfo(sourceFile).fileName()));

    QStringList arguments = buildArguments(sourceFile, destinationFile);

    emit logMessage(QString("Waifu2x-Converter command: \"%1\" %2").arg(settings.programPath).arg(arguments.join(" ")));
    m_process->start(settings.programPath, arguments);
}

void Waifu2xConverterProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QFileInfo checkFile(m_destinationFile);
    bool success = false;

    if (exitStatus == QProcess::NormalExit && exitCode == 0 && checkFile.exists() && checkFile.size() > 0) {
        success = true;
        emit logMessage(QString("Waifu2x-Converter: Successfully processed file."));
        emit statusChanged(m_currentRowNum, tr("Finished"));
    } else {
        success = false;
        QString errorOutput = m_process->readAllStandardError();
        emit logMessage(QString("Waifu2x-Converter: Error processing file. Exit code: %1").arg(exitCode));
        if (!errorOutput.isEmpty()) emit logMessage("Waifu2x-Converter stderr: " + errorOutput.trimmed());
        emit statusChanged(m_currentRowNum, tr("Error"));
    }

    emit processingFinished(m_currentRowNum, success);
    cleanup();
}

void Waifu2xConverterProcessor::onProcessError(QProcess::ProcessError)
{
    emit logMessage(QString("[FATAL] Waifu2x-Converter process failed to start. Error: %1").arg(m_process->errorString()));
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void Waifu2xConverterProcessor::onReadyReadStandardOutput()
{
    // Waifu2x-Converter does not provide percentage progress, so this can remain empty
    // or be used to log the raw output if needed for debugging.
}

QStringList Waifu2xConverterProcessor::buildArguments(const QString &sourceFile, const QString &destinationFile)
{
    QStringList arguments;
    arguments << "--input_path" << QDir::toNativeSeparators(sourceFile);
    arguments << "--output_path" << QDir::toNativeSeparators(destinationFile);
    arguments << "--noise_level" << QString::number(m_settings.denoiseLevel);
    arguments << "--scale_ratio" << QString::number(m_settings.targetScale);
    arguments << "--block_size" << QString::number(m_settings.blockSize);
    arguments << "--processor" << m_settings.targetProcessor;

    if (m_settings.ttaEnabled) arguments << "-t";
    if (m_settings.forceOpenCL) arguments << "--force_ocl";
    if (m_settings.disableGpu) arguments << "--disable_gpu";

    arguments << "--tta_mode" << "none"; // Assuming no TTA mode selection in UI for now
    arguments << "--mode" << "noise_scale"; // Default mode

    return arguments;
}
