// file: realesrganprocessor.cpp
#include "realesrganprocessor.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QTranslator> // For tr()

RealEsrganProcessor::RealEsrganProcessor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
    m_ffmpegProcess = new QProcess(this);
    cleanup(); // Initialize state, including m_state = State::Idle

    connect(m_process, &QProcess::errorOccurred, this, &RealEsrganProcessor::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onProcessFinished);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &RealEsrganProcessor::onReadyReadStandardOutput);
    // connect(m_process, &QProcess::readyReadStandardError, this, &RealEsrganProcessor::onProcessStdErr); // If needed

    connect(m_ffmpegProcess, &QProcess::errorOccurred, this, &RealEsrganProcessor::onFfmpegError);
    connect(m_ffmpegProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RealEsrganProcessor::onFfmpegFinished);
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, &RealEsrganProcessor::onFfmpegStdErr);
}

RealEsrganProcessor::~RealEsrganProcessor()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
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

    if (!m_currentPassInputFile.isEmpty() && m_currentPassInputFile != m_originalSourceFile) {
        QFile::remove(m_currentPassInputFile);
    }

    m_originalSourceFile.clear();
    m_finalDestinationFile.clear();
    m_currentPassInputFile.clear();
    m_currentPassOutputFile.clear();
    m_state = State::Idle; // Reset state
}

void RealEsrganProcessor::processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    // If m_state is not Idle, it could be called from processVideo for a frame.
    // Only block if it's an external call and the processor is truly busy with another top-level task.
    if (m_state != State::Idle && m_state != State::ProcessingVideoFrames) {
        emit logMessage("[ERROR] RealEsrganProcessor is already busy with a different task.");
        return;
    }
    // If called for a frame, m_process might be busy with the *previous* frame.
    // The startNextPass logic already handles m_process state.

    if (m_state == State::Idle) { // This is a top-level image processing request
      m_state = State::ProcessingImage;
      m_currentRowNum = rowNum; // Only set for top-level tasks
      m_settings = settings;
      m_originalSourceFile = sourceFile;
      m_finalDestinationFile = destinationFile;
      emit statusChanged(m_currentRowNum, tr("Processing..."));
      emit logMessage(QString("Real-ESRGAN: Starting image job for %1...").arg(QFileInfo(sourceFile).fileName()));
    } else if (m_state == State::ProcessingVideoFrames) {
      // This is a call for a video frame.
      // m_currentRowNum, m_settings are already set by processVideo.
      // m_originalSourceFile for a frame is its input path.
      // m_finalDestinationFile for a frame is its output path.
      m_originalSourceFile = sourceFile; // For this frame
      m_finalDestinationFile = destinationFile; // For this frame
      // emit logMessage(QString("Real-ESRGAN: Starting frame processing for %1...").arg(QFileInfo(sourceFile).fileName()));
    }


    m_currentPassInputFile = sourceFile;

    // --- Determine scaling sequence ---
    m_scaleSequence.clear(); // Clear for each new image/frame
    int remainingScale = m_settings.targetScale;
    int nativeScale = m_settings.modelNativeScale > 1 ? m_settings.modelNativeScale : 2; // Safeguard
    while (remainingScale >= nativeScale) {
        m_scaleSequence.enqueue(nativeScale);
        remainingScale /= nativeScale;
    }
    // If targetScale is less than nativeScale (e.g. target x1 with x4 model), or if it's not perfectly divisible.
    // We still need at least one pass using the nativeScale. The executable should handle downscaling if -s is larger.
    // Or, more simply, if after division there's still scaling to do (e.g. target x3, model x2 -> one pass x2, then what?)
    // The current logic assumes targetScale is a multiple of modelNativeScale or smaller.
    // For simplicity, if scaleSequence is empty, it means targetScale < nativeScale.
    // We'll just run one pass with nativeScale.
    if (m_scaleSequence.isEmpty()) {
        m_scaleSequence.enqueue(nativeScale); // At least one pass with the model's scale
    }


    startNextPass();
}

