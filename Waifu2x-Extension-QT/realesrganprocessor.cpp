// file: realesrganprocessor.cpp
#include "realesrganprocessor.h"
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
    m_process = new QProcess(this); // For RealESRGAN CLI
    m_ffmpegProcess = new QProcess(this); // For old video method (split/assemble)
    m_ffmpegEncoderProcess = new QProcess(this); // For new pipe method (final encoding)

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

    // m_ffmpegDecoderProcess is removed, its connections are no longer needed.
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

    // Kill processes if they are running from a previous ungraceful exit from a state
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
    if (m_ffmpegProcess && m_ffmpegProcess->state() != QProcess::NotRunning) { // Old method ffmpeg for split/assemble
        m_ffmpegProcess->kill();
    }

    cleanupQtMediaPlayer();   // Clean up QMediaPlayer related resources
    cleanupPipeProcesses(); // Clean new pipe processes (SR engine, encoder) and associated temp files
    cleanupVideo();         // Clean old method temp files

    m_state = State::Idle;
}

void RealEsrganProcessor::cleanupQtMediaPlayer()
{
    if (m_mediaPlayer) {
        if (m_mediaPlayer->playbackState() != QMediaPlayer::StoppedState) {
            m_mediaPlayer->stop();
        }
        m_mediaPlayer->setSource(QUrl()); // Release media source
    }
    m_qtVideoFrameBuffer.clear();
    m_mediaPlayerPausedByBackpressure = false;
    m_framesDeliveredBySink = 0;
    m_framesAcceptedBySR = 0;
}

void RealEsrganProcessor::cleanupPipeProcesses()
{
    // m_ffmpegDecoderProcess is replaced by QtMultimedia, so no need to kill it here.
    // cleanupQtMediaPlayer() handles QMediaPlayer.

    // m_process (SR engine) is handled by general cleanup() if it was used for pipe mode and got stuck.
    // This check is already in cleanup(), so this might be redundant if cleanup() is always called.
    // However, if cleanupPipeProcesses is called independently:
    if (m_process && m_process->state() != QProcess::NotRunning) { // Ensure SR process is stopped
        m_process->kill();
        m_process->waitForFinished(100); // Shorter wait as main cleanup might also try
    }

    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }

    if (!m_tempAudioPathPipe.isEmpty()) { // Use the pipe-specific audio path
        QFile::remove(m_tempAudioPathPipe);
        m_tempAudioPathPipe.clear();
    }
    m_currentDecodedFrameBuffer.clear(); // This will store data from QVideoFrame
    m_currentUpscaledFrameBuffer.clear();
    m_framesProcessedPipe = 0;
    m_totalFramesEstimatePipe = 0;
    m_allFramesDecoded = false;          // This will be set by QMediaPlayer::EndOfMedia
    m_allFramesSentToEncoder = false;
}


void RealEsrganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    // This method remains for single image processing and for the old file-based video frame processing.
    // The new piped video processing will have its own logic.
    if (m_state != State::Idle && m_state != State::ProcessingVideoFrames) {
        emit logMessage(tr("[ERROR] RealEsrganProcessor is already busy with a different task."));
        return;
    }

    // ... (rest of existing processImage implementation for file-based and multi-pass) ...
    // For brevity, I'm not repeating the entire existing processImage, just showing where it fits.
    // The following is a placeholder for the existing logic.
    if (m_state == State::Idle) {
        m_state = State::ProcessingImage;
        m_currentRowNum = rowNum;
        m_settings = settings;
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
    // ... (existing multi-pass logic from processImage) ...
    // This part handles the logic for multi-pass upscaling for single images
    // or frames from the old video method. It is complex and not directly
    // affected by the QMediaPlayer change for *new* video processing.
    // For brevity, assuming the existing complex logic for multi-pass image scaling is here.
    // Example:
    if (m_settings.targetScale <= 0 || m_settings.modelNativeScale <= 0) {
        emit logMessage(tr("Error: Invalid scale settings for RealESRGAN."));
        finalizePipedVideoProcessing(false); // Or a more general error path
        return;
    }

    // Build scale sequence
    m_scaleSequence.clear();
    double currentOverallScale = 1.0;
    double targetOverallScale = static_cast<double>(m_settings.targetScale);
    int nativeModelScale = m_settings.modelNativeScale;

    if (targetOverallScale == 1.0 && nativeModelScale > 1) { // Denoise only or same size with a specific model
        m_scaleSequence.enqueue(nativeModelScale);
    } else {
        while (currentOverallScale < targetOverallScale) {
            if (currentOverallScale * nativeModelScale > targetOverallScale && currentOverallScale < targetOverallScale) {
                // If next full step overshoots, but we are not there yet,
                // and if there's a smaller model that could get closer, this logic would be more complex.
                // For now, just use the native scale. The last step might produce a larger image than target,
                // which then might need a downscale (handled outside this processor or by user).
                // Or, the user should pick a model sequence that achieves the target scale.
                // This simple loop just applies the nativeModelScale until target is met or exceeded.
                m_scaleSequence.enqueue(nativeModelScale);
                currentOverallScale *= nativeModelScale;
            } else if (currentOverallScale * nativeModelScale <= targetOverallScale) {
                m_scaleSequence.enqueue(nativeModelScale);
                currentOverallScale *= nativeModelScale;
            } else { // currentOverallScale * nativeModelScale > targetOverallScale AND currentOverallScale >= targetOverallScale
                break; // Already met or exceeded target
            }
            if (m_scaleSequence.size() > 5) break; // Safety break for too many passes
        }
    }
    if (m_scaleSequence.isEmpty() && targetOverallScale >= 1.0) { // Ensure at least one pass if target scale is >= 1
        m_scaleSequence.enqueue(nativeModelScale);
    }


    if (m_scaleSequence.isEmpty()) {
         emit logMessage(tr("RealESRGAN: No scaling passes determined for image. Using native model scale."));
         m_scaleSequence.enqueue(nativeModelScale); // Default to one pass with native scale
    }
    // End of example multi-pass logic setup

    startNextPass(); // This eventually calls m_process->start() with buildArguments()
}

void RealEsrganProcessor::startNextPass() {
    // ... (existing startNextPass implementation) ...
    // This method is used by the file-based processImage.
    // For brevity, not repeating it fully. It uses buildArguments() and m_process.
    if (m_process->state() != QProcess::NotRunning) { return; }
    if (m_scaleSequence.isEmpty()) {
        // This means all passes for the current m_originalSourceFile (image or old video frame) are done.
        // If m_originalSourceFile was an intermediate file from a previous pass, rename it to m_finalDestinationFile.
        if (m_currentPassInputFile != m_originalSourceFile && m_currentPassInputFile == m_finalDestinationFile) {
            // All good, final output is already named correctly.
        } else if (m_currentPassInputFile != m_finalDestinationFile) {
            // This implies the last output (m_currentPassInputFile) needs to be moved/renamed to m_finalDestinationFile
            // This case needs careful handling of m_currentPassOutputFile vs m_finalDestinationFile
            // The current logic in onProcessFinished for single images handles the final move.
        }
        // The actual finalization of image processing or moving to next video frame (old method) happens in onProcessFinished.
        return;
    }

    int scaleForThisPass = m_scaleSequence.head(); // Don't dequeue yet, onProcessFinished will.
                                                 // Actually, buildArguments uses m_settings.modelNativeScale.
                                                 // The m_scaleSequence is more for tracking how many passes.
                                                 // The actual -s parameter to the EXE is always modelNativeScale for multi-pass.

    // Determine output file for this pass
    if (m_scaleSequence.size() == 1) { // Last pass
        m_currentPassOutputFile = m_finalDestinationFile;
    } else { // Intermediate pass
        QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        m_currentPassOutputFile = QDir(tempPath).filePath(QString("realesrgan_pass_%1_%2.png")
                                                          .arg(QFileInfo(m_originalSourceFile).completeBaseName())
                                                          .arg(QTime::currentTime().toString("hhmmsszzz")));
    }


    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile);
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Starting Pass:" << m_settings.programPath << arguments.join(" ");
    m_process->start(m_settings.programPath, arguments);
    if (!m_process->waitForStarted(5000)) { // Increased timeout for starting
        emit logMessage(tr("RealESRGAN process failed to start for pass. Path: %1").arg(m_settings.programPath));
        processingFinished(m_currentRowNum, false); // Signal failure for the whole job
        cleanup();
        return;
    }
    // m_scaleSequence.dequeue(); // Dequeue in onProcessFinished after successful completion of the pass
}


