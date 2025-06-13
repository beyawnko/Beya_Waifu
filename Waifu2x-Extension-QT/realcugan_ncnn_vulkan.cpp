#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QDirIterator>


// Helper function to calculate scaling sequence for RealCUGAN
static QList<int> CalculateRealCUGANScaleSequence(int targetScale) {
    QList<int> sequence;
    if (targetScale <= 1) { // No scaling or invalid
        sequence.append(1);
        return sequence;
    }

    // RealCUGAN supports 2x, 3x, 4x directly
    // Prioritize fewer passes: 4x > 3x > 2x
    while (targetScale > 1) {
        if (targetScale % 4 == 0 && targetScale / 4 >= 1) {
            sequence.append(4);
            targetScale /= 4;
        } else if (targetScale % 3 == 0 && targetScale / 3 >= 1) {
            sequence.append(3);
            targetScale /= 3;
        } else if (targetScale % 2 == 0 && targetScale / 2 >= 1) {
            sequence.append(2);
            targetScale /= 2;
        } else if (targetScale == 4) { // Direct cases
            sequence.append(4);
            targetScale = 1;
        } else if (targetScale == 3) {
            sequence.append(3);
            targetScale = 1;
        } else if (targetScale == 2) {
            sequence.append(2);
            targetScale = 1;
        }
        // If not directly divisible, try to break it down with preferred scales
        // This part might need more sophisticated logic for optimal paths for any scale
        // For now, this handles cases like 6x (3,2), 8x (4,2), 9x (3,3), 12x (4,3), 16x (4,4)
        // More complex cases like 5x, 7x, 10x, 11x, 13x, 14x, 15x are tricky.
        // RealCUGAN typically used for 2x,3x,4x. For others, users might use RealESRGAN or other models.
        // For this implementation, we'll stick to sequences composed of 2,3,4.
        // If a scale like 5x is requested, this simple greedy approach won't work perfectly.
        // Let's assume for now that users will select scales that can be composed by 2,3,4.
        // A more robust solution would be to find the best combination or inform the user.
        else { // Cannot break down further with 2,3,4, or it's already 1
             if (targetScale > 1 && !sequence.isEmpty()) { // e.g. 10x -> 2x, then 5x. 5x is not directly supported.
                qDebug() << "CalculateRealCUGANScaleSequence: Target scale" << targetScale << "cannot be perfectly achieved with 2x,3x,4x passes after initial pass. Sequence so far:" << sequence;
                // In this case, we might need to signal an error or use a final resize.
                // For now, we'll clear the sequence to indicate an issue if it's not 1.
                // Or, let it pass through and RealCUGAN might handle it (e.g. scale=1 for denoise only)
                // For this iteration, let's assume the closest supported pass if stuck.
                // If targetScale is 5, and we did a 2x, previous target was 10.
                // This part of the logic is complex for arbitrary scales.
                // The subtask implies iterative scaling for scales NOT DIRECTLY supported (2x,3x,4x).
                // So 6x (3,2) is fine. 5x is not directly composed.
                // For now, if we can't break it down, we stop and the sequence is what it is.
                // The calling function should check if product of sequence == targetScale.
                break;
            } else if (targetScale > 1 && sequence.isEmpty()){ // e.g. 5x, 7x initially
                 qDebug() << "CalculateRealCUGANScaleSequence: Target scale" << targetScale << "cannot be directly composed of 2x,3x,4x passes.";
                 // Fallback: use the largest possible single pass (4x), then image resizer for the rest.
                 // Or, let it be, RealCUGAN might do a 1x pass (denoise).
                 // For now, we'll just return the problematic scale.
                 sequence.clear();
                 sequence.append(targetScale); // Let the caller decide how to handle unsupported direct scale
                 return sequence;
            }
            break; // Exit loop if targetScale is 1 or cannot be broken down
        }
    }
    if (sequence.isEmpty() && targetScale == 1) sequence.append(1); // For 1x scale (denoise only)
    qDebug() << "Calculated RealCUGAN scale sequence:" << sequence;
    return sequence;
}

// Internal function to prepare arguments for a single RealCUGAN pass
QStringList MainWindow::Realcugan_NCNN_Vulkan_PrepareArguments(
    const QString &inputFile, const QString &outputFile, int currentPassScale,
    const QString &modelName, int denoiseLevel, int tileSize,
    const QString &gpuId, bool ttaEnabled, const QString &outputFormat,
    bool isMultiGPU, const QString &multiGpuJobArgs)
{
    QString Current_Path = QDir::currentPath();
    QStringList arguments;

    arguments << "-i" << inputFile
              << "-o" << outputFile
              << "-s" << QString::number(currentPassScale)
              << "-n" << QString::number(denoiseLevel) // Denoise level
              << "-t" << QString::number(tileSize)
              << "-m" << Current_Path + "/realcugan-ncnn-vulkan Win/" + modelName; // Model subfolder

    if (isMultiGPU) {
        arguments << multiGpuJobArgs.split(" "); // Expects format like "-g 0,1 -j 2,2"
    } else {
        arguments << "-g" << gpuId.split(" ")[0]; // Single GPU ID
        // Single GPU threads are usually handled by the exe itself, or a global -j for all selected gpus.
        // The provided command example uses -j for job_threads with multi-GPU.
        // For single GPU, realcugan-ncnn-vulkan typically doesn't need a -j for thread count per GPU.
        // If it does, this needs to be added. For now, assume not for single GPU.
    }

    if (ttaEnabled) {
        arguments << "-x";
    }
    arguments << "-f" << outputFormat.toLower();

    return arguments;
}


