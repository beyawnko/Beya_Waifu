#pragma once
#include "mainwindow.h"
#include <QDebug>

// Stub implementations to satisfy the build

QStringList MainWindow::Realcugan_NCNN_Vulkan_PrepareArguments(const QString&, const QString&, int, const QString&, int, int, const QString&, bool, const QString&, bool, const QString&, bool)
{
    return {};
}

void MainWindow::Realcugan_NCNN_Vulkan_Video_BySegment(int)
{
    qDebug() << "Realcugan_NCNN_Vulkan_Video_BySegment stub";
}

void MainWindow::Realcugan_NCNN_Vulkan_Video(int)
{
    qDebug() << "Realcugan_NCNN_Vulkan_Video stub";
}

void MainWindow::Realcugan_NCNN_Vulkan_Image(int, bool, bool)
{
    qDebug() << "Realcugan_NCNN_Vulkan_Image stub";
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings()
{
}

void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int)
{
}

void MainWindow::Realcugan_NCNN_Vulkan_PreLoad_Settings()
{
}

void MainWindow::Realcugan_NCNN_Vulkan_CleanupTempFiles(const QString&, int, bool, const QString&)
{
}

void MainWindow::Realcugan_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)
{
}