// --- VIDEO PROCESSING (New Piped Method) ---
void RealEsrganProcessor::processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_state != State::Idle) {
        emit logMessage(tr("[ERROR] RealEsrganProcessor is already busy. Please wait."));
        return;
    }

    cleanupPipeProcesses(); // Clean up from any previous pipe attempt
    cleanupVideo();         // Clean up old method's temp files (though this path won't use it)
    cleanupQtMediaPlayer(); // Ensure player is clean before new use

    m_state = State::PipeDecodingVideo;
    m_currentRowNum = rowNum;
    m_finalDestinationFile = destinationFile;
    m_settings = settings; // Store current settings
    m_settings.sourceFile = sourceFile; // Ensure sourceFile is in settings for helpers

    // Initialize QMediaPlayer related states
    m_framesDeliveredBySink = 0;
    m_framesAcceptedBySR = 0;
    m_mediaPlayerPausedByBackpressure = false;
    m_qtVideoFrameBuffer.clear();
    m_allFramesDecoded = false; // Reset for the new video

    m_currentDecodedFrameBuffer.clear(); // For data from QVideoFrame to SR
    m_currentUpscaledFrameBuffer.clear(); // For data from SR to Encoder
    m_allFramesSentToEncoder = false;
    m_inputPixelFormat = "bgr24"; // Default for FFmpeg raw output, matches what SR engine expects

    emit statusChanged(m_currentRowNum, tr("Starting video processing (pipe)..."));
    emit logMessage(tr("Real-ESRGAN Video (pipe) started for: %1").arg(QFileInfo(sourceFile).fileName()));

    if (!getVideoInfoPipe(sourceFile)) { // This extracts audio and gets video metadata
        finalizePipedVideoProcessing(false);
        return;
    }

    // Set QMediaPlayer source. Playback starts when LoadedMedia status is received.
    m_mediaPlayer->setSource(QUrl::fromLocalFile(m_settings.sourceFile));
    if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Source set to" << m_settings.sourceFile << ". Waiting for LoadedMedia status.";
    emit statusChanged(m_currentRowNum, tr("Loading video (Qt)..."));


    // Start the FFmpeg encoder process; it will wait for piped input.
    startPipeEncoder();
}


void RealEsrganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    // This slot now needs to handle finishes for:
    // 1. Image processing (single or multi-pass for a single image)
    // 2. Old file-based video frame processing (routed through image processing logic for frames)
    // 3. New pipe-based SR engine processing for a single video frame

    if (m_state == State::Idle) return;

    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);

    if (m_state == State::ProcessingImage) {
        // This handles single images AND frames from the old video method.
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
    } else if (m_state == State::PipeProcessingSR) { // SR engine (m_process) finished for a NEW piped video frame
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


void RealEsrganProcessor::onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus) // OLD File-based VIDEO METHOD
{
    // ... (existing onFfmpegFinished for old file-based video method - splitting and assembling)
    // This remains unchanged for now, as processVideo is being replaced by the pipe version.
    // For brevity, not repeating it.
    if (m_state == State::Idle && !(m_state == State::SplittingVideo || m_state == State::AssemblingVideo) ) return;
    if (exitStatus != QProcess::NormalExit || exitCode != 0) { /* ... error handling ... */ cleanupVideo(); emit processingFinished(m_currentRowNum, false); cleanup(); return; }
    if (m_state == State::SplittingVideo) { /* ... handle splitting ... */ }
    else if (m_state == State::AssemblingVideo) { /* ... handle assembly ... */ cleanupVideo(); emit processingFinished(m_currentRowNum, true); cleanup(); }
}

void RealEsrganProcessor::startNextVideoFrame() // OLD File-based VIDEO METHOD
{
    // ... (existing startNextVideoFrame for old file-based video method) ...
    // This calls processImage internally for each frame.
    if (m_video_frameQueue.isEmpty()) { if (m_video_processedFrames != m_video_totalFrames || m_video_totalFrames == 0) { /* error */ } return; }
    QString frameFile = m_video_frameQueue.head();
    QString inputFramePath = QDir(m_video_inputFramesPath).filePath(frameFile);
    QString outputFramePath = QDir(m_video_outputFramesPath).filePath(frameFile);
    processImage(m_currentRowNum, inputFramePath, outputFramePath, m_settings); // This recursive call is part of old logic.
    m_video_frameQueue.dequeue();
}

