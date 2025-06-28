// file: realcuganprocessor.cpp
#include "RealCuganProcessor.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths> // For temporary paths
#include <QDateTime>      // For unique temp folder names
#include <QFile>          // For QFile::remove
#include <QTimer>

RealCuganProcessor::RealCuganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(); // No parent
    connect(m_process, &QProcess::errorOccurred, this, &RealCuganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealCuganProcessor::onReadyReadStandardOutput);

    m_ffmpegProcess = new QProcess(); // No parent // For old video method (splitting/assembly)
    connect(m_ffmpegProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onFfmpegError);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onFfmpegFinished);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onFfmpegStdErr);

    // m_ffmpegDecoderProcess is being replaced by QMediaPlayer/QVideoSink
    // Initialize QMediaPlayer and QVideoSink
    m_mediaPlayer = new QMediaPlayer(this);
    m_videoSink = new QVideoSink(this);
    m_mediaPlayer->setVideoSink(m_videoSink); // Crucial: Set sink before connecting signals related to sink

    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &RealCuganProcessor::onMediaPlayerStatusChanged);
    // QOverload<QMediaPlayer::Error, const QString &>::of is C++14, ensure compatibility or use static_cast
    connect(m_mediaPlayer, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error, const QString &)>(&QMediaPlayer::errorOccurred), this, &RealCuganProcessor::onMediaPlayerError);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &RealCuganProcessor::onQtVideoFrameChanged);


    // FFmpeg Encoder process for final video assembly (remains)
    m_ffmpegEncoderProcess = new QProcess(); // No parent
    connect(m_ffmpegEncoderProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onPipeEncoderReadyReadStandardError);
    connect(m_ffmpegEncoderProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onPipeEncoderFinished);
    connect(m_ffmpegEncoderProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onPipeEncoderError);

    // Old m_ffmpegDecoderProcess connections are removed
    // connect(m_ffmpegDecoderProcess, &QProcess::readyReadStandardOutput, this, &RealCuganProcessor::onPipeDecoderReadyReadStandardOutput);
    // connect(m_ffmpegDecoderProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onPipeDecoderReadyReadStandardError);
    // connect(m_ffmpegDecoderProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onPipeDecoderFinished);
    // connect(m_ffmpegDecoderProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onPipeDecoderError);

    cleanup(); // Initial cleanup to set state
}

RealCuganProcessor::~RealCuganProcessor()
{
    // Ensure all processes are stopped and cleaned up
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(500); // Short wait
    }
    if (m_ffmpegProcess && m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished(500);
    }
    // m_ffmpegDecoderProcess is removed, m_mediaPlayer/m_videoSink handled by Qt parent/child or explicit delete if needed
    // No, QMediaPlayer needs explicit stop. QVideoSink is fine.
    cleanupQtMediaPlayer(); // Stop player and release resources

    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }
    cleanupVideo(); // Clean up temp files for old method
    cleanupPipeProcesses(); // Clean up temp files for new method (e.g. audio)
    // m_mediaPlayer and m_videoSink are QObjects with `this` as parent, Qt should handle their deletion.
    // However, explicit deletion or reset can be done if needed, e.g. m_mediaPlayer->setMedia(QMediaContent());
}

void RealCuganProcessor::cleanup()
{
    // General cleanup applicable to any finished or aborted task
    m_state = State::Idle;
    m_currentRowNum = -1;
    m_finalDestinationFile.clear();
    // m_settings is value type, reset when new job starts

    // Kill processes if they are running from a previous ungraceful exit from a state
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
     if (m_ffmpegProcess && m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
    }
    // New pipe processes will be cleaned in cleanupPipeProcesses which should be called too
    cleanupQtMediaPlayer(); // Ensure QMediaPlayer is stopped and cleaned
    cleanupPipeProcesses(); // Clean SR and Encoder processes
    cleanupVideo(); // For old method temp files
}


void RealCuganProcessor::cleanupPipeProcesses()
{
    // m_ffmpegDecoderProcess is replaced by QtMultimedia, so no need to kill it here.
    // cleanupQtMediaPlayer() will handle QMediaPlayer.
    // if (m_ffmpegDecoderProcess && m_ffmpegDecoderProcess->state() != QProcess::NotRunning) {
    //    m_ffmpegDecoderProcess->kill();
    //    m_ffmpegDecoderProcess->waitForFinished(500);
    // }

    // m_process (SR engine) is handled by general cleanup() if it was used for pipe mode and got stuck.
    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }

    if (!m_tempAudioPath.isEmpty()) {
        QFile::remove(m_tempAudioPath); // Remove the audio file
        m_tempAudioPath.clear();
    }

    if (!m_tempVideoJobPath.isEmpty()) { // Now remove the job directory itself
        QDir jobDir(m_tempVideoJobPath);
        if (jobDir.exists()) { // Check if it exists before trying to remove
            if (jobDir.removeRecursively()) {
                if (m_settings.verboseLog) qDebug() << "Successfully removed temp job directory:" << m_tempVideoJobPath;
            } else {
                emit logMessage(tr("Warning: Could not remove temporary job directory: %1").arg(m_tempVideoJobPath));
            }
        }
        m_tempVideoJobPath.clear();
    }

    m_currentDecodedFrameBuffer.clear(); // This will store data from QVideoFrame
    m_qtVideoFrameBuffer.clear();        // Clear the QVideoFrame queue
    m_currentUpscaledFrameBuffer.clear();
    m_framesProcessedPipe = 0;
    m_totalFramesEstimatePipe = 0;
    m_allFramesDecoded = false;          // This will be set by QMediaPlayer::EndOfMedia
    m_allFramesSentToEncoder = false;
    m_framesDeliveredBySink = 0;
    m_framesAcceptedBySR = 0;
    m_mediaPlayerPausedByBackpressure = false;
}

