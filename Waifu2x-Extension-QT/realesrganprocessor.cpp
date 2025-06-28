// file: realesrganprocessor.cpp
#include "realesrganprocessor.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QTranslator> // For tr()
#include <QTime>       // For temporary file naming during multi-pass
#include <QStandardPaths> // For temporary paths (new)
#include <QFile>          // For QFile::remove (new)
#include <QTimer>


RealEsrganProcessor::RealEsrganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(); // No parent
    m_ffmpegProcess = new QProcess(); // No parent // For old video method (split/assemble)
    m_ffmpegEncoderProcess = new QProcess(); // No parent // For new pipe method (final encoding)

    // m_ffmpegDecoderProcess is being replaced by QMediaPlayer/QVideoSink
    // Initialize QMediaPlayer and QVideoSink
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoSink = new QVideoSink(this);
    m_mediaPlayer->setVideoSink(m_videoSink);

    cleanup(); // Initialize state, including m_state = State::Idle

    // Connections for m_process (RealESRGAN engine)
    connect(m_process, &QProcess::errorOccurred, this, &RealEsrganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealEsrganProcessor::onReadyReadStandardOutput);

    // Connections for m_ffmpegProcess (old video method)
    connect(m_ffmpegProcess, &QProcess::errorOccurred, this, &RealEsrganProcessor::onFfmpegError);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onFfmpegFinished);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RealEsrganProcessor::onFfmpegStdErr);

    // Connections for QMediaPlayer/QVideoSink
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &RealEsrganProcessor::onMediaPlayerStatusChanged);
    connect(m_mediaPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error, const QString &)>(&QMediaPlayer::errorOccurred), this, &RealEsrganProcessor::onMediaPlayerError);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &RealEsrganProcessor::onQtVideoFrameChanged);

    // Connections for m_ffmpegEncoderProcess (new piped video)
    connect(m_ffmpegEncoderProcess, &QProcess::readyReadStandardError, this, &RealEsrganProcessor::onPipeEncoderReadyReadStandardError);
    connect(m_ffmpegEncoderProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onPipeEncoderFinished);
    connect(m_ffmpegEncoderProcess, &QProcess::errorOccurred, this, &RealEsrganProcessor::onPipeEncoderError);
}

void RealEsrganProcessor::onProcessError(QProcess::ProcessError error)
{
    qWarning() << "RealEsrganProcessor::onProcessError occurred:" << error << m_process->errorString();
    if (m_state != State::Idle) {
         emit logMessage(tr("RealESRGAN main process error: %1 (%2). Current state: %3")
                         .arg(m_process->errorString())
                         .arg(error)
                         .arg(static_cast<int>(m_state)));
        // Actual finalization should be handled by onProcessFinished or specific error paths
    }
}

void RealEsrganProcessor::onReadyReadStandardOutput()
{
    if (!m_process) return;

    if (m_state == State::PipeProcessingSR) { // SR engine outputting frame data
        m_currentUpscaledFrameBuffer.append(m_process->readAllStandardOutput());

        int expectedFrameSize = m_outputFrameSize.width() * m_outputFrameSize.height() * m_inputFrameChannels;

        if (expectedFrameSize <= 0) {
             if (!m_currentUpscaledFrameBuffer.isEmpty()) {
                emit logMessage(tr("RealESRGAN SR Engine (pipe) stdout [Error state - invalid frame size]: %1").arg(QString::fromLocal8Bit(m_currentUpscaledFrameBuffer)));
                m_currentUpscaledFrameBuffer.clear();
             }
             finalizePipedVideoProcessing(false);
             return;
        }

        while(m_currentUpscaledFrameBuffer.size() >= expectedFrameSize) {
            QByteArray upscaledFrame = m_currentUpscaledFrameBuffer.left(expectedFrameSize);
            m_currentUpscaledFrameBuffer.remove(0, expectedFrameSize);
            pipeFrameToEncoder(upscaledFrame);
            m_framesProcessedPipe++;
             if (m_settings.verboseLog) qDebug() << "RealESRGAN: Sent frame" << m_framesProcessedPipe << "to encoder.";
        }
    } else { // Non-pipe mode, or verbose output from image processing
        QByteArray data = m_process->readAllStandardOutput();
        if (!data.isEmpty() && m_settings.verboseLog) {
            emit logMessage(tr("RealESRGAN stdout: %1").arg(QString::fromLocal8Bit(data).trimmed()));
        }
    }

    QByteArray errorData = m_process->readAllStandardError();
    if (!errorData.isEmpty()) {
       emit logMessage(tr("RealESRGAN stderr: %1").arg(QString::fromLocal8Bit(errorData).trimmed()));
    }
}

RealEsrganProcessor::~RealEsrganProcessor()
{
    // Ensure all processes are stopped and cleaned up
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(500);
    }
    if (m_ffmpegProcess && m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished(500);
    }
    cleanupQtMediaPlayer(); // Stop player and release QMediaPlayer resources
    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }
    cleanupVideo(); // Old method temps
    cleanupPipeProcesses(); // New method temps (audio, job dir)
}