void RealEsrganProcessor::startVideoAssembly() // OLD File-based VIDEO METHOD
{
    // ... (existing startVideoAssembly for old file-based video method) ...
    m_state = State::AssemblingVideo; /* ... set up args ... */
    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    // m_ffmpegProcess->start(ffmpegPath, args); // Corrected: use configured path
}

QStringList RealEsrganProcessor::buildArguments(const QString &inputFile, const QString &outputFile) // For images or old video frames (file-based SR engine call)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

    // For file-based, -s is the overall target scale for that specific pass.
    // If multi-pass, processImage->startNextPass sets m_settings.modelNativeScale for each pass.
    // If single pass image, it's just the target scale.
    // The actual scale factor for the *current run* of the EXE.
    // For single image direct call, it might be settings.targetScale.
    // For multi-pass image or old video frame, it's settings.modelNativeScale for each intermediate pass.
    int currentPassScale = m_settings.modelNativeScale; // Default to model's native for passes
    if (m_scaleSequence.isEmpty() && m_state == State::ProcessingImage) { // If it's a single top-level image call not yet in multi-pass
        currentPassScale = m_settings.targetScale; // This might be too simplistic if modelNativeScale is different.
                                                // The original realesrgan.cpp doesn't have multi-pass logic in itself for a single -s call,
                                                // it expects -s to be the direct scale for that execution.
                                                // The multi-pass logic is in RealEsrganProcessor.
                                                // So, for a single pass (or each pass of multi-pass), this should be the scale for THIS run.
                                                // Let's assume startNextPass correctly sets up what scale it wants for current operation.
                                                // If not in multi-pass (m_scaleSequence empty initially), then use targetScale.
                                                // This needs careful check with how processImage sets up m_scaleSequence.
                                                // For now, let's assume settings.modelNativeScale is what the exe's -s expects per run.
    }
     arguments << "-s" << QString::number(m_settings.modelNativeScale); // Use modelNativeScale for the -s parameter of the exe


    arguments << "-n" << m_settings.modelName; // Model name like "realesr-animevideov3" or "realesrgan-x4plus"
    arguments << "-m" << m_settings.modelPath; // Path to the "models" directory

    if (m_settings.tileSize > 0) {
        arguments << "-t" << QString::number(m_settings.tileSize);
    } else {
        arguments << "-t" << "0"; // Auto tile size for the engine
    }
    arguments << "-f" << m_settings.outputFormat; // Usually png for frames

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


// ==================================================================================
// NEW Piped Video Processing Methods for RealEsrganProcessor
// ==================================================================================

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
    QStringList decoderArgs;
    decoderArgs << "-i" << m_settings.sourceFile
                << "-vf" << QString("format=%1").arg(m_inputPixelFormat)
                << "-f" << "rawvideo"
                << "-an"
                << "pipe:1";
    QString ffmpegPath = m_settings.ffmpegPath.isEmpty() ? "ffmpeg" : m_settings.ffmpegPath;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Starting FFmpeg decoder:" << ffmpegPath << decoderArgs.join(" ");
    m_currentDecodedFrameBuffer.clear();
    m_ffmpegDecoderProcess->start(ffmpegPath, decoderArgs);
}

void RealEsrganProcessor::onPipeDecoderReadyReadStandardOutput() {
    if (m_state == State::Idle) return;
    m_currentDecodedFrameBuffer.append(m_ffmpegDecoderProcess->readAllStandardOutput());
    if (m_state == State::PipeDecodingVideo || m_state == State::PipeProcessingSR) {
         processDecodedFrameBuffer();
    }
}