void RealCuganProcessor::cleanupQtMediaPlayer()
{
    if (m_mediaPlayer) {
        if (m_mediaPlayer->playbackState() != QMediaPlayer::StoppedState) {
            m_mediaPlayer->stop();
        }
        // Reset source to release file handles if any are held
        // m_mediaPlayer->setMedia(QMediaContent()); // use setSource(QUrl()) with Qt6
        m_mediaPlayer->setSource(QUrl());
    }
    // m_videoSink is a child of m_mediaPlayer or this, and its resources are managed by Qt mostly.
    // No specific QVideoSink cleanup seems necessary beyond what QMediaPlayer::setVideoSink(nullptr) or player destruction does.
    // If m_videoSink was created with `this` as parent and not set to player, it's fine.
    // If it was set to player, player handles it. If it was `new QVideoSink(nullptr)`, then manual delete would be needed if not parented.
    // Here, it's `new QVideoSink(this)`, so Qt handles it.
    m_qtVideoFrameBuffer.clear();
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
        emit logMessage(tr("[ERROR] RealCuganProcessor is already busy. Please wait for the current task to complete."));
        // Optionally emit processingFinished to signal failure for this new request,
        // or implement a queuing mechanism if multiple concurrent jobs should be supported (more complex).
        // For now, we just reject the new job if busy.
        // emit processingFinished(rowNum, false); // If you want to signal failure for the new request
        return;
    }

    cleanupPipeProcesses(); // Ensure any previous pipe processes are dead
    cleanupVideo();         // Clean up old method's temp files just in case

    m_state = State::PipeDecodingVideo; // Initial state for the new pipeline
    m_currentRowNum = rowNum;
    m_finalDestinationFile = destinationFile;
    m_settings = settings;
    m_framesProcessedPipe = 0;
    m_totalFramesEstimatePipe = 0;
    m_currentDecodedFrameBuffer.clear();
    m_currentUpscaledFrameBuffer.clear();
    m_allFramesDecoded = false;
    m_allFramesSentToEncoder = false;
    m_inputPixelFormat = "bgr24"; // Default for FFmpeg raw output, matches what SR engine expects

    emit statusChanged(m_currentRowNum, tr("Starting video processing (pipe)..."));
    emit logMessage(tr("Video processing (pipe) started for: %1").arg(QFileInfo(sourceFile).fileName()));

    if (!getVideoInfo(sourceFile)) { // This will use ffprobe
        finalizePipedVideoProcessing(false);
        return;
    }

    // getVideoInfo should have populated:
    // m_inputFrameSize, m_inputFrameChannels,
    // m_totalFramesEstimatePipe, m_settings.videoFps (if not already set),
    // m_tempAudioPath
    // It also calculates m_outputFrameSize.

    m_settings.sourceFile = sourceFile; // Store for use by QMediaPlayer (and other functions)

    // Initialize QMediaPlayer related states
    m_framesDeliveredBySink = 0;
    m_framesAcceptedBySR = 0;
    m_mediaPlayerPausedByBackpressure = false;
    m_qtVideoFrameBuffer.clear();
    m_allFramesDecoded = false; // Reset for the new video

    // Set connections and sink in constructor. Here, just set source.
    // The onMediaPlayerStatusChanged slot will handle QMediaPlayer::LoadedMedia to call m_mediaPlayer->play().
    m_mediaPlayer->setSource(QUrl::fromLocalFile(m_settings.sourceFile));
    if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Source set to" << m_settings.sourceFile << ". Waiting for LoadedMedia status.";
    emit statusChanged(m_currentRowNum, tr("Loading video (Qt)..."));


    // Encoder is started, but will wait for data.
    startPipeEncoder();
}

// Removed startQtMediaPlayerDecoder() as it's now effectively inlined in processVideo()
// and onMediaPlayerStatusChanged() handles the play().

// --- SLOTS AND HELPERS ---
void RealCuganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle) return;

    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);

    if (m_state == State::ProcessingImage) {
        if (!success) {
            emit logMessage(QString("RealCUGAN: Image processing failed. Exit code: %1").arg(exitCode));
            QByteArray errorMsg = m_process->readAllStandardError();
            if (!errorMsg.isEmpty()) emit logMessage(tr("RealCUGAN stderr: %1").arg(QString::fromLocal8Bit(errorMsg)));
        } else {
            emit logMessage(tr("RealCUGAN image processing finished successfully for row %1.").arg(m_currentRowNum));
        }
        // cleanup(); // General cleanup will be called by finalizePipedVideoProcessing or similar in new flow
        m_state = State::Idle; // Set idle after image processing
        emit processingFinished(m_currentRowNum, success);

    } else if (m_state == State::ProcessingVideoFrames) { // Old video frame processing
        if (!success) {
            emit logMessage(QString("RealCUGAN: Failed to process frame (old method). Aborting video task."));
            cleanupVideo();
            // cleanup(); // Call general cleanup
            m_state = State::Idle;
            emit processingFinished(m_currentRowNum, false);
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
    } else if (m_state == State::PipeProcessingSR) { // SR engine (m_process) finished for a piped frame
        if (!success) {
            emit logMessage(tr("RealCUGAN pipe processing for a frame failed. Exit code: %1, Status: %2").arg(exitCode).arg(exitStatus));
            QByteArray errorMsg = m_process->readAllStandardError();
            if (!errorMsg.isEmpty()) emit logMessage(tr("RealCUGAN stderr (pipe): %1").arg(QString::fromLocal8Bit(errorMsg)));
            finalizePipedVideoProcessing(false);
            return;
        }

        if (m_settings.verboseLog) qDebug() << "RealCUGAN (pipe) finished for one frame. Output should have been read.";

        // onReadyReadStandardOutput for m_process should have handled the data.
        // This slot mainly confirms the SR process exited cleanly for the frame.
        // Now, check if all frames are done.
        if (m_allFramesDecoded && m_currentDecodedFrameBuffer.isEmpty()) {
            if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running) {
                 m_ffmpegEncoderProcess->closeWriteChannel(); // Signal EOF to encoder
                 if (m_settings.verboseLog) qDebug() << "All frames processed by SR, closing encoder stdin.";
                 m_allFramesSentToEncoder = true; // Mark that we've told encoder it's the end.
            }
            m_state = State::PipeEncodingVideo;
        } else {
             // If not all frames decoded, or some still buffered, go back to decoding state
             // to await next frame or process buffer.
            m_state = State::PipeDecodingVideo; // Ready for next frame from QVideoSink or buffer

            // Update progress based on frames accepted by SR vs total estimated
            if (m_totalFramesEstimatePipe > 0) {
                emit fileProgress(m_currentRowNum, (100 * m_framesAcceptedBySR) / m_totalFramesEstimatePipe);
            }

            // Call processDecodedFrameBuffer to handle the next frame from m_qtVideoFrameBuffer
            // or to finalize if all frames are done.
            processDecodedFrameBuffer();
        }
    }
}

void RealCuganProcessor::onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus) // OLD VIDEO METHOD
{
    if (m_state == State::Idle) return; // Should not happen if called for old method

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString("FFmpeg task (old method) failed. State: %1, Exit Code: %2").arg((int)m_state).arg(exitCode));
        QByteArray errorText = m_ffmpegProcess->readAllStandardError();
        if(!errorText.isEmpty()) emit logMessage(tr("FFmpeg stderr: %1").arg(QString::fromLocal8Bit(errorText)));
        cleanupVideo();
        // cleanup();
        m_state = State::Idle;
        emit processingFinished(m_currentRowNum, false);
        return;
    }

    if (m_state == State::SplittingVideo) {
        emit logMessage("Video splitting complete (old method). Starting frame processing...");
        emit statusChanged(m_currentRowNum, tr("Processing frames..."));
        m_state = State::ProcessingVideoFrames;

        QDir inputDir(m_video_inputFramesPath);
        QStringList frameFiles = inputDir.entryList(QStringList() << "*.png", QDir::Files, QDir::Name);
        m_video_frameQueue.clear();
        for (const QString &file : frameFiles) {
            m_video_frameQueue.enqueue(file);
        }
        m_video_totalFrames = m_video_frameQueue.size();
        m_video_processedFrames = 0;

        if (m_video_totalFrames > 0) {
            startNextVideoFrame();
        } else {
            emit logMessage("No frames found after splitting (old method). Aborting.");
            cleanupVideo();
            // cleanup();
            m_state = State::Idle;
            emit processingFinished(m_currentRowNum, false);
        }
    } else if (m_state == State::AssemblingVideo) {
        emit logMessage("Video assembly complete (old method).");
        emit statusChanged(m_currentRowNum, tr("Finished"));
        cleanupVideo();
        // cleanup();
        m_state = State::Idle;
        emit processingFinished(m_currentRowNum, true);
    }
}

