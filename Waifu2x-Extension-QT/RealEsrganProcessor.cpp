// file: RealEsrganProcessor.cpp
#include "RealEsrganProcessor.h" // Correct case
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QTranslator> // For tr()
#include <QTime>       // For temporary file naming during multi-pass

RealEsrganProcessor::RealEsrganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    m_ffmpegProcess = new QProcess(this);
    cleanup(); // Initialize state, including m_state = State::Idle

    connect(m_process, &QProcess::errorOccurred, this, &RealEsrganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealEsrganProcessor::onReadyReadStandardOutput);
    // If RealESRGAN has specific error messages on stderr that aren't captured by exit code:
    // connect(m_process, &QProcess::readyReadStandardError, this, &RealEsrganProcessor::onProcessStdErr);

    connect(m_ffmpegProcess, &QProcess::errorOccurred, this, &RealEsrganProcessor::onFfmpegError);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onFfmpegFinished);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RealEsrganProcessor::onFfmpegStdErr);
}

RealEsrganProcessor::~RealEsrganProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000); // Wait a bit for clean exit
    }
    if (m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished(1000);
    }
    cleanupVideo(); // Ensure temp video files are removed
}

void RealEsrganProcessor::cleanup()
{
    m_currentRowNum = -1;
    m_scaleSequence.clear();

    // Clean up temporary file from multi-pass if it exists and isn't the final output
    if (!m_currentPassInputFile.isEmpty() && m_currentPassInputFile != m_originalSourceFile && m_currentPassInputFile != m_finalDestinationFile) {
        if (QFile::exists(m_currentPassInputFile)) {
            QFile::remove(m_currentPassInputFile);
        }
    }

    m_originalSourceFile.clear();
    m_finalDestinationFile.clear();
    m_currentPassInputFile.clear();
    m_currentPassOutputFile.clear();
    m_state = State::Idle; // Reset state
}

void RealEsrganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_state != State::Idle && m_state != State::ProcessingVideoFrames) { // Allow if processing video frames
        emit logMessage(tr("[ERROR] RealEsrganProcessor is already busy with a different task."));
        return;
    }

    if (m_state == State::Idle) { // Top-level image task
        m_state = State::ProcessingImage;
        m_currentRowNum = rowNum;
        m_settings = settings;
        m_originalSourceFile = sourceFile;
        m_finalDestinationFile = destinationFile;
        emit statusChanged(m_currentRowNum, tr("Processing..."));
        emit logMessage(QString("Real-ESRGAN: Starting image job for %1...").arg(QFileInfo(sourceFile).fileName()));
    } else if (m_state == State::ProcessingVideoFrames) { // Called for a video frame
        // m_currentRowNum and m_settings are already set by processVideo
        m_originalSourceFile = sourceFile; // For this frame
        m_finalDestinationFile = destinationFile; // For this frame
        // No status change here, video status is "Processing frames..."
    }

    m_currentPassInputFile = sourceFile;
    m_scaleSequence.clear();

    if (m_settings.targetScale <= 0 || m_settings.modelNativeScale <= 0) {
         emit logMessage(tr("[ERROR] Target scale and model native scale must be positive."));
         emit processingFinished(m_currentRowNum, false);
         cleanup();
         return;
    }

    if (m_settings.targetScale == 1) { // No upscaling, but might denoise if model supports it (pass through)
        m_scaleSequence.enqueue(m_settings.modelNativeScale); // Run one pass with native model scale, let -s handle it.
    } else if (m_settings.targetScale < m_settings.modelNativeScale) {
        // Target scale is less than model's native scale (e.g., x2 target with x4 model)
        // Run one pass with model's native scale; the executable should handle downscaling.
        m_scaleSequence.enqueue(m_settings.modelNativeScale);
    } else {
        // Multi-pass scaling: targetScale >= modelNativeScale
        int remainingScale = m_settings.targetScale;
        while (remainingScale >= m_settings.modelNativeScale) {
            m_scaleSequence.enqueue(m_settings.modelNativeScale);
            remainingScale /= m_settings.modelNativeScale;
        }
        if (remainingScale > 1) { // If there's a remaining factor (e.g. target x6, model x4 -> one x4 pass, remaining x1.5)
            // This scenario is tricky. For now, we'll just do the whole number passes.
            // Or, one could do an additional pass with native scale and then downscale, but that's complex.
            // Current logic: if target is x6, model x4: queue one x4. Remaining x1.5 ignored.
            // If target x3, model x2: queue one x2. Remaining x1.5 ignored.
            // This means effective scale might be less than target if not a direct multiple.
            // For simplicity, we only chain full native scale passes.
            // User should choose targetScale that's a multiple of modelNativeScale for best results.
             emit logMessage(QString(tr("Warning: Target scale %1 is not a full multiple of model native scale %2. Effective scale might differ."))
                            .arg(m_settings.targetScale).arg(m_settings.modelNativeScale));
        }
    }
     if (m_scaleSequence.isEmpty()){ // Should only happen if targetscale was 1 and modelnative was 1.
        m_scaleSequence.enqueue(m_settings.modelNativeScale); // Ensure at least one pass
    }


    startNextPass();
}