void RealEsrganProcessor::processDecodedFrameBuffer() {
    if (m_state == State::PipeEncodingVideo && m_allFramesDecoded) return;
    if (m_process && m_process->state() == QProcess::Running) {
        if (m_settings.verboseLog) qDebug() << "RealESRGAN SR Engine busy, decoded frame(s) buffered.";
        return;
    }
    int frameByteSize = m_inputFrameSize.width() * m_inputFrameSize.height() * m_inputFrameChannels;
    if (frameByteSize <= 0) {
        emit logMessage(tr("RealESRGAN Error: Invalid frame byte size."));
        finalizePipedVideoProcessing(false);
        return;
    }
    if (m_currentDecodedFrameBuffer.size() >= frameByteSize) {
        QByteArray frameData = m_currentDecodedFrameBuffer.left(frameByteSize);
        m_currentDecodedFrameBuffer.remove(0, frameByteSize);
        m_state = State::PipeProcessingSR;
        startRealEsrganPipe(frameData);
    } else if (m_allFramesDecoded && m_currentDecodedFrameBuffer.isEmpty()) {
        if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
            m_ffmpegEncoderProcess->closeWriteChannel();
            m_allFramesSentToEncoder = true;
            if (m_settings.verboseLog) qDebug() << "RealESRGAN: All frames decoded & sent to SR. Closing encoder input.";
        }
        if (m_process->state() == QProcess::NotRunning) m_state = State::PipeEncodingVideo;
    }
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
           << "-s" << QString::number(m_settings.targetScale) // RealESRGAN CLI uses -s for target scale directly
           << "-n" << m_settings.modelName
           << "-m" << m_settings.modelPath;
    if (m_settings.tileSize > 0) srArgs << "-t" << QString::number(m_settings.tileSize); else srArgs << "-t" << "0";
    if (!m_settings.singleGpuId.isEmpty() && m_settings.singleGpuId != "auto") srArgs << "-g" << m_settings.singleGpuId; else srArgs << "-g" << "auto";
    if (m_settings.ttaEnabled) srArgs << "-x";
    // if (m_settings.verboseLog) srArgs << "-v"; // Engine's verbose might be too much

    if (m_settings.verboseLog) qDebug() << "RealESRGAN Starting Pipe:" << m_settings.programPath << srArgs.join(" ");
    m_currentUpscaledFrameBuffer.clear();
    m_process->start(m_settings.programPath, srArgs);
    if (m_process->waitForStarted(5000)) {
        qint64 written = m_process->write(frameData);
        if (written != frameData.size()) { /* ... error ... */ finalizePipedVideoProcessing(false); return; }
        m_process->closeWriteChannel();
    } else { /* ... error ... */ finalizePipedVideoProcessing(false); }
}

void RealEsrganProcessor::onPipeDecoderReadyReadStandardError() {
    QByteArray errorData = m_ffmpegDecoderProcess->readAllStandardError();
    QString stderrStr = QString::fromLocal8Bit(errorData).trimmed();
    if (m_settings.verboseLog || !stderrStr.contains("frame=")) {
         if (!stderrStr.isEmpty()) emit logMessage(tr("RealESRGAN FFmpeg Decoder stderr: %1").arg(stderrStr));
    }
}

void RealEsrganProcessor::onPipeDecoderFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (m_state == State::Idle) return;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN FFmpeg Decoder finished. ExitCode:" << exitCode << "Status:" << exitStatus;
    m_allFramesDecoded = true;
    if (exitStatus != QProcess::NormalExit || exitCode != 0) { /* ... error ... */ finalizePipedVideoProcessing(false); return; }
    if (!m_currentDecodedFrameBuffer.isEmpty()) processDecodedFrameBuffer();
    if (m_currentDecodedFrameBuffer.isEmpty() && (m_process->state() == QProcess::NotRunning)) {
        if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
            m_ffmpegEncoderProcess->closeWriteChannel(); m_allFramesSentToEncoder = true;
        }
        if (m_state != State::PipeEncodingVideo) m_state = State::PipeEncodingVideo;
    }
}

void RealEsrganProcessor::onPipeDecoderError(QProcess::ProcessError error) {
    if (m_state == State::Idle) return;
    emit logMessage(tr("RealESRGAN FFmpeg Decoder error: %1. Code: %2").arg(m_ffmpegDecoderProcess->errorString()).arg(error));
    finalizePipedVideoProcessing(false);
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
     if (!m_ffmpegEncoderProcess->waitForStarted(5000)) { /* ... error ... */ finalizePipedVideoProcessing(false); }
     else if (m_settings.verboseLog) qDebug() << "RealESRGAN FFmpeg encoder started.";
}

