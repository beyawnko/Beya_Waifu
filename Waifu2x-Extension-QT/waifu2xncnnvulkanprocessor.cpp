// file: waifu2xncnnvulkanprocessor.cpp
#include "waifu2xncnnvulkanprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

Waifu2xNcnnVulkanProcessor::Waifu2xNcnnVulkanProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    cleanup();

    connect(m_process, &QProcess::errorOccurred, this, &Waifu2xNcnnVulkanProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &Waifu2xNcnnVulkanProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Waifu2xNcnnVulkanProcessor::onReadyReadStandardOutput);
}

Waifu2xNcnnVulkanProcessor::~Waifu2xNcnnVulkanProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void Waifu2xNcnnVulkanProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_scaleSequence.clear();

    if (!m_currentPassInputFile.isEmpty() && m_currentPassInputFile != m_originalSourceFile && QFile::exists(m_currentPassInputFile)) {
        QFile::remove(m_currentPassInputFile);
    }

    m_originalSourceFile.clear();
    m_finalDestinationFile.clear();
    m_currentPassInputFile.clear();
    m_currentPassOutputFile.clear();
}

void Waifu2xNcnnVulkanProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Waifu2xNcnnVulkanSettings &settings)
{
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage("[ERROR] Waifu2xNcnnVulkanProcessor is already busy.");
        emit processingFinished(rowNum, false); // Notify failure if busy
        return;
    }

    m_currentRowNum = rowNum;
    m_settings = settings;
    m_originalSourceFile = sourceFile;
    m_finalDestinationFile = destinationFile;
    m_currentPassInputFile = sourceFile;
    m_scaleSequence.clear(); // Clear previous sequence

    // --- Determine scaling sequence (chaining 2x passes) ---
    int tempScale = m_settings.targetScale;
    if (tempScale < 1) tempScale = 1; // Ensure scale is at least 1

    if (tempScale == 1 && m_settings.denoiseLevel >= 0) { // Denoise only case
        m_scaleSequence.append(1);
    } else {
        while (tempScale > 1) {
            if (tempScale % 2 != 0) { // If scale is not a power of 2, it's tricky. Waifu2x NCNN only supports 2x directly.
                                      // For simplicity, we'll stick to powers of 2 or 1x.
                                      // Or, one could use 1x for denoise then a separate scaling tool, but that's out of scope here.
                emit logMessage(QString("[ERROR] Waifu2x-NCNN: Target scale %1 is not supported. Only powers of 2 or 1 (for denoise) are supported.").arg(m_settings.targetScale));
                emit statusChanged(m_currentRowNum, tr("Error: Invalid Scale"));
                emit processingFinished(m_currentRowNum, false);
                cleanup();
                return;
            }
            m_scaleSequence.append(2);
            tempScale /= 2;
        }
    }

    if (m_scaleSequence.isEmpty()) { // Should not happen if logic above is correct, but as a safeguard
        emit logMessage(QString("[ERROR] Waifu2x-NCNN: Could not determine scale sequence for target %1x.").arg(m_settings.targetScale));
        emit statusChanged(m_currentRowNum, tr("Error: Scale Logic"));
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }


    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("Waifu2x-NCNN: Starting job for %1. Target scale: %2x, Denoise: %3")
                    .arg(QFileInfo(sourceFile).fileName()).arg(m_settings.targetScale).arg(m_settings.denoiseLevel));
    startNextPass();
}

void Waifu2xNcnnVulkanProcessor::startNextPass()
{
    if (m_scaleSequence.isEmpty()) {
        // This means all passes are done, or it was a 1x denoise pass that completed.
        // The onProcessFinished should handle the final success.
        // If cleanup happened due to an error, this path shouldn't be hit with empty sequence.
        // However, if processImage determined no passes were needed (e.g. bad scale), it should have errored out there.
        // This might be redundant if onProcessFinished is robust.
        // For safety, if we reach here with an empty sequence and the process isn't running, assume completion.
        if(m_process->state() == QProcess::NotRunning) {
             // It's possible onProcessFinished was already called for the last pass.
             // Or, if it was a 1x process that didn't even start a QProcess (e.g. direct copy),
             // then this would be the place to emit success.
             // Given current logic, a QProcess always starts.
        }
        return;
    }

    int currentProcessingScaleFactor = m_scaleSequence.takeFirst(); // This is always 2, or 1 for denoise-only

    // Determine output path for this pass
    if (m_scaleSequence.isEmpty()) {
        // This is the last pass (or the only pass for 1x/2x)
        m_currentPassOutputFile = m_finalDestinationFile;
    } else {
        // This is an intermediate pass
        QFileInfo tempInfo(m_finalDestinationFile);
        // Create a unique name for intermediate files. Using a counter or more robust unique ID might be better.
        // For now, just appending "_tmp_pass"
        m_currentPassOutputFile = QDir(tempInfo.path()).filePath(QString("%1_tmp_pass.%2")
                                 .arg(tempInfo.completeBaseName())
                                 .arg(QFileInfo(m_originalSourceFile).suffix())); // Use original suffix for intermediate if possible
    }

    emit logMessage(QString("Waifu2x-NCNN: Starting pass (Actual scale factor for this pass: %1x)...").arg(currentProcessingScaleFactor));
    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile, currentProcessingScaleFactor);

    QFileInfo progInfo(m_settings.programPath);
    m_process->setWorkingDirectory(progInfo.absolutePath());
    m_process->start(m_settings.programPath, arguments);
}