void RealEsrganProcessor::cleanup()
{
    // General cleanup applicable to any finished or aborted task
    m_currentRowNum = -1;
    m_scaleSequence.clear();

    if (!m_currentPassInputFile.isEmpty() && m_currentPassInputFile != m_originalSourceFile && m_currentPassInputFile != m_finalDestinationFile) {
        if (QFile::exists(m_currentPassInputFile)) {
            QFile::remove(m_currentPassInputFile);
        }
    }

    m_originalSourceFile.clear();
    m_finalDestinationFile.clear();
    m_currentPassInputFile.clear();
    m_currentPassOutputFile.clear();

    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
    if (m_ffmpegProcess && m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
    }

    cleanupQtMediaPlayer();
    cleanupPipeProcesses();
    cleanupVideo();

    m_state = State::Idle;
}

void RealEsrganProcessor::cleanupQtMediaPlayer()
{
    if (m_mediaPlayer) {
        if (m_mediaPlayer->playbackState() != QMediaPlayer::StoppedState) {
            m_mediaPlayer->stop();
        }
        m_mediaPlayer->setSource(QUrl());
    }
    m_qtVideoFrameBuffer.clear();
    m_mediaPlayerPausedByBackpressure = false;
    m_framesDeliveredBySink = 0;
    m_framesAcceptedBySR = 0;
}

void RealEsrganProcessor::cleanupPipeProcesses()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(100);
    }

    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }

    if (!m_tempAudioPathPipe.isEmpty()) {
        QFile::remove(m_tempAudioPathPipe);
        m_tempAudioPathPipe.clear();
    }
    m_currentDecodedFrameBuffer.clear();
    m_currentUpscaledFrameBuffer.clear();
    m_framesProcessedPipe = 0;
    m_totalFramesEstimatePipe = 0;
    m_allFramesDecoded = false;
    m_allFramesSentToEncoder = false;
}


void RealEsrganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_state != State::Idle && m_state != State::ProcessingVideoFrames) {
        emit logMessage(tr("[ERROR] RealEsrganProcessor is already busy with a different task."));
        return;
    }

    if (m_state == State::Idle) {
        m_state = State::ProcessingImage;
        m_currentRowNum = rowNum;
        m_settings = settings;
        m_settings.programPath = QCoreApplication::applicationDirPath() + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";
        m_originalSourceFile = sourceFile;
        m_finalDestinationFile = destinationFile;
        emit statusChanged(m_currentRowNum, tr("Processing..."));
        emit logMessage(QString("Real-ESRGAN: Starting image job for %1...").arg(QFileInfo(sourceFile).fileName()));
    } else if (m_state == State::ProcessingVideoFrames) {
        m_originalSourceFile = sourceFile;
        m_finalDestinationFile = destinationFile;
    }
    m_currentPassInputFile = sourceFile;
    m_scaleSequence.clear();

    if (m_settings.targetScale <= 0 || m_settings.modelNativeScale <= 0) {
        emit logMessage(tr("Error: Invalid scale settings for RealESRGAN."));
        if (m_state == State::ProcessingVideoFrames) finalizePipedVideoProcessing(false);
        else {
            emit processingFinished(m_currentRowNum, false);
            cleanup();
        }
        return;
    }

    m_scaleSequence.clear();
    double currentOverallScale = 1.0;
    double targetOverallScale = static_cast<double>(m_settings.targetScale);
    int nativeModelScale = m_settings.modelNativeScale;

    if (targetOverallScale == 1.0 && nativeModelScale > 1) {
        m_scaleSequence.enqueue(nativeModelScale);
    } else {
        while (currentOverallScale < targetOverallScale) {
            if (currentOverallScale * nativeModelScale > targetOverallScale && currentOverallScale < targetOverallScale) {
                m_scaleSequence.enqueue(nativeModelScale);
                currentOverallScale *= nativeModelScale;
            } else if (currentOverallScale * nativeModelScale <= targetOverallScale) {
                m_scaleSequence.enqueue(nativeModelScale);
                currentOverallScale *= nativeModelScale;
            } else {
                break;
            }
            if (m_scaleSequence.size() > 5) break;
        }
    }
    if (m_scaleSequence.isEmpty() && targetOverallScale >= 1.0) {
        m_scaleSequence.enqueue(nativeModelScale);
    }

    if (m_scaleSequence.isEmpty()) {
         emit logMessage(tr("RealESRGAN: No scaling passes determined for image. Using native model scale."));
         m_scaleSequence.enqueue(nativeModelScale);
    }
    startNextPass();
}

void RealEsrganProcessor::startNextPass() {
    if (m_process->state() != QProcess::NotRunning) { return; }
    if (m_scaleSequence.isEmpty()) {
        if (m_currentPassInputFile != m_originalSourceFile && m_currentPassInputFile == m_finalDestinationFile) {
        } else if (m_currentPassInputFile != m_finalDestinationFile) {
        }
        return;
    }

    if (m_scaleSequence.size() == 1) {
        m_currentPassOutputFile = m_finalDestinationFile;
    } else {
        QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        m_currentPassOutputFile = QDir(tempPath).filePath(QString("realesrgan_pass_%1_%2.png")
                                                          .arg(QFileInfo(m_originalSourceFile).completeBaseName())
                                                          .arg(QTime::currentTime().toString("hhmmsszzz")));
    }

    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile);
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Starting Pass:" << m_settings.programPath << arguments.join(" ");
    m_process->start(m_settings.programPath, arguments);
    if (!m_process->waitForStarted(5000)) {
        emit logMessage(tr("RealESRGAN process failed to start for pass. Path: %1").arg(m_settings.programPath));
        processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }
}