void RealEsrganProcessor::startNextPass()
{
    if (m_process->state() != QProcess::NotRunning) {
        // This can happen if startNextPass is called rapidly, e.g. from onProcessFinished -> startNextPass
        // before the QProcess state has fully transitioned.
        // A small delay or a single shot timer might be needed if this becomes an issue,
        // but usually QProcess handles sequential calls fine once the previous one has emitted finished().
        emit logMessage("[WARNING] Real-ESRGAN process was asked to start a new pass while still running. This might indicate an issue.");
        // For now, we'll let it try, but this is a point of attention.
    }

    if (m_scaleSequence.isEmpty()) {
        // This means all passes for the current image/frame are done.
        // The onProcessFinished handler will take care of what's next (another frame, assembly, or finishing).
        // We call it with success state, assuming the last pass was successful.
        // The actual success/failure of the last pass is determined by its exit code in onProcessFinished.
        // This call path is typically from processImage -> startNextPass (when scaleSequence was already empty, e.g. target x1)
        // OR from onProcessFinished -> startNextPass (when the queue becomes empty after a successful pass).
        // To avoid recursive signaling or complex state, let onProcessFinished handle the final step of a scale sequence.
        // If we got here because the queue was *initially* empty for a new image/frame,
        // it means targetScale was < modelNativeScale. The single pass (setup in processImage) will run.
        // If we got here *after* a pass, onProcessFinished will handle it.
        // The critical thing is that onProcessFinished is the sole decider for the *end* of a multi-pass sequence.
        // So, if m_scaleSequence is empty HERE, it implies the sequence completed.
        // Let onProcessFinished (which is connected to m_process->finished()) handle the logic.
        // This condition might be hit if processImage is called with a scale sequence that resolves to empty
        // immediately (e.g. targetScale=1, modelNativeScale=4, resulting in one pass).
        // In that case, the single pass is started below. When it finishes, onProcessFinished is called.
        // If m_scaleSequence becomes empty *after* a pass, onProcessFinished is also the one to react.
        // This function's main job is to start *one* pass.
        return; // Let onProcessFinished handle completion.
    }

    int currentPassScaleFactor = m_scaleSequence.head(); // Peek, don't dequeue until pass starts successfully.

    // Determine output file for this pass
    m_currentPassOutputFile = (m_scaleSequence.size() == 1) // Is this the last pass in the sequence?
        ? m_finalDestinationFile // Output to the final destination for this image/frame
        : QDir(QFileInfo(m_finalDestinationFile).path()).filePath(QString("%1_pass_tmp_%2.%3")
          .arg(QFileInfo(m_finalDestinationFile).completeBaseName())
          .arg(QTime::currentTime().toString("hhmmsszzz")) // Add timestamp to avoid collision
          .arg(m_settings.outputFormat));


    emit logMessage(QString("Real-ESRGAN: Starting scaling pass (target scale factor for this pass: %1)... Input: %2, Output: %3").arg(currentPassScaleFactor).arg(m_currentPassInputFile).arg(m_currentPassOutputFile));
    QStringList arguments = buildArguments(m_currentPassInputFile, m_currentPassOutputFile);

    m_process->start(m_settings.programPath, arguments);
    if (m_process->waitForStarted(1000)) { // Check if process started
        m_scaleSequence.dequeue(); // Successfully started, now dequeue
    } else {
        emit logMessage(QString("Real-ESRGAN: Failed to start process for pass. Input: %1").arg(m_currentPassInputFile));
        // Treat as an error for this image/frame.
        if (m_state == State::ProcessingVideoFrames) {
            cleanupVideo(); // Clean up video processing attempt
        }
        emit statusChanged(m_currentRowNum, tr("Error starting process"));
        emit processingFinished(m_currentRowNum, false);
        cleanup(); // General cleanup
    }
}