void RealEsrganProcessor::startNextPass() {
    if (m_process->state() != QProcess::NotRunning) {
        emit logMessage(tr("[WARNING] Real-ESRGAN process was asked to start a new pass while still running."));
        // QTimer::singleShot(100, this, &RealEsrganProcessor::startNextPass); // Optional: retry after a short delay
        return;
    }

    if (m_scaleSequence.isEmpty()) {
        // This state is handled by onProcessFinished after the last pass successfully completes.
        // If called when sequence is already empty, it means something went wrong or it's a no-op.
        return;
    }

    int currentPassScaleFactor = m_scaleSequence.head(); // Actual scale factor for this specific pass

    // Determine output file for this pass
    // If it's the last pass in the sequence, output to the final destination.
    // Otherwise, create a temporary file.
    bool isLastPass = (m_scaleSequence.size() == 1);
    if (isLastPass) {
        m_currentPassOutputFile = m_finalDestinationFile;
    } else {
        QString tempName = QString("%1_pass_tmp_%2.%3")
                               .arg(QFileInfo(m_finalDestinationFile).completeBaseName())
                               .arg(QTime::currentTime().toString("hhmmsszzzms")) // More unique temp name
                               .arg(m_settings.outputFormat);
        m_currentPassOutputFile = QDir(QFileInfo(m_finalDestinationFile).path()).filePath(tempName);
    }

    emit logMessage(QString("Real-ESRGAN: Starting pass (scale factor: %1). Input: '%2', Output: '%3'")
                    .arg(currentPassScaleFactor)
                    .arg(QFileInfo(m_currentPassInputFile).fileName())
                    .arg(QFileInfo(m_currentPassOutputFile).fileName()));

    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile);

    m_process->start(m_settings.programPath, arguments);

    if (!m_process->waitForStarted(2000)) { // Increased timeout
        emit logMessage(QString(tr("[ERROR] Real-ESRGAN process failed to start for pass. Input: %1. Executable: %2. Args: %3"))
                        .arg(m_currentPassInputFile).arg(m_settings.programPath).arg(arguments.join(" ")));
        if (m_state == State::ProcessingVideoFrames) {
            cleanupVideo();
        }
        emit statusChanged(m_currentRowNum, tr("Error starting process"));
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }
    // Dequeue only after successful start attempt. ErrorOccurred might fire if it fails.
    m_scaleSequence.dequeue();
}


void RealEsrganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle) return;

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stdErr = m_process->readAllStandardError();
        QString stdOut = m_process->readAllStandardOutput(); // Capture stdout too for errors
        emit logMessage(QString("Real-ESRGAN STDOUT (Error):\n%1").arg(stdOut.trimmed()));
        emit logMessage(QString("Real-ESRGAN STDERR:\n%1").arg(stdErr.trimmed()));
        emit logMessage(QString(tr("Real-ESRGAN: A processing pass failed. File: %1, Exit code: %2"))
                        .arg(QFileInfo(m_currentPassInputFile).fileName())
                        .arg(exitCode));

        if (QFile::exists(m_currentPassOutputFile) && m_currentPassOutputFile != m_finalDestinationFile) {
            QFile::remove(m_currentPassOutputFile); // Clean up temp output of failed pass
        }

        if (m_state == State::ProcessingVideoFrames) {
             emit logMessage(tr("Real-ESRGAN: Frame processing failed. Aborting video operation."));
             cleanupVideo();
        }
        emit statusChanged(m_currentRowNum, tr("Error"));
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }

    // Current pass finished successfully.
    // Delete previous pass's input if it was a temporary file.
    if (m_currentPassInputFile != m_originalSourceFile &&
        !(m_state == State::ProcessingVideoFrames && m_currentPassInputFile.startsWith(m_video_inputFramesPath))) {
        if (QFile::exists(m_currentPassInputFile)) {
            QFile::remove(m_currentPassInputFile);
        }
    }
    m_currentPassInputFile = m_currentPassOutputFile; // Output of this pass is input for the next.

    if (m_scaleSequence.isEmpty()) {
        // All passes for the current image/frame are complete.
        if (m_state == State::ProcessingImage) {
            emit logMessage(QString(tr("Real-ESRGAN: Successfully processed image %1. Output: %2"))
                            .arg(QFileInfo(m_originalSourceFile).fileName())
                            .arg(m_finalDestinationFile));
            emit statusChanged(m_currentRowNum, tr("Finished"));
            emit fileProgress(m_currentRowNum, 100);
            emit processingFinished(m_currentRowNum, true);
            cleanup();
        } else if (m_state == State::ProcessingVideoFrames) {
            m_video_processedFrames++;
            emit logMessage(QString(tr("Real-ESRGAN: Successfully processed frame %1 (%2/%3)"))
                .arg(QFileInfo(m_originalSourceFile).fileName()) // originalSourceFile is the current frame's input path
                .arg(m_video_processedFrames)
                .arg(m_video_totalFrames));

            if (m_video_totalFrames > 0) {
                emit fileProgress(m_currentRowNum, (100 * m_video_processedFrames) / m_video_totalFrames);
            }

            if (m_video_frameQueue.isEmpty() && m_video_processedFrames == m_video_totalFrames) {
                startVideoAssembly();
            } else if (!m_video_frameQueue.isEmpty()){
                startNextVideoFrame();
            } else {
                // Queue is empty, but not all frames processed - discrepancy
                emit logMessage(tr("[ERROR] Real-ESRGAN: Video frame queue empty but not all frames processed."));
                cleanupVideo();
                emit processingFinished(m_currentRowNum, false);
                cleanup();
            }
        }
    } else {
        // More scaling passes are needed for the current image/frame.
        emit logMessage(QString(tr("Real-ESRGAN: Pass completed for %1. Starting next pass..."))
                        .arg(QFileInfo(m_originalSourceFile).fileName())); // originalSourceFile here is the initial input to the multi-pass sequence
        startNextPass();
    }
}

void RealEsrganProcessor::onProcessError(QProcess::ProcessError error)
{
    if (m_state == State::Idle) return;

    emit logMessage(QString(tr("[FATAL] Real-ESRGAN process failed to start or crashed. File: %1, Error: %2 (Code: %3)"))
                    .arg(QFileInfo(m_currentPassInputFile).fileName()) // Input to the failing pass
                    .arg(m_process->errorString())
                    .arg(error));

    if (m_state == State::ProcessingVideoFrames) {
        emit logMessage(tr("Real-ESRGAN: Frame processing failed critically. Aborting video operation."));
        cleanupVideo();
    }
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void RealEsrganProcessor::onReadyReadStandardOutput()
{
    if(m_state == State::Idle) return;
    QString output = m_process->readAllStandardOutput();
    // Real-ESRGAN (nihui's version) usually outputs progress like "12%" on a single line,
    // or nothing until it's done.
    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatchIterator i = re.globalMatch(output);
    int lastProgress = -1;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        lastProgress = match.captured(1).toInt();
    }

    if (lastProgress != -1) {
        if (m_state == State::ProcessingImage) { // Top-level image task
            emit fileProgress(m_currentRowNum, lastProgress);
        }
        // For video frames, progress is handled when a frame finishes.
        // Individual frame progress from stdout might be too noisy for overall UI.
    }
    // Optionally, log all stdout for debugging if needed:
    // emit logMessage("Real-ESRGAN STDOUT: " + output.trimmed());
}