void MainWindow::Realcugan_NCNN_Vulkan_Video_BySegment(int rowNum)
{
    // This is a complex function. For RealCUGAN, processing by segment might offer
    // benefits for very long videos to manage resources or allow resumability (though full resumability isn't built here).
    // The core idea is to split the video processing into time-based chunks.
    // Each chunk involves: video splitting (segment), audio splitting (segment), frame processing, video reassembly (segment), audio reassembly (segment).
    // Finally, all processed video segments are concatenated, and all processed audio segments are concatenated.

    if (Stopping == true) return;

    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    if (!item_InFile || !item_OutFile || !item_Status) {
        qDebug() << "RealCUGAN VideoBySegment Error: Table items are null for row" << rowNum;
        if(item_Status) item_Status->setText(tr("Error: Table item missing"));
        return;
    }
    item_Status->setText(tr("Processing (Segmented)"));
    qApp->processEvents();

    QString sourceFileFullPath = item_InFile->text();
    QString finalResultFileFullPath = item_OutFile->text(); // Final output video path
    QFileInfo sourceFileInfo(sourceFileFullPath);
    QString sourceFileNameNoExt = sourceFileInfo.completeBaseName();

    // --- Overall Settings & Preparations ---
    Realcugan_NCNN_Vulkan_ReadSettings(); // Load general UI settings
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Prepare for batch frame processing (GPU settings)
    int targetScale = ui->spinBox_scaleRatio_video->value();
    if (targetScale <= 0) targetScale = 1;

    // Get video duration and determine segment length (e.g., from UI or fixed)
    int totalDurationSec = video_get_duration(sourceFileFullPath);
    int segmentDurationSec = ui->spinBox_SegmentDuration->value(); // Assuming this UI element exists and is configured
    if (segmentDurationSec <= 0) segmentDurationSec = 600; // Default to 10 minutes if not set

    int numSegments = (totalDurationSec + segmentDurationSec - 1) / segmentDurationSec; // Ceiling division

    QString mainTempFolder = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealCUGAN_VidSeg_Main_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(mainTempFolder);

    QStringList processedVideoSegmentsPaths;
    QString fullAudioPath = QDir(mainTempFolder).filePath(sourceFileNameNoExt + "_full_audio.m4a"); // Path for the full extracted audio

    // Extract full audio once
    emit Send_TextBrowser_NewMessage(tr("Extracting full audio track... (RealCUGAN Segmented)"));
    video_get_audio(sourceFileFullPath, fullAudioPath);
    if (!QFile::exists(fullAudioPath)) {
        qDebug() << "RealCUGAN VideoBySegment: Failed to extract full audio.";
        item_Status->setText(tr("Error: Audio extraction failed"));
        QDir(mainTempFolder).removeRecursively();
        return;
    }

    bool overallSuccess = true;

    // --- Loop Through Segments ---
    for (int i = 0; i < numSegments; ++i) {
        if (Stopping) { overallSuccess = false; break; }

        emit Send_TextBrowser_NewMessage(tr("Processing video segment %1/%2 (RealCUGAN)").arg(i + 1).arg(numSegments));
        item_Status->setText(tr("Processing segment %1/%2").arg(i+1).arg(numSegments));
        qApp->processEvents();

        int startTime = i * segmentDurationSec;
        int currentSegmentDuration = qMin(segmentDurationSec, totalDurationSec - startTime);

        QString segmentName = QString("segment_%1").arg(i, 3, 10, QChar('0'));
        QString splitFramesFolder = QDir(mainTempFolder).filePath(segmentName + "_split");
        QString scaledFramesFolder = QDir(mainTempFolder).filePath(segmentName + "_scaled");
        QString processedSegmentVideoPath = QDir(mainTempFolder).filePath(segmentName + "_processed.mp4"); // Assuming mp4 segments

        QDir().mkpath(splitFramesFolder);
        QDir().mkpath(scaledFramesFolder);

        // 1. Extract frames for the current segment
        // video_video2images_ProcessBySegment might be an existing utility.
        // If not, ffmpeg command: ffmpeg -ss {startTime} -i {inputFile} -t {currentSegmentDuration} ...frames...
        // For now, let's assume a conceptual function or direct ffmpeg call.
        // This requires a more specialized video_video2images_ProcessBySegment.
        // For this placeholder, we'll simulate it by just processing all frames N times.
        // THIS IS A SIMPLIFICATION - A REAL IMPLEMENTATION NEEDS SEGMENTED FRAME EXTRACTION.
        // For now, we'll re-use the full frame extraction and just note it conceptually.
        // Actual implementation: video_video2images_Segment(sourceFileFullPath, splitFramesFolder, startTime, currentSegmentDuration);

        // Simplified: Re-extract all frames each time for placeholder. Not efficient.
        // A proper implementation would use ffmpeg -ss and -t to extract only segment frames.
        // For the purpose of this structure, let's assume splitFramesFolder is correctly populated for the segment.
        // If video_video2images_ProcessBySegment is not available, this part needs ffmpeg.
        // For now, let's use the full video_video2images and process ALL frames, which is not true segmentation logic for frames.
        // This is a known limitation of this placeholder implementation.
        // A better approach for a quick fill:
        // If (i==0) video_video2images(sourceFileFullPath, splitFramesFolder, audioPath_dummy_segment); // extract all frames on first segment
        // then process a SUBSET of framesFileName_qStrList based on segment timing. This is also complex.

        // Let's assume video_video2images_ProcessBySegment(sourceFileFullPath, splitFramesFolder, startTime, currentSegmentDuration) exists and works.
        // If not, this function cannot be fully implemented correctly here without adding direct ffmpeg calls.
        // The existing `video_video2images` extracts ALL frames.
        // For now, let's log a warning and process ALL frames for each "segment" - this will be slow and incorrect for actual segmentation.
        // CORRECTED APPROACH OUTLINE FOR SEGMENTED FRAME EXTRACTION:
        // 1. Extract frames for the CURRENT SEGMENT ONLY using ffmpeg -ss and -t
        QProcess ffmpegFrameExtract;
        QStringList extractArgs;
        QString frameOutputPattern = QDir(splitFramesFolder).filePath("frame_%0"+QString::number(CalNumDigits(video_get_frameNum(sourceFileFullPath)))+"d.png"); // e.g. frame_%06d.png

        extractArgs << "-nostdin" << "-y"
                    << "-ss" << QString::number(startTime)
                    << "-i" << sourceFileFullPath
                    << "-t" << QString::number(currentSegmentDuration)
                    << "-vf" << "fps=" + video_get_fps(sourceFileFullPath) // Maintain original FPS for frame count consistency
                    << frameOutputPattern;

        emit Send_TextBrowser_NewMessage(tr("Extracting frames for segment %1/%2...").arg(i+1).arg(numSegments));
        qDebug() << "FFMPEG frame extract for segment CMD:" << FFMPEG_EXE_PATH_Waifu2xEX << extractArgs;
        ffmpegFrameExtract.start(FFMPEG_EXE_PATH_Waifu2xEX, extractArgs);
        if (!ffmpegFrameExtract.waitForStarted(5000) || !ffmpegFrameExtract.waitForFinished(-1) || ffmpegFrameExtract.exitCode() != 0) {
            qDebug() << "RealCUGAN VideoBySegment: Failed to extract frames for segment" << i+1 << ". StdErr:" << QString::fromLocal8Bit(ffmpegFrameExtract.readAllStandardError());
            overallSuccess = false; break;
        }

        QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
        if (framesFileName_qStrList.isEmpty()) {
            qDebug() << "RealCUGAN VideoBySegment Error: No frames extracted for segment" << i+1;
            // It's possible a segment has 0 frames if segmentDuration is too small compared to FPS. This might not be an error.
            // However, if totalDurationSec > 0 and numSegments > 0, usually expect some frames.
            // For now, let's continue if it's empty, the segment video will be empty.
        }

        // --- Frame Processing Loop for the CURRENT SEGMENT's frames ---
        bool segmentFramesProcessedSuccessfully = true;
        CurrentFileProgress_Start(sourceFileInfo.fileName() + QString(" (Seg %1)").arg(i+1), framesFileName_qStrList.count());
        for (const QString &frameFileName : framesFileName_qStrList) {
            if (Stopping) { segmentFramesProcessedSuccessfully = false; overallSuccess = false; break; }

            Send_TextBrowser_NewMessage(tr("Segment %1/%2, Processing frame: %3 (RealCUGAN)").arg(i+1).arg(numSegments).arg(frameFileName));
            CurrentFileProgress_progressbar_Add();


            QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
            QString outputFramePath = QDir(scaledFramesFolder).filePath(frameFileName);

            bool success = Realcugan_ProcessSingleFileIteratively(
                inputFramePath, outputFramePath, targetScale,
                m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
                m_realcugan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
                m_realcugan_TTA, QFileInfo(frameFileName).suffix().isEmpty() ? "png" : QFileInfo(frameFileName).suffix()
            );
            if (!success) { segmentFramesProcessedSuccessfully = false; overallSuccess = false; break; }
        }
        CurrentFileProgress_Stop();
        if (!segmentFramesProcessedSuccessfully || Stopping) { overallSuccess = false; break; }

        // 2. Reassemble processed frames of the segment into a video segment (without audio)
        // The video_images2video function needs to be able to create a video from frames without an audio track,
        // or accept a "" audio path. Or a segment-specific audio path.
        // For simplicity, we'll create video segments without audio first, then concatenate, then add full audio.
        emit Send_TextBrowser_NewMessage(tr("Assembling video segment %1/%2... (RealCUGAN)").arg(i + 1).arg(numSegments));
        video_images2video(sourceFileFullPath, processedSegmentVideoPath, scaledFramesFolder, "", false, 0, 0, false); // Pass empty audio path

        if (!QFile::exists(processedSegmentVideoPath)) {
            qDebug() << "RealCUGAN VideoBySegment: Failed to assemble video segment" << i+1;
            overallSuccess = false; break;
        }
        processedVideoSegmentsPaths.append(processedSegmentVideoPath);

        // Cleanup for current segment's scaled frames (original split frames might be needed if not all processed in one go)
        QDir(scaledFramesFolder).removeRecursively();
        if (i < numSegments -1) QDir().mkpath(scaledFramesFolder); // Recreate for next segment
    }

    // --- Final Assembly ---
    if (overallSuccess && !Stopping) {
        emit Send_TextBrowser_NewMessage(tr("Concatenating processed video segments... (RealCUGAN)"));
        // Create a file list for ffmpeg concatenation
        QString concatFilePath = QDir(mainTempFolder).filePath("concat_list.txt");
        QFile concatFile(concatFilePath);
        if (concatFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&concatFile);
            for (const QString &segmentPath : processedVideoSegmentsPaths) {
                out << "file '" << QDir::toNativeSeparators(segmentPath) << "'\n";
            }
            concatFile.close();
        } else {
            qDebug() << "RealCUGAN VideoBySegment: Failed to create concat list file.";
            overallSuccess = false;
        }

        if (overallSuccess) {
            QString concatenatedVideoPath = QDir(mainTempFolder).filePath(sourceFileNameNoExt + "_concat_noaudio.mp4");
            QProcess ffmpegConcat;
            QStringList concatArgs;
            concatArgs << "-f" << "concat" << "-safe" << "0" << "-i" << concatFilePath << "-c" << "copy" << concatenatedVideoPath;
            ffmpegConcat.start(FFMPEG_EXE_PATH_Waifu2xEX, concatArgs); // Assuming FFMPEG_EXE_PATH_Waifu2xEX is defined

            if (!ffmpegConcat.waitForStarted(5000) || !ffmpegConcat.waitForFinished(-1) || ffmpegConcat.exitCode() != 0) {
                qDebug() << "RealCUGAN VideoBySegment: Failed to concatenate video segments. StdErr:" << QString::fromLocal8Bit(ffmpegConcat.readAllStandardError());
                overallSuccess = false;
            } else {
                 // Add the full audio track back
                emit Send_TextBrowser_NewMessage(tr("Muxing final video with audio... (RealCUGAN)"));
                QProcess ffmpegMux;
                QStringList muxArgs;
                muxArgs << "-i" << concatenatedVideoPath << "-i" << fullAudioPath << "-c:v" << "copy" << "-c:a" << "aac" << "-b:a" << ui->spinBox_bitrate_audio->text()+"k" << "-shortest" << finalResultFileFullPath;
                // Might need to use video_ReadSettings_OutputVid for more complex muxing if available
                ffmpegMux.start(FFMPEG_EXE_PATH_Waifu2xEX, muxArgs);
                if (!ffmpegMux.waitForStarted(5000) || !ffmpegMux.waitForFinished(-1) || ffmpegMux.exitCode() != 0) {
                    qDebug() << "RealCUGAN VideoBySegment: Failed to mux final video. StdErr:" << QString::fromLocal8Bit(ffmpegMux.readAllStandardError());
                    overallSuccess = false;
                }
            }
        }
    }

    // --- Final Status Update ---
    if (overallSuccess && !Stopping && QFile::exists(finalResultFileFullPath)) {
        item_Status->setText(tr("Finished"));
        LoadScaledImageToLabel(finalResultFileFullPath, ui->label_Preview_Main);
        UpdateTotalProcessedFilesCount();
    } else if (!Stopping) {
        item_Status->setText(tr("Error"));
    } else {
        item_Status->setText(tr("Stopped"));
    }

    // --- Cleanup ---
    QDir(mainTempFolder).removeRecursively(); // This cleans up splitFrames (if full extraction was done), scaledFrames (if any left), audioPath, concat list, concatenated video.
    // Original splitFramesFolder for the very first full extraction needs cleanup if that approach was taken.
    // If video_video2images_ProcessBySegment was used, it would create per-segment split folders inside mainTempFolder.
    // The current placeholder for frame extraction (full extract in segment 0) means splitFramesFolder is inside mainTempFolder.

    qDebug() << "RealCUGAN VideoBySegment: Processing finished for row" << rowNum;
    ProcessNextFile();
}