void RealCuganProcessor::startNextVideoFrame() // OLD VIDEO METHOD
{
    if (m_video_frameQueue.isEmpty()) return; // Should be handled by caller logic
    if (m_state != State::ProcessingVideoFrames) return;

    QString frameFile = m_video_frameQueue.dequeue();
    QString inputFramePath = m_video_inputFramesPath + "/" + frameFile;
    QString outputFramePath = m_video_outputFramesPath + "/" + frameFile;
    QStringList args = buildArguments(inputFramePath, outputFramePath);
    if (m_settings.verboseLog) qDebug() << "RealCUGAN (old video frame) args:" << args.join(" ");
    m_process->start(m_settings.programPath, args);
}

void RealCuganProcessor::startVideoAssembly() // OLD VIDEO METHOD
{
    emit logMessage("All frames processed (old method). Assembling final video...");
    emit statusChanged(m_currentRowNum, tr("Assembling video..."));
    m_state = State::AssemblingVideo;
    QStringList args;
    QString inputFps = m_settings.videoFps > 0 ? QString::number(m_settings.videoFps) : "25";
    args << "-y" << "-framerate" << inputFps
         << "-i" << QDir::toNativeSeparators(m_video_outputFramesPath + "/frame%08d.png");

    if (QFile::exists(m_video_audioPath)) {
        args << "-i" << QDir::toNativeSeparators(m_video_audioPath) << "-c:a" << "copy";
    } else {
        args << "-an"; // No audio input
    }
    args << "-c:v" << m_settings.videoEncoderCodec
         << "-preset" << m_settings.videoEncoderPreset
         << "-crf" << QString::number(m_settings.videoEncoderCRF)
         << "-pix_fmt" << m_settings.videoOutputPixFmt
         << "-shortest" << QDir::toNativeSeparators(m_finalDestinationFile);

    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    if (m_settings.verboseLog) qDebug() << "FFmpeg assembly (old method) args:" << ffmpegPath << args.join(" ");
    m_ffmpegProcess->start(ffmpegPath, args);
}

QStringList RealCuganProcessor::buildArguments(const QString &inputFile, const QString &outputFile) // For images or old video frames
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

void RealCuganProcessor::onProcessError(QProcess::ProcessError error) {
    if (m_state == State::Idle && m_currentRowNum == -1) {
        // Process might have been killed during cleanup of a previous failed job,
        // and errorOccurred is emitted after 'finished'. Or it's an unexpected error.
        // If no active job (m_currentRowNum == -1), probably safe to ignore or just log verbosely.
        if (m_settings.verboseLog) {
            qDebug() << "RealCuganProcessor::onProcessError received in Idle state with no active job. Error:" << error << m_process->errorString();
        }
        return;
    }

    QString logMsgOwner = "RealCUGAN";
    if (m_state == State::ProcessingImage) logMsgOwner = "RealCUGAN (Image)";
    else if (m_state == State::PipeProcessingSR) logMsgOwner = "RealCUGAN (SR Pipe)";
    else if (m_state == State::ProcessingVideoFrames) logMsgOwner = "RealCUGAN (Old Video Frame)";


    QString errorUserMsg = tr("%1 process error: %2. Code: %3").arg(logMsgOwner).arg(m_process->errorString()).arg(error);
    emit logMessage(errorUserMsg);
    if (m_settings.verboseLog) { // Also log to qDebug for more detailed debugging if enabled
        qDebug() << errorUserMsg;
    }


    // Determine how to finalize based on state
    if (m_state == State::ProcessingImage) {
        m_state = State::Idle;
        emit processingFinished(m_currentRowNum, false);
    } else if (m_state == State::PipeProcessingSR || m_state == State::PipeDecodingVideo || m_state == State::PipeEncodingVideo) {
        // finalizePipedVideoProcessing calls cleanup() which sets state to Idle and processingFinished
        finalizePipedVideoProcessing(false);
    } else if (m_state == State::ProcessingVideoFrames) { // Old video method
        cleanupVideo(); // Specific cleanup for old method's temp files
        m_state = State::Idle;
        emit processingFinished(m_currentRowNum, false);
    } else {
        // Fallback for any other unexpected state, ensure cleanup and signal failure if a job was active
        State oldState = m_state;
        cleanup(); // General cleanup sets state to Idle
        if (m_currentRowNum != -1) {
            emit logMessage(tr("Error occurred in an unexpected state (%1) for job %2.").arg(static_cast<int>(oldState)).arg(m_currentRowNum));
            emit processingFinished(m_currentRowNum, false);
        }
    }
    // m_currentRowNum is reset by finalizePipedVideoProcessing or explicitly after emitting processingFinished
}

void RealCuganProcessor::onReadyReadStandardOutput() {
    if (m_state == State::PipeProcessingSR) {
        // SR engine in pipe mode outputs raw frame data to stdout
        qint64 bytesAvailableInSRProcess = m_process->bytesAvailable();
        bool readData = true;

        if (m_currentUpscaledFrameBuffer.size() + bytesAvailableInSRProcess > MAX_UPSCALED_BUFFER_SIZE && m_currentUpscaledFrameBuffer.size() > MAX_UPSCALED_BUFFER_SIZE / 2) {
            if (m_settings.verboseLog) {
                qDebug() << "Approaching MAX_UPSCALED_BUFFER_SIZE (" << MAX_UPSCALED_BUFFER_SIZE
                         << "bytes). Current upscaled buffer size:" << m_currentUpscaledFrameBuffer.size()
                         << ". Available from SR process:" << bytesAvailableInSRProcess
                         << ". Delaying read from SR process to allow encoder to catch up.";
            }
            // Don't read more from SR process for now if buffer is getting too full,
            // allow pipeToEncoder to drain it. OS pipe will exert backpressure on SR engine.
            readData = false;
        }

        if (readData && bytesAvailableInSRProcess > 0) {
            m_currentUpscaledFrameBuffer.append(m_process->readAllStandardOutput());
        }

        // Always try to pipe whatever is in the buffer.
        // pipeFrameToEncoder should ideally handle partial frames if that's possible,
        // or this assumes SR engine sends data in meaningful chunks (e.g., full frames).
        if (!m_currentUpscaledFrameBuffer.isEmpty()) {
            pipeFrameToEncoder(m_currentUpscaledFrameBuffer);
            m_currentUpscaledFrameBuffer.clear(); // Clear after attempting to send.
                                                 // If pipeFrameToEncoder buffers internally or blocks, this is fine.
        }
        // No percentage progress for piped video frames from SR engine stdout.
    } else if (m_state == State::ProcessingImage) {
        // Image processing outputs percentage progress
        QString output = QString::fromLocal8Bit(m_process->readAllStandardOutput());
        QRegularExpression re("(\\d+)%");
        QRegularExpressionMatch match = re.match(output);
        if (match.hasMatch()) {
            emit fileProgress(m_currentRowNum, match.captured(1).toInt());
        }
         // Optionally log other output if verbose and not matched as progress
        else if (m_settings.verboseLog && !output.trimmed().isEmpty()) {
            qDebug() << "RealCUGAN (Image) stdout:" << output.trimmed();
        }
    } else {
        // Output from m_process in other states? Log if verbose.
        if (m_settings.verboseLog) {
            QByteArray unexpectedData = m_process->readAllStandardOutput();
            if (!unexpectedData.isEmpty()) {
                qDebug() << "RealCUGAN m_process sent stdout data in unexpected state" << (int)m_state << ":" << QString::fromLocal8Bit(unexpectedData);
            }
        }
    }
}