void RealEsrganProcessor::processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_state != State::Idle) {
        emit logMessage(tr("[ERROR] RealEsrganProcessor is already busy. Please wait."));
        return;
    }

    cleanupPipeProcesses();
    cleanupVideo();
    cleanupQtMediaPlayer();

    m_state = State::PipeDecodingVideo;
    m_currentRowNum = rowNum;
    m_finalDestinationFile = destinationFile;
    m_settings = settings;
    m_settings.sourceFile = sourceFile;

    m_framesDeliveredBySink = 0;
    m_framesAcceptedBySR = 0;
    m_mediaPlayerPausedByBackpressure = false;
    m_qtVideoFrameBuffer.clear();
    m_allFramesDecoded = false;

    m_currentDecodedFrameBuffer.clear();
    m_currentUpscaledFrameBuffer.clear();
    m_allFramesSentToEncoder = false;
    m_inputPixelFormat = "bgr24";

    emit statusChanged(m_currentRowNum, tr("Starting video processing (pipe)..."));
    emit logMessage(tr("Real-ESRGAN Video (pipe) started for: %1").arg(QFileInfo(sourceFile).fileName()));

    if (!getVideoInfoPipe(sourceFile)) {
        finalizePipedVideoProcessing(false);
        return;
    }

    m_mediaPlayer->setSource(QUrl::fromLocalFile(m_settings.sourceFile));
    if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Source set to" << m_settings.sourceFile << ". Waiting for LoadedMedia status.";
    emit statusChanged(m_currentRowNum, tr("Loading video (Qt)..."));

    startPipeEncoder();
}


void RealEsrganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle) return;
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);

    if (m_state == State::ProcessingImage) {
        if (!success) {
            emit logMessage(tr("RealESRGAN: Pass failed for %1. Exit code: %2").arg(m_currentPassInputFile).arg(exitCode));
            if (!QFileInfo(m_originalSourceFile).suffix().isEmpty()) {
                 emit processingFinished(m_currentRowNum, false);
            } else {
                 emit logMessage(tr("RealESRGAN: Frame processing failed for old video method. Aborting video."));
                 emit processingFinished(m_currentRowNum, false);
            }
            cleanup();
            return;
        }

        if (!m_scaleSequence.isEmpty()) {
             m_scaleSequence.dequeue();
        }

        if (m_scaleSequence.isEmpty()) {
            if (m_currentPassOutputFile != m_finalDestinationFile) {
                if (QFile::exists(m_finalDestinationFile)) QFile::remove(m_finalDestinationFile);
                if (!QFile::rename(m_currentPassOutputFile, m_finalDestinationFile)) {
                    emit logMessage(tr("RealESRGAN: Failed to rename temp file %1 to %2.")
                                    .arg(m_currentPassOutputFile).arg(m_finalDestinationFile));
                    success = false;
                } else {
                     if (m_settings.verboseLog) qDebug() << "RealESRGAN: Renamed" << m_currentPassOutputFile << "to" << m_finalDestinationFile;
                }
            }
            if (m_currentPassInputFile != m_originalSourceFile && m_currentPassInputFile != m_finalDestinationFile) {
                 if (QFile::exists(m_currentPassInputFile)) QFile::remove(m_currentPassInputFile);
            }

            if (QFileInfo(m_originalSourceFile).suffix().isEmpty()) {
                m_video_processedFrames++;
                if (m_video_totalFrames > 0) emit fileProgress(m_currentRowNum, (100 * m_video_processedFrames) / m_video_totalFrames);
                if (m_video_frameQueue.isEmpty()) {
                    startVideoAssembly();
                } else {
                    startNextVideoFrame();
                }
            } else {
                emit processingFinished(m_currentRowNum, success);
                cleanup();
            }
        } else {
            m_currentPassInputFile = m_currentPassOutputFile;
            startNextPass();
        }
    } else if (m_state == State::PipeProcessingSR) {
        if (!success) {
            emit logMessage(tr("RealESRGAN pipe processing for a frame failed. Exit code: %1, Status: %2").arg(exitCode).arg(exitStatus));
            QByteArray errorMsg = m_process->readAllStandardError();
            if (!errorMsg.isEmpty()) emit logMessage(tr("RealESRGAN stderr (pipe): %1").arg(QString::fromLocal8Bit(errorMsg)));
            finalizePipedVideoProcessing(false);
            return;
        }

        if (m_settings.verboseLog) qDebug() << "RealESRGAN (pipe) finished one frame for SR. Accepted by SR:" << m_framesAcceptedBySR;

        if (m_totalFramesEstimatePipe > 0 && m_framesAcceptedBySR <= m_totalFramesEstimatePipe) {
             emit fileProgress(m_currentRowNum, (100 * m_framesAcceptedBySR) / m_totalFramesEstimatePipe);
        }

        m_state = State::PipeDecodingVideo;
        processDecodedFrameBuffer();
    }
}


void RealEsrganProcessor::onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle && !(m_state == State::SplittingVideo || m_state == State::AssemblingVideo) ) return;
    if (exitStatus != QProcess::NormalExit || exitCode != 0) { cleanupVideo(); emit processingFinished(m_currentRowNum, false); cleanup(); return; }
    if (m_state == State::SplittingVideo) {  }
    else if (m_state == State::AssemblingVideo) { cleanupVideo(); emit processingFinished(m_currentRowNum, true); cleanup(); }
}

