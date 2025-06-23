/*
    Copyright (C) 2025  beyawnko
    This file is part of Waifu2x-Extension-GUI.
    (Full license header as before)
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RealCuganProcessor.h"
#include "realcugan_settings.h"
#include <QDebug>

/*******************************************************
*      RealCUGAN Integration (Refactored Wrappers)
*******************************************************/

/**
 * @brief Wrapper for Image Processing.
 * Gathers settings from the UI and passes the job to the RealCuganProcessor.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Image(int rowNum, bool, bool)
{
    qDebug() << "Dispatching RealCUGAN Image job for row" << rowNum;

    QString sourceFile = Table_model_image->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realcugan");

    RealCuganSettings settings;
    settings.programPath = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealCUGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_image->value();
    settings.denoiseLevel = ui->spinBox_DenoiseLevel_image->value();
    settings.tileSize = ui->spinBox_TileSize_RealCUGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealCUGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealCUGAN->currentText();

    // Pass the job to the processor
    if (realCuganProcessor) {
        realCuganProcessor->processImage(rowNum, sourceFile, destFile, settings);
    } else {
        qWarning() << "RealCuganProcessor is null! Cannot process image.";
        onProcessingFinished(rowNum, false);
    }
}

/**
 * @brief Wrapper for Video Processing.
 * Gathers settings from the UI and passes the job to the RealCuganProcessor.
 */
void MainWindow::Realcugan_NCNN_Vulkan_Video(int rowNum)
{
    qDebug() << "Dispatching RealCUGAN Video job for row" << rowNum;

    QString sourceFile = Table_model_video->item(rowNum, 2)->text();
    QString destFile = Generate_Output_Path(sourceFile, "realcugan-video.mp4");

    RealCuganSettings settings;
    settings.programPath = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    settings.modelName = ui->comboBox_Model_RealCUGAN->currentText();
    settings.targetScale = ui->doubleSpinBox_ScaleRatio_video->value();
    settings.denoiseLevel = ui->spinBox_DenoiseLevel_video->value();
    settings.tileSize = ui->spinBox_TileSize_RealCUGAN->value();
    settings.ttaEnabled = ui->checkBox_TTA_RealCUGAN->isChecked();
    settings.singleGpuId = ui->comboBox_GPUID_RealCUGAN->currentText();

    // Pass the job to the processor
    if (realCuganProcessor) {
        realCuganProcessor->processVideo(rowNum, sourceFile, destFile, settings);
    } else {
        qWarning() << "RealCuganProcessor is null! Cannot process video.";
        onProcessingFinished(rowNum, false);
    }
}

/**
 * @brief STUB for GIF Processing.
 * This engine's GIF processing has not been refactored into the processor yet.
 * This stub ensures the application does not hang or crash if a user selects it.
 */
void MainWindow::Realcugan_NCNN_Vulkan_GIF(int rowNum)
{
    qWarning() << "RealCUGAN GIF processing is not yet refactored. Failing job for row" << rowNum;
    TextBrowser_NewMessage(tr("This feature is not available yet for this engine."));
    onProcessingFinished(rowNum, false); // Immediately fail the job
}

// NOTE: All other old functions like Realcugan_NCNN_Vulkan_Video_BySegment,
// Realcugan_NCNN_Vulkan_ReadSettings, Realcugan_ncnn_vulkan_DetectGPU, etc.
// have been intentionally removed from this file as their logic is now
// obsolete or has been moved inside the RealCuganProcessor.