void RealEsrganProcessor::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle) return; // Should not happen if processing was started

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString stdErr = m_process->readAllStandardError();
        QString stdOut = m_process->readAllStandardOutput();
        emit logMessage(QString("Real-ESRGAN Pass STDOUT:\n%1").arg(stdOut));
        emit logMessage(QString("Real-ESRGAN Pass STDERR:\n%1").arg(stdErr));
        emit logMessage(QString("Real-ESRGAN: A process failed. State: %1, File: %2, Exit code: %3")
                        .arg((int)m_state)
                        .arg(QFileInfo(m_originalSourceFile).fileName()) // originalSourceFile here might be a frame
                        .arg(exitCode));

        if (m_currentPassOutputFile != m_finalDestinationFile) { // If temp output was created for this failed pass
            QFile::remove(m_currentPassOutputFile);
        }

        if (m_state == State::ProcessingVideoFrames) {
             emit logMessage("Real-ESRGAN: Frame processing failed. Aborting video operation.");
             cleanupVideo(); // Cleans up all video temp files and resets video state
        }
        emit statusChanged(m_currentRowNum, tr("Error"));
        emit processingFinished(m_currentRowNum, false); // Signal overall failure for the job
        cleanup(); // General cleanup (resets state to Idle)
        return;
    }

    // A pass (or whole image if single pass) finished successfully.
    // Delete previous pass's input if it was a temporary file
    // Note: m_originalSourceFile is the true original (video or image path)
    // m_currentPassInputFile is the input to the just-finished pass
    if (m_currentPassInputFile != m_originalSourceFile && !(m_state == State::ProcessingVideoFrames && m_currentPassInputFile.startsWith(m_video_inputFramesPath))) {
         // Don't delete original video frames from inputFramesPath
        if (QFile::exists(m_currentPassInputFile)) { // Check existence before removing
            QFile::remove(m_currentPassInputFile);
        }
    }
    m_currentPassInputFile = m_currentPassOutputFile; // Output of this pass is input for the next

    if (m_scaleSequence.isEmpty()) {
        // This means a full image/frame has been completely scaled.
        if (m_state == State::ProcessingImage) {
            emit logMessage(QString("Real-ESRGAN: Successfully processed image %1. Output: %2")
                            .arg(QFileInfo(m_originalSourceFile).fileName()) // originalSourceFile is the image path
                            .arg(m_finalDestinationFile));
            emit statusChanged(m_currentRowNum, tr("Finished"));
            emit fileProgress(m_currentRowNum, 100);
            emit processingFinished(m_currentRowNum, true);
            cleanup(); // Resets state to Idle
        } else if (m_state == State::ProcessingVideoFrames) {
            m_video_processedFrames++;
            emit logMessage(QString("Real-ESRGAN: Successfully processed frame %1 (%2/%3)")
                .arg(QFileInfo(m_originalSourceFile).fileName()) // originalSourceFile is the frame input path
                .arg(m_video_processedFrames)
                .arg(m_video_totalFrames));

            if (m_video_totalFrames > 0) {
                emit fileProgress(m_currentRowNum, (100 * m_video_processedFrames) / m_video_totalFrames);
            }

            if (m_video_frameQueue.isEmpty() && m_video_processedFrames == m_video_totalFrames) {
                startVideoAssembly();
            } else {
                startNextVideoFrame();
            }
        }
    } else {
        // More scaling passes are needed for the current image/frame.
        emit logMessage(QString("Real-ESRGAN: Pass completed for %1. Next pass...").arg(QFileInfo(m_originalSourceFile).fileName()));
        startNextPass();
    }
}