void RealEsrganProcessor::startNextVideoFrame()
{
    if (m_video_frameQueue.isEmpty()) { if (m_video_processedFrames != m_video_totalFrames || m_video_totalFrames == 0) {  } return; }
    QString frameFile = m_video_frameQueue.head();
    QString inputFramePath = QDir(m_video_inputFramesPath).filePath(frameFile);
    QString outputFramePath = QDir(m_video_outputFramesPath).filePath(frameFile);
    processImage(m_currentRowNum, inputFramePath, outputFramePath, m_settings);
    m_video_frameQueue.dequeue();
}

void RealEsrganProcessor::startVideoAssembly()
{
    m_state = State::AssemblingVideo;
    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    // m_ffmpegProcess->start(ffmpegPath, args);
}

QStringList RealEsrganProcessor::buildArguments(const QString &inputFile, const QString &outputFile)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

     arguments << "-s" << QString::number(m_settings.modelNativeScale);

    arguments << "-n" << m_settings.modelName;
    arguments << "-m" << m_settings.modelPath;

    if (m_settings.tileSize > 0) {
        arguments << "-t" << QString::number(m_settings.tileSize);
    } else {
        arguments << "-t" << "0";
    }
    arguments << "-f" << m_settings.outputFormat;

    if (!m_settings.singleGpuId.trimmed().isEmpty() && m_settings.singleGpuId.toLower() != "auto") {
        arguments << "-g" << m_settings.singleGpuId;
    } else {
        arguments << "-g" << "auto";
    }
    if (m_settings.ttaEnabled) {
        arguments << "-x";
    }
    if (m_settings.verboseLog) {
        arguments << "-v";
    }
    return arguments;
}

bool RealEsrganProcessor::getVideoInfoPipe(const QString& inputFile) {
    QProcess ffprobeProcess;
    QStringList probeArgs;
    probeArgs << "-v" << "error"
              << "-select_streams" << "v:0"
              << "-show_entries" << "stream=width,height,avg_frame_rate,nb_frames,duration"
              << "-of" << "default=noprint_wrappers=1:nokey=1"
              << inputFile;

    QString ffprobePath = m_settings.ffprobePath.isEmpty() ? "ffprobe" : m_settings.ffprobePath;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Probing video:" << ffprobePath << probeArgs.join(" ");

    ffprobeProcess.start(ffprobePath, probeArgs);
    if (!ffprobeProcess.waitForStarted(5000)) {
        emit logMessage(tr("RealESRGAN: Failed to start ffprobe for video info."));
        return false;
    }
    if (!ffprobeProcess.waitForFinished(15000)) {
        emit logMessage(tr("RealESRGAN: ffprobe timeout while getting video info."));
        ffprobeProcess.kill();
        return false;
    }

    if (ffprobeProcess.exitStatus() != QProcess::NormalExit || ffprobeProcess.exitCode() != 0) {
        emit logMessage(tr("RealESRGAN: ffprobe failed. Exit: %1. Error: %2")
                        .arg(ffprobeProcess.exitCode())
                        .arg(QString::fromLocal8Bit(ffprobeProcess.readAllStandardError()).trimmed()));
        return false;
    }

    QString output = QString::fromLocal8Bit(ffprobeProcess.readAllStandardOutput()).trimmed();
    if (m_settings.verboseLog) qDebug() << "RealESRGAN ffprobe output:" << output;
    QStringList info = output.split('\n', Qt::SkipEmptyParts);

    if (info.size() < 3) {
        emit logMessage(tr("RealESRGAN: Failed to parse ffprobe output (width, height, fps). Output: %1").arg(output));
        return false;
    }

    bool ok_w, ok_h;
    int width = info[0].toInt(&ok_w);
    int height = info[1].toInt(&ok_h);
    if (!ok_w || !ok_h || width <= 0 || height <= 0) {
        emit logMessage(tr("RealESRGAN: Invalid width/height from ffprobe: %1x%2").arg(info[0], info[1]));
        return false;
    }
    m_inputFrameSize.setWidth(width);
    m_inputFrameSize.setHeight(height);

    QString fpsStr = info[2];
    if (fpsStr.contains('/')) {
        QStringList parts = fpsStr.split('/');
        if (parts.size() == 2 && parts[1].toDouble() != 0) {
            m_settings.videoFps = parts[0].toDouble() / parts[1].toDouble();
        } else {
             m_settings.videoFps = 25.0;
        }
    } else {
        m_settings.videoFps = fpsStr.toDouble();
    }
    if (m_settings.videoFps <= 0) m_settings.videoFps = 25.0;

    if (info.size() > 3 && !info[3].isEmpty() && info[3] != "N/A") {
        m_totalFramesEstimatePipe = info[3].toInt();
    } else if (info.size() > 4 && !info[4].isEmpty() && info[4] != "N/A") {
        double duration = info[4].toDouble();
        if (duration > 0 && m_settings.videoFps > 0) {
            m_totalFramesEstimatePipe = static_cast<int>(round(duration * m_settings.videoFps));
        }
    }
     if (m_totalFramesEstimatePipe <= 0 && m_settings.verboseLog) {
        qDebug() << "RealESRGAN Warning: Could not determine total frames from ffprobe.";
    }

    m_inputFrameChannels = 3;
    m_outputFrameSize.setWidth(m_inputFrameSize.width() * m_settings.targetScale);
    m_outputFrameSize.setHeight(m_inputFrameSize.height() * m_settings.targetScale);

    QString tempVideoJobPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                           QString("/realesrgan_pipe_job_%1").arg(QDateTime::currentMSecsSinceEpoch());
    QDir tempJobDir(tempVideoJobPath);
    if (!tempJobDir.mkpath(".")) {
        emit logMessage(tr("RealESRGAN: Error creating temp dir for audio: %1").arg(tempVideoJobPath));
        m_tempAudioPathPipe.clear();
    } else {
        m_tempAudioPathPipe = tempJobDir.filePath("audio.aac");
    }

    if (!m_tempAudioPathPipe.isEmpty()) {
        QProcess extractAudioProcess;
        QStringList audioArgs;
        audioArgs << "-y" << "-i" << inputFile << "-vn" << "-acodec" << "copy" << m_tempAudioPathPipe;

        QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
        if (m_settings.verboseLog) qDebug() << "RealESRGAN Extracting audio:" << ffmpegPath << audioArgs.join(" ");

        extractAudioProcess.start(ffmpegPath, audioArgs);
        if (!extractAudioProcess.waitForFinished(15000)) {
            emit logMessage(tr("RealESRGAN: Audio extraction timeout. No audio."));
            extractAudioProcess.kill();
            QFile::remove(m_tempAudioPathPipe);
            m_tempAudioPathPipe.clear();
        } else if (extractAudioProcess.exitCode() != 0) {
            emit logMessage(tr("RealESRGAN: Audio extraction failed. Error: %1. No audio.")
                            .arg(QString::fromLocal8Bit(extractAudioProcess.readAllStandardError()).trimmed()));
            QFile::remove(m_tempAudioPathPipe);
            m_tempAudioPathPipe.clear();
        } else {
             if (!QFile::exists(m_tempAudioPathPipe) || QFileInfo(m_tempAudioPathPipe).size() == 0) {
                 emit logMessage(tr("RealESRGAN: Audio extraction no file or empty. No audio."));
                 QFile::remove(m_tempAudioPathPipe);
                 m_tempAudioPathPipe.clear();
            } else if (m_settings.verboseLog) {
                 qDebug() << "RealESRGAN Audio extracted to:" << m_tempAudioPathPipe;
            }
        }
    }

    if (m_settings.verboseLog) {
        qDebug() << "RealESRGAN Video Info: InputSize=" << m_inputFrameSize << "Channels=" << m_inputFrameChannels
                 << "FPS=" << m_settings.videoFps << "TotalFramesEst=" << m_totalFramesEstimatePipe
                 << "AudioPath=" << m_tempAudioPathPipe << "OutputUpscaledSize=" << m_outputFrameSize;
    }
    return m_inputFrameSize.width() > 0 && m_inputFrameSize.height() > 0;
}