void RealCuganProcessor::onFfmpegError(QProcess::ProcessError error) {
    // This is for the OLD video processing method (m_ffmpegProcess)
    if (m_state == State::Idle && m_currentRowNum == -1) {
        if (m_settings.verboseLog) {
            qDebug() << "RealCuganProcessor::onFfmpegError received in Idle state with no active job. Error:" << error << m_ffmpegProcess->errorString();
        }
        return;
    }
    // Check if it's one of the states that uses m_ffmpegProcess
    if (m_state == State::SplittingVideo || m_state == State::AssemblingVideo) {
        QString errorUserMsg = tr("FFmpeg (old video method) process error: %1. Code: %2").arg(m_ffmpegProcess->errorString()).arg(error);
        emit logMessage(errorUserMsg);
        if (m_settings.verboseLog) {
            qDebug() << errorUserMsg;
        }
        cleanupVideo();
        m_state = State::Idle;
        emit processingFinished(m_currentRowNum, false);
    } else if (m_settings.verboseLog) {
        // Log if error occurs in an unexpected state for m_ffmpegProcess
        qDebug() << "onFfmpegError called for m_ffmpegProcess in unexpected state:" << (int)m_state << "Error:" << error << m_ffmpegProcess->errorString();
    }
}

void RealCuganProcessor::onFfmpegStdErr() {
    emit logMessage("FFmpeg: " + QString::fromLocal8Bit(m_ffmpegProcess->readAllStandardError()).trimmed());
}

// ==================================================================================
// NEW Piped Video Processing Methods
// ==================================================================================

