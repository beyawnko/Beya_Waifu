#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::on_pushButton_compatibilityTest_clicked()
{
    ui->pushButton_compatibilityTest->setEnabled(false);
    if (ui->progressBar_CompatibilityTest) {
        ui->progressBar_CompatibilityTest->setRange(0, 2);
        ui->progressBar_CompatibilityTest->setValue(0);
        ui->progressBar_CompatibilityTest->setVisible(true);
    }
    compatibilityTestFuture = QtConcurrent::run([this]() {
        this->Simple_Compatibility_Test();
    });
}

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