void RealEsrganProcessor::startPipeDecoder() {
    if (m_state != State::PipeDecodingVideo) {
        emit logMessage(tr("RealESRGAN: Decoder started in incorrect state."));
        return;
    }
    if (m_settings.verboseLog) qDebug() << "RealESRGAN startPipeDecoder: QMediaPlayer will handle decoding.";
}

void RealEsrganProcessor::startRealEsrganPipe(const QByteArray& frameData) {
    if (m_process->state() == QProcess::Running) {
        emit logMessage(tr("RealESRGAN Error: SR process already running."));
        return;
    }
    QStringList srArgs;
    srArgs << "--use-pipe"
           << "--input-width" << QString::number(m_inputFrameSize.width())
           << "--input-height" << QString::number(m_inputFrameSize.height())
           << "--input-channels" << QString::number(m_inputFrameChannels)
           << "--pixel-format" << (m_inputPixelFormat == "bgr24" ? "BGR" : "RGB")
           << "-s" << QString::number(m_settings.targetScale)
           << "-n" << m_settings.modelName
           << "-m" << m_settings.modelPath;
    if (m_settings.tileSize > 0) srArgs << "-t" << QString::number(m_settings.tileSize); else srArgs << "-t" << "0";
    if (!m_settings.singleGpuId.isEmpty() && m_settings.singleGpuId != "auto") srArgs << "-g" << m_settings.singleGpuId; else srArgs << "-g" << "auto";
    if (m_settings.ttaEnabled) srArgs << "-x";

    if (m_settings.verboseLog) qDebug() << "RealESRGAN Starting Pipe:" << m_settings.programPath << srArgs.join(" ");
    m_currentUpscaledFrameBuffer.clear();
    m_process->start(m_settings.programPath, srArgs);
    if (m_process->waitForStarted(5000)) {
        qint64 written = m_process->write(frameData);
        if (written != frameData.size()) { finalizePipedVideoProcessing(false); return; }
        m_process->closeWriteChannel();
    } else { finalizePipedVideoProcessing(false); }
}