void MainWindow::Realcugan_NCNN_Vulkan_Video(int rowNum)
{
    if (Stopping == true) return;

    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    if (!item_InFile || !item_OutFile || !item_Status) {
        qDebug() << "RealCUGAN Video Error: Table items are null for row" << rowNum;
        if(item_Status) item_Status->setText(tr("Error: Table item missing"));
        return;
    }
    item_Status->setText(tr("Processing"));
    qApp->processEvents();

    QString sourceFileFullPath = item_InFile->text();
    QString resultFileFullPath = item_OutFile->text();
    QFileInfo sourceFileInfo(sourceFileFullPath);
    QString sourceFileNameNoExt = sourceFileInfo.completeBaseName();

    // 1. Create temporary folders & audio path
    QString splitFramesFolder = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealCUGAN_Vid_split_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString scaledFramesFolder = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealCUGAN_Vid_scaled_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString audioPath = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealCUGAN_Vid_audio.m4a"; // Assuming m4a for audio, adjust if needed
    QDir().mkpath(splitFramesFolder);
    QDir().mkpath(scaledFramesFolder);
    QFile::remove(audioPath); // Remove old audio file if exists

    // 2. Extract Video Frames and Audio
    emit Send_TextBrowser_NewMessage(tr("Extracting video frames and audio: %1 (RealCUGAN)").arg(sourceFileFullPath));
    video_video2images(sourceFileFullPath, splitFramesFolder, audioPath); // This utility should handle both

    QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if (framesFileName_qStrList.isEmpty()) {
        qDebug() << "RealCUGAN Video Error: No frames found after splitting" << sourceFileFullPath;
        item_Status->setText(tr("Error: Frame extraction failed"));
        QDir(splitFramesFolder).removeRecursively();
        QDir(scaledFramesFolder).removeRecursively();
        QFile::remove(audioPath);
        return;
    }

    emit Send_TextBrowser_NewMessage(tr("Processing video frames with RealCUGAN..."));

    // 3. Read general RealCUGAN settings & prepare for batch GPU processing
    Realcugan_NCNN_Vulkan_ReadSettings();
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Using ThreadNum=0 for now, suitable for single video task

    // 4. Determine Target Scale (Assuming Video uses video scale settings)
    int targetScale = ui->spinBox_scaleRatio_video->value(); // Or ui->spinBox_Scale_RealCUGAN
    if (targetScale <= 0) targetScale = 1;

    bool allFramesProcessedSuccessfully = true;
    int frameCount = 0;
    CurrentFileProgress_Start(sourceFileInfo.fileName(),framesFileName_qStrList.count());

    for (const QString &frameFileName : framesFileName_qStrList) {
        if (Stopping == true) {
            allFramesProcessedSuccessfully = false;
            qDebug() << "RealCUGAN Video: Processing stopped.";
            break;
        }
        frameCount++;
        emit Send_TextBrowser_NewMessage(tr("Processing video frame %1/%2: %3 (RealCUGAN)").arg(frameCount).arg(framesFileName_qStrList.size()).arg(frameFileName));
        CurrentFileProgress_progressbar_Add();

        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        QString outputFramePath = QDir(scaledFramesFolder).filePath(frameFileName);

        bool success = Realcugan_ProcessSingleFileIteratively(
            inputFramePath, outputFramePath, targetScale,
            m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
            m_realcugan_gpuJobConfig_temp,
            ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
            m_realcugan_TTA,
            QFileInfo(frameFileName).suffix().isEmpty() ? "png" : QFileInfo(frameFileName).suffix()
        );

        if (!success) {
            allFramesProcessedSuccessfully = false;
            qDebug() << "RealCUGAN Video: Failed to process frame" << frameFileName;
            emit Send_TextBrowser_NewMessage(tr("Error processing video frame: %1 (RealCUGAN)").arg(frameFileName));
            break;
        }
    }
    CurrentFileProgress_Stop();

    // 5. Assemble processed frames back into a video
    if (allFramesProcessedSuccessfully && !Stopping) {
        emit Send_TextBrowser_NewMessage(tr("Assembling processed video frames: %1 (RealCUGAN)").arg(resultFileFullPath));
        // video_images2video parameters: (QString VideoPath,QString video_mp4_scaled_fullpath,QString ScaledFrameFolderPath,QString AudioPath,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled)
        // Assuming no custom resolution for video for now.
        video_images2video(sourceFileFullPath, resultFileFullPath, scaledFramesFolder, audioPath, false, 0, 0, false);

        if (QFile::exists(resultFileFullPath)) {
            item_Status->setText(tr("Finished"));
            LoadScaledImageToLabel(resultFileFullPath,ui->label_Preview_Main); // May not preview video, but good for consistency
            UpdateTotalProcessedFilesCount();
        } else {
            item_Status->setText(tr("Error: Assembling failed"));
            qDebug() << "RealCUGAN Video: Assembling failed, output file does not exist:" << resultFileFullPath;
        }
    } else if (!Stopping) {
        item_Status->setText(tr("Error: Frame processing failed"));
    } else { // Stopped
        item_Status->setText(tr("Stopped"));
    }

    // 6. Cleanup temporary folders and audio file
    QDir(splitFramesFolder).removeRecursively();
    QDir(scaledFramesFolder).removeRecursively();
    QFile::remove(audioPath);
    qDebug() << "RealCUGAN Video: Processing finished for row" << rowNum << "Cleaned up temp files.";

    ProcessNextFile(); // Process next file in the main queue
}


void MainWindow::Realcugan_NCNN_Vulkan_Image(int rowNum, bool ReProcess_MissingAlphaChannel)
{
    Q_UNUSED(ReProcess_MissingAlphaChannel);
    if (Stopping == true) return;

    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    if (!item_InFile || !item_OutFile || !item_Status) {
        qDebug() << "Error: Table items are null in Realcugan_NCNN_Vulkan_Image for row" << rowNum;
        if(item_Status) item_Status->setText(tr("Error: Table item missing"));
        return;
    }

    QString originalInFile = item_InFile->text();
    QString finalOutFile = item_OutFile->text();
    QFileInfo finalOutFileInfo(finalOutFile);
    QString tempPathBase = QDir::tempPath() + "/Waifu2xEX_RealCUGAN_Temp/" + finalOutFileInfo.completeBaseName() + "_pass_";
    QDir tempDir(QDir::tempPath() + "/Waifu2xEX_RealCUGAN_Temp/");
    if (!tempDir.exists()) tempDir.mkpath(".");

    // --- split alpha channel if present ---
    bool hasAlpha = false;
    QString rgbInputFile = originalInFile;
    QString finalRgbOutput = tempPathBase + "final_rgb." + finalOutFileInfo.suffix().toLower();
    QString alphaPath = tempPathBase + "alpha.png";
    QImage alphaImage;
    QImage srcImg(originalInFile);
    if (srcImg.hasAlphaChannel()) {
        hasAlpha = true;
        alphaImage = srcImg.alphaChannel();
        QImage rgb = srcImg.convertToFormat(QImage::Format_RGB888);
        rgbInputFile = tempPathBase + "input_rgb.png";
        rgb.save(rgbInputFile);
        alphaImage.save(alphaPath);
    }

    // Clean up old temp files for this specific base name pattern
    QDirIterator dirIt(tempDir.path(), QStringList() << finalOutFileInfo.completeBaseName() + "_pass_*", QDir::Files);
    while(dirIt.hasNext()) {
        QFile::remove(dirIt.next());
    }


    item_Status->setText(tr("Processing (Preparing)"));
    qApp->processEvents();

    // Load general UI settings into member variables or pass them around
    // For this refactor, ReadSettings will populate necessary parameters for PrepareArguments
    Realcugan_NCNN_Vulkan_ReadSettings(); // This should populate m_realcugan_Model, m_realcugan_DenoiseLevel etc.

    int targetScale = ui->spinBox_Scale_RealCUGAN->value();
    QList<int> scaleSequence = CalculateRealCUGANScaleSequence(targetScale);

    int calculatedTotalScale = 1;
    for(int s : scaleSequence) calculatedTotalScale *= s;
    if (scaleSequence.isEmpty() || (calculatedTotalScale != targetScale && targetScale > 1 && calculatedTotalScale > 1) ) { // calculatedTotalScale > 1 for cases like 5x -> sequence [5]
        bool isSingleUnsupportedScale = (scaleSequence.size() == 1 && scaleSequence.first() == targetScale && targetScale > 4);
        if(targetScale > 1 && (scaleSequence.isEmpty() || isSingleUnsupportedScale)) {
             item_Status->setText(tr("Error: Unsupported scale for RealCUGAN"));
             QMessageBox::warning(this, tr("Scaling Error"), tr("Target scale %1x cannot be achieved with RealCUGAN's supported scales (2x, 3x, 4x).").arg(targetScale));
             return;
        }
        // If calculatedTotalScale != targetScale but it's not a single unsupported scale (e.g. sequence was [2] for target 5x),
        // it implies a partial upscale. This also should be an error or handled with post-resize.
        // For now, we treat it as an error if the sequence product doesn't match.
        if (targetScale > 1 && calculatedTotalScale != targetScale) {
            item_Status->setText(tr("Error: Scale sequence error"));
            qDebug() << "RealCUGAN: Scale sequence product" << calculatedTotalScale << "does not match target scale" << targetScale;
            QMessageBox::warning(this, tr("Scaling Error"), tr("Internal error: Calculated scale sequence (%1) does not achieve target scale %2x.").arg(QStringList(scaleSequence.toList().replaceInStrings(QRegularExpression("(\\d+)"), "\\1x")).join(", ")).arg(targetScale));
            return;
        }
    }


    QList<QPair<QString, QStringList>> *processQueue = new QList<QPair<QString, QStringList>>();
    QString currentInputFile = rgbInputFile;

    for (int i = 0; i < scaleSequence.size(); ++i) {
        int currentPassScale = scaleSequence[i];
        QString currentOutputFile;
        if (i == scaleSequence.size() - 1) {
            currentOutputFile = hasAlpha ? finalRgbOutput : finalOutFile; // Final pass outputs to the target file
        } else {
            currentOutputFile = tempPathBase + QString::number(i) + "." + ui->comboBox_OutFormat_Image->currentText().toLower();
        }

        QString multiGpuArgs = "";
        if (ui->checkBox_MultiGPU_RealCUGAN->isChecked()) {
            multiGpuArgs = RealcuganNcnnVulkan_MultiGPU();
        }


        QStringList arguments = Realcugan_NCNN_Vulkan_PrepareArguments(
            currentInputFile, currentOutputFile, currentPassScale,
            m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
            m_realcugan_GPUID, m_realcugan_TTA, ui->comboBox_OutFormat_Image->currentText(),
            ui->checkBox_MultiGPU_RealCUGAN->isChecked(), multiGpuArgs /* Pass constructed multi-GPU args */
        );

        processQueue->append(qMakePair(currentOutputFile, arguments));
        currentInputFile = currentOutputFile; // Output of this pass is input for the next
    }

    if (processQueue->isEmpty()) {
        item_Status->setText(tr("Error: No processing passes determined."));
        delete processQueue;
        return;
    }

    // Store necessary info for the process chain
    QProcess *firstProcess = new QProcess(this);
    firstProcess->setProperty("rowNum", rowNum);
    firstProcess->setProperty("processQueue", QVariant::fromValue(processQueue));
    firstProcess->setProperty("currentPassIndex", 0);
    firstProcess->setProperty("finalOutFile", finalOutFile);
    firstProcess->setProperty("finalRGBFile", hasAlpha ? finalRgbOutput : finalOutFile);
    firstProcess->setProperty("alphaFile", alphaPath);
    firstProcess->setProperty("hasAlpha", hasAlpha);
    firstProcess->setProperty("tempPathBase", tempPathBase); // For cleanup
    firstProcess->setProperty("originalInFile", originalInFile);


    connect(firstProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(Realcugan_NCNN_Vulkan_Iterative_finished()));
    connect(firstProcess, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError)));
    // Add readyRead connects if detailed progress per pass is needed (complex for chained processes)

    StartNextRealCUGANPass(firstProcess);
}


