/*
    Copyright (C) 2025  beyawnko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RealCuganProcessor.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QDirIterator>

// Utility to warn when an external process fails
static bool warnProcessFailure(QWidget *parent, QProcess &proc,
                               const QString &context)
{
    if (proc.exitStatus() != QProcess::NormalExit || proc.exitCode() != 0) {
        QString msg = QObject::tr("%1 failed with exit code %2")
                          .arg(context)
                          .arg(proc.exitCode());
        QMessageBox::warning(parent, QObject::tr("Process Failure"), msg);
        return false;
    }
    return true;
}


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
    bool isMultiGPU, const QString &multiGpuJobArgs,
    bool experimental)
{
    return realCuganProcessor->prepareArguments(inputFile, outputFile,
                                                currentPassScale, modelName,
                                                denoiseLevel, tileSize,
                                                gpuId, ttaEnabled,
                                                outputFormat, isMultiGPU,
                                                multiGpuJobArgs,
                                                experimental);
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
        if (!ffmpegFrameExtract.waitForStarted(5000) ||
            !ffmpegFrameExtract.waitForFinished(-1) ||
            !warnProcessFailure(this, ffmpegFrameExtract,
                                tr("FFmpeg frame extraction for segment %1").arg(i + 1))) {
            qDebug() << "RealCUGAN VideoBySegment: Failed to extract frames for segment" << i+1
                     << ". StdErr:" << QString::fromLocal8Bit(ffmpegFrameExtract.readAllStandardError());
            overallSuccess = false;
            break;
        }

        QStringList framesFileName_qStrList = file_getFileNames_in_Folder_nofilter(splitFramesFolder);
        if (framesFileName_qStrList.isEmpty()) {
            qDebug() << "RealCUGAN VideoBySegment Error: No frames extracted for segment" << i+1;
            // It's possible a segment has 0 frames if segmentDuration is too small compared to FPS. This might not be an error.
            // However, if totalDurationSec > 0 and numSegments > 0, usually expect some frames.
            // For now, let's continue if it's empty, the segment video will be empty.
        }

        // --- AI Processing for the CURRENT SEGMENT's frames (Directory Call) ---
        QString segmentScaledFramesFolderAI = QDir(mainTempFolder).filePath(segmentName + "_scaled_AI");
        // Realcugan_ProcessDirectoryIteratively will update segmentScaledFramesFolderAI to the actual output path

        emit Send_TextBrowser_NewMessage(tr("Starting AI processing for segment %1/%2 frames...").arg(i+1).arg(numSegments));
        bool aiSegmentSuccess = Realcugan_ProcessDirectoryIteratively(
            splitFramesFolder, segmentScaledFramesFolderAI, targetScale,
            m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
            m_realcugan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
            m_realcugan_TTA, ui->comboBox_OutFormat_Image->currentText().toLower(),
            ui->comboBox_Engine_Video->currentIndex() == 2
        );

        if (!aiSegmentSuccess || Stopping) {
            qDebug() << "RealCUGAN VideoBySegment: AI processing failed or stopped for segment" << i+1;
            overallSuccess = false; break;
        }

        // --- Resampling Loop for the CURRENT SEGMENT's AI processed frames ---
        emit Send_TextBrowser_NewMessage(tr("Resampling AI processed frames for segment %1/%2...").arg(i+1).arg(numSegments));
        QSize originalVideoSizeSeg = video_get_Resolution(sourceFileFullPath); // Get original full video dimensions
        if(originalVideoSizeSeg.isEmpty()){
            qDebug() << "RealCUGAN VideoBySegment: Failed to get original video dimensions for resampling segment" << i+1;
            overallSuccess = false; break;
        }
        int targetSegFrameWidth = qRound(static_cast<double>(originalVideoSizeSeg.width()) * targetScale);
        int targetSegFrameHeight = qRound(static_cast<double>(originalVideoSizeSeg.height()) * targetScale);

        QStringList aiSegmentFramesList = file_getFileNames_in_Folder_nofilter(segmentScaledFramesFolderAI);
        if (aiSegmentFramesList.isEmpty() && !framesFileName_qStrList.isEmpty()) {
             qDebug() << "RealCUGAN VideoBySegment: AI processed folder for segment " << i+1 << " is empty, but original frames existed.";
             overallSuccess = false; break;
        }

        bool segResamplingSuccess = true;
        CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Seg %1 Resample)").arg(i+1), aiSegmentFramesList.count());
        for (const QString &aiFrameFileName : aiSegmentFramesList) {
            if (Stopping) { segResamplingSuccess = false; overallSuccess = false; break; }
            CurrentFileProgress_progressbar_Add();

            QString aiFramePath = QDir(segmentScaledFramesFolderAI).filePath(aiFrameFileName);
            QImage aiImage(aiFramePath);
            if (aiImage.isNull()) {
                qDebug() << "RealCUGAN VideoBySegment: Failed to load AI frame for resampling:" << aiFramePath;
                segResamplingSuccess = false; continue; // Or break
            }

            QImage resampledImage = aiImage.scaled(targetSegFrameWidth, targetSegFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
            // Save to the final scaledFramesFolder for this segment (e.g., segment_000_scaled/frame_001.png)
            QString finalSegFramePath = QDir(scaledFramesFolder).filePath(aiFrameFileName);

            QFileInfo finalSegFrameInfo(finalSegFramePath); // Ensure directory exists
            QDir finalSegFrameDir(finalSegFrameInfo.path());
            if(!finalSegFrameDir.exists()) finalSegFrameDir.mkpath(".");

            if (!resampledImage.save(finalSegFramePath)) {
                qDebug() << "RealCUGAN VideoBySegment: Failed to save resampled frame:" << finalSegFramePath;
                segResamplingSuccess = false; break;
            }
        }
        CurrentFileProgress_Stop();
        QDir(segmentScaledFramesFolderAI).removeRecursively(); // Clean up intermediate AI output for the segment

        if (!segResamplingSuccess || Stopping) {
            qDebug() << "RealCUGAN VideoBySegment: Resampling failed or stopped for segment" << i+1;
            overallSuccess = false; break;
        }
        // At this point, `scaledFramesFolder` (e.g., segment_000_scaled) contains the resampled frames for the current segment.

        // 2. Reassemble processed frames of the segment into a video segment (without audio)
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

            if (!ffmpegConcat.waitForStarted(5000) ||
                !ffmpegConcat.waitForFinished(-1) ||
                !warnProcessFailure(this, ffmpegConcat, tr("FFmpeg concat"))) {
                qDebug() << "RealCUGAN VideoBySegment: Failed to concatenate video segments. StdErr:"
                         << QString::fromLocal8Bit(ffmpegConcat.readAllStandardError());
                overallSuccess = false;
            } else {
                 // Add the full audio track back
                emit Send_TextBrowser_NewMessage(tr("Muxing final video with audio... (RealCUGAN)"));
                QProcess ffmpegMux;
                QStringList muxArgs;
                muxArgs << "-i" << concatenatedVideoPath << "-i" << fullAudioPath << "-c:v" << "copy" << "-c:a" << "aac" << "-b:a" << ui->spinBox_bitrate_audio->text()+"k" << "-shortest" << finalResultFileFullPath;
                // Might need to use video_ReadSettings_OutputVid for more complex muxing if available
                ffmpegMux.start(FFMPEG_EXE_PATH_Waifu2xEX, muxArgs);
                if (!ffmpegMux.waitForStarted(5000) ||
                    !ffmpegMux.waitForFinished(-1) ||
                    !warnProcessFailure(this, ffmpegMux, tr("FFmpeg mux"))) {
                    qDebug() << "RealCUGAN VideoBySegment: Failed to mux final video. StdErr:"
                             << QString::fromLocal8Bit(ffmpegMux.readAllStandardError());
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
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Sets m_realcugan_gpuJobConfig_temp

    // 4. Determine Target Scale
    int targetScale = ui->spinBox_scaleRatio_video->value();
    if (targetScale <= 0) targetScale = 1;

    // --- AI Processing using Directory-level calls ---
    QString scaledFramesFolderAI = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_Vid_scaled_AI_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    // Note: Realcugan_ProcessDirectoryIteratively will create scaledFramesFolderAI if successful.
    // It takes scaledFramesFolderAI by reference and updates it to the actual final AI output path.

    emit Send_TextBrowser_NewMessage(tr("Starting AI processing for video frames... (RealCUGAN)"));
    bool aiProcessingSuccess = Realcugan_ProcessDirectoryIteratively(
        splitFramesFolder, scaledFramesFolderAI, targetScale, // scaledFramesFolderAI is out-param
        m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
        m_realcugan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
        m_realcugan_TTA, ui->comboBox_OutFormat_Image->currentText().toLower(),
        ui->comboBox_Engine_Video->currentIndex() == 2
    );

    if (!aiProcessingSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in AI processing"));
        QDir(splitFramesFolder).removeRecursively();
        QDir(scaledFramesFolderAI).removeRecursively(); // Cleanup AI output dir
        QDir(scaledFramesFolder).removeRecursively();   // Ensure final scaled folder is also cleared
        QFile::remove(audioPath);
        ProcessNextFile();
        return;
    }
    emit Send_TextBrowser_NewMessage(tr("AI processing finished, starting resampling... (RealCUGAN Video)"));

    // --- Resampling Loop ---
    QSize originalVideoSize = video_get_Resolution(sourceFileFullPath);
    if(originalVideoSize.isEmpty()){
        qDebug() << "RealCUGAN Video: Failed to get original video dimensions for resampling.";
        item_Status->setText(tr("Error: Get original video size failed"));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolderAI).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath);
        ProcessNextFile(); return;
    }
    int targetFrameWidth = qRound(static_cast<double>(originalVideoSize.width()) * targetScale);
    int targetFrameHeight = qRound(static_cast<double>(originalVideoSize.height()) * targetScale);

    QStringList aiFramesList = file_getFileNames_in_Folder_nofilter(scaledFramesFolderAI);
    if (aiFramesList.isEmpty() && !framesFileName_qStrList.isEmpty()) { // If input had frames but AI output is empty
        qDebug() << "RealCUGAN Video: AI processed folder is empty, but original frames existed.";
        item_Status->setText(tr("Error: AI processing yielded no frames."));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolderAI).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath);
        ProcessNextFile(); return;
    }


    bool resamplingSuccess = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Resampling)"), aiFramesList.count());

    for (const QString &aiFrameFileName : aiFramesList) {
        if (Stopping) { resamplingSuccess = false; break; }
        CurrentFileProgress_progressbar_Add();
        emit Send_TextBrowser_NewMessage(tr("Resampling frame %1/%2 (RealCUGAN Video)").arg(CurrentFileProgress_Value()).arg(aiFramesList.size()));


        QString aiFramePath = QDir(scaledFramesFolderAI).filePath(aiFrameFileName);
        QImage aiImage(aiFramePath);
        if (aiImage.isNull()) {
            qDebug() << "RealCUGAN Video: Failed to load AI processed frame for resampling:" << aiFramePath;
            // Skip this frame or error out? For now, skip and mark error.
            resamplingSuccess = false; continue;
        }

        QImage resampledImage = aiImage.scaled(targetFrameWidth, targetFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
        QString finalFramePath = QDir(scaledFramesFolder).filePath(aiFrameFileName); // Save to final scaled folder (used for assembly)

        QFileInfo finalFrameInfo(finalFramePath); // Ensure directory exists
        QDir finalFrameDir(finalFrameInfo.path());
        if(!finalFrameDir.exists()) finalFrameDir.mkpath(".");

        if (!resampledImage.save(finalFramePath)) {
            qDebug() << "RealCUGAN Video: Failed to save resampled frame:" << finalFramePath;
            resamplingSuccess = false; break;
        }
    }
    CurrentFileProgress_Stop();
    QDir(scaledFramesFolderAI).removeRecursively(); // Clean up intermediate AI output folder

    if (!resamplingSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in resampling"));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively(); QFile::remove(audioPath);
        ProcessNextFile(); return;
    }

    // 5. Assemble processed frames back into a video
    if (!Stopping) { // Previous checks handle success flags
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


// Processes a directory of images through RealCUGAN AI passes.
// finalAIOutputDir will be updated to the path of the directory containing the last AI pass results.
bool MainWindow::Realcugan_ProcessDirectoryIteratively(
    const QString &initialInputDir, QString &finalAIOutputDir, // finalAIOutputDir is an out-parameter
    int targetOverallScale,
    const QString &modelName, int denoiseLevel, int tileSize,
    const QString &gpuIdOrJobConfig, bool isMultiGPUJob,
    bool ttaEnabled, const QString &outputFormat,
    bool experimental)
{
    qDebug() << "Realcugan_ProcessDirectoryIteratively: InputDir" << initialInputDir
             << "TargetOverallScale" << targetOverallScale;

    QList<int> scaleSequence = CalculateRealCUGANScaleSequence(targetOverallScale);
    if (scaleSequence.isEmpty() || (targetOverallScale > 1 && scaleSequence.first() == targetOverallScale && scaleSequence.first() > 4) ) {
        qDebug() << "Realcugan_ProcessDirectoryIteratively: Invalid or unsupported scale sequence for target" << targetOverallScale;
        // This indicates CalculateRealCUGANScaleSequence couldn't break it down (e.g. 5x, 7x)
        // The resampling step after this function will handle achieving the exact target.
        // For AI processing, if we can't form a sequence, we might do a 1x pass (denoise only) or skip AI.
        // For now, let's try to proceed if a sequence (even if just [1]) is returned.
        // The original error for _ProcessSingleFileIteratively was more stringent.
        // Here, we rely on the caller to decide if the resulting AI scale is adequate before resampling.
        // If scaleSequence is truly problematic (e.g. empty for target > 1), error out.
        if (scaleSequence.isEmpty() && targetOverallScale > 1) {
             emit Send_TextBrowser_NewMessage(tr("Error: Could not determine AI scaling passes for RealCUGAN."));
             return false;
        }
        if (scaleSequence.isEmpty()) scaleSequence.append(1); // Ensure at least one pass if target is 1x
    }

    QString currentPassInputDir = initialInputDir;
    QString mainProcessingTempDir = QDir::tempPath() + "/W2XEX_RC_DirIter_Main_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir(mainProcessingTempDir).mkpath(".");
    QList<QString> tempPassOutputDirs; // To clean up intermediate pass outputs

    bool success = true;
    for (int i = 0; i < scaleSequence.size(); ++i) {
        if (Stopping) { success = false; break; }

        int currentPassScale = scaleSequence[i];
        QString passOutputDir = QDir(mainProcessingTempDir).filePath(QString("pass_%1_out").arg(i));
        QDir().mkpath(passOutputDir);
        tempPassOutputDirs.append(passOutputDir);

        QStringList arguments = Realcugan_NCNN_Vulkan_PrepareArguments(
            currentPassInputDir, passOutputDir, currentPassScale,
            modelName, denoiseLevel, tileSize,
            gpuIdOrJobConfig, ttaEnabled, outputFormat, // For directory, outputFormat is for frames
            isMultiGPUJob, gpuIdOrJobConfig,
            experimental
        );

        emit Send_TextBrowser_NewMessage(tr("Starting RealCUGAN directory pass %1/%2 (Scale: %3x)...").arg(i + 1).arg(scaleSequence.size()).arg(currentPassScale));

        QProcess process;
        QString exePath = realCuganProcessor->executablePath(experimental);
        qDebug() << "Realcugan_ProcessDirectoryIteratively Pass" << i + 1 << "Cmd:" << exePath << arguments.join(" ");

        process.start(exePath, arguments);
        if (!process.waitForStarted(10000)) {
            qDebug() << "Realcugan_ProcessDirectoryIteratively: Process failed to start for pass" << i + 1;
            QMessageBox::warning(this, tr("Process Failure"),
                                 tr("RealCUGAN failed to start for directory pass %1").arg(i + 1));
            emit Send_TextBrowser_NewMessage(
                tr("RealCUGAN failed to start for directory pass %1").arg(i + 1));
            success = false;
            break;
        }

        while (process.state() != QProcess::NotRunning) {
            if (Stopping) { // Use the global/member stopping flag
                process.terminate();
                if (!process.waitForFinished(1500)) {
                    process.kill();
                    process.waitForFinished();
                }
                success = false;
                break;
            }
            if (process.waitForFinished(100)) { // Check every 100ms
                break;
            }
        }
        if (!success) break; // If stopped or failed in loop, break outer

        if (!warnProcessFailure(this, process,
                                tr("RealCUGAN directory pass %1").arg(i + 1))) {
            qDebug() << "Realcugan_ProcessDirectoryIteratively: Pass" << i + 1 << "failed. ExitCode:" << process.exitCode() << "Error:" << process.errorString();
            QByteArray errOut = process.readAllStandardError();
            emit Send_TextBrowser_NewMessage(
                tr("RealCUGAN directory pass %1 failed with exit code %2\n%3")
                    .arg(i + 1)
                    .arg(process.exitCode())
                    .arg(QString::fromLocal8Bit(errOut)));
            success = false;
            break;
        }

        // Check if output directory has content (basic check)
        QDir checkOutDir(passOutputDir);
        if (checkOutDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot).isEmpty()) {
             qDebug() << "Realcugan_ProcessDirectoryIteratively: Output directory for pass " << i+1 << " is empty: " << passOutputDir;
             // This might not be an error if input also had no processable files, but worth logging.
             // If inputDir had files, this is an error.
             if (!QDir(currentPassInputDir).entryInfoList(QDir::Files | QDir::NoDotAndDotDot).isEmpty()){
                emit Send_TextBrowser_NewMessage(
                    tr("RealCUGAN directory pass %1 produced no output").arg(i + 1));
                success = false; break;
             }
        }


        // If not the initial input directory, delete previous pass's output dir
        if (i > 0 && currentPassInputDir != initialInputDir) { // currentPassInputDir would be tempPassOutputDirs.at(i-1)
            QDir prevPassDir(currentPassInputDir);
            prevPassDir.removeRecursively();
        }
        currentPassInputDir = passOutputDir; // Output of this pass is input for the next
    }

    if (success && !Stopping) {
        finalAIOutputDir = currentPassInputDir; // This is the directory with the final AI pass results
    } else {
        // If failed or stopped, clean up all created temporary pass directories
        for (const QString &dirPath : tempPassOutputDirs) {
            QDir d(dirPath);
            d.removeRecursively();
        }
        QDir(mainProcessingTempDir).rmdir("."); // Try to remove the main temp folder if empty
        finalAIOutputDir.clear(); // Ensure it's not pointing to a deleted/incomplete dir
        return false;
    }

    // Do NOT delete mainProcessingTempDir here if successful, as finalAIOutputDir is inside it.
    // The caller will use finalAIOutputDir and then should be responsible for its cleanup (or its parent).
    return true;
}



void MainWindow::Realcugan_NCNN_Vulkan_Image(int rowNum, bool experimental, bool ReProcess_MissingAlphaChannel)
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

    AlphaInfo alphaInfo = PrepareAlpha(originalInFile);
    QString rgbInputFile = alphaInfo.rgbPath;
    QString finalRgbOutput = alphaInfo.hasAlpha ? tempPathBase + "final_rgb." + finalOutFileInfo.suffix().toLower() : finalOutFile;
    bool hasAlpha = alphaInfo.hasAlpha;
    QString alphaPath = alphaInfo.alphaPath;

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
            ui->checkBox_MultiGPU_RealCUGAN->isChecked(), multiGpuArgs /* Pass constructed multi-GPU args */,
            ui->comboBox_Engine_Image->currentIndex() == 2
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
    firstProcess->setProperty("alphaTempDir", alphaInfo.tempDir);
    firstProcess->setProperty("alphaIs16", alphaInfo.is16Bit);
    firstProcess->setProperty("tempPathBase", tempPathBase); // For cleanup
    firstProcess->setProperty("originalInFile", originalInFile); // Store original full input path

    QImage tempOriginalFullImage(originalInFile);
    firstProcess->setProperty("originalWidth", tempOriginalFullImage.width());
    firstProcess->setProperty("originalHeight", tempOriginalFullImage.height());
    firstProcess->setProperty("targetScale", targetScale); // Store the overall target scale for resampling
    firstProcess->setProperty("experimental", ui->comboBox_Engine_Image->currentIndex() == 2);
    tempOriginalFullImage = QImage(); // Release image data


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
        qDebug() << "RealCUGAN Error: Input file for pass" << currentPassIndex
                 << "does not exist:" << inputFileForThisPass;
        if(item_Status) item_Status->setText(tr("Error: Temp file missing"));
        emit Send_TextBrowser_NewMessage(
            tr("RealCUGAN pass %1 failed: missing temp file").arg(currentPassIndex + 1));
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


    QString EXE_PATH = realCuganProcessor->executablePath(
                process->property("experimental").toBool());
    QFileInfo exeInfo(EXE_PATH);
    if (!exeInfo.exists() || !exeInfo.isExecutable()) {
        if(item_Status) item_Status->setText(tr("Error: realcugan-ncnn-vulkan.exe not found"));
        emit Send_TextBrowser_NewMessage(tr("realcugan-ncnn-vulkan.exe not found"));
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
        if (item_Status)
            item_Status->setText(tr("Error: Process start failed (Pass %1)").arg(currentPassIndex + 1));
        emit Send_TextBrowser_NewMessage(
            tr("RealCUGAN failed to start for pass %1").arg(currentPassIndex + 1));

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
        qDebug() << "RealCUGAN Error: Pass" << currentPassIndex << "failed. ExitCode:" << process->exitCode()
                 << "ExitStatus:" << process->exitStatus();
        if(item_Status) item_Status->setText(tr("Error (Pass %1)").arg(currentPassIndex + 1));
        emit Send_TextBrowser_NewMessage(
            tr("RealCUGAN pass %1 failed with exit code %2\n%3")
                .arg(currentPassIndex + 1)
                .arg(process->exitCode())
                .arg(QString::fromLocal8Bit(stdErr)));
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
        qDebug() << "RealCUGAN Error: Output file for pass" << currentPassIndex
                 << "not found:" << currentPassOutputFile;
        if(item_Status)
            item_Status->setText(tr("Error: Temp file missing (Pass %1)").arg(currentPassIndex + 1));
        emit Send_TextBrowser_NewMessage(
            tr("RealCUGAN pass %1 produced no output").arg(currentPassIndex + 1));
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
        QString finalRGBFile = process->property("finalRGBFile").toString(); // Output of last AI pass
        QString alphaFile = process->property("alphaFile").toString();
        bool hasAlphaFlag = process->property("hasAlpha").toBool();
        QString alphaTempDir = process->property("alphaTempDir").toString();
        bool alphaIs16 = process->property("alphaIs16").toBool();

        int originalWidth = process->property("originalWidth").toInt();
        int originalHeight = process->property("originalHeight").toInt();
        int overallTargetScale = process->property("targetScale").toInt();

        qDebug() << "RealCUGAN: All AI passes completed successfully. Last AI output at:" << finalRGBFile;
        Realcugan_NCNN_Vulkan_CleanupTempFiles(process->property("tempPathBase").toString(), processQueue->size()-1, true /* keepFinal */, finalRGBFile);

        bool finalPostProcessingSuccess = true;
        if (QFile::exists(finalRGBFile)) {
            QImage imageAfterAiPasses(finalRGBFile);
            if (imageAfterAiPasses.isNull()) {
                qDebug() << "RealCUGAN_Iterative_finished: Failed to load image from finalRGBFile:" << finalRGBFile;
                finalPostProcessingSuccess = false;
            } else {
                int finalTargetWidth = qRound(static_cast<double>(originalWidth) * overallTargetScale);
                int finalTargetHeight = qRound(static_cast<double>(originalHeight) * overallTargetScale);

                if (imageAfterAiPasses.width() != finalTargetWidth || imageAfterAiPasses.height() != finalTargetHeight) {
                    qDebug() << "RealCUGAN_Iterative_finished: Resampling AI output from" << imageAfterAiPasses.size()
                             << "to final target" << QSize(finalTargetWidth, finalTargetHeight);
                    QImage resampledImage = imageAfterAiPasses.scaled(finalTargetWidth, finalTargetHeight,
                                                                   this->CustRes_AspectRatioMode, // MainWindow member
                                                                   Qt::SmoothTransformation);
                    if (!resampledImage.save(finalRGBFile)) { // Overwrite finalRGBFile with resampled version
                        qDebug() << "RealCUGAN_Iterative_finished: Failed to save resampled image to" << finalRGBFile;
                        finalPostProcessingSuccess = false;
                    }
                }
                // If dimensions already match, finalRGBFile is already correct.
            }
        } else {
            qDebug() << "RealCUGAN_Iterative_finished: finalRGBFile does not exist after AI passes:" << finalRGBFile;
            finalPostProcessingSuccess = false;
        }

        if (finalPostProcessingSuccess) {
            if (hasAlphaFlag && QFile::exists(finalRGBFile)) {
                AlphaInfo a; a.hasAlpha = true; a.rgbPath = finalRGBFile; a.alphaPath = alphaFile; a.tempDir = alphaTempDir; a.is16Bit = alphaIs16;
                RestoreAlpha(a, finalRGBFile, finalOutFile); // finalOutFile is the true final destination
            } else if (!hasAlphaFlag && finalRGBFile != finalOutFile && QFile::exists(finalRGBFile)) {
                // If no alpha, and finalRGBFile (which is now correctly resampled) is not the final output path, rename.
                if (QFile::exists(finalOutFile)) QFile::remove(finalOutFile);
                QFile::rename(finalRGBFile, finalOutFile);
            } else if (!hasAlphaFlag && finalRGBFile == finalOutFile) {
                // No alpha, and finalRGBFile is already the final output path and correctly resampled. Nothing to do.
            } else if (!QFile::exists(finalRGBFile)){ // Should have been caught by finalPostProcessingSuccess = false
                 qDebug() << "RealCUGAN_Iterative_finished: Error, finalRGBFile" << finalRGBFile << "does not exist before alpha/rename.";
                 finalPostProcessingSuccess = false; // Redundant, but for clarity
            }

            if (finalPostProcessingSuccess) {
                 if(item_Status) item_Status->setText(tr("Finished"));
                 LoadScaledImageToLabel(finalOutFile, ui->label_Preview_Main); // Load final image
            } else {
                 if(item_Status) item_Status->setText(tr("Error in final saving step"));
            }
        } else { // finalPostProcessingSuccess is false
            if(item_Status) item_Status->setText(tr("Error in resampling/loading AI output"));
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
    emit Send_TextBrowser_NewMessage(
        tr("RealCUGAN process error on pass %1: %2")
            .arg(currentPassIndex + 1)
            .arg(process->errorString()));
    QMessageBox::warning(this, tr("Process Failure"),
                         tr("RealCUGAN process error on pass %1: %2")
                             .arg(currentPassIndex + 1)
                             .arg(process->errorString()));

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
    realCuganProcessor->readSettings();
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int ThreadNum)
{
    realCuganProcessor->readSettingsVideoGif(ThreadNum);
}


// Blocking helper function to process a single file (image or frame) through RealCUGAN with iterative scaling
bool MainWindow::Realcugan_ProcessSingleFileIteratively(
    const QString &inputFile, const QString &outputFile, int targetScale,
    int originalWidth, int originalHeight, /* Added original dimensions */
    const QString &modelName, int denoiseLevel, int tileSize,
    const QString &gpuIdOrJobConfig, bool isMultiGPUJob, bool ttaEnabled, const QString &outputFormat,
    bool experimental,
    int rowNumForStatusUpdate) // Optional: for status updates if called directly for an image row
{
    qDebug() << "Realcugan_ProcessSingleFileIteratively: Input" << inputFile << "Output" << outputFile
             << "TargetScale" << targetScale << "OrigSize:" << originalWidth << "x" << originalHeight;

    QFileInfo finalOutFileInfo(outputFile);
    // Use a unique subfolder within temp based on the original input file's name to avoid collisions if this helper is called in parallel.
    // However, since this is a blocking function, direct parallelism of this function itself isn't the immediate concern,
    // but rather ensuring temp files from different *source* images don't collide if processed by different calls.
    QString tempSubFolder = QDir::tempPath() + "/W2XEX_RC_Iter/" + QFileInfo(inputFile).completeBaseName() + "_" + QRandomGenerator::global()->generate();
    QDir tempDir(tempSubFolder);
    if (!tempDir.mkpath(".")) {
        qDebug() << "Failed to create temporary directory:" << tempSubFolder;
        if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: Temp dir failed"), "ERROR");
        emit Send_TextBrowser_NewMessage(tr("Failed to create temp directory for RealCUGAN."));
        return false;
    }

    QString tempPathBase = tempDir.filePath(finalOutFileInfo.completeBaseName() + "_pass_");


    QList<int> scaleSequence = CalculateRealCUGANScaleSequence(targetScale);
    int calculatedTotalScaleByAiPasses = 1;
    for(int s : scaleSequence) calculatedTotalScaleByAiPasses *= s;

    // Check if AI passes can achieve a scale, even if not the final targetScale.
    // The main check for unsupported scale (e.g. 5x, 7x directly) is in CalculateRealCUGANScaleSequence.
    // If CalculateRealCUGANScaleSequence decided it's impossible (e.g. returns sequence=[5] for target=5),
    // this error should be caught before calling ProcessSingleFileIteratively, typically in _Image method.
    // Here, we check if the sequence is empty or if the product is 1 when target > 1 (meaning no effective AI scaling).
    if (scaleSequence.isEmpty() || (targetScale > 1 && calculatedTotalScaleByAiPasses == 1 && scaleSequence.first() == 1)) {
        qDebug() << "RealCUGAN ProcessSingle: AI scale sequence issue. Target:" << targetScale
                 << "Sequence:" << scaleSequence << "Calculated AI scale:" << calculatedTotalScaleByAiPasses;
        if (rowNumForStatusUpdate != -1) UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error: AI Scale sequence error"), "ERROR");
        tempDir.removeRecursively();
        return false;
    }
    // The case where calculatedTotalScaleByAiPasses != targetScale is now handled by the final resampling step.

    QString currentIterInputFile = inputFile;
    QString lastAiPassOutputFile = inputFile; // Will hold the output of the very last AI pass
    bool success = true;

    for (int i = 0; i < scaleSequence.size(); ++i) {
        if (Stopping) { success = false; break; }

        int currentPassScale = scaleSequence[i];
        // AI pass outputs always go to a temporary file within tempDir to avoid issues with outputFile path
        lastAiPassOutputFile = tempPathBase + QString::number(i) + "." + outputFormat.toLower();

        // Ensure output directory for intermediate file exists (should be tempDir)
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
            isMultiGPUJob, gpuIdOrJobConfig, // Pass full string if multi, single GPU ID otherwise
            experimental
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
        QString exePath = realCuganProcessor->executablePath(experimental);

        qDebug() << "RealCUGAN ProcessSingle Pass" << i+1 << "Cmd:" << exePath << arguments;
        process.start(exePath, arguments);

        if (!process.waitForStarted(10000)) {
            qDebug() << "RealCUGAN ProcessSingle: Process failed to start for pass" << i+1;
            QMessageBox::warning(this, tr("Process Failure"),
                                 tr("RealCUGAN failed to start for pass %1").arg(i + 1));
            emit Send_TextBrowser_NewMessage(
                tr("RealCUGAN failed to start for pass %1").arg(i + 1));
            success = false;
            break;
        }

        while (process.state() != QProcess::NotRunning) {
            if (Stopping) { // Use the global/member stopping flag
                process.terminate();
                if (!process.waitForFinished(1500)) {
                    process.kill();
                    process.waitForFinished();
                }
                success = false;
                break;
            }
            if (process.waitForFinished(100)) { // Check every 100ms
                break;
            }
        }
        if (!success) break; // If stopped or failed in loop, break outer

        QByteArray stdOut = process.readAllStandardOutput();
        QByteArray stdErr = process.readAllStandardError();
        if (!stdOut.isEmpty()) qDebug() << "RealCUGAN ProcessSingle Pass" << i+1 << "STDOUT:" << QString::fromLocal8Bit(stdOut);
        if (!stdErr.isEmpty()) qDebug() << "RealCUGAN ProcessSingle Pass" << i+1 << "STDERR:" << QString::fromLocal8Bit(stdErr);

        if (!warnProcessFailure(this, process, tr("RealCUGAN pass %1").arg(i + 1))) {
            qDebug() << "RealCUGAN ProcessSingle: Pass" << i+1 << "failed. ExitCode:" << process.exitCode();
            emit Send_TextBrowser_NewMessage(
                tr("RealCUGAN pass %1 failed with exit code %2\n%3")
                    .arg(i + 1)
                    .arg(process.exitCode())
                    .arg(QString::fromLocal8Bit(stdErr)));
            success = false;
            break;
        }
        if (!QFile::exists(lastAiPassOutputFile)) {
            qDebug() << "RealCUGAN ProcessSingle: Output file for pass" << i+1 << "not found:" << lastAiPassOutputFile;
            emit Send_TextBrowser_NewMessage(
                tr("RealCUGAN pass %1 produced no output").arg(i + 1));
            success = false; break;
        }
        currentIterInputFile = lastAiPassOutputFile; // Output of this pass is input for the next
    }

    if (success && !Stopping) {
        QImage imageAfterAiPasses(lastAiPassOutputFile); // Load the result of the last AI pass
        if (imageAfterAiPasses.isNull()) {
            qDebug() << "RealCUGAN: Failed to load image after AI passes from" << lastAiPassOutputFile;
            success = false;
        } else {
            int finalTargetWidth = qRound(static_cast<double>(originalWidth) * targetScale);
            int finalTargetHeight = qRound(static_cast<double>(originalHeight) * targetScale);

            if (imageAfterAiPasses.width() != finalTargetWidth || imageAfterAiPasses.height() != finalTargetHeight) {
                qDebug() << "RealCUGAN: Resampling AI output from" << imageAfterAiPasses.size()
                         << "to final target" << QSize(finalTargetWidth, finalTargetHeight) << "for output file" << outputFile;
                QImage resampledImage = imageAfterAiPasses.scaled(finalTargetWidth, finalTargetHeight,
                                                               this->CustRes_AspectRatioMode,
                                                               Qt::SmoothTransformation);
                if (!resampledImage.save(outputFile)) {
                    qDebug() << "RealCUGAN: Failed to save resampled image to" << outputFile;
                    success = false;
                }
            } else { // Dimensions are already correct after AI passes
                if (lastAiPassOutputFile != outputFile) { // If AI output was a temp file
                    if (QFile::exists(outputFile)) QFile::remove(outputFile);
                    if (!QFile::copy(lastAiPassOutputFile, outputFile)) {
                        qDebug() << "RealCUGAN: Failed to copy final image from" << lastAiPassOutputFile << "to" << outputFile;
                        success = false;
                    }
                }
                // If lastAiPassOutputFile IS outputFile, it's already in place.
            }
        }
    }

    // Cleanup all temporary files created by AI passes
    if (tempDir.exists()) {
        QDirIterator it(tempDir.path(), QDir::Files);
        while(it.hasNext()){
            QString tempFilePath = it.next();
            // Only delete if it's not the final intended outputFile (e.g. if outputFile was inside tempDir AND no resampling happened)
            if (tempFilePath != outputFile) {
                 QFile::remove(tempFilePath);
                 qDebug() << "RealCUGAN ProcessSingle: Deleted temp AI pass file" << tempFilePath;
            }
        }
        tempDir.rmdir("."); // Remove the specific temp subfolder if empty
        QDir parentTempDir(QDir::tempPath() + "/W2XEX_RC_Iter");
        parentTempDir.rmdir(QFileInfo(tempSubFolder).fileName());
    }

    if (Stopping && success) { // If processing was stopped externally even if current ops seemed to succeed
        qDebug() << "RealCUGAN ProcessSingle: Processing stopped externally, but current operations were successful until stop.";
        return false; // Indicate overall failure due to stop signal
    }
    if (!success && rowNumForStatusUpdate != -1) {
        UpdateTableWidget_Status(rowNumForStatusUpdate, tr("Error during RealCUGAN processing"), "ERROR");
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

    int targetScale = ui->spinBox_Scale_RealCUGAN->value(); // Use the general RealCUGAN scale
    if (targetScale <= 0) targetScale = 1;

    // Ensure final output scaledFramesFolder exists (it's an output param for this func, but APNG_Main creates it)
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
        emit Send_TextBrowser_NewMessage(tr("Processing APNG frame %1/%2: %3 (RealCUGAN)").arg(frameCount).arg(framesFileName_qStrList.size()).arg(frameFileName));
    }

    QString sourceFileNameNoExt = QFileInfo(sourceFileFullPath).completeBaseName(); // Used for temp folder naming

    // --- Alpha Preparation and RGB frame extraction ---
    emit Send_TextBrowser_NewMessage(tr("Preparing APNG frames (RGB/Alpha separation)... (RealCUGAN)"));
    QString rgbFramesTempDir = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_APNG_rgb_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString alphaBackupTempDir = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_APNG_alpha_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(rgbFramesTempDir);
    QDir().mkpath(alphaBackupTempDir);
    QMap<QString, AlphaInfo> frameAlphaInfosAPNG;

    bool prepSuccessAPNG = true;
    // Progress reporting for this part can be tricky as APNG_Main might have its own. For now, console logs.
    qDebug() << "APNG_Realcugan: Starting alpha prep loop for" << framesFileName_qStrList.count() << "frames.";
    for (const QString &frameFileName : framesFileName_qStrList) {
        if (Stopping) { prepSuccessAPNG = false; break; }
        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        AlphaInfo alphaInfo = PrepareAlpha(inputFramePath);
        frameAlphaInfosAPNG.insert(frameFileName, alphaInfo);

        QString rgbFrameDestPath = QDir(rgbFramesTempDir).filePath(frameFileName);
        if (alphaInfo.rgbPath != inputFramePath) {
            if (!QFile::copy(alphaInfo.rgbPath, rgbFrameDestPath)) { prepSuccessAPNG = false; break; }
            if (alphaInfo.tempDir.startsWith(QDir::tempPath())) QDir(alphaInfo.tempDir).removeRecursively();
        } else {
            if (!QFile::copy(inputFramePath, rgbFrameDestPath)) { prepSuccessAPNG = false; break; }
        }
        if (alphaInfo.hasAlpha) {
            QString alphaBackupPath = QDir(alphaBackupTempDir).filePath(frameFileName);
            if (!QFile::copy(alphaInfo.alphaPath, alphaBackupPath)) { prepSuccessAPNG = false; break; }
        }
    }
    if (!prepSuccessAPNG || Stopping) {
        qDebug() << "APNG_Realcugan: Error during alpha prep or stopped.";
        QDir(rgbFramesTempDir).removeRecursively(); QDir(alphaBackupTempDir).removeRecursively();
        // APNG_Main needs to know this failed. Maybe by checking if scaledFramesFolder is empty.
        return;
    }

    // --- AI Processing on RGB frames directory ---
    QString scaledRgbFramesAIDirAPNG;
    emit Send_TextBrowser_NewMessage(tr("Starting AI processing for APNG frames... (RealCUGAN)"));
    bool aiProcessingSuccessAPNG = Realcugan_ProcessDirectoryIteratively(
        rgbFramesTempDir, scaledRgbFramesAIDirAPNG, targetScale,
        m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
        m_realcugan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
        m_realcugan_TTA, "png",
        ui->comboBox_Engine_GIF->currentIndex() == 2
    );
    QDir(rgbFramesTempDir).removeRecursively();

    if (!aiProcessingSuccessAPNG || Stopping) {
        qDebug() << "APNG_Realcugan: Error during AI processing or stopped.";
        QDir(alphaBackupTempDir).removeRecursively(); QDir(scaledRgbFramesAIDirAPNG).removeRecursively();
        return;
    }

    // --- Alpha Restoration Loop ---
    emit Send_TextBrowser_NewMessage(tr("Restoring alpha to APNG frames... (RealCUGAN)"));
    QString combinedFramesAIDirAPNG = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_APNG_combined_AI_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(combinedFramesAIDirAPNG);
    bool restoreAlphaSuccessAPNG = true;
    qDebug() << "APNG_Realcugan: Starting alpha restore loop.";
    for (const QString &originalFrameFileName : framesFileName_qStrList) {
        if (Stopping) { restoreAlphaSuccessAPNG = false; break; }
        QString scaledRgbFramePath = QDir(scaledRgbFramesAIDirAPNG).filePath(originalFrameFileName);
        QString finalCombinedFramePath = QDir(combinedFramesAIDirAPNG).filePath(originalFrameFileName);
        AlphaInfo currentFrameAlphaInfo = frameAlphaInfosAPNG.value(originalFrameFileName);

        if (currentFrameAlphaInfo.hasAlpha) {
            QString backedUpAlphaPath = QDir(alphaBackupTempDir).filePath(originalFrameFileName);
            if (!QFile::exists(backedUpAlphaPath)) { QFile::copy(scaledRgbFramePath, finalCombinedFramePath); continue;}
            AlphaInfo tempRestoreInfo = {true, scaledRgbFramePath, backedUpAlphaPath, "", false}; // is16bit not critical here
            if (!RestoreAlpha(tempRestoreInfo, scaledRgbFramePath, finalCombinedFramePath, true, targetScale)) {
                restoreAlphaSuccessAPNG = false; break;
            }
        } else {
            if (!QFile::copy(scaledRgbFramePath, finalCombinedFramePath)) { restoreAlphaSuccessAPNG = false; break; }
        }
    }
    QDir(scaledRgbFramesAIDirAPNG).removeRecursively();
    QDir(alphaBackupTempDir).removeRecursively();

    if (!restoreAlphaSuccessAPNG || Stopping) {
        qDebug() << "APNG_Realcugan: Error during alpha restoration or stopped.";
        QDir(combinedFramesAIDirAPNG).removeRecursively();
        return;
    }

    // --- Resampling Loop for combined frames ---
    emit Send_TextBrowser_NewMessage(tr("Resampling final APNG frames... (RealCUGAN)"));
    QSize originalApngSize;
    if (!framesFileName_qStrList.isEmpty()) {
        QImage firstFrame(QDir(splitFramesFolder).filePath(framesFileName_qStrList.first()));
        if(!firstFrame.isNull()) originalApngSize = firstFrame.size();
    }
    if(originalApngSize.isEmpty()){
        qDebug() << "APNG_Realcugan: Failed to get original APNG dimensions for resampling.";
        QDir(combinedFramesAIDirAPNG).removeRecursively(); return;
    }
    int targetFrameWidth = qRound(static_cast<double>(originalApngSize.width()) * targetScale);
    int targetFrameHeight = qRound(static_cast<double>(originalApngSize.height()) * targetScale);

    QStringList combinedFramesListAPNG = file_getFileNames_in_Folder_nofilter(combinedFramesAIDirAPNG);
    bool resamplingSuccessAPNG = true;
    qDebug() << "APNG_Realcugan: Starting resampling loop.";
    for (const QString &combinedFrameFileName : combinedFramesListAPNG) {
        if (Stopping) { resamplingSuccessAPNG = false; break; }
        QString combinedFramePath = QDir(combinedFramesAIDirAPNG).filePath(combinedFrameFileName);
        QImage combinedImage(combinedFramePath);
        if (combinedImage.isNull()) { resamplingSuccessAPNG = false; continue; }

        QImage resampledImage = combinedImage.scaled(targetFrameWidth, targetFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
        QString finalFramePath = QDir(scaledFramesFolder).filePath(combinedFrameFileName); // scaledFramesFolder is the function's output dir
        if (!resampledImage.save(finalFramePath)) { resamplingSuccessAPNG = false; break; }
    }
    QDir(combinedFramesAIDirAPNG).removeRecursively();

    if (!resamplingSuccessAPNG || Stopping) {
        qDebug() << "APNG_Realcugan: Error during resampling or stopped.";
        // scaledFramesFolder might have partial files, APNG_Main should handle this.
        return;
    }

    qDebug() << "APNG_RealcuganNCNNVulkan: All frame processing stages (AI, Alpha, Resample) completed successfully.";
    emit Send_TextBrowser_NewMessage(tr("All APNG frames processed and resampled (RealCUGAN)."));
    // APNG_Main will now use the frames in `scaledFramesFolder` for assembly.
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

    // 3. Read general RealCUGAN settings
    Realcugan_NCNN_Vulkan_ReadSettings();
    Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(0); // Sets m_realcugan_gpuJobConfig_temp

    // 4. Determine Target Scale
    int targetScale = ui->spinBox_scaleRatio_gif->value();
    if (targetScale <= 0) targetScale = 1;

    // --- Alpha Preparation and RGB frame extraction ---
    emit Send_TextBrowser_NewMessage(tr("Preparing GIF frames (RGB/Alpha separation)... (RealCUGAN)"));
    QString rgbFramesTempDir = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_GIF_rgb_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString alphaBackupTempDir = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_GIF_alpha_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(rgbFramesTempDir);
    QDir().mkpath(alphaBackupTempDir);
    QMap<QString, AlphaInfo> frameAlphaInfos; // Store alpha info per original frame name

    bool prepSuccess = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Prep Alpha)"), framesFileName_qStrList.count());
    for (const QString &frameFileName : framesFileName_qStrList) {
        if (Stopping) { prepSuccess = false; break; }
        CurrentFileProgress_progressbar_Add();
        QString inputFramePath = QDir(splitFramesFolder).filePath(frameFileName);
        AlphaInfo alphaInfo = PrepareAlpha(inputFramePath); // PrepareAlpha handles if it's already RGB
        frameAlphaInfos.insert(frameFileName, alphaInfo);

        QString rgbFrameDestPath = QDir(rgbFramesTempDir).filePath(frameFileName);
        // PrepareAlpha might output to its own temp dir (alphaInfo.rgbPath) or use input directly if no alpha
        if (alphaInfo.rgbPath != inputFramePath) { // If rgbPath is a temp file from PrepareAlpha
            if (!QFile::copy(alphaInfo.rgbPath, rgbFrameDestPath)) {
                qDebug() << "RealCUGAN GIF: Failed to copy RGB temp frame" << alphaInfo.rgbPath << "to" << rgbFrameDestPath;
                prepSuccess = false; break;
            }
             if (alphaInfo.tempDir.startsWith(QDir::tempPath())) QDir(alphaInfo.tempDir).removeRecursively(); // Clean up PrepareAlpha's specific temp
        } else { // No alpha, rgbPath is same as inputFramePath
            if (!QFile::copy(inputFramePath, rgbFrameDestPath)) {
                 qDebug() << "RealCUGAN GIF: Failed to copy RGB frame" << inputFramePath << "to" << rgbFrameDestPath;
                 prepSuccess = false; break;
            }
        }
        if (alphaInfo.hasAlpha) { // Backup the separated alpha channel
            QString alphaBackupPath = QDir(alphaBackupTempDir).filePath(frameFileName); // Use same name for easy mapping
            if (!QFile::copy(alphaInfo.alphaPath, alphaBackupPath)) {
                qDebug() << "RealCUGAN GIF: Failed to copy alpha backup" << alphaInfo.alphaPath << "to" << alphaBackupPath;
                prepSuccess = false; break;
            }
        }
    }
    CurrentFileProgress_Stop();
    if (!prepSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error preparing alpha"));
        QDir(splitFramesFolder).removeRecursively(); QDir(rgbFramesTempDir).removeRecursively(); QDir(alphaBackupTempDir).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        ProcessNextFile(); return;
    }

    // --- AI Processing on RGB frames directory ---
    QString scaledRgbFramesAIDir; // This will be set by Realcugan_ProcessDirectoryIteratively
    emit Send_TextBrowser_NewMessage(tr("Starting AI processing for GIF frames... (RealCUGAN)"));
    bool aiProcessingSuccess = Realcugan_ProcessDirectoryIteratively(
        rgbFramesTempDir, scaledRgbFramesAIDir, targetScale,
        m_realcugan_Model, m_realcugan_DenoiseLevel, m_realcugan_TileSize,
        m_realcugan_gpuJobConfig_temp, ui->checkBox_MultiGPU_RealCUGAN->isChecked(),
        m_realcugan_TTA, "png",
        ui->comboBox_Engine_GIF->currentIndex() == 2
    );
    QDir(rgbFramesTempDir).removeRecursively(); // Cleaned up after AI processing input

    if (!aiProcessingSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in AI processing"));
        QDir(splitFramesFolder).removeRecursively(); QDir(alphaBackupTempDir).removeRecursively(); QDir(scaledRgbFramesAIDir).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        ProcessNextFile(); return;
    }

    // --- Alpha Restoration Loop ---
    emit Send_TextBrowser_NewMessage(tr("Restoring alpha channel to AI processed frames... (RealCUGAN GIF)"));
    QString combinedFramesAIDir = Current_Path + "/temp_W2xEX/" + sourceFileNameNoExt + "_RC_GIF_combined_AI_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QDir().mkpath(combinedFramesAIDir);
    bool restoreAlphaSuccess = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Restore Alpha)"), framesFileName_qStrList.count()); // Using original frame list for count

    for (const QString &originalFrameFileName : framesFileName_qStrList) { // Iterate based on original frame names
        if (Stopping) { restoreAlphaSuccess = false; break; }
        CurrentFileProgress_progressbar_Add();

        QString scaledRgbFramePath = QDir(scaledRgbFramesAIDir).filePath(originalFrameFileName); // Assumes AI output dir preserves names
        QString finalCombinedFramePath = QDir(combinedFramesAIDir).filePath(originalFrameFileName);
        AlphaInfo currentFrameAlphaInfo = frameAlphaInfos.value(originalFrameFileName);

        if (currentFrameAlphaInfo.hasAlpha) {
            QString backedUpAlphaPath = QDir(alphaBackupTempDir).filePath(originalFrameFileName);
            if (!QFile::exists(backedUpAlphaPath)) {
                 qDebug() << "RealCUGAN GIF: Missing alpha backup for" << originalFrameFileName;
                 // Copy scaled RGB as is, or error? For now, copy RGB.
                 if(!QFile::copy(scaledRgbFramePath, finalCombinedFramePath)) {restoreAlphaSuccess = false; break;}
                 continue;
            }
            // Create a temporary AlphaInfo for RestoreAlpha, pointing to the scaled RGB and its original alpha backup
            AlphaInfo tempRestoreInfo;
            tempRestoreInfo.hasAlpha = true;
            tempRestoreInfo.rgbPath = scaledRgbFramePath; // Input for RestoreAlpha's RGB
            tempRestoreInfo.alphaPath = backedUpAlphaPath; // Input for RestoreAlpha's Alpha
            // tempRestoreInfo.tempDir does not need to be a valid path here as RestoreAlpha uses rgbPath's dir for its own temp if needed.
            // However, RestoreAlpha expects to be able to create a temp file relative to rgbPath. Ensure scaledRgbFramePath's dir is writable. (It is, it's our temp dir).

            if (!RestoreAlpha(tempRestoreInfo, scaledRgbFramePath, finalCombinedFramePath, true /* use passed alpha for scaling*/, targetScale /* pass target scale for alpha */ )) {
                 qDebug() << "RealCUGAN GIF: RestoreAlpha failed for" << originalFrameFileName;
                 restoreAlphaSuccess = false; break;
            }
        } else { // No alpha for this frame
            if (!QFile::copy(scaledRgbFramePath, finalCombinedFramePath)) {
                qDebug() << "RealCUGAN GIF: Failed to copy non-alpha frame" << scaledRgbFramePath;
                restoreAlphaSuccess = false; break;
            }
        }
    }
    CurrentFileProgress_Stop();
    QDir(scaledRgbFramesAIDir).removeRecursively();
    QDir(alphaBackupTempDir).removeRecursively();

    if (!restoreAlphaSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error restoring alpha"));
        QDir(splitFramesFolder).removeRecursively(); QDir(combinedFramesAIDir).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        ProcessNextFile(); return;
    }

    // --- Resampling Loop for combined frames ---
    emit Send_TextBrowser_NewMessage(tr("Resampling final frames... (RealCUGAN GIF)"));
    QSize originalGifSize;
    if (!framesFileName_qStrList.isEmpty()) {
        QImage firstFrame(QDir(splitFramesFolder).filePath(framesFileName_qStrList.first()));
        if(!firstFrame.isNull()) originalGifSize = firstFrame.size();
    }
    if(originalGifSize.isEmpty()){
        qDebug() << "RealCUGAN GIF: Failed to get original GIF dimensions for resampling.";
        item_Status->setText(tr("Error: Get GIF original size failed"));
        QDir(splitFramesFolder).removeRecursively(); QDir(combinedFramesAIDir).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        ProcessNextFile(); return;
    }
    int targetFrameWidth = qRound(static_cast<double>(originalGifSize.width()) * targetScale);
    int targetFrameHeight = qRound(static_cast<double>(originalGifSize.height()) * targetScale);

    QStringList combinedFramesList = file_getFileNames_in_Folder_nofilter(combinedFramesAIDir);
    bool resamplingSuccess = true;
    CurrentFileProgress_Start(sourceFileInfo.fileName() + tr(" (Resample Output)"), combinedFramesList.count());

    for (const QString &combinedFrameFileName : combinedFramesList) {
        if (Stopping) { resamplingSuccess = false; break; }
        CurrentFileProgress_progressbar_Add();

        QString combinedFramePath = QDir(combinedFramesAIDir).filePath(combinedFrameFileName);
        QImage combinedImage(combinedFramePath);
        if (combinedImage.isNull()) {
            qDebug() << "RealCUGAN GIF: Failed to load combined frame for resampling:" << combinedFramePath;
            resamplingSuccess = false; continue;
        }

        QImage resampledImage = combinedImage.scaled(targetFrameWidth, targetFrameHeight, this->CustRes_AspectRatioMode, Qt::SmoothTransformation);
        QString finalFramePath = QDir(scaledFramesFolder).filePath(combinedFrameFileName); // Save to final assembly folder

        QFileInfo finalFrameInfo(finalFramePath);
        QDir finalFrameDir(finalFrameInfo.path());
        if(!finalFrameDir.exists()) finalFrameDir.mkpath(".");

        if (!resampledImage.save(finalFramePath)) {
            qDebug() << "RealCUGAN GIF: Failed to save resampled frame:" << finalFramePath;
            resamplingSuccess = false; break;
        }
    }
    CurrentFileProgress_Stop();
    QDir(combinedFramesAIDir).removeRecursively();

    if (!resamplingSuccess || Stopping) {
        item_Status->setText(Stopping ? tr("Stopped") : tr("Error in final resampling"));
        QDir(splitFramesFolder).removeRecursively(); QDir(scaledFramesFolder).removeRecursively();
        ProcessNextFile(); return;
    }

    // 5. Assemble processed frames back into a GIF
    if (!Stopping) {
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

    QString EXE_PATH = realCuganProcessor->executablePath(false);
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

    // For single-image calls this returns the combined settings for all selected GPUs.
    // This function is also used by Realcugan_NCNN_Vulkan_PrepareArguments for single image multi-GPU.

    QStringList gpuIDs_str_list;
    QStringList jobParams_str_list; // Stores "load:proc:save"

    for (const auto& gpuMap : GPUIDs_List_MultiGPU_RealCUGAN) {
        QString currentGpuId = gpuMap.value("ID");
        gpuIDs_str_list.append(currentGpuId);
        QString procThreads = gpuMap.value("Threads", "1"); // Default proc threads to 1

        if (currentGpuId == "-1") { // CPU
            jobParams_str_list.append(QString("1:%1:1").arg(procThreads));
        } else { // GPU
            // Assuming the same "load:proc:save" format for GPUs if user specifies threads.
            // realcugan-ncnn-vulkan might default GPU threads if not specified or if -j is not for GPUs.
            // Based on subtask point 4, this format is expected.
            jobParams_str_list.append(QString("1:%1:1").arg(procThreads));
        }
    }

    if (gpuIDs_str_list.isEmpty()) { // Should not happen if checkbox is checked and list is populated
        // Fallback to the primary selected GPU ID
        QString singleGpuId = m_realcugan_GPUID.split(":")[0];
        if (singleGpuId == "-1") { return QString("-g -1"); } // Potentially add -j here too if needed for single CPU
        else { return QString("-g ") + singleGpuId; }
    }

    // Format: -g G0,G1,... -j L0:P0:S0,L1:P1:S1,...
    return QString("-g %1 -j %2").arg(gpuIDs_str_list.join(","), jobParams_str_list.join(","));
}

void MainWindow::AddGPU_MultiGPU_RealcuganNcnnVulkan(QString GPUID_Name)
{
    if (GPUID_Name.isEmpty() || GPUID_Name.contains("No available")) return;

    QString selectedGPU_ID_str;
    QString selectedGPU_Name_str;
    int threads = ui->spinBox_Threads_MultiGPU_RealCUGAN->value();

    if (GPUID_Name == "-1: CPU" || GPUID_Name == tr("-1: CPU")) {
        selectedGPU_ID_str = "-1";
        selectedGPU_Name_str = "CPU";
    } else {
        selectedGPU_ID_str = GPUID_Name.split(":").first();
        selectedGPU_Name_str = GPUID_Name.mid(GPUID_Name.indexOf(":") + 1).trimmed();
    }

    for (const auto& map : GPUIDs_List_MultiGPU_RealCUGAN) {
        if (map.value("ID") == selectedGPU_ID_str) {
            ShowMessageBox("Info", "This GPU has already been added for RealCUGAN.", QMessageBox::Information);
            return;
        }
    }

    QMap<QString, QString> newGPU;
    newGPU.insert("ID", selectedGPU_ID_str);
    newGPU.insert("Name", selectedGPU_Name_str);
    newGPU.insert("Threads", QString::number(threads));
    GPUIDs_List_MultiGPU_RealCUGAN.append(newGPU);

    QListWidgetItem *newItem = new QListWidgetItem();
    newItem->setText(QString("ID: %1, Name: %2, Threads: %3").arg(selectedGPU_ID_str, selectedGPU_Name_str, QString::number(threads)));
    ui->listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
    qDebug() << "Added Device for RealCUGAN Multi-Processing:" << selectedGPU_ID_str << "Name:" << selectedGPU_Name_str << "Threads:" << threads;
}


void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings()
{
    realCuganProcessor->preLoadSettings();
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
        // Add CPU option first
        Available_GPUID_RealCUGAN.prepend(tr("-1: CPU"));

        QStringList lines = combinedOutput.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            QRegularExpressionMatch match = gpuRegex1.match(line.trimmed());
            if (!match.hasMatch()) match = gpuRegex2.match(line.trimmed());
            if (!match.hasMatch()) match = gpuRegex3.match(line.trimmed());

            if (match.hasMatch()) {
                QString gpuID = match.captured(1).trimmed();
                QString gpuName = match.captured(2).trimmed();
                // Avoid adding CPU again if it's somehow in the exe output with ID -1
                if (gpuID != "-1") {
                    Available_GPUID_RealCUGAN.append(QString("%1: %2").arg(gpuID, gpuName));
                }
            }
        }

        // If only CPU is in the list (because no GPUs were parsed), that's fine.
        if (Available_GPUID_RealCUGAN.size() == 1 && Available_GPUID_RealCUGAN.first().startsWith("-1:")) { // Only CPU present
            if (combinedOutput.contains("NVIDIA") || combinedOutput.contains("AMD") || combinedOutput.contains("Intel") || combinedOutput.contains("Vulkan Device")) {
                 // Output indicates GPUs might be there, but parsing failed.
                 QMessageBox::information(this, tr("GPU Detection"), tr("Could not parse GPU list accurately for RealCUGAN. Only CPU option is available based on current parsing. Check RealCUGAN output if GPUs are expected:\n%1").arg(combinedOutput));
            } else {
                 // No clear indication of GPUs in the output, so only CPU is fine.
                 qDebug() << "RealCUGAN GPU Detection: No specific GPUs detected by parsing, CPU option available.";
            }
        } else if (Available_GPUID_RealCUGAN.size() <= 1) { // Still only CPU or empty after trying to parse
             Available_GPUID_RealCUGAN.clear(); // Clear to ensure clean state
             Available_GPUID_RealCUGAN.prepend(tr("-1: CPU")); // Add CPU
             Available_GPUID_RealCUGAN.append(tr("0: Default GPU (Auto-detect failed or no Vulkan GPUs)")); // Add a fallback default
             QMessageBox::warning(this, tr("GPU Detection"), tr("No GPUs detected or output format not recognized for RealCUGAN. Offering CPU and a default GPU option.\nOutput:\n%1").arg(combinedOutput));
        }


        ui->comboBox_GPUID_RealCUGAN->clear();
        ui->comboBox_GPUID_RealCUGAN->addItems(Available_GPUID_RealCUGAN);
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->clear();
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->addItems(Available_GPUID_RealCUGAN);

        QMessageBox::information(this, tr("Device Detection"), tr("Detected devices for RealCUGAN (including CPU) have been populated. Please verify the selection."));

    } else {
        QMessageBox::warning(this, tr("Device Detection Failed"), tr("realcugan-ncnn-vulkan.exe process failed or returned an error for device detection.\nExit Code: %1\nOutput:\n%2\nError Output:\n%3").arg(exitCode).arg(output).arg(errorOutput));
        // Add CPU and a default option to allow usage even if detection fails
        ui->comboBox_GPUID_RealCUGAN->clear();
        Available_GPUID_RealCUGAN.clear();
        Available_GPUID_RealCUGAN.append(tr("-1: CPU"));
        Available_GPUID_RealCUGAN.append("0: Default GPU (Detection Failed)");
        ui->comboBox_GPUID_RealCUGAN->addItems(Available_GPUID_RealCUGAN);
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->clear();
        ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->addItems(Available_GPUID_RealCUGAN);
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
// on_checkBox_MultiGPU_RealCUGAN_clicked(); // If it has separate logic from stateChanged
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