bool RealCuganProcessor::getVideoInfo(const QString& inputFile) {
    QProcess ffprobeProcess;
    QStringList probeArgs;
    // Attempt to get width, height, avg_frame_rate (more reliable than r_frame_rate for variable fps), nb_frames, duration
    probeArgs << "-v" << "error"
              << "-select_streams" << "v:0"
              << "-show_entries" << "stream=width,height,avg_frame_rate,nb_frames,duration"
              << "-of" << "default=noprint_wrappers=1:nokey=1"
              << inputFile;

    QString ffprobePath = m_settings.ffprobePath.isEmpty() ? "ffprobe" : m_settings.ffprobePath;
    if (m_settings.verboseLog) qDebug() << "Probing video:" << ffprobePath << probeArgs.join(" ");

    ffprobeProcess.start(ffprobePath, probeArgs);
    if (!ffprobeProcess.waitForStarted(5000)) {
        emit logMessage(tr("Failed to start ffprobe for video info."));
        return false;
    }
    if (!ffprobeProcess.waitForFinished(15000)) { // 15s timeout
        emit logMessage(tr("ffprobe timeout while getting video info."));
        ffprobeProcess.kill();
        return false;
    }

    if (ffprobeProcess.exitStatus() != QProcess::NormalExit || ffprobeProcess.exitCode() != 0) {
        emit logMessage(tr("ffprobe failed to get video info. Exit code: %1. Error: %2")
                        .arg(ffprobeProcess.exitCode())
                        .arg(QString::fromLocal8Bit(ffprobeProcess.readAllStandardError()).trimmed()));
        return false;
    }

    QString output = QString::fromLocal8Bit(ffprobeProcess.readAllStandardOutput()).trimmed();
    if (m_settings.verboseLog) qDebug() << "ffprobe output:" << output;
    QStringList info = output.split('\n', Qt::SkipEmptyParts);

    // Expected order: width, height, avg_frame_rate, nb_frames, duration
    if (info.size() < 3) {
        emit logMessage(tr("Failed to parse ffprobe output for essential video info (width, height, fps). Output: %1").arg(output));
        return false;
    }

    bool ok_w, ok_h;
    int width = info[0].toInt(&ok_w);
    int height = info[1].toInt(&ok_h);
    if (!ok_w || !ok_h || width <= 0 || height <= 0) {
        emit logMessage(tr("Invalid width/height from ffprobe: %1x%2").arg(info[0], info[1]));
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
             m_settings.videoFps = 25.0; // Default FPS
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
     if (m_totalFramesEstimatePipe <= 0) {
        emit logMessage(tr("Warning: Could not determine total frames from ffprobe, progress might be inaccurate or based on duration."));
        // If duration is also missing, this will be 0, progress will be an issue.
    }

    m_inputFrameChannels = 3; // We will request bgr24 from FFmpeg decoder
    m_outputFrameSize.setWidth(m_inputFrameSize.width() * m_settings.targetScale);
    m_outputFrameSize.setHeight(m_inputFrameSize.height() * m_settings.targetScale);

    // Extract audio to a temporary path
    // Create a unique temp directory for this job if it doesn't exist for audio
    m_tempVideoJobPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                           QString("/realcugan_pipe_job_%1").arg(QDateTime::currentMSecsSinceEpoch());
    QDir tempJobDir(m_tempVideoJobPath);
    if (!tempJobDir.mkpath(".")) {
        emit logMessage(tr("Error: Could not create temporary directory for audio: %1").arg(m_tempVideoJobPath));
        m_tempAudioPath.clear();
        m_tempVideoJobPath.clear(); // Clear if path creation failed
    } else {
        m_tempAudioPath = tempJobDir.filePath("audio.aac"); // Standardize to aac for simplicity
    }

    if (!m_tempAudioPath.isEmpty()) {
        QProcess extractAudioProcess;
        QStringList audioArgs;
        // Overwrite, input file, no video, copy audio codec, output path
        audioArgs << "-y" << "-i" << inputFile << "-vn" << "-acodec" << "copy" << m_tempAudioPath;

        QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
        if (m_settings.verboseLog) qDebug() << "Extracting audio:" << ffmpegPath << audioArgs.join(" ");

        extractAudioProcess.start(ffmpegPath, audioArgs);
        if (!extractAudioProcess.waitForFinished(15000)) {
            emit logMessage(tr("Audio extraction timeout. Proceeding without audio."));
            extractAudioProcess.kill();
            QFile::remove(m_tempAudioPath);
            m_tempAudioPath.clear();
        } else if (extractAudioProcess.exitCode() != 0) {
            emit logMessage(tr("Audio extraction failed. Error: %1. Proceeding without audio.")
                            .arg(QString::fromLocal8Bit(extractAudioProcess.readAllStandardError()).trimmed()));
            QFile::remove(m_tempAudioPath);
            m_tempAudioPath.clear();
        } else {
            if (!QFile::exists(m_tempAudioPath) || QFileInfo(m_tempAudioPath).size() == 0) {
                 emit logMessage(tr("Audio extraction seemed to succeed but no audio file found or audio is empty. Proceeding without audio."));
                 QFile::remove(m_tempAudioPath);
                 m_tempAudioPath.clear();
            } else if (m_settings.verboseLog) {
                 qDebug() << "Audio extracted to:" << m_tempAudioPath;
            }
        }
    }

    if (m_settings.verboseLog) {
        qDebug() << "Video Info: InputSize=" << m_inputFrameSize << "Channels=" << m_inputFrameChannels
                 << "FPS=" << m_settings.videoFps << "TotalFramesEst=" << m_totalFramesEstimatePipe
                 << "AudioPath=" << m_tempAudioPath << "OutputUpscaledSize=" << m_outputFrameSize;
    }

    return m_inputFrameSize.width() > 0 && m_inputFrameSize.height() > 0;
}


void RealCuganProcessor::startPipeDecoder() {
    if (m_state != State::PipeDecodingVideo) {
        emit logMessage(tr("Decoder started in incorrect state."));
        return;
    }

    QStringList decoderArgs;
    decoderArgs << "-i" << m_settings.sourceFile
                << "-vf" << QString("format=%1").arg(m_inputPixelFormat)
                << "-f" << "rawvideo"
                << "-an" // No audio from this pipe, audio handled separately
                << "pipe:1";

    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    if (m_settings.verboseLog) qDebug() << "Starting FFmpeg decoder:" << ffmpegPath << decoderArgs.join(" ");

    m_currentDecodedFrameBuffer.clear();
    m_ffmpegDecoderProcess->start(ffmpegPath, decoderArgs);
}

void RealCuganProcessor::onPipeDecoderReadyReadStandardOutput() {
    // This slot is for the old m_ffmpegDecoderProcess.
    // It's not used when QMediaPlayer/QVideoSink provides frames.
    // Kept for now in case of fallback or other uses, but should not be active in QMediaPlayer flow.
    if (m_settings.verboseLog) qDebug() << "onPipeDecoderReadyReadStandardOutput called - this should NOT happen in QMediaPlayer flow.";
    /*
    if (m_state != State::PipeDecodingVideo && m_state != State::PipeProcessingSR && m_state != State::PipeEncodingVideo) {
        if (m_state == State::Idle) return;
        if (m_settings.verboseLog) qDebug() << "Old Decoder output received in state: " << (int)m_state;
    }
    // ... (original body commented out as it pertains to m_ffmpegDecoderProcess) ...
    */
}

void RealCuganProcessor::processDecodedFrameBuffer() {
    // This function is now primarily driven by QVideoFrames from m_qtVideoFrameBuffer
    // or by SR process finishing, triggering the next frame.
    if (m_state == State::PipeEncodingVideo && m_allFramesDecoded) {
        // All frames decoded and sent to SR, now just waiting for encoder.
        // No new frames to process from decoder.
        return;
    }

    if (m_process && m_process->state() == QProcess::Running) {
        // SR engine is busy with a previous frame, wait for it to finish before sending another.
        // The buffer will be processed when SR finishes (its onProcessFinished will call this again or onReadyReadStandardOutput of SR will allow next).
        if (m_settings.verboseLog) qDebug() << "SR Engine busy, decoded frame(s) buffered.";
        return;
    }

    int frameByteSize = m_inputFrameSize.width() * m_inputFrameSize.height() * m_inputFrameChannels;
    if (frameByteSize <= 0) {
        emit logMessage(tr("Error: Invalid frame byte size calculation (%1x%2x%3).").arg(m_inputFrameSize.width()).arg(m_inputFrameSize.height()).arg(m_inputFrameChannels));
        finalizePipedVideoProcessing(false);
        return;
    }

    if (m_currentDecodedFrameBuffer.size() >= frameByteSize) {
        QByteArray frameData = m_currentDecodedFrameBuffer.left(frameByteSize);
        m_currentDecodedFrameBuffer.remove(0, frameByteSize);

        m_state = State::PipeProcessingSR;
        startRealCuganPipe(frameData);
    } else if (m_allFramesDecoded && m_currentDecodedFrameBuffer.isEmpty()) {
        // All frames were decoded, buffer is now empty, means all frames have been sent to SR.
        if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
            m_ffmpegEncoderProcess->closeWriteChannel();
            m_allFramesSentToEncoder = true;
            if (m_settings.verboseLog) qDebug() << "All frames decoded & sent to SR. Closing encoder input.";
        }
        // If SR is not running (meaning last frame was processed by SR and this is called from decoder finished)
        // then transition to encoding. If SR is running, its finish will handle this.
        if (m_process->state() == QProcess::NotRunning) {
             m_state = State::PipeEncodingVideo;
        }
    }
    // If buffer has data but less than a full frame, and decoder is not finished, wait for more data.
    // If decoder is finished and buffer has partial data, it's an error. (Handled in onPipeDecoderFinished)

    // This was the old logic for m_currentDecodedFrameBuffer (QByteArray from ffmpeg pipe).
    // The new logic for QVideoFrame based decoding is primarily in onQtVideoFrameChanged and
    // this function will now pull from m_qtVideoFrameBuffer.

    if (m_process && m_process->state() == QProcess::Running) {
        // SR engine is busy, cannot process another frame now.
        return;
    }

    if (m_qtVideoFrameBuffer.isEmpty()) {
        // No buffered QVideoFrames to process.
        // Check if all frames have been decoded and if it's time to close the encoder.
        if (m_allFramesDecoded && m_currentDecodedFrameBuffer.isEmpty() && (m_process == nullptr || m_process->state() == QProcess::NotRunning)) {
            if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
                m_ffmpegEncoderProcess->closeWriteChannel();
                m_allFramesSentToEncoder = true;
                if (m_settings.verboseLog) qDebug() << "processDecodedFrameBuffer: All frames processed, closing encoder input.";
            }
            if (m_state != State::PipeEncodingVideo && m_state != State::Idle) m_state = State::PipeEncodingVideo;
        }
        return;
    }

    // Dequeue a QVideoFrame
    QVideoFrame frame = m_qtVideoFrameBuffer.dequeue();
    m_framesAcceptedBySR++; // Increment here as we are about to process it for SR
    if (m_settings.verboseLog && m_framesAcceptedBySR % 100 == 0) {
        qDebug() << "processDecodedFrameBuffer: Processing buffered QVideoFrame " << m_framesAcceptedBySR << ". " << m_qtVideoFrameBuffer.size() << " remaining in QBuffer.";
    }


    QVideoFrame mappedFrame = frame;
    if (!mappedFrame.map(QVideoFrame::ReadOnly)) {
        emit logMessage(tr("Error: Could not map buffered QVideoFrame. Skipping frame."));
        // Try to resume player if it was paused for backpressure, as we've cleared one item.
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play();
            m_mediaPlayerPausedByBackpressure = false;
        }
        // Attempt to process next frame if any
        QTimer::singleShot(0, this, &RealCuganProcessor::processDecodedFrameBuffer); // Try next frame
        return;
    }

    QImage::Format targetFormat = (m_inputPixelFormat == "rgb24") ? QImage::Format_RGB888 : QImage::Format_BGR888;
    QImage image = mappedFrame.toImage().convertToFormat(targetFormat);
    mappedFrame.unmap();

    if (image.isNull()) {
        emit logMessage(tr("Error: Failed to convert buffered QVideoFrame to QImage. Skipping frame."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play();
            m_mediaPlayerPausedByBackpressure = false;
        }
        QTimer::singleShot(0, this, &RealCuganProcessor::processDecodedFrameBuffer); // Try next frame
        return;
    }

    m_currentDecodedFrameBuffer = QByteArray(reinterpret_cast<const char*>(image.constBits()), qsizetype(image.sizeInBytes()));

    if (m_currentDecodedFrameBuffer.isEmpty()) {
         emit logMessage(tr("Error: Converted QImage to QByteArray is empty. Skipping frame."));
         if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play();
            m_mediaPlayerPausedByBackpressure = false;
        }
        QTimer::singleShot(0, this, &RealCuganProcessor::processDecodedFrameBuffer); // Try next frame
        return;
    }

    // Now that m_currentDecodedFrameBuffer has data, send it to SR
    m_state = State::PipeProcessingSR; // Set state before starting SR
    startRealCuganPipe(m_currentDecodedFrameBuffer);
    m_currentDecodedFrameBuffer.clear(); // Clear after passing to startRealCuganPipe

    // If player was paused due to backpressure and the QVideoFrame buffer is now getting low, resume it.
    if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
        m_mediaPlayer->play();
        m_mediaPlayerPausedByBackpressure = false;
        if (m_settings.verboseLog) qDebug() << "processDecodedFrameBuffer: Resuming QMediaPlayer. QBuffer low.";
    }
}