// --- VIDEO PROCESSING ---
void RealEsrganProcessor::processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_state != State::Idle) {
        emit logMessage(tr("[ERROR] RealEsrganProcessor is already busy."));
        return;
    }

    m_state = State::SplittingVideo;
    m_currentRowNum = rowNum;
    m_settings = settings;
    m_originalSourceFile = sourceFile; // Original video file
    m_finalDestinationFile = destinationFile; // Final output video file

    m_video_tempPath = QDir(QFileInfo(destinationFile).path()).filePath("temp_video_realesrgan_" + QFileInfo(sourceFile).completeBaseName() + "_" + QTime::currentTime().toString("hhmmsszzz"));
    m_video_inputFramesPath = QDir(m_video_tempPath).filePath("input_frames");
    m_video_outputFramesPath = QDir(m_video_tempPath).filePath("output_frames");
    m_video_audioPath = QDir(m_video_tempPath).filePath("audio.m4a"); // Common format

    if (!QDir().mkpath(m_video_inputFramesPath) || !QDir().mkpath(m_video_outputFramesPath)) {
        emit logMessage(tr("[ERROR] Could not create temporary directories for video processing."));
        cleanup();
        emit processingFinished(m_currentRowNum, false);
        return;
    }

    emit statusChanged(rowNum, tr("Splitting video..."));
    emit logMessage(QString("Real-ESRGAN Video: Splitting '%1' into frames...").arg(QFileInfo(sourceFile).fileName()));

    QStringList args;
    args << "-i" << sourceFile
         << "-qscale:v" << "1" // High quality PNGs
         << "-vsync" << "0"    // As fast as possible, ensure frame numbering
         << QDir(m_video_inputFramesPath).filePath("frame%08d.png")
         << "-vn" // No video for this audio extraction stream
         << "-c:a" << "aac" << "-b:a" << "192k" // Or copy if original audio is fine
         << m_video_audioPath;

    m_ffmpegProcess->start("ffmpeg", args);
}

void RealEsrganProcessor::onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle && !(m_state == State::SplittingVideo || m_state == State::AssemblingVideo) ) return;


    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString(tr("FFmpeg task failed. State: %1, Exit Code: %2, Status: %3"))
                        .arg((int)m_state).arg(exitCode).arg(exitStatus));
        cleanupVideo();
        emit statusChanged(m_currentRowNum, tr("FFmpeg Error"));
        emit processingFinished(m_currentRowNum, false);
        cleanup();
        return;
    }

    if (m_state == State::SplittingVideo) {
        emit logMessage(tr("Real-ESRGAN Video: Splitting complete. Starting frame processing..."));
        emit statusChanged(m_currentRowNum, tr("Processing frames..."));
        m_state = State::ProcessingVideoFrames;

        QDir inputDir(m_video_inputFramesPath);
        inputDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        inputDir.setSorting(QDir::Name); // Process frames in order
        m_video_frameQueue = inputDir.entryList();
        m_video_totalFrames = m_video_frameQueue.size();
        m_video_processedFrames = 0;

        if (m_video_totalFrames > 0) {
            emit fileProgress(m_currentRowNum, 0); // Initial progress
            startNextVideoFrame();
        } else {
            emit logMessage(tr("Real-ESRGAN Video: No frames found after splitting. Aborting."));
            cleanupVideo();
            emit statusChanged(m_currentRowNum, tr("No frames found"));
            emit processingFinished(m_currentRowNum, false);
            cleanup();
        }
    } else if (m_state == State::AssemblingVideo) {
        emit logMessage(QString(tr("Real-ESRGAN Video: Assembly complete. Output: %1")).arg(m_finalDestinationFile));
        emit statusChanged(m_currentRowNum, tr("Finished"));
        emit fileProgress(m_currentRowNum, 100);
        cleanupVideo();
        emit processingFinished(m_currentRowNum, true);
        cleanup();
    }
}