void RealEsrganProcessor::onProcessError(QProcess::ProcessError error)
{
    // This slot is for errors that prevent the process from starting.
    // Only relevant if m_state is ProcessingImage or ProcessingVideoFrames.
    if (m_state == State::Idle) return;

    emit logMessage(QString("[FATAL] Real-ESRGAN process failed to start. State: %1, File: %2, Error: %3 (Code: %4)")
                    .arg((int)m_state)
                    .arg(QFileInfo(m_originalSourceFile).fileName()) // This could be an image or a frame
                    .arg(m_process->errorString())
                    .arg(error));

    if (m_state == State::ProcessingVideoFrames) {
        emit logMessage("Real-ESRGAN: Frame processing failed to start. Aborting video operation.");
        cleanupVideo();
    }
    emit statusChanged(m_currentRowNum, tr("Fatal Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup(); // Resets state to Idle
}

void RealEsrganProcessor::onReadyReadStandardOutput()
{
    if(m_state == State::Idle) return; // Don't process output if not actively working

    QString output = m_process->readAllStandardOutput();
    // emit logMessage(QString("Real-ESRGAN STDOUT chunk: %1").arg(output)); // Debug

    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatchIterator i = re.globalMatch(output);
    int lastProgress = -1;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        lastProgress = match.captured(1).toInt();
    }

    if (lastProgress != -1) {
        if (m_state == State::ProcessingImage) {
            emit fileProgress(m_currentRowNum, lastProgress);
        } else if (m_state == State::ProcessingVideoFrames) {
            // For video frames, progress is more complex.
            // This 'lastProgress' is for the current frame. We need overall video progress.
            // Let onProcessFinished update overall video progress when a frame *completes*.
            // However, we can emit a sub-progress if needed, e.g. for a status tip.
            // For now, let's stick to updating main progress on frame completion.
        }
    }
}

QStringList RealEsrganProcessor::buildArguments(const QString &inputFile, const QString &outputFile)
{
    QStringList arguments;
    arguments << "-i" << QDir::toNativeSeparators(inputFile);
    arguments << "-o" << QDir::toNativeSeparators(outputFile);

    // When m_scaleSequence is not empty, head() gives the scale for the current pass.
    // If it's somehow empty here (should be guarded before calling), default to modelNativeScale.
    int scaleForThisPass = m_settings.modelNativeScale; // Default
    if (!m_scaleSequence.isEmpty()) {
         scaleForThisPass = m_scaleSequence.head(); // This is the scale factor for *this specific pass*
    } else if (m_settings.targetScale < m_settings.modelNativeScale) {
        // If overall target is smaller, but we run one pass, use model's native scale.
        // The executable might downscale or this might be an edge case.
        scaleForThisPass = m_settings.modelNativeScale;
    }


    arguments << "-s" << QString::number(scaleForThisPass);
    arguments << "-m" << m_settings.modelName;
    arguments << "-t" << QString::number(m_settings.tileSize);
    arguments << "-f" << m_settings.outputFormat;

    if (!m_settings.singleGpuId.trimmed().isEmpty() && m_settings.singleGpuId.toLower() != "auto") {
        arguments << "-g" << m_settings.singleGpuId;
    }

    if (m_settings.ttaEnabled) {
        arguments << "-x";
    }

    return arguments;
}

// --- Video Processing Methods ---

void RealEsrganProcessor::processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings)
{
    if (m_state != State::Idle) {
        emit logMessage("[ERROR] RealEsrganProcessor is already busy.");
        return;
    }

    m_state = State::SplittingVideo;
    m_currentRowNum = rowNum;
    m_settings = settings; // Store settings for frame processing
    m_originalSourceFile = sourceFile; // Store original video source path
    m_finalDestinationFile = destinationFile; // Store final video output path

    // Create temporary directories for video processing
    m_video_tempPath = QDir(QFileInfo(destinationFile).path()).filePath("temp_video_realesrgan_" + QFileInfo(sourceFile).completeBaseName() + "_" + QTime::currentTime().toString("hhmmsszzz"));
    m_video_inputFramesPath = QDir(m_video_tempPath).filePath("input_frames");
    m_video_outputFramesPath = QDir(m_video_tempPath).filePath("output_frames");
    m_video_audioPath = QDir(m_video_tempPath).filePath("audio.m4a"); // Assuming m4a, check ffmpeg output

    if (!QDir().mkpath(m_video_inputFramesPath) || !QDir().mkpath(m_video_outputFramesPath)) {
        emit logMessage("[ERROR] Could not create temporary directories for video processing.");
        cleanup(); // Reset state to Idle
        emit processingFinished(m_currentRowNum, false);
        return;
    }

    emit statusChanged(rowNum, tr("Splitting video..."));
    emit logMessage(QString("Real-ESRGAN Video: Splitting '%1' into frames...").arg(QFileInfo(sourceFile).fileName()));

    // Use FFmpeg to extract frames and audio
    QStringList args;
    // TODO: Get fps from source video for assembly. For now, hardcode or make it a setting.
    // Example: ffmpeg -i source.mp4 -q:a 0 -ac 2 -vn audio.m4a -vsync 0 input_frames/frame%08d.png
    args << "-i" << sourceFile
         << "-qscale:v" << "1" // Try to get high quality PNGs
         << "-vsync" << "0" // Or "cfr" if issues with frame numbering/timing
         << QDir(m_video_inputFramesPath).filePath("frame%08d.png")
         << "-vn" // No video for this output stream
         << "-c:a" << "aac" // Common audio codec
         << "-b:a" << "192k" // Decent audio bitrate
         << m_video_audioPath;


    m_ffmpegProcess->start("ffmpeg", args);
}