void RealCuganProcessor::startRealCuganPipe(const QByteArray& frameData) {
    // State should be PipeProcessingSR, set by caller (processDecodedFrameBuffer)
     if (m_process->state() == QProcess::Running) {
        emit logMessage(tr("Error: Attempted to start RealCUGAN pipe while SR process is already running."));
        // This should ideally not happen if processDecodedFrameBuffer checks m_process->state()
        // However, if it does, we should probably re-queue or handle.
        // For now, just log and return. The frame data might be lost or overwritten if not handled.
        // A robust way would be to re-enqueue to m_qtVideoFrameBuffer if frameData came from QVideoFrame.
        // But frameData is QByteArray here. This indicates a logic flaw if this path is hit.
        return;
    }

    QStringList srArgs;
    srArgs << "--use-pipe"
           << "--input-width" << QString::number(m_inputFrameSize.width())
           << "--input-height" << QString::number(m_inputFrameSize.height())
           << "--input-channels" << QString::number(m_inputFrameChannels)
           << "--pixel-format" << (m_inputPixelFormat == "bgr24" ? "BGR" : (m_inputPixelFormat == "rgb24" ? "RGB" : "BGR")) // Basic mapping
           << "-s" << QString::number(m_settings.targetScale)
           << "-n" << QString::number(m_settings.denoiseLevel)
           << "-m" << m_settings.modelPath;

    if (m_settings.tileSize > 0) srArgs << "-t" << QString::number(m_settings.tileSize);
    else srArgs << "-t" << "0";

    if (!m_settings.singleGpuId.isEmpty() && m_settings.singleGpuId != "auto") srArgs << "-g" << m_settings.singleGpuId;
    else srArgs << "-g" << "auto";

    if (m_settings.ttaEnabled) srArgs << "-x";
    // Only add -v if our own verboseLog is true, to avoid flooding if not desired.
    // The SR engine's own verbose might be too much for regular piping.
    // if (m_settings.verboseLog) srArgs << "-v";

    if (m_settings.verboseLog) qDebug() << "Starting RealCUGAN (pipe):" << m_settings.programPath << srArgs.join(" ");

    m_currentUpscaledFrameBuffer.clear();

    QString program = m_settings.programPath;
    if (!QFileInfo(program).isAbsolute()) {
        program = QCoreApplication::applicationDirPath() + "/" + program;
    }

    m_process->start(program, srArgs);
    if (m_process->waitForStarted(5000)) {
        qint64 written = m_process->write(frameData);
        if (written != frameData.size()) {
            emit logMessage(tr("Error writing full frame to RealCUGAN stdin. Wrote %1 of %2").arg(written).arg(frameData.size()));
            finalizePipedVideoProcessing(false);
            return;
        }
        m_process->closeWriteChannel();
    } else {
        emit logMessage(tr("Failed to start RealCUGAN engine process for pipe: %1").arg(m_process->errorString()));
        finalizePipedVideoProcessing(false);
    }
}

void RealCuganProcessor::onPipeDecoderReadyReadStandardError() {
    QByteArray errorData = m_ffmpegDecoderProcess->readAllStandardError();
    // FFmpeg decoding often prints frame counts, time, bitrate etc to stderr.
    // We might want to parse this for more accurate progress or only log actual errors.
    QString stderrStr = QString::fromLocal8Bit(errorData).trimmed();
    if (m_settings.verboseLog || !stderrStr.contains("frame=")) {
         if (!stderrStr.isEmpty()) emit logMessage(tr("FFmpeg Decoder stderr: %1").arg(stderrStr));
    }
}

void RealCuganProcessor::onPipeDecoderFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    // This slot is for the old m_ffmpegDecoderProcess.
    // Not used in QMediaPlayer flow. EndOfMedia from QMediaPlayer handles this.
    if (m_settings.verboseLog) qDebug() << "onPipeDecoderFinished called - this should NOT happen in QMediaPlayer flow. ExitCode:" << exitCode << "Status:" << exitStatus;
    /*
    if (m_state == State::Idle) return;
    // ... (original body commented out) ...
    */
}

void RealCuganProcessor::onPipeDecoderError(QProcess::ProcessError error) {
    // This slot is for the old m_ffmpegDecoderProcess.
    // Not used in QMediaPlayer flow. onMediaPlayerError handles this.
    if (m_settings.verboseLog) qDebug() << "onPipeDecoderError called - this should NOT happen in QMediaPlayer flow. Error:" << error;
    /*
    if (m_state == State::Idle) return;
    // ... (original body commented out) ...
    */
}


void RealCuganProcessor::startPipeEncoder() {
    if (m_state == State::Idle) return;
    if (m_ffmpegEncoderProcess->state() == QProcess::Running) {
        emit logMessage(tr("Encoder already running.")); // Should not happen
        return;
    }

    QStringList encoderArgs;
    encoderArgs << "-y"
                << "-f" << "rawvideo"
                << "-pix_fmt" << m_inputPixelFormat
                << "-s" << QString("%1x%2").arg(m_outputFrameSize.width()).arg(m_outputFrameSize.height())
                << "-r" << QString::number(m_settings.videoFps,'f', 2) // Format FPS with decimals
                << "-i" << "pipe:0";

    if (!m_tempAudioPath.isEmpty() && QFile::exists(m_tempAudioPath)) {
        encoderArgs << "-i" << QDir::toNativeSeparators(m_tempAudioPath) << "-c:a" << "copy";
    } else {
        encoderArgs << "-an";
    }

    encoderArgs << "-c:v" << m_settings.videoEncoderCodec
                << "-preset" << m_settings.videoEncoderPreset // Preset might not apply to all codecs, but common for x264/x265
                << "-crf" << QString::number(m_settings.videoEncoderCRF) // CRF applies to x264/x265, others might use -b:v
                << "-pix_fmt" << m_settings.videoOutputPixFmt
                << QDir::toNativeSeparators(m_finalDestinationFile);

    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    if (m_settings.verboseLog) qDebug() << "Starting FFmpeg encoder:" << ffmpegPath << encoderArgs.join(" ");

    m_ffmpegEncoderProcess->start(ffmpegPath, encoderArgs);
     if (!m_ffmpegEncoderProcess->waitForStarted(5000)) {
        emit logMessage(tr("Failed to start FFmpeg encoder process: %1").arg(m_ffmpegEncoderProcess->errorString()));
        finalizePipedVideoProcessing(false);
    } else {
        // State transition to PipeEncodingVideo should occur when first frame is actually sent to encoder,
        // or when SR processing is all done and we are only waiting for encoder.
        // For now, we've just started it. The state will be managed by data flow.
        if (m_settings.verboseLog) qDebug() << "FFmpeg encoder started.";
    }
}

void RealCuganProcessor::pipeFrameToEncoder(const QByteArray& upscaledFrameData) {
    if (m_state == State::Idle || !m_ffmpegEncoderProcess || m_ffmpegEncoderProcess->state() != QProcess::Running) {
        if (m_state != State::Idle) {
             emit logMessage(tr("Error: FFmpeg encoder not running when trying to pipe frame. Current state: %1").arg((int)m_state));
        }
        return;
    }
    if (m_settings.verboseLog) qDebug() << "Piping" << upscaledFrameData.size() << "bytes to encoder.";
    qint64 bytesWritten = m_ffmpegEncoderProcess->write(upscaledFrameData);
    if (bytesWritten != upscaledFrameData.size()) {
        emit logMessage(tr("Error writing full frame to FFmpeg encoder pipe. Wrote %1 of %2 bytes.").arg(bytesWritten).arg(upscaledFrameData.size()));
        // This could indicate the encoder process has died or the pipe is broken.
        // Consider this a fatal error for the current video processing.
        finalizePipedVideoProcessing(false);
    }
}