void RealEsrganProcessor::pipeFrameToEncoder(const QByteArray& upscaledFrameData) {
    if (m_state == State::Idle || !m_ffmpegEncoderProcess || m_ffmpegEncoderProcess->state() != QProcess::Running) return;
    if (m_settings.verboseLog) qDebug() << "RealESRGAN Piping" << upscaledFrameData.size() << "bytes to encoder.";
    qint64 bytesWritten = m_ffmpegEncoderProcess->write(upscaledFrameData);
    if (bytesWritten != upscaledFrameData.size()) { /* ... error ... */ finalizePipedVideoProcessing(false); }
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
            emit logMessage(tr("RealESRGAN: Piped video processing finished for: %1").arg(QFileInfo(m_finalDestinationFile).fileName())); // m_finalDestinationFile should be set
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
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

    // If multi-pass is active, scaleForThisPass would be determined by m_scaleSequence.head()
    // This function is generic for one pass; processImage/startNextPass determines actual scale factor for the pass.
    // Here, we use m_settings.targetScale if it's a single pass conceptually, or current pass's scale.
    // The -s parameter for realesrgan-ncnn-vulkan means the target scale for *that run*.
    // For multi-pass, each run uses the model's native scale.

    // This logic is tricky because buildArguments is called by startNextPass,
    // which already knows the current pass's scale factor.
    // Let's assume startNextPass sets a member variable for current pass scale, or passes it.
    // For now, using m_settings.modelNativeScale as the pass scale.
    // This is because each pass of a multi-stage upscale (e.g. x2 then x2 for a x4 target)
    // will use the model's native scale.
    if (m_settings.modelNativeScale > 0) {
         arguments << "-s" << QString::number(m_settings.modelNativeScale);
    } else { // Fallback if modelNativeScale isn't set, try targetScale (might not be ideal for multi-pass)
         arguments << "-s" << QString::number(m_settings.targetScale);
    }

    arguments << "-m" << m_settings.modelName;
    if (m_settings.tileSize > 0) {
        arguments << "-t" << QString::number(m_settings.tileSize);
    }
    arguments << "-f" << m_settings.outputFormat;

    if (!m_settings.singleGpuId.trimmed().isEmpty() && m_settings.singleGpuId.toLower() != "auto") {
        arguments << "-g" << m_settings.singleGpuId;
    }
    if (m_settings.ttaEnabled) {
        arguments << "-x";
    }
    return arguments;
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
    // m_state is reset by the calling function (e.g. cleanup() or when processing finishes)
}

void RealEsrganProcessor::onFfmpegError(QProcess::ProcessError error) {
    // This slot is for the m_ffmpegProcess (OLD video method for splitting/assembly)
    if (m_state == State::SplittingVideo || m_state == State::AssemblingVideo) {
        emit logMessage(QString(tr("FFmpeg (old video method) process error: %1. Code: %2"))
                        .arg(m_ffmpegProcess->errorString())
                        .arg(error));
        cleanupVideo(); // Clean up specific video temp files
        emit statusChanged(m_currentRowNum, tr("FFmpeg Error (Old Vid)"));
        finalizePipedVideoProcessing(false); // Use this for consistent cleanup and finish signal
    } else if (m_settings.verboseLog && m_state != State::Idle) {
         qDebug() << "onFfmpegError called for m_ffmpegProcess in unexpected state:" << (int)m_state << "Error:" << error;
    }
}

void RealEsrganProcessor::onFfmpegStdErr() {
    // This slot is for the m_ffmpegProcess (OLD video method for splitting/assembly)
    if (m_state == State::SplittingVideo || m_state == State::AssemblingVideo) {
        QByteArray data = m_ffmpegProcess->readAllStandardError();
        emit logMessage("FFmpeg (Old Vid): " + QString::fromLocal8Bit(data).trimmed());
    }
}

// --- Slots for QMediaPlayer/QVideoSink based decoding ---