void RealEsrganProcessor::onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_state == State::Idle) return; // Not expecting FFmpeg signals if idle

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        emit logMessage(QString("FFmpeg task failed. State: %1, Exit Code: %2, Exit Status: %3")
                        .arg((int)m_state).arg(exitCode).arg(exitStatus));
        cleanupVideo(); // Clean up any video temp files created
        emit statusChanged(m_currentRowNum, tr("FFmpeg Error"));
        emit processingFinished(m_currentRowNum, false);
        cleanup(); // General cleanup, reset state to Idle
        return;
    }

    if (m_state == State::SplittingVideo) {
        emit logMessage("Real-ESRGAN Video: Splitting complete. Starting frame processing...");
        emit statusChanged(m_currentRowNum, tr("Processing frames..."));
        m_state = State::ProcessingVideoFrames;

        QDir inputDir(m_video_inputFramesPath);
        inputDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        inputDir.setSorting(QDir::Name); // Ensure frames are processed in order
        m_video_frameQueue = inputDir.entryList();

        m_video_totalFrames = m_video_frameQueue.size();
        m_video_processedFrames = 0;

        if (m_video_totalFrames > 0) {
            emit fileProgress(m_currentRowNum, 0); // Initial progress
            startNextVideoFrame(); // Kick off the first frame
        } else {
            emit logMessage("Real-ESRGAN Video: No frames found after splitting. Aborting.");
            cleanupVideo();
            emit statusChanged(m_currentRowNum, tr("No frames found"));
            emit processingFinished(m_currentRowNum, false);
            cleanup();
        }

    } else if (m_state == State::AssemblingVideo) {
        emit logMessage(QString("Real-ESRGAN Video: Assembly complete. Output: %1").arg(m_finalDestinationFile));
        emit statusChanged(m_currentRowNum, tr("Finished"));
        emit fileProgress(m_currentRowNum, 100);
        cleanupVideo(); // Success, clean up temp video files
        emit processingFinished(m_currentRowNum, true);
        cleanup(); // General cleanup, reset state to Idle
    }
}