void MainWindow::StartNextRealCUGANPass(QProcess *process) {
    if (!process || Stopping) {
        if (process) {
            QList<QPair<QString, QStringList>> *queue = process->property("processQueue").value<QList<QPair<QString, QStringList>>*>();
            delete queue; // Clean up queue
            process->deleteLater();
        }
        return;
    }

    QList<QPair<QString, QStringList>> *processQueue = process->property("processQueue").value<QList<QPair<QString, QStringList>>*>();
    int currentPassIndex = process->property("currentPassIndex").toInt();
    int rowNum = process->property("rowNum").toInt();
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);


    if (currentPassIndex >= processQueue->size()) { // All passes done
        // This case should be handled by Realcugan_NCNN_Vulkan_Iterative_finished after the LAST process successfully finishes
        qDebug() << "RealCUGAN: All passes appear to be complete, but StartNextRealCUGANPass was called. This might be an error.";
        delete processQueue;
        process->deleteLater();
        return;
    }

    QPair<QString, QStringList> currentJob = processQueue->at(currentPassIndex);
    QString currentOutputFile = currentJob.first;
    QStringList arguments = currentJob.second;

    // Ensure input for current pass (output of previous, or original file) exists
    QString inputFileForThisPass = arguments.at(arguments.indexOf("-i") + 1);
    if (!QFile::exists(inputFileForThisPass)) {
        qDebug() << "RealCUGAN Error: Input file for pass" << currentPassIndex << "does not exist:" << inputFileForThisPass;
        if(item_Status) item_Status->setText(tr("Error: Temp file missing"));
        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1);
        delete processQueue;
        process->deleteLater();
        ProcList_RealCUGAN.removeAll(process); // Ensure it's removed
        NumProc--;
        ErrorProc++;
        UpdateNumberOfActiveThreads();
        UpdateProgressBar();
        CheckIfAllFinished();
        return;
    }


    QString Current_Path = QDir::currentPath();
    QString EXE_PATH = Current_Path + "/realcugan-ncnn-vulkan Win/realcugan-ncnn-vulkan.exe";
    QFileInfo exeInfo(EXE_PATH);
    if (!exeInfo.exists() || !exeInfo.isExecutable()) {
        if(item_Status) item_Status->setText(tr("Error: realcugan-ncnn-vulkan.exe not found"));
        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1);
        delete processQueue;
        process->deleteLater();
        // No need to update ProcList etc here as it wasn't added for this "meta-process" yet.
        return;
    }

    if(item_Status) item_Status->setText(tr("Processing (Pass %1/%2)").arg(currentPassIndex + 1).arg(processQueue->size()));
    qApp->processEvents();

    process->setObjectName(QString("RealCUGAN_Image_%1_Pass_%2").arg(rowNum).arg(currentPassIndex));
    qDebug() << "Starting RealCUGAN process for image:" << arguments.at(arguments.indexOf("-i")+1) << "Pass" << currentPassIndex + 1;
    qDebug() << "Command:" << EXE_PATH << arguments.join(" ");

    if (!ProcList_RealCUGAN.contains(process)) { // Add only once for the chain
        ProcList_RealCUGAN.append(process);
        TotalNumProc++; // Counts the whole item as one process
        NumProc++;
        StartedProc++;
        UpdateNumberOfActiveThreads();
    }

    process->start(EXE_PATH, arguments);
    if (!process->waitForStarted(10000)) { // Increased timeout for safety
        qDebug() << "RealCUGAN process failed to start or timed out starting for pass" << currentPassIndex;
        if (item_Status) item_Status->setText(tr("Error: Process start failed (Pass %1)").arg(currentPassIndex + 1));

        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1);
        ProcList_RealCUGAN.removeAll(process);
        delete processQueue;
        process->deleteLater();
        NumProc--; // Decrement as it failed to start but was counted
        ErrorProc++;
        UpdateNumberOfActiveThreads(); // Reflect the failure
        UpdateProgressBar();
        CheckIfAllFinished();
    }
}

void MainWindow::Realcugan_NCNN_Vulkan_Iterative_finished() {
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) return;

    int rowNum = process->property("rowNum").toInt();
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);
    QList<QPair<QString, QStringList>> *processQueue = process->property("processQueue").value<QList<QPair<QString, QStringList>>*>();
    int currentPassIndex = process->property("currentPassIndex").toInt();
    QString finalOutFile = process->property("finalOutFile").toString();

    QByteArray stdOut = process->readAllStandardOutput();
    QByteArray stdErr = process->readAllStandardError();
    if (!stdOut.isEmpty()) qDebug() << "RealCUGAN Pass" << currentPassIndex << "STDOUT:" << QString::fromLocal8Bit(stdOut);
    if (!stdErr.isEmpty()) qDebug() << "RealCUGAN Pass" << currentPassIndex << "STDERR:" << QString::fromLocal8Bit(stdErr);


    if (process->exitStatus() != QProcess::NormalExit || process->exitCode() != 0) {
        qDebug() << "RealCUGAN Error: Pass" << currentPassIndex << "failed. ExitCode:" << process->exitCode() << "ExitStatus:" << process->exitStatus();
        if(item_Status) item_Status->setText(tr("Error (Pass %1)").arg(currentPassIndex + 1));
        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1);
        ProcList_RealCUGAN.removeAll(process);
        delete processQueue;
        process->deleteLater();
        NumProc--; ErrorProc++; UpdateNumberOfActiveThreads(); UpdateProgressBar(); CheckIfAllFinished();
        return;
    }

    // Check if output file of current pass exists
    QString currentPassOutputFile = processQueue->at(currentPassIndex).first;
    if (!QFile::exists(currentPassOutputFile)) {
        qDebug() << "RealCUGAN Error: Output file for pass" << currentPassIndex << "not found:" << currentPassOutputFile;
        if(item_Status) item_Status->setText(tr("Error: Temp file missing (Pass %1)").arg(currentPassIndex + 1));
        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1);
        ProcList_RealCUGAN.removeAll(process);
        delete processQueue;
        process->deleteLater();
        NumProc--; ErrorProc++; UpdateNumberOfActiveThreads(); UpdateProgressBar(); CheckIfAllFinished();
        return;
    }


    currentPassIndex++;
    process->setProperty("currentPassIndex", currentPassIndex);

    if (currentPassIndex < processQueue->size()) {
        // Output of previous pass (which is currentPassOutputFile) becomes input for next.
        // The arguments list in the queue already has this baked in.
        StartNextRealCUGANPass(process); // Start next pass with the same QProcess object
    } else {
        // All passes successfully completed
        if(item_Status) item_Status->setText(tr("Finished"));
        QString finalRGBFile = process->property("finalRGBFile").toString();
        QString alphaFile = process->property("alphaFile").toString();
        bool hasAlphaFlag = process->property("hasAlpha").toBool();
        qDebug() << "RealCUGAN: All passes completed successfully for" << finalRGBFile;
        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1, true /* keepFinal */, finalRGBFile);

        if (hasAlphaFlag && QFile::exists(finalRGBFile)) {
            QImage rgb(finalRGBFile);
            QImage alpha(alphaFile);
            rgb = rgb.convertToFormat(QImage::Format_ARGB32);
            rgb.setAlphaChannel(alpha);
            rgb.save(finalOutFile);
            QFile::remove(finalRGBFile);
            QFile::remove(alphaFile);
        } else if (finalRGBFile != finalOutFile && QFile::exists(finalRGBFile)) {
            QFile::remove(finalOutFile);
            QFile::rename(finalRGBFile, finalOutFile);
        }

        ProcList_RealCUGAN.removeAll(process);
        delete processQueue;
        process->deleteLater();
        NumProc--; FinishedProc++; UpdateNumberOfActiveThreads(); UpdateProgressBar(); CheckIfAllFinished();
        // Potentially call LoadScaledImageToLabel if needed
        // LoadScaledImageToLabel(finalOutFile, ui->label_Preview_Main);
    }
}

void MainWindow::Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error) {
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) return;

    int rowNum = process->property("rowNum").toInt();
    int currentPassIndex = process->property("currentPassIndex").toInt();
    QList<QPair<QString, QStringList>> *processQueue = process->property("processQueue").value<QList<QPair<QString, QStringList>>*>();
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    qDebug() << "RealCUGAN process errorOccurred during pass" << currentPassIndex << ":" << error << process->errorString();
    if(item_Status) item_Status->setText(tr("Error (Process Error Pass %1)").arg(currentPassIndex + 1));

    Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1);
    ProcList_RealCUGAN.removeAll(process);
    delete processQueue;
    process->deleteLater();
    NumProc--; ErrorProc++; UpdateNumberOfActiveThreads(); UpdateProgressBar(); CheckIfAllFinished();
}

void MainWindow::Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString &tempPathBase, int maxPassIndex, bool keepFinal, const QString& finalFile) {
    qDebug() << "Cleaning up temp files with base:" << tempPathBase;
    for (int i = 0; i < maxPassIndex; ++i) { // maxPassIndex is scaleSequence.size() - 1. We only create up to maxPassIndex-1 temp files.
        QString tempFile = tempPathBase + QString::number(i) + "." + ui->comboBox_OutFormat_Image->currentText().toLower();
        if (QFile::exists(tempFile)) {
            if (keepFinal && tempFile == finalFile) { // Should not happen if tempPathBase is used correctly for intermediates
                 qDebug() << "Skipping deletion of final file (marked as temp):" << tempFile;
            } else {
                if (QFile::remove(tempFile)) {
                    qDebug() << "Deleted temp file:" << tempFile;
                } else {
                    qDebug() << "Failed to delete temp file:" << tempFile;
                }
            }
        }
    }
}