void RealCuganProcessor::onPipeEncoderReadyReadStandardError() {
    if(!m_ffmpegEncoderProcess) return;
    QByteArray errorData = m_ffmpegEncoderProcess->readAllStandardError();
     if (!errorData.trimmed().isEmpty()) {
        QString stderrStr = QString::fromLocal8Bit(errorData).trimmed();
        if (m_settings.verboseLog || (!stderrStr.contains("frame=") && !stderrStr.contains("bitrate="))) {
            emit logMessage(tr("FFmpeg Encoder stderr: %1").arg(stderrStr));
        }
        // Could parse "frame=" here for more granular progress if desired,
        // but overall progress is already based on m_framesProcessedPipe / m_totalFramesEstimatePipe
    }
}

void RealCuganProcessor::onPipeEncoderFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (m_state == State::Idle && exitCode == 0 && exitStatus == QProcess::NormalExit) {
        // If already finalized successfully, do nothing.
        return;
    }
    if (m_state == State::Idle && (exitCode !=0 || exitStatus != QProcess::NormalExit)) {
        // If already finalized due to an error, also do nothing more here.
        return;
    }

    if (m_settings.verboseLog) qDebug() << "FFmpeg Encoder finished. ExitCode:" << exitCode << "Status:" << exitStatus;

    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    if(!success && m_state != State::Idle) { // Don't emit error if already cleaned up and idled
        emit logMessage(tr("FFmpeg Encoder failed. Exit: %1, Status: %2").arg(exitCode).arg(exitStatus));
        // Stderr already read by onPipeEncoderReadyReadStandardError
    }
    finalizePipedVideoProcessing(success);
}

void RealCuganProcessor::onPipeEncoderError(QProcess::ProcessError error) {
    if (m_state == State::Idle) return;
    emit logMessage(tr("FFmpeg Encoder process error: %1. Error code: %2").arg(m_ffmpegEncoderProcess->errorString()).arg(error));
    finalizePipedVideoProcessing(false);
}

void RealCuganProcessor::finalizePipedVideoProcessing(bool success) {
    State currentState = m_state; // Capture state before cleanup changes it.

    if (m_settings.verboseLog) qDebug() << "Finalizing piped video processing. Success:" << success << "Current State:" << (int)currentState;

    // Ensure all pipe-related processes are stopped.
    cleanupQtMediaPlayer(); // Stop QMediaPlayer if it was used
    // cleanup() will call cleanupPipeProcesses() for SR and Encoder.
    cleanup(); // This sets state to Idle, kills QProcess members, and cleans general vars.

    if (currentState != State::Idle) { // Only emit processingFinished if we weren't already idle.
        if (success) {
            emit logMessage(tr("Piped video processing finished successfully for: %1").arg(QFileInfo(m_finalDestinationFile).fileName()));
            emit statusChanged(m_currentRowNum, tr("Finished"));
        } else {
            emit logMessage(tr("Piped video processing failed for: %1").arg(QFileInfo(m_settings.sourceFile).fileName())); // Use sourceFile for error source
            emit statusChanged(m_currentRowNum, tr("Error"));
            if (!m_finalDestinationFile.isEmpty() && QFile::exists(m_finalDestinationFile)) {
                if (QFile::remove(m_finalDestinationFile)) {
                    emit logMessage(tr("Partially created output file %1 removed.").arg(m_finalDestinationFile));
                } else {
                    emit logMessage(tr("Warning: Could not remove partially created output file %1.").arg(m_finalDestinationFile));
                }
            }
        }
        emit processingFinished(m_currentRowNum, success);
    } else if (!success && m_currentRowNum != -1 && currentState != State::Idle) { // ensure not already idle when this is called
        // If it was already idle but an error callback is trying to finalize,
        // ensure the finish signal for that job is emitted as error if not already.
        // This is a safeguard.
        emit processingFinished(m_currentRowNum, false);
        // m_currentRowNum is reset by cleanup()
    }
    // m_currentRowNum is reset by cleanup()
}

// --- New slots for QMediaPlayer/QVideoSink ---
void RealCuganProcessor::onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (m_state != State::PipeDecodingVideo) return; // Only act if we are in the relevant state

    switch (status) {
    case QMediaPlayer::LoadedMedia:
        if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Media loaded. Total frames estimate (from ffprobe):" << m_totalFramesEstimatePipe;
        m_mediaPlayer->play();
        emit statusChanged(m_currentRowNum, tr("Decoding (Qt)..."));
        break;
    case QMediaPlayer::EndOfMedia:
        if (m_settings.verboseLog) qDebug() << "QMediaPlayer: EndOfMedia reached. Frames delivered by sink:" << m_framesDeliveredBySink << "Frames accepted by SR:" << m_framesAcceptedBySR;
        m_allFramesDecoded = true; // Signal that QMediaPlayer has finished sending frames.
        // Process any remaining frames in m_qtVideoFrameBuffer or m_currentDecodedFrameBuffer
        processDecodedFrameBuffer(); // This will try to send any last buffered QVideoFrame
                                     // and then if m_currentDecodedFrameBuffer is also empty, it will close encoder.
        // If after processDecodedFrameBuffer, everything is truly empty and SR is not running:
    // This check is now more robustly handled within processDecodedFrameBuffer itself when m_allFramesDecoded is true.
    // No need to explicitly check m_process state here again as processDecodedFrameBuffer does that.
    if (m_qtVideoFrameBuffer.isEmpty() && m_currentDecodedFrameBuffer.isEmpty()) {
         // If processDecodedFrameBuffer didn't transition state (e.g. SR was still running one last frame)
         // but now everything is clear, ensure we are in encoding state or let processDecodedFrameBuffer handle it.
         // The crucial part is that processDecodedFrameBuffer is called. If it determines everything is done,
         // it will close the encoder pipe and set state.
        if (m_process == nullptr || m_process->state() == QProcess::NotRunning) {
            if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
                 m_ffmpegEncoderProcess->closeWriteChannel();
                 m_allFramesSentToEncoder = true;
                 if (m_settings.verboseLog) qDebug() << "QMediaPlayer EndOfMedia: All frames processed by SR, closing encoder input.";
            }
            if (m_state != State::Idle) m_state = State::PipeEncodingVideo; // Transition to waiting for encoder if not already idle
            }
    } // else: processDecodedFrameBuffer will continue to be called as SR finishes frames.
        break;
    case QMediaPlayer::InvalidMedia:
        emit logMessage(tr("QMediaPlayer Error: Invalid media - %1").arg(m_settings.sourceFile));
        finalizePipedVideoProcessing(false);
        break;
    case QMediaPlayer::NoMedia:
    if (m_settings.verboseLog) qDebug() << "QMediaPlayer: NoMedia status (possibly after stop/cleanup or error).";
    // If this happens during active processing, it might be an issue.
    if (m_state == State::PipeDecodingVideo) {
        emit logMessage(tr("QMediaPlayer Error: No media present during decoding. Source: %1").arg(m_settings.sourceFile));
        finalizePipedVideoProcessing(false);
    }
        break;
    case QMediaPlayer::BufferingMedia:
        if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Buffering media...";
        emit statusChanged(m_currentRowNum, tr("Buffering (Qt)..."));
        break;
    case QMediaPlayer::BufferedMedia:
        if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Media buffered.";
    if (m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && !m_mediaPlayerPausedByBackpressure) {
        if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Media buffered, resuming playback.";
        m_mediaPlayer->play(); // Resume if it was paused for buffering, not by backpressure
    }
        break;
    case QMediaPlayer::LoadingMedia:
         if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Loading media...";
     // Status already set in processVideo
        break;
    default:
        if (m_settings.verboseLog) qDebug() << "QMediaPlayer: Unhandled media status:" << status;
        break;
    }
}