void RealEsrganProcessor::startNextVideoFrame()
{
    if (m_video_frameQueue.isEmpty()) {
        // This might mean all frames are processed, or the queue was empty to begin with.
        // onProcessFinished for the last frame will handle transition to assembly.
        if (m_video_processedFrames == m_video_totalFrames && m_video_totalFrames > 0) {
             emit logMessage("Real-ESRGAN Video: All frames processed from queue. Assembly should start if not already.");
             // The logic in onProcessFinished should trigger assembly.
        } else if (m_video_totalFrames == 0) {
            // This case should be handled after splitting.
            emit logMessage("Real-ESRGAN Video: Frame queue is empty and no frames were processed. Aborting.");
            cleanupVideo();
            emit statusChanged(m_currentRowNum, tr("Error"));
            emit processingFinished(m_currentRowNum, false);
            cleanup();
        }
        return;
    }

    QString frameFile = m_video_frameQueue.dequeue();
    QString inputFramePath = QDir(m_video_inputFramesPath).filePath(frameFile);
    QString outputFramePath = QDir(m_video_outputFramesPath).filePath(frameFile); // Save with same name in output dir

    // Use the existing processImage logic for the frame.
    // processImage will set its own m_originalSourceFile and m_finalDestinationFile for the frame.
    // It also sets up m_scaleSequence based on m_settings.
    processImage(m_currentRowNum, inputFramePath, outputFramePath, m_settings);
}

void RealEsrganProcessor::startVideoAssembly()
{
    if (m_state != State::ProcessingVideoFrames) {
        emit logMessage(QString("Real-ESRGAN Video: Assembly called in unexpected state: %1. Aborting assembly.").arg((int)m_state));
        return;
    }

    emit logMessage(QString("Real-ESRGAN Video: All %1 frames processed. Assembling final video...").arg(m_video_totalFrames));
    emit statusChanged(m_currentRowNum, tr("Assembling video..."));
    m_state = State::AssemblingVideo;

    QStringList args;
    // TODO: Get framerate from original video. For now, assume 25 or make it configurable.
    // A more robust way would be to use ffprobe on the original video during splitting.
    QString framerate = "25"; // Placeholder

    args << "-framerate" << framerate
         << "-i" << QDir(m_video_outputFramesPath).filePath("frame%08d.png")
         << "-i" << m_video_audioPath; // Add audio stream

    // Check if audio file exists, if not, don't try to map it.
    if (!QFile::exists(m_video_audioPath)) {
        emit logMessage("Real-ESRGAN Video: No audio file found. Assembling video without audio.");
        args.removeLast(); // Remove the -i m_video_audioPath
        args << "-c:v" << "libx264" << "-pix_fmt" << "yuv420p"; // Video codec only
    } else {
        args << "-c:v" << "libx264" << "-pix_fmt" << "yuv420p" // Video codec
             << "-c:a" << "aac" // Or "copy" if audio was just extracted and not re-encoded
             << "-shortest"; // Finish encoding when the shortest input stream ends (video or audio)
    }
    args << "-y" << m_finalDestinationFile; // Output file, overwrite if exists

    m_ffmpegProcess->start("ffmpeg", args);
}

void RealEsrganProcessor::cleanupVideo() {
    if (m_ffmpegProcess->state() != QProcess::NotRunning) {
        m_ffmpegProcess->kill();
        m_ffmpegProcess->waitForFinished(500); // Brief wait
    }
    if (!m_video_tempPath.isEmpty()) {
        QDir dir(m_video_tempPath);
        if (dir.exists()) {
            emit logMessage(QString("Real-ESRGAN Video: Cleaning up temporary video path: %1").arg(m_video_tempPath));
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
    // Do not reset m_state here, let cleanup() or successful completion do it.
}

void RealEsrganProcessor::onFfmpegError(QProcess::ProcessError error) {
    // This is for errors starting ffmpeg itself. Runtime errors are in onFfmpegFinished.
    if (m_state == State::Idle) return;

    emit logMessage(QString("FFmpeg process failed to start. State: %1, Error: %2 (Code: %3)")
                    .arg((int)m_state)
                    .arg(m_ffmpegProcess->errorString())
                    .arg(error));
    cleanupVideo();
    emit statusChanged(m_currentRowNum, tr("FFmpeg Start Error"));
    emit processingFinished(m_currentRowNum, false);
    cleanup(); // General cleanup, sets state to Idle
}

void RealEsrganProcessor::onFfmpegStdErr() {
    if (m_state == State::Idle) return;
    QByteArray data = m_ffmpegProcess->readAllStandardError();
    emit logMessage("FFmpeg: " + QString::fromLocal8Bit(data).trimmed());
}
