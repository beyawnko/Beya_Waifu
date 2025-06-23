#include "mainwindow.h"
#include "ui_mainwindow.h"

// Removed definition for MainWindow::on_pushButton_compatibilityTest_clicked()
// Its body is now a stub in mainwindow.cpp

int MainWindow::Simple_Compatibility_Test()
{
    QString realcuganExe = Current_Path + "/realcugan-ncnn-vulkan/realcugan-ncnn-vulkan.exe";
    QString realesrganExe = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe";

    isCompatible_RealCUGAN_NCNN_Vulkan = QFile::exists(realcuganExe);
    emit Send_Add_progressBar_CompatibilityTest();

    isCompatible_RealESRGAN_NCNN_Vulkan = QFile::exists(realesrganExe);
    emit Send_Add_progressBar_CompatibilityTest();

    QMetaObject::invokeMethod(this, "Finish_progressBar_CompatibilityTest", Qt::QueuedConnection);
    emit Send_Waifu2x_Compatibility_Test_finished();
    return 0;
}

void MainWindow::waitForCompatibilityTest()
{
    if (compatibilityTestFuture.isRunning())
    {
        compatibilityTestFuture.waitForFinished();
    }
}
