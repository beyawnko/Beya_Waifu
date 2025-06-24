// file: realcuganprocessor.cpp
#include "RealCuganProcessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths> // For temporary paths
#include <QDateTime>      // For unique temp folder names
#include <QFile>          // For QFile::remove

RealCuganProcessor::RealCuganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    connect(m_process, &QProcess::errorOccurred, this, &RealCuganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealCuganProcessor::onReadyReadStandardOutput);
    // It's good practice to also connect readyReadStandardError for m_process if not done elsewhere or for debugging
    // connect(m_process, &QProcess::readyReadStandardError, this, &SomeSlotForSRErrorOutput);


    m_ffmpegProcess = new QProcess(this); // For old video method
    connect(m_ffmpegProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onFfmpegError);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onFfmpegFinished);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onFfmpegStdErr);

    // New processes for piped video
    m_ffmpegDecoderProcess = new QProcess(this);
    connect(m_ffmpegDecoderProcess, &QProcess::readyReadStandardOutput, this, &RealCuganProcessor::onPipeDecoderReadyReadStandardOutput);
    connect(m_ffmpegDecoderProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onPipeDecoderReadyReadStandardError);
    connect(m_ffmpegDecoderProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onPipeDecoderFinished);
    connect(m_ffmpegDecoderProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onPipeDecoderError);

    m_ffmpegEncoderProcess = new QProcess(this);
    connect(m_ffmpegEncoderProcess, &QProcess::readyReadStandardError, this, &RealCuganProcessor::onPipeEncoderReadyReadStandardError);
    connect(m_ffmpegEncoderProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealCuganProcessor::onPipeEncoderFinished);
    connect(m_ffmpegEncoderProcess, &QProcess::errorOccurred, this, &RealCuganProcessor::onPipeEncoderError);

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
    if (m_ffmpegDecoderProcess && m_ffmpegDecoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegDecoderProcess->kill();
        m_ffmpegDecoderProcess->waitForFinished(500);
    }
    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }
    cleanupVideo(); // Clean up temp files for old method
    cleanupPipeProcesses(); // Clean up temp files for new method (e.g. audio)
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
    cleanupPipeProcesses();
    cleanupVideo(); // For old method temp files
}


void RealCuganProcessor::cleanupPipeProcesses()
{
    if (m_ffmpegDecoderProcess && m_ffmpegDecoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegDecoderProcess->kill();
        m_ffmpegDecoderProcess->waitForFinished(500);
    }
    // m_process (SR engine) is handled by general cleanup() if it was used for pipe mode and got stuck.
    if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() != QProcess::NotRunning) {
        m_ffmpegEncoderProcess->kill();
        m_ffmpegEncoderProcess->waitForFinished(500);
    }

    if (!m_tempAudioPath.isEmpty()) {
        QFile::remove(m_tempAudioPath);
        m_tempAudioPath.clear();
    }
    m_currentDecodedFrameBuffer.clear();
    m_currentUpscaledFrameBuffer.clear();
    m_framesProcessedPipe = 0;
    m_totalFramesEstimatePipe = 0;
    m_allFramesDecoded = false;
    m_allFramesSentToEncoder = false;
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

    // Now start the actual pipeline
    startPipeDecoder(); // This will start FFmpeg to decode frames to its stdout
    startPipeEncoder(); // Start FFmpeg encoder, waiting for input on its stdin
}


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
            m_state = State::PipeDecodingVideo;
            if (!m_currentDecodedFrameBuffer.isEmpty()){ // Process next available decoded frame
                processDecodedFrameBuffer();
            } else if (m_allFramesDecoded) {
                // This case should ideally be caught by the above if block (all decoded and buffer empty)
                // but as a safeguard:
                if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
                    m_ffmpegEncoderProcess->closeWriteChannel();
                    m_allFramesSentToEncoder = true;
                     if (m_settings.verboseLog) qDebug() << "Safeguard: All frames SR'd, closing encoder stdin.";
                }
                m_state = State::PipeEncodingVideo;
            }
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
    args << "-c:v" << "libx264" << "-pix_fmt" << "yuv420p" // TODO: Make video codec/params configurable
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
    QString tempVideoJobPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                           QString("/realcugan_pipe_job_%1").arg(QDateTime::currentMSecsSinceEpoch());
    QDir tempJobDir(tempVideoJobPath);
    if (!tempJobDir.mkpath(".")) {
        emit logMessage(tr("Error: Could not create temporary directory for audio: %1").arg(tempVideoJobPath));
        // Decide if proceeding without audio is acceptable or return false
        m_tempAudioPath.clear(); // No audio path
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
    if (m_state != State::PipeDecodingVideo && m_state != State::PipeProcessingSR && m_state != State::PipeEncodingVideo) {
        // Allow reading if we are waiting for SR or Encoder to finish last frame, but decoder still sends data
        if (m_state == State::Idle) return; // If completely idle, something is wrong
        if (m_settings.verboseLog) qDebug() << "Decoder output received in state: " << (int)m_state;
    }

    m_currentDecodedFrameBuffer.append(m_ffmpegDecoderProcess->readAllStandardOutput());

    // Only try to process if we are in decoding or waiting for SR state.
    // If we are already in encoding video state, it means all frames sent to SR.
    if (m_state == State::PipeDecodingVideo || m_state == State::PipeProcessingSR) {
         processDecodedFrameBuffer();
    }
}