void RealEsrganProcessor::startPipeEncoder() {
    if (m_state == State::Idle) return;
    if (m_ffmpegEncoderProcess->state() == QProcess::Running) return;
    QStringList encoderArgs;
    encoderArgs << "-y" << "-f" << "rawvideo" << "-pix_fmt" << m_inputPixelFormat
                << "-s" << QString("%1x%2").arg(m_outputFrameSize.width()).arg(m_outputFrameSize.height())
                << "-r" << QString::number(m_settings.videoFps,'f', 2) << "-i" << "pipe:0";
    if (!m_tempAudioPathPipe.isEmpty() && QFile::exists(m_tempAudioPathPipe)) {
        encoderArgs << "-i" << QDir::toNativeSeparators(m_tempAudioPathPipe) << "-c:a" << "copy";
    } else { encoderArgs << "-an"; }
    encoderArgs << "-c:v" << "libx264" << "-preset" << "medium" << "-crf" << "23" << "-pix_fmt" << "yuv420p"
                << QDir::toNativeSeparators(m_finalDestinationFile);
    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Starting FFmpeg encoder:" << ffmpegPath << encoderArgs.join(" ");
    m_ffmpegEncoderProcess->start(ffmpegPath, encoderArgs);
     if (!m_ffmpegEncoderProcess->waitForStarted(5000)) { finalizePipedVideoProcessing(false); }
     else if (m_settings.verboseLog) qDebug() << "RealESRGAN FFmpeg encoder started.";
}

void RealEsrganProcessor::pipeFrameToEncoder(const QByteArray& upscaledFrameData) {
    if (m_state == State::Idle || !m_ffmpegEncoderProcess || m_ffmpegEncoderProcess->state() != QProcess::Running) return;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Piping" << upscaledFrameData.size() << "bytes to encoder.";
    qint64 bytesWritten = m_ffmpegEncoderProcess->write(upscaledFrameData);
    if (bytesWritten != upscaledFrameData.size()) { finalizePipedVideoProcessing(false); }
}

void RealEsrganProcessor::onPipeEncoderReadyReadStandardError() {
    if(!m_ffmpegEncoderProcess) return;
    QByteArray errorData = m_ffmpegEncoderProcess->readAllStandardError();
    QString stderrStr = QString::fromLocal8Bit(errorData).trimmed();
    if (m_settings.verboseLog || (!stderrStr.contains("frame=") && !stderrStr.contains("bitrate="))) {
        if (!stderrStr.isEmpty()) emit logMessage(tr("RealESRGAN FFmpeg Encoder stderr: %1").arg(stderrStr));
    }
}

void RealEsrganProcessor::onPipeEncoderFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (m_state == State::Idle && exitCode == 0 && exitStatus == QProcess::NormalExit) return;
    if (m_state == State::Idle && (exitCode !=0 || exitStatus != QProcess::NormalExit)) return;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN FFmpeg Encoder finished. ExitCode:" << exitCode << "Status:" << exitStatus;
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    if(!success && m_state != State::Idle) emit logMessage(tr("RealESRGAN FFmpeg Encoder failed. Exit: %1, Status: %2").arg(exitCode).arg(exitStatus));
    finalizePipedVideoProcessing(success);
}

void RealEsrganProcessor::onPipeEncoderError(QProcess::ProcessError error) {
    if (m_state == State::Idle) return;
    emit logMessage(tr("RealESRGAN FFmpeg Encoder error: %1. Code: %2").arg(m_ffmpegEncoderProcess->errorString()).arg(error));
    finalizePipedVideoProcessing(false);
}

void RealEsrganProcessor::finalizePipedVideoProcessing(bool success) {
    State currentState = m_state;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Finalizing pipe processing. Success:" << success << "State:" << (int)currentState;
    cleanup();
    if (currentState != State::Idle) {
        if (success) {
            emit logMessage(tr("RealESRGAN: Piped video processing finished for: %1").arg(QFileInfo(m_finalDestinationFile).fileName()));
            emit statusChanged(m_currentRowNum, tr("Finished"));
        } else {
            emit logMessage(tr("RealESRGAN: Piped video processing failed for: %1").arg(QFileInfo(m_settings.sourceFile).fileName()));
            emit statusChanged(m_currentRowNum, tr("Error"));
            if (!m_finalDestinationFile.isEmpty() && QFile::exists(m_finalDestinationFile)) {
                QFile::remove(m_finalDestinationFile);
            }
        }
        emit processingFinished(m_currentRowNum, success);
    } else if (!success && m_currentRowNum != -1) {
        emit processingFinished(m_currentRowNum, false);
        m_currentRowNum = -1;
    }
}

void RealEsrganProcessor::cleanupVideo() {
    if (m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished(500);
    }
    if (!m_video_tempPath.isEmpty()) {
        QDir dir(m_video_tempPath);
        if (dir.exists()) {
            emit logMessage(QString(tr("Real-ESRGAN Video: Cleaning up temporary video path: %1")).arg(m_video_tempPath));
            dir.removeRecursively();
        }
        m_video_tempPath.clear();
    }
    m_video_inputFramesPath.clear();
    m_video_outputFramesPath.clear();
    m_video_audioPath.clear();
    m_video_frameQueue.clear();
    m_video_totalFrames = 0;
    m_video_processedFrames = 0;
}

void RealEsrganProcessor::onFfmpegError(QProcess::ProcessError error) {
    if (m_state == State::SplittingVideo || m_state == State::AssemblingVideo) {
        emit logMessage(QString(tr("FFmpeg (old video method) process error: %1. Code: %2"))
                        .arg(m_ffmpegProcess->errorString())
                        .arg(error));
        cleanupVideo();
        emit statusChanged(m_currentRowNum, tr("FFmpeg Error (Old Vid)"));
        finalizePipedVideoProcessing(false);
    } else if (m_settings.verboseLog && m_state != State::Idle) {
         qDebug() << "onFfmpegError called for m_ffmpegProcess in unexpected state:" << (int)m_state << "Error:" << error;
    }
}