void RealCuganProcessor::onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    // Avoid acting on errors if we're already idle (e.g., after a successful cleanup or previous error)
    // unless it's a new error that wasn't the cause of idling.
    if (m_state == State::Idle && error == QMediaPlayer::NoError) return; // Player might emit NoError after stop.

    emit logMessage(tr("QMediaPlayer Error: %1 (Code: %2). Source: %3").arg(errorString).arg(error).arg(m_settings.sourceFile));
    finalizePipedVideoProcessing(false);
}

void RealCuganProcessor::onQtVideoFrameChanged(const QVideoFrame &frame)
{
    if (m_state != State::PipeDecodingVideo && m_state != State::PipeProcessingSR) {
        // If not in active decoding/processing state, just drop the frame.
        // This can happen if EndOfMedia was reached and this signal arrives late.
        if (frame.isValid() && m_settings.verboseLog) {
            qDebug() << "QVideoSink: Frame received in non-processing state" << (int)m_state << "- dropping.";
        }
        return;
    }

    if (!frame.isValid()) {
        if (m_settings.verboseLog) qDebug() << "QVideoSink: Invalid frame received.";
        return;
    }

    m_framesDeliveredBySink++;
    if (m_settings.verboseLog && m_framesDeliveredBySink % 100 == 0) { // Log every 100 frames
        qDebug() << "QVideoSink: Frame" << m_framesDeliveredBySink << "received. Format:" << frame.pixelFormat();
    }


    // Backpressure mechanism: If SR process is busy OR qtVideoFrameBuffer is large, pause QMediaPlayer
    if ( (m_process && m_process->state() == QProcess::Running) || m_qtVideoFrameBuffer.size() > 5) { // Buffer 5 frames max from QVideoSink
        if (m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PlayingState && !m_mediaPlayerPausedByBackpressure) {
            m_mediaPlayer->pause();
            m_mediaPlayerPausedByBackpressure = true;
            if (m_settings.verboseLog) qDebug() << "QVideoSink: Pausing QMediaPlayer due to backpressure. SR Busy or Frame Buffer Full. Buffered QVideoFrames:" << m_qtVideoFrameBuffer.size();
        }
        // Enqueue the frame for later processing
        m_qtVideoFrameBuffer.enqueue(frame); // QVideoFrame is implicitly shared
        return; // Don't process immediately if backpressure applied
    }

    // If no backpressure or it was just released, try to process this frame or a buffered one.
    QVideoFrame frameToProcess;
    if (!m_qtVideoFrameBuffer.isEmpty()) {
        frameToProcess = m_qtVideoFrameBuffer.dequeue();
         if (m_settings.verboseLog) qDebug() << "QVideoSink: Processing a buffered QVideoFrame. " << m_qtVideoFrameBuffer.size() << "remaining in buffer.";
    } else {
        frameToProcess = frame; // Process current frame directly
    }

    QVideoFrame mappedFrame = frameToProcess; // Make a copy for mapping
    if (!mappedFrame.map(QVideoFrame::ReadOnly)) {
        emit logMessage(tr("Error: Could not map QVideoFrame."));
        // This is a problem. Can't get data. Maybe try to continue for other frames or abort?
        // For now, let's try to continue, but log it.
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
             m_mediaPlayer->play(); // Try to resume if paused
             m_mediaPlayerPausedByBackpressure = false;
        }
        return;
    }

    // Ensure pixel format is suitable for RealCUGAN (e.g. BGR24 or RGB24)
    // RealCUGAN pipe input expects raw BGR typically.
    // QVideoFrame::pixelFormat() can be complex (YUV formats etc.)
    // For simplicity, convert to a QImage in a known format like Format_RGB888 or Format_BGR888
    // Then get bits from QImage. This adds a conversion step but ensures correct format.
    QImage::Format targetFormat = QImage::Format_BGR888; // RealCUGAN often prefers BGR.
    if (m_inputPixelFormat == "rgb24") targetFormat = QImage::Format_RGB888;


    QImage image = mappedFrame.toImage().convertToFormat(targetFormat);
    mappedFrame.unmap(); // Unmap original as soon as possible

    if (image.isNull()) {
        emit logMessage(tr("Error: Failed to convert QVideoFrame to QImage or target format."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
             m_mediaPlayer->play(); // Try to resume if paused
             m_mediaPlayerPausedByBackpressure = false;
        }
        return;
    }

    // Now m_currentDecodedFrameBuffer holds the raw pixels in BGR888 or RGB888 format
    m_currentDecodedFrameBuffer = QByteArray(reinterpret_cast<const char*>(image.constBits()), qsizetype(image.sizeInBytes()));


    // At this point, m_currentDecodedFrameBuffer has one frame.
    // The existing processDecodedFrameBuffer will take this and send it to SR engine.
    // It checks if SR is busy. If it is, this frame data will just sit in m_currentDecodedFrameBuffer.
    // This is a potential issue: onQtVideoFrameChanged might be called again before SR is free,
    // overwriting m_currentDecodedFrameBuffer.
    // SOLUTION: processDecodedFrameBuffer must now handle the m_qtVideoFrameBuffer directly,
    // or this function should only add to m_currentDecodedFrameBuffer if it's empty and SR is free.

    // Revised logic: this function just prepares data in m_currentDecodedFrameBuffer if SR is free.
    // If SR is busy, the frame was already enqueued to m_qtVideoFrameBuffer.
    // The call to processDecodedFrameBuffer will happen when SR finishes.

    m_framesAcceptedBySR++;
    if (m_settings.verboseLog && m_framesAcceptedBySR % 100 == 0) {
         qDebug() << "QVideoSink: Frame" << m_framesAcceptedBySR << "being sent to SR processing logic.";
    }
    startRealCuganPipe(m_currentDecodedFrameBuffer); // This starts the SR engine if not busy
    m_currentDecodedFrameBuffer.clear(); // Clear after sending to SR pipe.

    // If player was paused for backpressure and now the buffer is low, resume
    if (m_mediaPlayerPausedByBackpressure && m_qtVideoFrameBuffer.size() < 2 && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
        m_mediaPlayer->play();
        m_mediaPlayerPausedByBackpressure = false;
        if (m_settings.verboseLog) qDebug() << "QVideoSink: Resuming QMediaPlayer. Buffer low.";
    }

    // After processing current frame (or enqueuing it), try to process from buffer if SR is free.
    // This ensures buffered frames are processed if SR becomes available.
    if (m_process && m_process->state() == QProcess::NotRunning && !m_qtVideoFrameBuffer.isEmpty()) {
        processDecodedFrameBuffer(); // This will now try to take from m_qtVideoFrameBuffer
    }
}