void RealEsrganProcessor::startNextVideoFrame()
{
    if (m_video_frameQueue.isEmpty()) {
        // This should ideally be caught by onProcessFinished logic for the last frame.
        // If queue is empty and not all frames processed, it's an error.
        if (m_video_processedFrames != m_video_totalFrames || m_video_totalFrames == 0) {
             emit logMessage(tr("[ERROR] Real-ESRGAN: Video frame queue empty prematurely or no frames to process."));
             cleanupVideo();
             emit statusChanged(m_currentRowNum, tr("Error processing frames"));
             emit processingFinished(m_currentRowNum, false);
             cleanup();
        }
        // If all processed, onProcessFinished would have called startVideoAssembly.
        return;
    }

    QString frameFile = m_video_frameQueue.head(); // Peek, don't dequeue until processImage starts its part
    QString inputFramePath = QDir(m_video_inputFramesPath).filePath(frameFile);
    QString outputFramePath = QDir(m_video_outputFramesPath).filePath(frameFile);

    // Use processImage for the actual frame scaling. It will handle multi-pass if needed.
    // It will set m_state to ProcessingImage internally for the duration of the frame.
    // When the frame is done, its onProcessFinished will trigger the next video frame or assembly.
    processImage(m_currentRowNum, inputFramePath, outputFramePath, m_settings);
    // Dequeue after *successfully* starting the processing for this frame in processImage->startNextPass
    // However, processImage itself calls startNextPass, which calls QProcess::start.
    // The current design of processImage might need adjustment if it's to be purely synchronous for this call.
    // For now, let's assume processImage sets up and starts. The dequeue should happen in onProcessFinished (of the frame)
    // *before* it calls startNextVideoFrame again OR in startNextVideoFrame *after* a successful launch.
    // To simplify, let's dequeue here, assuming processImage will attempt to start.
    // If processImage fails to start its process, that's an error handled by its own logic.
    m_video_frameQueue.dequeue();
}

void RealEsrganProcessor::startVideoAssembly()
{
    // Ensure this is only called when all frames are truly done.
    if (m_state != State::ProcessingVideoFrames && m_state != State::AssemblingVideo) { // Allow re-entry if already assembling (e.g. retry?)
        emit logMessage(QString(tr("Real-ESRGAN Video: Assembly called in unexpected state: %1. Aborting assembly.")).arg((int)m_state));
        return;
    }
     if (m_state == State::ProcessingVideoFrames) { // First time entering assembly
        emit logMessage(QString(tr("Real-ESRGAN Video: All %1 frames processed. Assembling final video...")).arg(m_video_totalFrames));
        emit statusChanged(m_currentRowNum, tr("Assembling video..."));
    }
    m_state = State::AssemblingVideo;


    QStringList args;
    // TODO: Get framerate from original video (e.g. via ffprobe during splitting).
    QString framerate = "25"; // Placeholder - make this configurable or detect

    args << "-framerate" << framerate
         << "-i" << QDir(m_video_outputFramesPath).filePath("frame%08d.png");

    if (QFile::exists(m_video_audioPath)) {
        args << "-i" << m_video_audioPath
             << "-c:v" << "libx264" << "-pix_fmt" << "yuv420p" // Common video settings
             << "-c:a" << "aac" // Or "copy" if audio doesn't need re-encoding
             << "-shortest"; // Finish when shortest input (video/audio frames) ends
    } else {
        emit logMessage(tr("Real-ESRGAN Video: No audio file found. Assembling video without audio."));
        args << "-c:v" << "libx264" << "-pix_fmt" << "yuv420p";
    }
    args << "-y" << m_finalDestinationFile; // Output file, overwrite

    m_ffmpegProcess->start("ffmpeg", args);
}


QStringList RealEsrganProcessor::buildArguments(const QString &inputFile, const QString &outputFile)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

    int scaleForThisPass = m_settings.modelNativeScale > 0 ? m_settings.modelNativeScale : 2; // Default if not set
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
    if (m_state == State::Idle) return;
    emit logMessage(QString(tr("FFmpeg process failed to start. State: %1, Error: %2 (Code: %3)"))
                    .arg((int)m_state)
                    .arg(m_ffmpegProcess->errorString())
                    .arg(error));
    cleanupVideo();
    emit statusChanged(m_currentRowNum, tr("FFmpeg Start Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup();
}

void RealEsrganProcessor::onFfmpegStdErr() {
    if (m_state == State::Idle) return;
    QByteArray data = m_ffmpegProcess->readAllStandardError();
    // FFmpeg often outputs verbose info to stderr, even on success.
    // May need filtering if it's too noisy for general logs.
    emit logMessage("FFmpeg: " + QString::fromLocal8Bit(data).trimmed());
}