void RealEsrganProcessor::onFfmpegStdErr() {
    if (m_state == State::SplittingVideo || m_state == State::AssemblingVideo) {
        QByteArray data = m_ffmpegProcess->readAllStandardError();
        emit logMessage("FFmpeg (Old Vid): " + QString::fromLocal8Bit(data).trimmed());
    }
}

void RealEsrganProcessor::onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (m_state != State::PipeDecodingVideo) {
        if (status == QMediaPlayer::InvalidMedia && m_currentRowNum != -1) {
             emit logMessage(tr("QMediaPlayer Error: Invalid media - %1").arg(m_settings.sourceFile));
             finalizePipedVideoProcessing(false);
        }
        return;
    }

    switch (status) {
    case QMediaPlayer::LoadedMedia:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Media loaded. Total frames estimate:" << m_totalFramesEstimatePipe;
        m_mediaPlayer->play();
        emit statusChanged(m_currentRowNum, tr("Decoding (Qt)..."));
        break;
    case QMediaPlayer::EndOfMedia:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: EndOfMedia. Delivered:" << m_framesDeliveredBySink << "Accepted by SR:" << m_framesAcceptedBySR;
        m_allFramesDecoded = true;
        processDecodedFrameBuffer();
        break;
    case QMediaPlayer::InvalidMedia:
        emit logMessage(tr("QMediaPlayer Error: Invalid media - %1").arg(m_settings.sourceFile));
        finalizePipedVideoProcessing(false);
        break;
    case QMediaPlayer::NoMedia:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: NoMedia status.";
        if (m_state == State::PipeDecodingVideo) {
            emit logMessage(tr("QMediaPlayer Error: No media present during decoding for %1.").arg(m_settings.sourceFile));
            finalizePipedVideoProcessing(false);
        }
        break;
    case QMediaPlayer::BufferingMedia:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Buffering media...";
        emit statusChanged(m_currentRowNum, tr("Buffering (Qt)..."));
        break;
    case QMediaPlayer::BufferedMedia:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Media buffered.";
        if (m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && !m_mediaPlayerPausedByBackpressure) {
            if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Media buffered, resuming playback.";
            m_mediaPlayer->play();
        }
        break;
    case QMediaPlayer::LoadingMedia:
         if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Loading media status confirmed.";
        break;
    default:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Unhandled media status:" << status;
        break;
    }
}

void RealEsrganProcessor::onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    if (m_state == State::Idle && error == QMediaPlayer::NoError) return;

    emit logMessage(tr("RealESRGAN QMediaPlayer Error: %1 (Code: %2). Source: %3").arg(errorString).arg(error).arg(m_settings.sourceFile));
    finalizePipedVideoProcessing(false);
}

