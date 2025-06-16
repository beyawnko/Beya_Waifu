#pragma once
#include "mainwindow.h"
#include <QDebug>

QStringList MainWindow::Realesrgan_NCNN_Vulkan_PrepareArguments(const QString&, const QString&, int, const QString&, int, const QString&, bool, const QString&, bool, const QString&, bool)
{
    return {};
}

void MainWindow::Realesrgan_NCNN_Vulkan_Image(int, bool)
{
    qDebug() << "Realesrgan_NCNN_Vulkan_Image stub";
}

void MainWindow::Realesrgan_NCNN_Vulkan_GIF(int)
{
    qDebug() << "Realesrgan_NCNN_Vulkan_GIF stub";
}

void MainWindow::Realesrgan_NCNN_Vulkan_Video(int)
{
    qDebug() << "Realesrgan_NCNN_Vulkan_Video stub";
}

void MainWindow::Realesrgan_NCNN_Vulkan_Video_BySegment(int)
{
    qDebug() << "Realesrgan_NCNN_Vulkan_Video_BySegment stub";
}

void MainWindow::Realesrgan_NCNN_Vulkan_ReadSettings()
{
}

void MainWindow::Realesrgan_NCNN_Vulkan_ReadSettings_Video_GIF(int)
{
}

void MainWindow::Realesrgan_NCNN_Vulkan_PreLoad_Settings()
{
}

void MainWindow::Realesrgan_NCNN_Vulkan_CleanupTempFiles(const QString&, int, bool, const QString&)
{
}

void MainWindow::Realesrgan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)
{
}

