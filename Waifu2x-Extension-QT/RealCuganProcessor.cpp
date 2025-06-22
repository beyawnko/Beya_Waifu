// file: realcuganprocessor.cpp
#include "realcuganprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

RealCuganProcessor::RealCuganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    connect(m_process, &QProcess::errorOccurred, this, &RealCuganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealCuganProcessor::onReadyReadStandardOutput);

    m_ffmpegProcess = new QProcess(this);
    connect(m_ffmpegProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onFfmpegError);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onFfmpegFinished);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onFfmpegStdErr);

    cleanup();
}

RealCuganProcessor::~RealCuganProcessor()
{
    // Destructor logic to ensure processes are killed
}

void RealCuganProcessor::cleanup()
{
    m_state = State::Idle;
    m_currentRowNum = -1;
    m_finalDestinationFile.clear();
}

void RealCuganProcessor::cleanupVideo()
{
    if (!m_video_tempPath.isEmpty()) {
        QDir(m_video_tempPath).removeRecursively();
        m_video_tempPath.clear();
    }
    m_video_frameQueue.clear();
    m_video_totalFrames = 0;
    m_video_processedFrames = 0;
}

// --- IMAGE PROCESSING ---
void RealCuganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealCuganSettings &settings)
{
    if (m_state != State::Idle) {
        emit logMessage("[ERROR] RealCuganProcessor is already busy.");
        return;
    }
    m_state = State::ProcessingImage;
    m_currentRowNum = rowNum;
    m_settings = settings;
    m_finalDestinationFile = destinationFile;

    emit statusChanged(m_currentRowNum, tr("Processing..."));
    emit logMessage(QString("RealCUGAN: Starting image job for %1...").arg(QFileInfo(sourceFile).fileName()));

    QStringList args = buildArguments(sourceFile, destinationFile);
    m_process->start(m_settings.programPath, args);
}

// --- VIDEO PROCESSING ---
void RealCuganProcessor::processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealCuganSettings &settings)
{
    if (m_state != State::Idle) {
        emit logMessage("[ERROR] RealCuganProcessor is already busy.");
        return;
    }

    m_state = State::SplittingVideo;
    m_currentRowNum = rowNum;
    m_settings = settings;
    m_finalDestinationFile = destinationFile;

    m_video_tempPath = QDir(QFileInfo(destinationFile).path()).filePath("temp_video_" + QFileInfo(sourceFile).completeBaseName());
    m_video_inputFramesPath = m_video_tempPath + "/input";
    m_video_outputFramesPath = m_video_tempPath + "/output";
    m_video_audioPath = m_video_tempPath + "/audio.m4a";
    QDir().mkpath(m_video_inputFramesPath);
    QDir().mkpath(m_video_outputFramesPath);

    emit statusChanged(rowNum, tr("Splitting video..."));
    emit logMessage("Splitting video into frames...");

    QStringList args;
    args << "-i" << sourceFile << "-q:a" << "0" << "-ac" << "2" << "-vn" << m_video_audioPath << "-vsync" << "0" << m_video_inputFramesPath + "/frame%08d.png";
    m_ffmpegProcess->start("ffmpeg", args);
}

// --- SLOTS AND HELPERS ---
void RealCuganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle) return;

    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);

    if (m_state == State::ProcessingImage) {
        if (!success) {
            emit logMessage(QString("RealCUGAN: Image processing failed. Exit code: %1").arg(exitCode));
        }
        emit processingFinished(m_currentRowNum, success);
        cleanup();
    } else if (m_state == State::ProcessingVideoFrames) {
        if (!success) {
            emit logMessage(QString("RealCUGAN: Failed to process frame. Aborting video task."));
            cleanupVideo();
            emit processingFinished(m_currentRowNum, false);
            cleanup();
            return;
        }

        m_video_processedFrames++;
        if (m_video_totalFrames > 0) {
            emit fileProgress(m_currentRowNum, (100 * m_video_processedFrames) / m_video_totalFrames);
        }

        if (m_video_frameQueue.isEmpty()) {
            startVideoAssembly();
        } else {
            startNextVideoFrame();
        }
    }
}

void RealCuganProcessor::onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString("FFmpeg task failed. State: %1, Exit Code: %2").arg((int)m_state).arg(exitCode));
        cleanupVideo();
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }

    if (m_state == State::SplittingVideo) {
        emit logMessage("Video splitting complete. Starting frame processing...");
        emit statusChanged(m_currentRowNum, tr("Processing frames..."));
        m_state = State::ProcessingVideoFrames;

        QDir inputDir(m_video_inputFramesPath);
        m_video_frameQueue = inputDir.entryList(QStringList() << "*.png", QDir::Files, QDir::Name);
        m_video_totalFrames = m_video_frameQueue.size();
        m_video_processedFrames = 0;

        if (m_video_totalFrames > 0) {
            startNextVideoFrame();
        } else {
            emit logMessage("No frames found after splitting. Aborting.");
            cleanupVideo();
            emit processingFinished(m_currentRowNum, false);
            cleanup();
        }
    } else if (m_state == State::AssemblingVideo) {
        emit logMessage("Video assembly complete.");
        emit statusChanged(m_currentRowNum, tr("Finished"));
        cleanupVideo();
        emit processingFinished(m_currentRowNum, true);
        cleanup();
    }
}

void RealCuganProcessor::startNextVideoFrame()
{
    if (m_video_frameQueue.isEmpty()) return;
    QString frameFile = m_video_frameQueue.dequeue();
    QString inputFramePath = m_video_inputFramesPath + "/" + frameFile;
    QString outputFramePath = m_video_outputFramesPath + "/" + frameFile;
    QStringList args = buildArguments(inputFramePath, outputFramePath);
    m_process->start(m_settings.programPath, args);
}

void RealCuganProcessor::startVideoAssembly()
{
    emit logMessage("All frames processed. Assembling final video...");
    emit statusChanged(m_currentRowNum, tr("Assembling video..."));
    m_state = State::AssemblingVideo;
    QStringList args;
    args << "-framerate" << "25" << "-i" << m_video_outputFramesPath + "/frame%08d.png"
         << "-i" << m_video_audioPath << "-c:v" << "libx264" << "-pix_fmt" << "yuv420p"
         << "-c:a" << "copy" << "-y" << m_finalDestinationFile;
    m_ffmpegProcess->start("ffmpeg", args);
}

QStringList RealCuganProcessor::buildArguments(const QString &inputFile, const QString &outputFile)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);
    arguments << "-s" << QString::number(m_settings.targetScale);
    arguments << "-n" << QString::number(m_settings.denoiseLevel);
    arguments << "-m" << m_settings.modelName;
    arguments << "-t" << QString::number(m_settings.tileSize);
    arguments << "-f" << m_settings.outputFormat;
    arguments << "-g" << m_settings.singleGpuId;
    if (m_settings.ttaEnabled) arguments << "-x";
    return arguments;
}

void RealCuganProcessor::onProcessError(QProcess::ProcessError) { /* Stub */ }
void RealCuganProcessor::onReadyReadStandardOutput() {
    QString output = m_process->readAllStandardOutput();
    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        if(m_state == State::ProcessingImage) {
             emit fileProgress(m_currentRowNum, match.captured(1).toInt());
        }
    }
}
void RealCuganProcessor::onFfmpegError(QProcess::ProcessError) { /* Stub */ }
void RealCuganProcessor::onFfmpegStdErr() {
    emit logMessage("FFmpeg: " + QString::fromLocal8Bit(m_ffmpegProcess->readAllStandardError()).trimmed());
}