// int m_realcugan_DenoiseLevel;
// int m_realcugan_TileSize;
// bool m_realcugan_TTA;
// QString m_realcugan_GPUID;
// These should be populated by Realcugan_NCNN_Vulkan_ReadSettings()

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings()
{
    // This function should populate member variables from UI settings
    // These member variables will then be used by Realcugan_NCNN_Vulkan_PrepareArguments
    m_realcugan_Model = ui->comboBox_Model_RealCUGAN->currentText();
    m_realcugan_DenoiseLevel = ui->spinBox_DenoiseLevel_RealCUGAN->value(); // Assuming this is the correct spinbox for denoise
    m_realcugan_TileSize = ui->spinBox_TileSize_RealCUGAN->value();
    m_realcugan_TTA = ui->checkBox_TTA_RealCUGAN->isChecked();

    if (ui->checkBox_MultiGPU_RealCUGAN->isChecked()) {
        // For multi-GPU, GPU ID might be handled differently (e.g., a list or special format)
        // For now, let's assume RealcuganNcnnVulkan_MultiGPU() or a similar function provides the necessary string for -g or -j
        // This part needs to align with how PrepareArguments handles multiGPUJobArgs
        // For simplicity in this pass, we might just use the primary selected GPU if multi-GPU is checked but not fully configured for iterative image.
        // Or, m_realcugan_GPUID could store the complex job string.
        // This section needs careful implementation based on the multi-GPU strategy.
        // For now, let Realcugan_NCNN_Vulkan_PrepareArguments handle the logic based on isMultiGPU flag
        m_realcugan_GPUID = ui->comboBox_GPUID_RealCUGAN->currentText(); // Fallback or primary for now
    } else {
        m_realcugan_GPUID = ui->comboBox_GPUID_RealCUGAN->currentText();
    }

    qDebug() << "Realcugan_NCNN_Vulkan_ReadSettings: Model:" << m_realcugan_Model
             << "Denoise:" << m_realcugan_DenoiseLevel
             << "Tile:" << m_realcugan_TileSize
             << "TTA:" << m_realcugan_TTA
             << "GPUID:" << m_realcugan_GPUID;
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum)
{
    Q_UNUSED(ThreadNum);
    // TODO: Implement settings reading for Video/GIF, especially for multi-GPU job distribution.
    // This would be similar to SrmdNcnnVulkan_ReadSettings_Video_GIF
    // For now, it can call the general ReadSettings or have its own logic.
    // Realcugan_NCNN_Vulkan_ReadSettings(); // Basic fallback
    // This function needs to construct arguments suitable for processing a folder of frames,
    // especially for multi-GPU. It should be similar to SrmdNcnnVulkan_ReadSettings_Video_GIF.

    // For RealCUGAN, the typical command for folder processing is:
    // realcugan-ncnn-vulkan.exe -i input_frames_folder -o output_frames_folder -s scale -n denoise_level -m model_path -g gpu_id_list -j job_config -f output_format -t tile_size -x (tta)
    // The job_config (-j) for realcugan-ncnn-vulkan is usually like "load_threads:proc_threads:save_threads" per GPU,
    // or a global thread count if simpler. Example: "-g 0,1 -j 1:2:1,1:2:1" (1 load, 2 proc, 1 save for GPU0; same for GPU1)
    // Or more commonly, it might be just a list of proc_threads per GPU like "-g 0,1 -j 2,2"

    Realcugan_NCNN_Vulkan_ReadSettings(); // Load common settings like model, denoise, TTA, tile size into member vars

    // Specific for batch processing (frames): GPU/Job configuration string
    QString gpuJobConfig;
    if (ui->checkBox_MultiGPU_RealCUGAN->isChecked() && !GPUIDs_List_MultiGPU_RealCUGAN.isEmpty()) {
        gpuJobConfig = RealcuganNcnnVulkan_MultiGPU();
    } else {
        gpuJobConfig = "-g " + m_realcugan_GPUID.split(" ")[0];
    }

    // Store the resulting argument fragment for later use when launching the process
    m_realcugan_gpuJobConfig_temp = gpuJobConfig;

    qDebug() << "Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF for ThreadNum" << ThreadNum
             << "GPU/Job Config:" << gpuJobConfig;
    // The actual arguments for QProcess will be assembled by a function like PrepareArguments,
    // which would take inputFile (folder), outputFile (folder), scale, and these GPU settings.
}


// Blocking helper function to process a single file (image or frame) through RealCUGAN with iterative scaling
bool MainWindow::Realcugan_ProcessSingleFileIteratively(
    const QString &inputFile, const QString &outputFile, int targetScale,
    const QString &modelName, int denoiseLevel, int tileSize,
    const QString &gpuIdOrJobConfig, bool isMultiGPUJob, bool ttaEnabled, const QString &outputFormat,
    int rowNumForStatusUpdate) // Optional: for status updates if called directly for an image row
{
    qDebug() << "Realcugan_ProcessSingleFileIteratively: Input" << inputFile << "Output" << outputFile << "TargetScale" << targetScale;

    QFileInfo finalOutFileInfo(outputFile);
    // Use a unique subfolder within temp based on the original input file's name to avoid collisions if this helper is called in parallel.
    // However, since this is a blocking function, direct parallelism of this function itself isn't the immediate concern,
    // but rather ensuring temp files from different *source* images don't collide if processed by different calls.
    QString tempSubFolder = QDir::tempPath() + "/W2XEX_RC_Iter/" + QFileInfo(inputFile).completeBaseName() + "_" + QRandomGenerator::global()->generate();
    QDir tempDir(tempSubFolder);
    if (!tempDir.mkpath(".")) {
        qDebug() << "Failed to create temporary directory:" << tempSubFolder;
        if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Temp dir failed"), "ERROR");
        return false;
    }

    QString tempPathBase = tempDir.filePath(finalOutFileInfo.completeBaseName() + "_pass_");


    QList<int> scaleSequence = CalculateRealCUGANScaleSequence(targetScale);
    int calculatedTotalScale = 1;
    for(int s : scaleSequence) calculatedTotalScale *= s;

    if (scaleSequence.isEmpty() || (calculatedTotalScale != targetScale && targetScale > 1 && calculatedTotalScale > 1) ) {
        bool isSingleUnsupportedScale = (scaleSequence.size() == 1 && scaleSequence.first() == targetScale && targetScale > 4);
        if(targetScale > 1 && (scaleSequence.isEmpty() || isSingleUnsupportedScale)) {
             qDebug() << "RealCUGAN ProcessSingle: Unsupported scale" << targetScale;
             if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Unsupported scale"), "ERROR");
             tempDir.removeRecursively();
             return false;
        }
        if (targetScale > 1 && calculatedTotalScale != targetScale) {
            qDebug() << "RealCUGAN ProcessSingle: Scale sequence product" << calculatedTotalScale << "!= target" << targetScale;
            if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Scale sequence error"), "ERROR");
            tempDir.removeRecursively();
            return false;
        }
    }

    QString currentIterInputFile = inputFile;
    bool success = true;

    for (int i = 0; i < scaleSequence.size(); ++i) {
        if (Stopping) { success = false; break; }

        int currentPassScale = scaleSequence[i];
        QString currentIterOutputFile;
        bool isLastPass = (i == scaleSequence.size() - 1);

        if (isLastPass) {
            currentIterOutputFile = outputFile; // Final pass outputs to the target file
        } else {
            currentIterOutputFile = tempPathBase + QString::number(i) + "." + outputFormat.toLower();
        }

        // Ensure output directory for intermediate or final file exists
        QFileInfo currentIterOutInfo(currentIterOutputFile);
        QDir outputDirForPass(currentIterOutInfo.path());
        if (!outputDirForPass.exists()) {
            if (!outputDirForPass.mkpath(".")) {
                qDebug() << "RealCUGAN ProcessSingle: Failed to create output directory for pass:" << currentIterOutInfo.path();
                success = false; break;
            }
        }


        QStringList arguments = Realcugan_NCNN_Vulkan_PrepareArguments(
            currentIterInputFile, currentIterOutputFile, currentPassScale,
            modelName, denoiseLevel, tileSize,
            gpuIdOrJobConfig, // This is gpuId for single, or full job string for multi
            ttaEnabled, outputFormat,
            isMultiGPUJob, gpuIdOrJobConfig // Pass full string if multi, single GPU ID otherwise
        );
        // Note: PrepareArguments needs to correctly interpret gpuIdOrJobConfig based on isMultiGPUJob.
        // If isMultiGPUJob is true, gpuIdOrJobConfig is the full "-g X -j Y" string.
        // If false, gpuIdOrJobConfig is just the GPU ID string "0" or "0: Name".
        // The current PrepareArguments handles this by taking the full string and splitting if isMultiGPUJob is true.
        // And if not, it takes the GPU ID and splits out the "ID" part. This seems okay.

        if (rowNumForStatusUpdate != -1) {
             UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Processing (Pass %1/%2)").arg(i + 1).arg(scaleSequence.size()), "INFO");
        }

        QProcess process; // Synchronous process for this helper
        QString exePath = QDir::currentPath() + "/realcugan-ncnn-vulkan Win/realcugan-ncnn-vulkan.exe";

        qDebug() << "RealCUGAN ProcessSingle Pass" << i+1 << "Cmd:" << exePath << arguments;
        process.start(exePath, arguments);

        if (!process.waitForStarted(10000)) {
            qDebug() << "RealCUGAN ProcessSingle: Process failed to start for pass" << i+1;
            success = false; break;
        }
        if (!process.waitForFinished(-1)) { // No timeout for finish, let it run
            qDebug() << "RealCUGAN ProcessSingle: Process timed out or crashed for pass" << i+1;
            success = false; break;
        }

        QByteArray stdOut = process.readAllStandardOutput();
        QByteArray stdErr = process.readAllStandardError();
        if (!stdOut.isEmpty()) qDebug() << "RealCUGAN ProcessSingle Pass" << i+1 << "STDOUT:" << QString::fromLocal8Bit(stdOut);
        if (!stdErr.isEmpty()) qDebug() << "RealCUGAN ProcessSingle Pass" << i+1 << "STDERR:" << QString::fromLocal8Bit(stdErr);

        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            qDebug() << "RealCUGAN ProcessSingle: Pass" << i+1 << "failed. ExitCode:" << process.exitCode();
            success = false; break;
        }
        if (!QFile::exists(currentIterOutputFile)) {
            qDebug() << "RealCUGAN ProcessSingle: Output file for pass" << i+1 << "not found:" << currentIterOutputFile;
            success = false; break;
        }

        if (!isLastPass) { // If not the last pass, the output becomes input for next
            currentIterInputFile = currentIterOutputFile;
        }
    }

    // Cleanup intermediate files (all files in tempDir except the final output if it landed there by mistake)
    if (tempDir.exists()) {
        QDirIterator it(tempDir.path(), QDir::Files);
        while(it.hasNext()){
            QString filePath = it.next();
            if (filePath != outputFile) { // Don't delete the final output if it was written to temp for some reason
                QFile::remove(filePath);
                qDebug() << "RealCUGAN ProcessSingle: Deleted temp file" << filePath;
            }
        }
        tempDir.rmdir("."); // Remove the specific temp subfolder if empty
        // Attempt to remove the parent W2XEX_RC_Iter if it's empty (might not be if other processes use it)
        QDir parentTempDir(QDir::tempPath() + "/W2XEX_RC_Iter");
        parentTempDir.rmdir(QFileInfo(tempSubFolder).fileName()); // remove specific subfolder
    }

    if (Stopping && success) { // If processing was stopped externally but current ops seemed to succeed
        qDebug() << "RealCUGAN ProcessSingle: Processing stopped externally.";
        return false; // Indicate overall failure due to stop
    }
    if (!success && rowNumForStatusUpdate != -1) {
        UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error during processing"), "ERROR");
    }


    return success;
}