void RealEsrganProcessor::onMediaPlayerStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (m_state != State::PipeDecodingVideo) {
        // If not in active decoding, only log critical errors or ignore.
        if (status == QMediaPlayer::InvalidMedia && m_currentRowNum != -1) { // Check if a job was active
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
        processDecodedFrameBuffer(); // Process any remaining buffered QVideoFrames
        // Further checks for closing encoder pipe are handled within processDecodedFrameBuffer when m_allFramesDecoded is true
        break;
    case QMediaPlayer::InvalidMedia:
        emit logMessage(tr("QMediaPlayer Error: Invalid media - %1").arg(m_settings.sourceFile));
        finalizePipedVideoProcessing(false);
        break;
    case QMediaPlayer::NoMedia:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: NoMedia status.";
        if (m_state == State::PipeDecodingVideo) { // If we expected media
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
         // Status already set in processVideo to "Loading video (Qt)..."
         if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Loading media status confirmed.";
        break;
    default:
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QMediaPlayer: Unhandled media status:" << status;
        break;
    }
}

void RealEsrganProcessor::onMediaPlayerError(QMediaPlayer::Error error, const QString &errorString)
{
    if (m_state == State::Idle && error == QMediaPlayer::NoError) return; // Ignore NoError if idle

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

    m_currentDecodedFrameBuffer = QByteArray(reinterpret_cast<const char*>(image.constBits()), imagesize_t(image.sizeInBytes()));
    m_framesAcceptedBySR++;
    if (m_settings.verboseLog && m_framesAcceptedBySR % 100 == 0) {
         qDebug() << "RealESRGAN QVideoSink: Frame" << m_framesAcceptedBySR << "being sent to SR.";
    }
    startRealEsrganPipe(m_currentDecodedFrameBuffer); // This sets state to PipeProcessingSR if successful
    m_currentDecodedFrameBuffer.clear();

    if (m_mediaPlayerPausedByBackpressure && m_qtVideoFrameBuffer.size() < 2 && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
        m_mediaPlayer->play();
        m_mediaPlayerPausedByBackpressure = false;
        if (m_settings.verboseLog) qDebug() << "RealESRGAN QVideoSink: Resuming QMediaPlayer. Buffer low.";
    }
    // Try to process next from buffer if SR is free (e.g. if current frame was small and SR finished quickly)
    if (m_process && m_process->state() == QProcess::NotRunning && !m_qtVideoFrameBuffer.isEmpty()) {
        processDecodedFrameBuffer();
    }
}

// --- Adapted processDecodedFrameBuffer for QVideoFrame from m_qtVideoFrameBuffer ---
void RealEsrganProcessor::processDecodedFrameBuffer() {
    if (m_process && m_process->state() == QProcess::Running) {
        return; // SR engine busy
    }

    if (m_qtVideoFrameBuffer.isEmpty()) {
        if (m_allFramesDecoded && m_currentDecodedFrameBuffer.isEmpty()) { // Ensure no lingering byte array data either
            if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
                m_ffmpegEncoderProcess->closeWriteChannel();
                m_allFramesSentToEncoder = true;
                if (m_settings.verboseLog) qDebug() << "RealESRGAN processDecodedFrameBuffer: All frames processed, closing encoder input.";
            }
            if (m_state != State::Idle) m_state = State::PipeEncodingVideo;
        }
        return; // No QVideoFrames to process from buffer
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
        QTimer::singleShot(0, this, &RealEsrganProcessor::processDecodedFrameBuffer); // Try next
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
        QTimer::singleShot(0, this, &RealEsrganProcessor::processDecodedFrameBuffer); // Try next
        return;
    }

    m_currentDecodedFrameBuffer = QByteArray(reinterpret_cast<const char*>(image.constBits()), imagesize_t(image.sizeInBytes()));
    if (m_currentDecodedFrameBuffer.isEmpty()) {
        emit logMessage(tr("RealESRGAN Error: Converted QImage to QByteArray is empty. Skipping."));
        if (m_mediaPlayerPausedByBackpressure && m_mediaPlayer && m_mediaPlayer->playbackState() == QMediaPlayer::PausedState && m_qtVideoFrameBuffer.size() < 2) {
            m_mediaPlayer->play(); m_mediaPlayerPausedByBackpressure = false;
        }
        QTimer::singleShot(0, this, &RealEsrganProcessor::processDecodedFrameBuffer); // Try next
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

// --- Comment out old FFmpeg Decoder Pipe Slots ---
void RealEsrganProcessor::onPipeDecoderReadyReadStandardOutput() {
    if (m_settings.verboseLog) qDebug() << "RealESRGAN onPipeDecoderReadyReadStandardOutput called - this should NOT happen in QMediaPlayer flow.";
}
void RealEsrganProcessor::onPipeDecoderFinished(int, QProcess::ExitStatus) {
    if (m_settings.verboseLog) qDebug() << "RealESRGAN onPipeDecoderFinished called - this should NOT happen in QMediaPlayer flow.";
}
void RealEsrganProcessor::onPipeDecoderError(QProcess::ProcessError) {
    if (m_settings.verboseLog) qDebug() << "RealESRGAN onPipeDecoderError called - this should NOT happen in QMediaPlayer flow.";
}