void RealCuganProcessor::processDecodedFrameBuffer() {
    // This function is called when new data arrives from decoder, or when SR engine becomes free.
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
}


void RealCuganProcessor::startRealCuganPipe(const QByteArray& frameData) {
    // State should be PipeProcessingSR, set by caller (processDecodedFrameBuffer)
     if (m_process->state() == QProcess::Running) {
        emit logMessage(tr("Error: Attempted to start RealCUGAN pipe while SR process is already running."));
        return; // Should not happen if logic in processDecodedFrameBuffer is correct
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
    m_process->start(m_settings.programPath, srArgs);
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
    if (m_state == State::Idle) return;

    if (m_settings.verboseLog) qDebug() << "FFmpeg Decoder finished. ExitCode:" << exitCode << "Status:" << exitStatus;
    m_allFramesDecoded = true;

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(tr("FFmpeg Decoder failed. Exit: %1, Status: %2").arg(exitCode).arg(exitStatus));
        // Error output already read by onPipeDecoderReadyReadStandardError
        finalizePipedVideoProcessing(false);
        return;
    }

    // Process any remaining data in the buffer from the last readyRead
    if (!m_currentDecodedFrameBuffer.isEmpty()) {
        processDecodedFrameBuffer();
    }

    // If buffer is now empty, and SR isn't running, all decoded frames have been passed to SR.
    // (This logic is duplicated/similar in processDecodedFrameBuffer, ensure consistency)
    if (m_currentDecodedFrameBuffer.isEmpty() && (m_process->state() == QProcess::NotRunning)) {
        if (m_ffmpegEncoderProcess && m_ffmpegEncoderProcess->state() == QProcess::Running && !m_allFramesSentToEncoder) {
            m_ffmpegEncoderProcess->closeWriteChannel();
            m_allFramesSentToEncoder = true;
            if (m_settings.verboseLog) qDebug() << "Decoder finished, buffer empty, SR not running. Closing encoder input.";
        }
        if (m_state != State::PipeEncodingVideo) m_state = State::PipeEncodingVideo;
    }
}

void RealCuganProcessor::onPipeDecoderError(QProcess::ProcessError error) {
    if (m_state == State::Idle) return;
    emit logMessage(tr("FFmpeg Decoder process error: %1. Error code: %2").arg(m_ffmpegDecoderProcess->errorString()).arg(error));
    finalizePipedVideoProcessing(false);
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

    encoderArgs << "-c:v" << "libx264"
                << "-preset" << "medium"
                << "-crf" << "23"
                << "-pix_fmt" << "yuv420p"
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
    // cleanup() will call cleanupPipeProcesses().
    cleanup(); // This sets state to Idle, kills processes, and cleans general vars.

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
    } else if (!success && m_currentRowNum != -1) {
        // If it was already idle but an error callback is trying to finalize,
        // ensure the finish signal for that job is emitted as error if not already.
        // This is a safeguard.
        emit processingFinished(m_currentRowNum, false);
        m_currentRowNum = -1; // Reset after emitting.
    }
}