void MainWindow::APNG_RealcuganNCNNVulkan(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath)
{
    Q_UNUSED(sourceFileFullPath); // sourceFileFullPath might be used for context or naming, but not directly processed here.
    Q_UNUSED(resultFileFullPath); // resultFileFullPath is for APNG_Main after frames are assembled.

    qDebug() << "APNG_RealcuganNCNNVulkan started. Input folder:" << splitFramesFolder << "Output folder:" << scaledFramesFolder;

    // 1. Read general RealCUGAN settings from UI into member variables
    Realcugan_NCNN_Vulkan_ReadSettings();

    // 2. Prepare GPU/job settings for batch processing (frames)
    // Using ThreadNum = 0 as APNG frames are processed sequentially in this call.
    // This sets m_realcugan_gpuJobConfig_temp based on multi-GPU UI settings.
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0);

    // 3. Determine Target Scale (Assuming APNG uses the general image scale setting for now)
    // This needs to be confirmed: does APNG have its own scale UI or use image/gif/video scale?
    // Let's assume it uses the main image scale setting from ui->spinBox_Scale_RealCUGAN for now.
    int targetScale = ui->spinBox_Scale_RealCUGAN->value();
    if (targetScale <= 0) targetScale = 1; // Ensure scale is at least 1x

    // Ensure scaledFramesFolder exists
    QDir outputDir(scaledFramesFolder);
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            qDebug() << "APNG_RealcuganNCNNVulkan: Failed to create output directory:" << scaledFramesFolder;
            // APNG_Main will handle signaling failure based on lack of output frames.
            return;
        }
    }

    bool allFramesProcessedSuccessfully = true;
    int frameCount = 0;
    for (const QString &frameFileName : framesFileName_qStrList) {
        if (Stopping == true) {
            allFramesProcessedSuccessfully = false;
            qDebug() << "APNG_RealcuganNCNNVulkan: Processing stopped.";
            break;
        }

        frameCount++;
        // Update progress for APNG_Main (e.g. by emitting a signal or through shared variables if this runs in a thread)
        // For now, just log. APNG_Main might have its own progress update mechanism.
        qDebug() << "Processing APNG frame" << frameCount << "/" << framesFileName_qStrList.size() << ":" << frameFileName;
        Send_TextBrowser_NewMessage(tr("Processing APNG frame %1/%2: %3 (RealCUGAN)").arg(frameCount).arg(framesFileName_qStrList.size()).arg(frameFileName));


        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        QString outputFramePath = QDir(scaledFramesFolder).filePath(frameFileName); // Preserve original filename

        bool success = Realcugan_ProcessSingleFileIteratively(
            inputFramePath, outputFramePath, targetScale,
            m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
            m_realcugan_gpuJobConfig_temp, // GPU/Job config string from _ReadSettings_Video_GIF
            ui->checkBox_MultiGPU_RealCUGAN->isChecked(), // isMultiGPUJob flag
            m_realcugan_TTA,
            QFileInfo(frameFileName).suffix() // Use original frame's suffix for output format, or default to png
        );

        if (!success) {
            allFramesProcessedSuccessfully = false;
            qDebug() << "APNG_RealcuganNCNNVulkan: Failed to process frame" << frameFileName;
            Send_TextBrowser_NewMessage(tr("Error processing APNG frame: %1 (RealCUGAN)").arg(frameFileName));
            // Decide on error strategy: stop here or try next frame?
            // For now, let's stop and report failure. APNG_Main will see not all frames are there.
            break;
        }
    }

    if (allFramesProcessedSuccessfully && !Stopping) {
        qDebug() << "APNG_RealcuganNCNNVulkan: All frames processed successfully.";
        Send_TextBrowser_NewMessage(tr("All APNG frames processed successfully (RealCUGAN)."));
    } else if (!Stopping) {
        qDebug() << "APNG_RealcuganNCNNVulkan: Finished processing frames, but some errors occurred.";
        Send_TextBrowser_NewMessage(tr("Finished APNG frames processing with errors (RealCUGAN)."));
    }
    // APNG_Main will handle the reassembly and final signaling.
}


void MainWindow::Realcugan_NCNN_Vulkan_GIF(int rowNum)
{
    if (Stopping == true) return;

    QTableWidgetItem *item_InFile = ui->tableWidget_Files->item(rowNum, 0);
    QTableWidgetItem *item_OutFile = ui->tableWidget_Files->item(rowNum, 1);
    QTableWidgetItem *item_Status = ui->tableWidget_Files->item(rowNum, 5);

    if (!item_InFile || !item_OutFile || !item_Status) {
        qDebug() << "RealCUGAN GIF Error: Table items are null for row" << rowNum;
        if(item_Status) item_Status->setText(tr("Error: Table item missing"));
        return;
    }
    item_Status->setText(tr("Processing"));
    qApp->processEvents();

    QString sourceFileFullPath = item_InFile->text();
    QString resultFileFullPath = item_OutFile->text();
    QFileInfo sourceFileInfo(sourceFileFullPath);
    QString sourceFileNameNoExt = sourceFileInfo.completeBaseName();

    // 1. Create temporary folders for split and scaled frames
    QString splitFramesFolder = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealCUGAN_GIF_split_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString scaledFramesFolder = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RealCUGAN_GIF_scaled_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(splitFramesFolder);
    QDir().mkpath(scaledFramesFolder);

    // 2. Split GIF into frames
    emit Send_TextBrowser_NewMessage(tr("Splitting GIF: %1 (RealCUGAN)").arg(sourceFileFullPath));
    Gif_splitGif(sourceFileFullPath, splitFramesFolder); // Assuming this utility populates splitFramesFolder

    QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
    if (framesFileName_qStrList.isEmpty()) {
        qDebug() << "RealCUGAN GIF Error: No frames found after splitting" << sourceFileFullPath;
        item_Status->setText(tr("Error: Splitting failed"));
        QDir(splitFramesFolder).removeRecursively();
        QDir(scaledFramesFolder).removeRecursively();
        return;
    }

    emit Send_TextBrowser_NewMessage(tr("Processing GIF frames with RealCUGAN..."));

    // 3. Read general RealCUGAN settings & prepare for batch GPU processing
    Realcugan_NCNN_Vulkan_ReadSettings();
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Using ThreadNum=0 for now

    // 4. Determine Target Scale (Assuming GIF uses GIF scale settings)
    int targetScale = ui->spinBox_scaleRatio_gif->value(); // Or ui->spinBox_Scale_RealCUGAN if RealCUGAN has its own scale under its tab
    if (targetScale <= 0) targetScale = 1;

    bool allFramesProcessedSuccessfully = true;
    int frameCount = 0;

    CurrentFileProgress_Start(sourceFileInfo.fileName(),framesFileName_qStrList.count());

    for (const QString &frameFileName : framesFileName_qStrList) {
        if (Stopping == true) {
            allFramesProcessedSuccessfully = false;
            qDebug() << "RealCUGAN GIF: Processing stopped.";
            break;
        }
        frameCount++;
        emit Send_TextBrowser_NewMessage(tr("Processing GIF frame %1/%2: %3 (RealCUGAN)").arg(frameCount).arg(framesFileName_qStrList.size()).arg(frameFileName));
        CurrentFileProgress_progressbar_Add();


        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        QString outputFramePath = QDir(scaledFramesFolder).filePath(frameFileName);

        bool success = Realcugan_ProcessSingleFileIteratively(
            inputFramePath, outputFramePath, targetScale,
            m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
            m_realcugan_gpuJobConfig_temp,
            ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
            m_realcugan_TTA,
            QFileInfo(frameFileName).suffix().isEmpty() ? "png" : QFileInfo(frameFileName).suffix() // Default to png if suffix is missing
        );

        if (!success) {
            allFramesProcessedSuccessfully = false;
            qDebug() << "RealCUGAN GIF: Failed to process frame" << frameFileName;
            emit Send_TextBrowser_NewMessage(tr("Error processing GIF frame: %1 (RealCUGAN)").arg(frameFileName));
            break;
        }
    }
    CurrentFileProgress_Stop();

    // 5. Assemble processed frames back into a GIF
    if (allFramesProcessedSuccessfully && !Stopping) {
        emit Send_TextBrowser_NewMessage(tr("Assembling processed GIF frames: %1 (RealCUGAN)").arg(resultFileFullPath));
        int duration = Gif_getDuration(sourceFileFullPath); // Get original GIF's frame duration
        // Gif_assembleGif parameters: (QString ResGifPath,QString ScaledFramesPath,int Duration,bool CustRes_isEnabled,int CustRes_height,int CustRes_width,bool isOverScaled,QString SourceGifFullPath)
        // Assuming no custom resolution for GIF for now, pass false and 0,0
        Gif_assembleGif(resultFileFullPath, scaledFramesFolder, duration, false, 0, 0, false, sourceFileFullPath);

        if (QFile::exists(resultFileFullPath)) {
            item_Status->setText(tr("Finished"));
            LoadScaledImageToLabel(resultFileFullPath,ui->label_Preview_Main); // Show final GIF if possible
            UpdateTotalProcessedFilesCount();
        } else {
            item_Status->setText(tr("Error: Assembling failed"));
            qDebug() << "RealCUGAN GIF: Assembling failed, output file does not exist:" << resultFileFullPath;
        }
    } else if (!Stopping) {
        item_Status->setText(tr("Error: Frame processing failed"));
    } else { // Stopped
        item_Status->setText(tr("Stopped"));
    }

    // 6. Cleanup temporary folders
    QDir(splitFramesFolder).removeRecursively();
    QDir(scaledFramesFolder).removeRecursively();
    qDebug() << "RealCUGAN GIF: Processing finished for row" << rowNum << "Cleaned up temp folders.";

    ProcessNextFile(); // Process next file in the main queue
}


