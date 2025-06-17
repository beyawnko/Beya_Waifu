#include "mainwindow.h"
#include <QDebug>

QStringList MainWindow::RealESRGAN_NCNN_Vulkan_PrepareArguments(const QString&, const QString&, int, const QString&, int, const QString&, bool, bool, const QString&)
{
    return {};
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int, bool)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_Image stub";
}

void MainWindow::RealESRGAN_NCNN_Vulkan_GIF(int)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_GIF stub";
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Video(int)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_Video stub";
}

void MainWindow::RealESRGAN_NCNN_Vulkan_Video_BySegment(int)
{
    qDebug() << "RealESRGAN_NCNN_Vulkan_Video_BySegment stub";
}

void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings()
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_ReadSettings_Video_GIF(int)
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_PreLoad_Settings()
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_CleanupTempFiles(const QString&, int, bool, const QString&)
{
}

void MainWindow::RealESRGAN_NCNN_Vulkan_DetectGPU_errorOccurred(QProcess::ProcessError)
{
}