void Waifu2xNcnnVulkanProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_currentRowNum == -1) return; // Cleanup already called, or job never started properly

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString("Waifu2x-NCNN: A pass failed. Exit code: %1, Status: %2").arg(exitCode).arg(exitStatus));
        QString stdErr = m_process->readAllStandardError();
        if(!stdErr.isEmpty()) emit logMessage(QString("Waifu2x-NCNN Stderr: %1").arg(stdErr));
        emit statusChanged(m_currentRowNum, tr("Error"));
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }

    // Clean up the input file of the completed pass if it was an intermediate file
    if (m_currentPassInputFile != m_originalSourceFile && QFile::exists(m_currentPassInputFile)) {
        QFile::remove(m_currentPassInputFile);
    }

    m_currentPassInputFile = m_currentPassOutputFile; // The output of this pass is the input for the next

    if (m_scaleSequence.isEmpty()) {
        // All passes are complete
        emit logMessage(QString("Waifu2x-NCNN: Successfully processed %1.").arg(QFileInfo(m_originalSourceFile).fileName()));
        emit statusChanged(m_currentRowNum, tr("Finished"));
        emit processingFinished(m_currentRowNum, true);
        cleanup(); // Final cleanup
    } else {
        // More passes to go
        startNextPass();
    }
}

void Waifu2xNcnnVulkanProcessor::onProcessError(QProcess::ProcessError error)
{
    if (m_currentRowNum == -1 && error != QProcess::UnknownError) return; // Avoid duplicate errors if cleanup already ran

    emit logMessage(QString("[FATAL] Waifu2x-NCNN process failed to start or crashed. Error: %1 (%2)")
                    .arg(m_process->errorString()).arg(error));
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void Waifu2xNcnnVulkanProcessor::onReadyReadStandardOutput()
{
    QString output = m_process->readAllStandardOutput();
    // Waifu2x-ncnn-vulkan output is typically like "xx.xx% xx/xx" or just "xx.xx%"
    // For simplicity, let's try to catch the percentage.
    QRegularExpression re("(\\d+\\.?\\d*)\\s*%");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        emit fileProgress(m_currentRowNum, static_cast<int>(match.captured(1).toDouble()));
    }
    // emit logMessage(output.trimmed()); // Optionally log all stdout
}

QStringList Waifu2xNcnnVulkanProcessor::buildArguments(const QString &inputFile, const QString &outputFile, int currentPassScaleFactor)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

    // For waifu2x-ncnn-vulkan, -s is the scale factor, which is always 2 for iterative scaling, or 1 for denoise-only.
    arguments << "-s" << QString::number(currentPassScaleFactor);

    arguments << "-n" << QString::number(m_settings.denoiseLevel);

    if(m_settings.tileSize > 0) { // Tile size 0 means auto
        arguments << "-t" << QString::number(m_settings.tileSize);
    }

    if (m_settings.ttaEnabled) {
        arguments << "-x";
    }

    // Determine model path based on style
    // Model name in settings is "upconv_7" or "cunet"
    // Image style is "2D Anime" or "3D Real-life"
    QString modelDir;
    if (m_settings.model == "cunet") {
        modelDir = "models-cunet";
    } else if (m_settings.model == "upconv_7") {
        if (m_settings.imageStyle == "2D Anime") {
            modelDir = "models-upconv_7_anime_style_art_rgb";
        } else { // "3D Real-life"
            modelDir = "models-upconv_7_photo";
        }
    } else { // Fallback or if model string is different
        modelDir = "models-cunet"; // Default to cunet
        emit logMessage(QString("[WARNING] Waifu2x-NCNN: Unknown model '%1', defaulting to cunet.").arg(m_settings.model));
    }
    arguments << "-m" << modelDir;

    // GPU Settings
    if (m_settings.multiGpuEnabled && !m_settings.multiGpuConfig.isEmpty()) {
        QStringList jobArgs;
        for(const auto& gpuJob : m_settings.multiGpuConfig){
            QString gpuId = gpuJob.value("id", "0").split(":").first(); // Extract numeric ID part
            jobArgs.append(gpuId);
        }
        arguments << "-g" << jobArgs.join(","); // Example: -g 0,1,0 for 3 jobs on GPU0 and GPU1
    } else {
        QString gpuId = m_settings.singleGpuId.split(":").first(); // Extract numeric ID part
        arguments << "-g" << gpuId;
    }

    // arguments << "-v"; // Verbose output, useful for debugging, can be conditional

    return arguments;
}