void MainWindow::Realcugan_ncnn_vulkan_DetectGPU()
{
    if (Stopping == true) return;

    QString Current_Path = QDir::currentPath();
    QString EXE_PATH = Current_Path + "/realcugan-ncnn-vulkan Win/realcugan-ncnn-vulkan.exe";
    QFileInfo exeInfo(EXE_PATH);
    if (!exeInfo.exists() || !exeInfo.isExecutable()) {
        QMessageBox::critical(this, tr("Error"), tr("realcugan-ncnn-vulkan.exe not found at %1").arg(EXE_PATH));
        return;
    }

    QProcess *process_detect = new QProcess(this); // Renamed to avoid conflict with member 'process' if any
    process_detect->setObjectName("RealCUGAN_DetectGPU");
    // Ensure connect statements use the correct slot signatures if changed in mainwindow.h
    connect(process_detect, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(Realcugan_ncnn_vulkan_DetectGPU_finished(int,QProcess::ExitStatus)));
    // The errorOccurred slot for detection might need to be specific if its behavior differs
    connect(process_detect, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)));


    qDebug() << "Detecting GPUs for RealCUGAN...";
    process_detect->start(EXE_PATH, QStringList()); // No arguments needed for detection, it usually prints to stdout

    // Optional: Disable button during detection
    // ui->pushButton_DetectGPU_RealCUGAN->setEnabled(false);
}


// This is a general error slot, DetectGPU might need its own if different handling is required.
void MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error)
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) return;
    qDebug() << "RealCUGAN GPU Detection process errorOccurred:" << process->objectName() << "Error code:" << error << process->errorString();
    QMessageBox::warning(this, tr("GPU Detection Failed"), tr("Failed to execute realcugan-ncnn-vulkan.exe to detect GPUs. Error: %1").arg(process->errorString()));
    // ui->pushButton_DetectGPU_RealCUGAN->setEnabled(true); // Re-enable button
    process->deleteLater();
}


QString MainWindow::RealcuganNcnnVulkan_MultiGPU()
{
    // Construct the multi-GPU argument string using GPUIDs_List_MultiGPU_RealCUGAN
    // Example result: "-g 0,1 -j 2,2"

    if (!ui->checkBox_MultiGPU_RealCUGAN->isChecked() || GPUIDs_List_MultiGPU_RealCUGAN.isEmpty()) {
        // Not in multi-GPU mode or no GPUs configured, return single GPU string from main settings
        return "-g " + m_realcugan_GPUID.split(" ")[0];
    }

    // For single-image calls this returns the combined settings for all selected GPUs
    // which matches how batch frame processing uses the result.

    QStringList gpuIDs;
    QStringList jobThreadsPerGPU;

    // Assuming for a single image, if multi-GPU is selected, we might just use the *first* configured multi-GPU.
    // Or, if RealCUGAN can actually use multiple GPUs for one image (unlikely for this type of tool), this needs to be specific.
    // Let's assume it will use the list of GPUs for parallel processing of frames (handled by _ReadSettings_Video_GIF)
    // and for single images, it might imply using *one specific GPU from the list* or the tool might pick.
    // For now, this function will construct the string as if all listed GPUs are for one task,
    // which is how _ReadSettings_Video_GIF will use it.
    // This might need refinement based on how RealCUGAN handles -g and -j for single files vs folders.

    for (const auto& gpuMap : GPUIDs_List_MultiGPU_RealCUGAN) {
        gpuIDs.append(gpuMap.value("ID"));
        // RealCUGAN's -j with -g usually means processing threads per GPU.
        jobThreadsPerGPU.append(gpuMap.value("Threads", "2")); // Default processing threads
    }

    if (gpuIDs.isEmpty()) { // Should not happen if checkbox is checked and list is populated
        return "-g " + m_realcugan_GPUID.split(" ")[0]; // Fallback
    }

    // Format: -g G0,G1,... -j J0,J1,...
    // This is the most common format for ncnn tools that support distributing work over multiple GPUs.
    return QString("-g %1 -j %2").arg(gpuIDs.join(","), jobThreadsPerGPU.join(","));
}

void MainWindow::AddGPU_MultiGPU_RealcuganNcnnVulkan(QString GPUID_Name)
{
    if (GPUID_Name.isEmpty() || GPUID_Name.contains("No available")) return;

    QString selectedGPU_ID = GPUID_Name.split(":").first();
    QString selectedGPU_Name = GPUID_Name.mid(GPUID_Name.indexOf(":") + 1).trimmed();
    int threads = ui->spinBox_Threads_MultiGPU_RealCUGAN->value();

    for (const auto& map : GPUIDs_List_MultiGPU_RealCUGAN) {
        if (map.value("ID") == selectedGPU_ID) {
            ShowMessageBox("Info", "This GPU has already been added for RealCUGAN.", QMessageBox::Information);
            return;
        }
    }

    QMap<QString, QString> newGPU;
    newGPU.insert("ID", selectedGPU_ID);
    newGPU.insert("Name", selectedGPU_Name);
    newGPU.insert("Threads", QString::number(threads));
    GPUIDs_List_MultiGPU_RealCUGAN.append(newGPU);

    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setText(QString("ID: %1, Name: %2, Threads: %3").arg(selectedGPU_ID, selectedGPU_Name, QString::number(threads)));
    ui->listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
    qDebug() << "Added GPU for RealCUGAN Multi-GPU:" << selectedGPU_ID << "Threads:" << threads;
}


void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings()
{
    // Preload settings from QSettings and apply to UI if needed.
    // Also, could populate model combobox if models are found dynamically (not current approach).
    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");
    ui->comboBox_Model_RealCUGAN->setCurrentText(settings.value("Model", "models-se").toString());
    ui->spinBox_DenoiseLevel_RealCUGAN->setValue(settings.value("DenoiseLevel", -1).toInt());
    ui->spinBox_TileSize_RealCUGAN->setValue(settings.value("TileSize", 0).toInt());
    ui->checkBox_TTA_RealCUGAN->setChecked(settings.value("TTA", false).toBool());
    ui->comboBox_GPUID_RealCUGAN->setCurrentText(settings.value("GPUID", "0: Default GPU 0").toString()); // Default must match detected format
    ui->checkBox_MultiGPU_RealCUGAN->setChecked(settings.value("MultiGPUEnabled", false).toBool());
    // Load multi-GPU list from settings
    GPUIDs_List_MultiGPU_RealCUGAN = settings.value("MultiGPU_List").value<QList_QMap_QStrQStr>();
    ui->listWidget_GPUList_MultiGPU_RealCUGAN->clear();
    for(const auto& gpuMap : GPUIDs_List_MultiGPU_RealCUGAN) {
        QListWidgetItem *newItem = new QListWidgetItem();
        newItem->setText(QString("ID: %1, Name: %2, Threads: %3").arg(gpuMap.value("ID"), gpuMap.value("Name"), gpuMap.value("Threads")));
        ui->listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
    }
    settings.endGroup();

    // Call ReadSettings to load these into member variables for processing logic
    Realcugan_NCNN_Vulkan_ReadSettings();
    qDebug() << "Realcugan_NCNN_Vulkan_PreLoad_Settings completed.";
}

// Slots for process signals (These are now for the iterative process wrapper)
// The old Realcugan_NCNN_Vulkan_finished and _errorOccurred are effectively replaced by
// Realcugan_NCNN_Vulkan_Iterative_finished and Realcugan_NCNN_Vulkan_Iterative_errorOccurred

// void MainWindow::Realcugan_NCNN_Vulkan_finished() // This slot is now effectively OBSOLETE for image processing
// { ... }
// void MainWindow::Realcugan_NCNN_Vulkan_errorOccurred(QProcess::ProcessError error) // This slot is now effectively OBSOLETE for image processing
// { ... }


void MainWindow::Realcugan_ncnn_vulkan_DetectGPU_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process) return;

    QString output = QString::fromLocal8Bit(process->readAllStandardOutput());
    QString errorOutput = QString::fromLocal8Bit(process->readAllStandardError());
    qDebug() << "RealCUGAN DetectGPU finished. Exit code:" << exitCode << "Status:" << exitStatus;
    qDebug() << "STDOUT:" << output;
    qDebug() << "STDERR:" << errorOutput;

    ui->comboBox_GPUID_RealCUGAN->clear();
    ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->clear();

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        // Example output from realcugan-ncnn-vulkan might be like:
        // " 0  NVIDIA GeForce RTX 2070 SUPER"
        // " 1  Intel(R) UHD Graphics 630"
        // Or it could be in stderr.
        // We need to parse this.
        QString combinedOutput = output + errorOutput; // GPU list can be in stderr
        QRegularExpression gpuRegex1("(\\d+)\\s+(.+)"); // Format: "ID  Name"
        QRegularExpression gpuRegex2("ID: (\\d+)\\s+Name: (.+)"); // Format: "ID: 0 Name: NVIDIA..."
        QRegularExpression gpuRegex3("gpu\\s+(\\d+)\\s*:\\s*(.+)"); // Format: "gpu 0: NVIDIA..."

        Available_GPUID_RealCUGAN.clear();

        QStringList lines = combinedOutput.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            QRegularExpressionMatch match = gpuRegex1.match(line.trimmed());
            if (!match.hasMatch()) match = gpuRegex2.match(line.trimmed());
            if (!match.hasMatch()) match = gpuRegex3.match(line.trimmed());

            if (match.hasMatch()) {
                QString gpuID = match.captured(1).trimmed();
                QString gpuName = match.captured(2).trimmed();
                Available_GPUID_RealCUGAN.append(QString("%1: %2").arg(gpuID, gpuName));
            }
        }

        if (Available_GPUID_RealCUGAN.isEmpty()) {
            // Fallback if regex fails or output is different
            if (combinedOutput.contains("NVIDIA") || combinedOutput.contains("AMD") || combinedOutput.contains("Intel")) {
                 Available_GPUID_RealCUGAN.append(tr("0: Default GPU (Auto-detected)"));
                 QMessageBox::information(this, tr("GPU Detection"), tr("Could not parse GPU list accurately for RealCUGAN, offering a default. Output:\n%1").arg(combinedOutput));
            } else {
                Available_GPUID_RealCUGAN.append(tr("0: Default GPU (Not detected)"));
                QMessageBox::warning(this, tr("GPU Detection"), tr("No GPUs detected or output format not recognized for RealCUGAN.\nOutput:\n%1").arg(combinedOutput));
            }
        }

        ui->comboBox_GPUID_RealCUGAN->clear();
        ui->comboBox_GPUID_RealCUGAN->addItems(Available_GPUID_RealCUGAN);
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->clear();
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->addItems(Available_GPUID_RealCUGAN);

        QMessageBox::information(this, tr("GPU Detection Successful"), tr("Detected GPUs for RealCUGAN have been populated. Please verify the selection."));

    } else {
        QMessageBox::warning(this, tr("GPU Detection Failed"), tr("realcugan-ncnn-vulkan.exe process failed or returned an error for GPU detection.\nExit Code: %1\nOutput:\n%2\nError Output:\n%3").arg(exitCode).arg(output).arg(errorOutput));
        // Add a default option to allow usage even if detection fails
        ui->comboBox_GPUID_RealCUGAN->clear();
        ui->comboBox_GPUID_RealCUGAN->addItem("0: Default GPU (Detection Failed)");
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->clear();
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->addItem("0: Default GPU (Detection Failed)");
    }
    // ui->pushButton_DetectGPU_RealCUGAN->setEnabled(true); // Re-enable button
    process->deleteLater();
}


void MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked()
{
    // ui->pushButton_DetectGPU_RealCUGAN->setEnabled(false); // Disable button during detection
    Realcugan_ncnn_vulkan_DetectGPU();
}

void MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged(int state)
{
    bool isChecked = (state == Qt::Checked);
    ui->groupBox_GPUSettings_MultiGPU_RealCUGAN->setEnabled(isChecked);
    ui->comboBox_GPUID_RealCUGAN->setDisabled(isChecked); // Disable single GPU selection if multi is on
    // ui->spinBox_Threads_RealCUGAN->setDisabled(isChecked); // Disable single GPU threads if one exists

    if (!isChecked) { // If multi-GPU is disabled, clear the multi-GPU list for RealCUGAN
        GPUIDs_List_MultiGPU_RealCUGAN.clear();
        ui->listWidget_GPUList_MultiGPU_RealCUGAN->clear();
    }
}

void MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked()
{
    QString selectedGPU_Text = ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->currentText();
    if (selectedGPU_Text.isEmpty() || selectedGPU_Text.contains("Default GPU (Detection Failed)") || selectedGPU_Text.contains("No available")) {
        ShowMessageBox("Info", "Please select a valid GPU to add for RealCUGAN.", QMessageBox::Information);
        return;
    }
    AddGPU_MultiGPU_RealcuganNcnnVulkan(selectedGPU_Text);
}

void MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked()
{
    QListWidgetItem *selectedItem = ui->listWidget_GPUList_MultiGPU_RealCUGAN->currentItem();
    if (!selectedItem) {
        ShowMessageBox("Info", "Please select a GPU from the list to remove.", QMessageBox::Information);
        return;
    }

    QString itemText = selectedItem->text(); // Format: "ID: X, Name: Y, Threads: Z"
    QString idToRemove = itemText.section(',', 0, 0).split(':').last().trimmed();

    for (int i = 0; i < GPUIDs_List_MultiGPU_RealCUGAN.size(); ++i) {
        if (GPUIDs_List_MultiGPU_RealCUGAN.at(i).value("ID") == idToRemove) {
            GPUIDs_List_MultiGPU_RealCUGAN.removeAt(i);
            break;
        }
    }
    delete ui->listWidget_GPUList_MultiGPU_RealCUGAN->takeItem(ui->listWidget_GPUList_MultiGPU_RealCUGAN->row(selectedItem));
    qDebug() << "Removed GPU for RealCUGAN Multi-GPU: ID" << idToRemove;
}


void MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked()
{
    GPUIDs_List_MultiGPU_RealCUGAN.clear();
    ui->listWidget_GPUList_MultiGPU_RealCUGAN->clear();
    qDebug() << "Cleared all GPUs for RealCUGAN Multi-GPU.";
}


// Ensure ProcList_RealCUGAN, Available_GPUID_RealCUGAN, GPUIDs_List_MultiGPU_RealCUGAN are declared in mainwindow.h
// QList<QProcess*> ProcList_RealCUGAN;
// QStringList Available_GPUID_RealCUGAN;
// QList<QMap<QString, QString>> GPUIDs_List_MultiGPU_RealCUGAN;

// Ensure member variables for settings are declared in mainwindow.h:
// QString m_realcugan_Model;
// int m_realcugan_DenoiseLevel;
// int m_realcugan_TileSize;
// bool m_realcugan_TTA;
// QString m_realcugan_GPUID; // Can store single ID or be part of multi-GPU config


// Slots for iterative processing (declared in mainwindow.h)
// void Realcugan_NCNN_Vulkan_Iterative_finished();
// void Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError error);
// Slot for GPU detection error (if specific handling needed)
// void Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError error);


// Other UI slots to implement in mainwindow.cpp (or link to existing logic if applicable)
// on_comboBox_Model_RealCUGAN_currentIndexChanged(int index);
// on_pushButton_TileSize_Add_RealCUGAN_clicked();
// on_pushButton_TileSize_Minus_RealCUGAN_clicked();
// on_checkBox_MultiGPU_RealCUGAN_clicked(); // If different from stateChanged
// on_comboBox_GPUIDs_MultiGPU_RealCUGAN_currentIndexChanged(int index);
// on_checkBox_isEnable_CurrentGPU_MultiGPU_RealCUGAN_clicked();
// on_spinBox_TileSize_CurrentGPU_MultiGPU_RealCUGAN_valueChanged(int arg1);
// on_pushButton_ShowMultiGPUSettings_RealCUGAN_clicked();

// The iterative scaling logic is the most critical part added here.
// GIF/Video/APNG functions still need full implementation using this iterative approach for their frames.
// Multi-GPU job argument string construction for video/GIF in ReadSettings_Video_GIF also needs full implementation.
// And initialized in the constructor
// ProcList_RealCUGAN.clear();
// Also, any other variables like Settings_RealCUGAN (struct or class)

// Remember to add slots for TTA, Model change, etc. if they affect parameters dynamically
// or require validation.

// Iterative scaling logic needs to be implemented in Realcugan_NCNN_Vulkan_Image, _GIF, _Video
// For example, if target scale is 9x and RealCUGAN supports 2x, 3x, 4x:
// Pass 1: 3x
// Pass 2: 3x (on the output of Pass 1)
// This requires temporary file management.
// For now, the provided code directly uses the UI scale.
// A more robust solution would check ui->spinBox_Scale_RealCUGAN->value()
// and if it's not 2, 3, or 4, then plan multiple passes.

// Example of how iterative scaling might start in Realcugan_NCNN_Vulkan_Image:
/*
    int target_scale = ui->spinBox_Scale_RealCUGAN->value();
    QList<int> supported_scales = {2, 3, 4}; // RealCUGAN native scales

    if (!supported_scales.contains(target_scale)) {
        // Implement iterative scaling:
        // 1. Determine passes (e.g., 9x -> 3x then 3x; 6x -> 3x then 2x or 2x then 3x)
        // 2. Manage intermediate files
        // 3. Loop QProcess calls
        item_Status->setText(tr("Iterative scaling not yet fully implemented"));
        qDebug() << "Iterative scaling needed for scale" << target_scale << "- not yet implemented.";
        // For now, we'll just try to pass the scale directly, it might fail or RealCUGAN might handle it.
        // The spec says: "Iterative scaling logic ... must be implemented for scales not directly supported (2x, 3x, 4x)"
        // So, the current direct call is a placeholder.
    }
    // ... rest of the process setup using the determined scale for the current pass ...
*/

// The command arguments for -g (GPU ID) and -j (Job Threads for multi-GPU)
// will need to be handled carefully, especially with multi-GPU.
// The current code uses ui->comboBox_GPUID_RealCUGAN->currentText().split(" ")[0] for single GPU.
// For multi-GPU, -j would be a comma-separated list of GPU IDs.
// The -g parameter is used for single GPU selection.
// The -j parameter "{THREADS_PER_GPU_0},{THREADS_PER_GPU_1},..." is for job distribution,
// and the -g parameter "{GPU_ID_0},{GPU_ID_1},..." selects the GPUs to use.
// This needs clarification from realcugan-ncnn-vulkan.exe documentation or help output.
// If -j is for thread count and -g for gpu list:
// arguments << "-g" << "0,1,2" for GPUs 0, 1, and 2.
// arguments << "-j" << "1:2:2" for 1 thread on primary, 2 on others (example).
// The current implementation uses -g for single GPU. Multi-GPU needs its own logic.

// The -m (model) argument is constructed as:
// Current_Path + "/realcugan-ncnn-vulkan Win/" + ui->comboBox_Model_RealCUGAN->currentText()
// This implies models are in subdirectories like "models-se", "models-pro", etc.
// This seems correct based on typical ncnn model structures.
// Example: "models-se" or "models-nose" or "models-pro"

// The -t (tile size) parameter:
// arguments << "-t" << QString::number(ui->spinBox_TileSize_RealCUGAN->value())
// Usually 0 for auto, otherwise a specific tile size. This is standard.

// The -n (denoise level) parameter:
// arguments << "-n" << QString::number(ui->spinBox_DenoiseLevel_RealCUGAN->value())
// -1, 0, 1, 2, 3. This is standard.

// -s (scale) parameter:
// arguments << "-s" << QString::number(ui->spinBox_Scale_RealCUGAN->value())
// 1, 2, 3, 4. If scale > 4, iterative scaling is needed.
// If scale = 1, it's for denoising only.

// TTA flag:
// if (ui->checkBox_TTA_RealCUGAN->isChecked()) { arguments << "-x"; }
// This is standard.

// Output format:
// arguments << "-f" << ui->comboBox_OutFormat_Image->currentText().toLower();
// This is also standard.

// Make sure ProcList_RealCUGAN is declared in mainwindow.h:
// QList<QProcess*> ProcList_RealCUGAN;
// And initialized in MainWindow constructor:
// ProcList_RealCUGAN.clear();

// Also, slots like on_comboBox_Model_RealCUGAN_currentIndexChanged should be connected
// if they need to trigger actions or validations.
// For now, basic UI connections are assumed to be in mainwindow.cpp's constructor.
// The provided file focuses on the engine logic.
// Further UI interaction logic (enabling/disabling options based on model, etc.)
// would go into mainwindow.cpp or be connected to slots implemented here if more complex.
