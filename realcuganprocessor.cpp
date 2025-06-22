// file: realcuganprocessor.cpp
#include "realcuganprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

RealCuganProcessor::RealCuganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    cleanup(); // Initialize state

    connect(m_process, &QProcess::errorOccurred, this, &RealCuganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealCuganProcessor::onReadyReadStandardOutput);
}

RealCuganProcessor::~RealCuganProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void RealCuganProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_targetScale = 1;
    m_currentScale = 1;
    m_scaleSequence.clear();

    // Delete intermediate file if it exists
    if (!m_currentPassInputFile.isEmpty() && m_currentPassInputFile != m_originalSourceFile) {
        QFile::remove(m_currentPassInputFile);
    }

    m_originalSourceFile.clear();
    m_finalDestinationFile.clear();
    m_currentPassInputFile.clear();
    m_currentPassOutputFile.clear();
}

void RealCuganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealCuganSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage("[ERROR] RealCuganProcessor is already busy.");
        return;
    }

    // --- Initialize State for the New Job ---
    m_currentRowNum = rowNum;
    m_settings = settings;
    m_originalSourceFile = sourceFile;
    m_finalDestinationFile = destinationFile;
    m_currentPassInputFile = sourceFile;
    m_targetScale = m_settings.targetScale;
    m_currentScale = 1;

    // --- Determine the sequence of scaling passes needed ---
    // RealCUGAN models are typically 2x, 3x, or 4x. We will chain 2x passes for >4x scales.
    int remainingScale = m_targetScale;
    while (remainingScale > 1) {
        if (remainingScale >= 4) { m_scaleSequence.append(4); remainingScale /= 4; }
        else if (remainingScale >= 3) { m_scaleSequence.append(3); remainingScale /= 3; }
        else if (remainingScale >= 2) { m_scaleSequence.append(2); remainingScale /= 2; }
        else { break; } // Should not happen with integer scales
    }
    if (m_scaleSequence.isEmpty()) m_scaleSequence.append(m_targetScale); // Handle 1x case

    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("RealCUGAN: Starting job for %1. Target scale: %2x")
                    .arg(QFileInfo(sourceFile).fileName()).arg(m_targetScale));

    startNextPass();
}

void RealCuganProcessor::startNextPass()
{
    if (m_scaleSequence.isEmpty()) {
        // This should not happen if logic is correct, but as a safeguard:
        onProcessFinished(0, QProcess::NormalExit);
        return;
    }

    int passScale = m_scaleSequence.takeFirst();
    m_currentScale *= passScale;

    // Determine output path for this pass
    if (m_scaleSequence.isEmpty()) {
        // This is the last pass, output to final destination
        m_currentPassOutputFile = m_finalDestinationFile;
    } else {
        // This is an intermediate pass
        QFileInfo tempInfo(m_finalDestinationFile);
        m_currentPassOutputFile = QDir(tempInfo.path()).filePath(QString("%1_pass_%2x.%3")
                                 .arg(tempInfo.completeBaseName()).arg(m_currentScale).arg(tempInfo.suffix()));
    }

    emit logMessage(QString("RealCUGAN: Starting pass (Scale %1x)...").arg(passScale));
    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile, passScale);
    m_process->start(m_settings.programPath, arguments);
}

void RealCuganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString("RealCUGAN: A pass failed. Exit code: %1").arg(exitCode));
        emit statusChanged(m_currentRowNum, tr("Error"));
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }

    // Clean up the input file of the completed pass if it was an intermediate file
    if (m_currentPassInputFile != m_originalSourceFile) {
        QFile::remove(m_currentPassInputFile);
    }

    m_currentPassInputFile = m_currentPassOutputFile; // The output of this pass is the input for the next

    if (m_scaleSequence.isEmpty()) {
        // All passes are complete
        emit logMessage(QString("RealCUGAN: Successfully processed %1.").arg(QFileInfo(m_originalSourceFile).fileName()));
        emit statusChanged(m_currentRowNum, tr("Finished"));
        emit processingFinished(m_currentRowNum, true);
        cleanup();
    } else {
        // More passes to go
        startNextPass();
    }
}

void RealCuganProcessor::onProcessError(QProcess::ProcessError error)
{
    emit logMessage(QString("[FATAL] RealCUGAN process failed to start. Error: %1").arg(m_process->errorString()));
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void RealCuganProcessor::onReadyReadStandardOutput()
{
    QString output = m_process->readAllStandardOutput();
    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        emit fileProgress(m_currentRowNum, match.captured(1).toInt());
    }
}

QStringList RealCuganProcessor::buildArguments(const QString &inputFile, const QString &outputFile, int currentPassScale)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);
    arguments << "-s" << QString::number(currentPassScale);
    arguments << "-n" << QString::number(m_settings.denoiseLevel);
    arguments << "-m" << m_settings.modelName;
    arguments << "-t" << QString::number(m_settings.tileSize);
    arguments << "-f" << m_settings.outputFormat;

    if (m_settings.ttaEnabled) arguments << "-x";

    // Handle GPU configuration
    if (m_settings.multiGpuEnabled) {
         // Build the complex job string if needed, or pass pre-formatted
         // For simplicity, we assume MainWindow builds the string.
         // arguments << "-j" << build_job_string(m_settings.multiGpuConfig)
    } else {
        arguments << "-g" << m_settings.singleGpuId;
    }

    return arguments;
}

void RealCuganProcessor::processVideo(int rowNum)
{
    emit logMessage("[INFO] RealCUGAN video processing has not been refactored yet.");
    emit statusChanged(rowNum, tr("Skipped (Not Implemented)"));
    emit processingFinished(rowNum, false);
}