void RealEsrganProcessor::onQtVideoFrameChanged(const QVideoFrame &frame)
{
    if (m_state != State::PipeDecodingVideo && m_state != State::PipeProcessingSR) {
        if (frame.isValid() && m_settings.verboseLog) {
            qDebug() << "RealESRGAN QVideoSink: Frame received in non-processing state" << (int)m_state << "- dropping.";
        }
        return;
    }

    if (!frame.isValid()) {
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QVideoSink: Invalid frame received.";
        return;
    }

    m_framesDeliveredBySink++;
    if (m_settings.verboseLog && m_framesDeliveredBySink % 100 == 0) {
        qDebug() << "RealESRGAN QVideoSink: Frame" << m_framesDeliveredBySink << "received. Format:" << frame.pixelFormat();
    }

    if ((m_process && m_process->state() == QProcess::Running) || m_qtVideoFrameBuffer.size() > 5) {
        if (m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState && !m_mediaPlayerPausedByBackpressure) {
            m_mediaPlayer->pause();
            m_mediaPlayerPausedByBackpressure = true;
            if (m_settings.verboseLog) qDebug() << "RealESRGAN QVideoSink: Pausing QMediaPlayer (backpressure). Buffered QFrames:" << m_qtVideoFrameBuffer.size();
        }
        m_qtVideoFrameBuffer.enqueue(frame);
        return;
    }

    QVideoFrame frameToProcess;
    if (!m_qtVideoFrameBuffer.isEmpty()) {
        frameToProcess = m_qtVideoFrameBuffer.dequeue();
    } else {
        frameToProcess = frame;
    }

    QVideoFrame mappedFrame = frameToProcess;
    if (!mappedFrame.map(QVideoFrame::ReadOnly)) {
        emit logMessage(tr("RealESRGAN Error: Could not map QVideoFrame."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
             m_mediaPlayer->play(); m_mediaPlayerPausedByBackpressure = false;
        }
        return;
    }

    QImage::Format targetFormat = (m_inputPixelFormat == "rgb24") ? QImage::Format_RGB888 : QImage::Format_BGR888;
    QImage image = mappedFrame.toImage().convertToFormat(targetFormat);
    mappedFrame.unmap();

    if (image.isNull()) {
        emit logMessage(tr("RealESRGAN Error: Failed to convert QVideoFrame to QImage."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
             m_mediaPlayer->play(); m_mediaPlayerPausedByBackpressure = false;
        }
        return;
    }

    m_currentDecodedFrameBuffer = QByteArray(reinterpret_cast<const char*>(image.constBits()), qsizetype(image.sizeInBytes()));
    m_framesAcceptedBySR++;
    if (m_settings.verboseLog && m_framesAcceptedBySR % 100 == 0) {
         qDebug() << "RealESRGAN QVideoSink: Frame" << m_framesAcceptedBySR << "being sent to SR.";
    }
    startRealEsrganPipe(m_currentDecodedFrameBuffer);
    m_currentDecodedFrameBuffer.clear();

    if (m_mediaPlayerPausedByBackpressure && m_qtVideoFrameBuffer.size() < 2 && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
        m_mediaPlayer->play();
        m_mediaPlayerPausedByBackpressure = false;
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QVideoSink: Resuming QMediaPlayer. Buffer low.";
    }
    if (m_process && m_process->state() == QProcess::NotRunning && !m_qtVideoFrameBuffer.isEmpty()) {
        processDecodedFrameBuffer();
    }
}

void RealEsrganProcessor::processDecodedFrameBuffer() { // Adapted for QVideoFrame buffer
    if (m_process && m_process->state() == QProcess::Running) {
        return;
    }

    if (m_qtVideoFrameBuffer.isEmpty()) {
        if (m_allFramesDecoded && m_currentDecodedFrameBuffer.isEmpty()) {
            if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
                m_ffmpegEncoderProcess->closeWriteChannel();
                m_allFramesSentToEncoder = true;
                if (m_settings.verboseLog) qDebug() << "RealESRGAN processDecodedFrameBuffer: All frames processed, closing encoder input.";
            }
            if (m_state != State::Idle) m_state = State::PipeEncodingVideo;
        }
        return;
    }

    QVideoFrame frame = m_qtVideoFrameBuffer.dequeue();
    m_framesAcceptedBySR++;
     if (m_settings.verboseLog && m_framesAcceptedBySR % 100 == 0) {
        qDebug() << "RealESRGAN processDecodedFrameBuffer: Processing buffered QVideoFrame " << m_framesAcceptedBySR << ". " << m_qtVideoFrameBuffer.size() << " remaining in QBuffer.";
    }

    QVideoFrame mappedFrame = frame;
    if (!mappedFrame.map(QVideoFrame::ReadOnly)) {
        emit logMessage(tr("RealESRGAN Error: Could not map buffered QVideoFrame. Skipping."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play(); m_mediaPlayerPausedByBackpressure = false;
        }
        QTimer::singleShot(0, this, &RealEsrganProcessor::processDecodedFrameBuffer);
        return;
    }

    QImage::Format targetFormat = (m_inputPixelFormat == "rgb24") ? QImage::Format_RGB888 : QImage::Format_BGR888;
    QImage image = mappedFrame.toImage().convertToFormat(targetFormat);
    mappedFrame.unmap();

    if (image.isNull()) {
        emit logMessage(tr("RealESRGAN Error: Failed to convert buffered QVideoFrame to QImage. Skipping."));
         if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play(); m_mediaPlayerPausedByBackpressure = false;
        }
        QTimer::singleShot(0, this, &RealEsrganProcessor::processDecodedFrameBuffer);
        return;
    }

    m_currentDecodedFrameBuffer = QByteArray(reinterpret_cast<const char*>(image.constBits()), qsizetype(image.sizeInBytes()));
    if (m_currentDecodedFrameBuffer.isEmpty()) {
        emit logMessage(tr("RealESRGAN Error: Converted QImage to QByteArray is empty. Skipping."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play(); m_mediaPlayerPausedByBackpressure = false;
        }
        QTimer::singleShot(0, this, &RealEsrganProcessor::processDecodedFrameBuffer);
        return;
    }

    m_state = State::PipeProcessingSR;
    startRealEsrganPipe(m_currentDecodedFrameBuffer);
    m_currentDecodedFrameBuffer.clear();

    if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
        m_mediaPlayer->play();
        m_mediaPlayerPausedByBackpressure = false;
        if (m_settings.verboseLog) qDebug() << "RealESRGAN processDecodedFrameBuffer: Resuming QMediaPlayer. QBuffer low.";
    }
}

void RealEsrganProcessor::onPipeDecoderReadyReadStandardOutput() {
    if (m_settings.verboseLog) qDebug() << "RealESRGAN onPipeDecoderReadyReadStandardOutput called - this should NOT happen in QMediaPlayer flow.";
}
void RealEsrganProcessor::onPipeDecoderFinished(int, QProcess::ExitStatus) {
    if (m_settings.verboseLog) qDebug() << "RealESRGAN onPipeDecoderFinished called - this should NOT happen in QMediaPlayer flow.";
}
void RealEsrganProcessor::onPipeDecoderError(QProcess::ProcessError) {
    if (m_settings.verboseLog) qDebug() << "RealESRGAN onPipeDecoderError called - this should NOT happen in QMediaPlayer flow.";
}
// The actual [end of file] marker was removed in the previous step by overwrite_file_with_block.
// This comment is just for clarity if this block were to be copied again.
// [end of Waifu2x-Extension-QT/realesrganprocessor.cpp] // This line was removed by the patch script
