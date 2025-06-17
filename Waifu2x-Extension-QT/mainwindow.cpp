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

    My Github homepage: https://github.com/beyawnko
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "RealCuganProcessor.h"
#include "VideoProcessor.h"
#include "ProcessRunner.h"
#include "GpuManager.h"
#include "UiController.h"
#include <QApplication>
#include <QDebug>
#include <QEventLoop>
#include <QMessageBox>
#include <QThreadPool>
#include <QThread>
#include <QEvent>
#include <QCloseEvent>

MainWindow::MainWindow(int maxThreadsOverride, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FFMPEG_EXE_PATH_Waifu2xEX = Current_Path + "/ffmpeg/ffmpeg.exe";
    realCuganProcessor = new RealCuganProcessor(this);
    videoProcessor = new VideoProcessor(this);
    qRegisterMetaType<FileMetadataCache>("FileMetadataCache");

    QVariant maxThreadCountSetting = Settings_Read_value("/settings/MaxThreadCount");
    globalMaxThreadCount = maxThreadCountSetting.isValid() ? maxThreadCountSetting.toInt() : 0;
    if (maxThreadsOverride > 0) {
        globalMaxThreadCount = maxThreadsOverride;
    }
    if (globalMaxThreadCount <= 0) {
        int cores = QThread::idealThreadCount();
        if (cores < 1) {
            cores = 1;
        }
        globalMaxThreadCount = cores * 2;
    }
    QThreadPool::globalInstance()->setMaxThreadCount(globalMaxThreadCount);
    ui->spinBox_ThreadNum_image->setMaximum(globalMaxThreadCount);
    ui->spinBox_ThreadNum_gif_internal->setMaximum(globalMaxThreadCount);
    ui->spinBox_ThreadNum_video_internal->setMaximum(globalMaxThreadCount);
    if (ui->spinBox_NumOfThreads_VFI) {
        ui->spinBox_NumOfThreads_VFI->setMaximum(globalMaxThreadCount);
    }
    setWindowTitle(QStringLiteral("Beya_Waifu %1 by beyawnko").arg(VERSION));
    translator = new QTranslator(this);
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange && isMinimized()) {
        if (ui->checkBox_MinimizeToTaskbar->isChecked()) {
            hide();
        }
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::Set_Font_fixed()
{
    uiController.setFontFixed(ui->checkBox_isCustFontEnable,
                              ui->fontComboBox_CustFont,
                              ui->spinBox_GlobalFontSize);
}

bool MainWindow::SystemPrefersDark() const
{
    return uiController.systemPrefersDark();
}

void MainWindow::ApplyDarkStyle()
{
    QVariant darkModeSetting = Settings_Read_value("/settings/DarkMode");
    uiController.applyDarkStyle(
        darkModeSetting.isValid() && !darkModeSetting.isNull() ? darkModeSetting.toInt() : 1);
}

bool MainWindow::runProcess(QProcess *process, const QString &cmd,
                            QByteArray *stdOut, QByteArray *stdErr)
{
    QEventLoop loop;
    if (stdOut) {
        stdOut->clear();
    }
    if (stdErr) {
        stdErr->clear();
    }
    if (stdOut) {
        QObject::connect(process, &QProcess::readyReadStandardOutput,
                         [&]() { stdOut->append(process->readAllStandardOutput()); });
    }
    if (stdErr) {
        QObject::connect(process, &QProcess::readyReadStandardError,
                         [&]() { stdErr->append(process->readAllStandardError()); });
    }
    QObject::connect(process,
                     QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     &loop,
                     &QEventLoop::quit);
    QObject::connect(process, &QProcess::errorOccurred, &loop, &QEventLoop::quit);
    process->start(cmd);
    loop.exec();
    return process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0;
}

void MainWindow::ShellMessageBox(const QString &title, const QString &text, QMessageBox::Icon icon)
{
    QMessageBox msg(icon, title, text, QMessageBox::Ok, this);
    msg.exec();
}

void MainWindow::setImageEngineIndex(int index)
{
    ui->comboBox_Engine_Image->setCurrentIndex(index);
}

void MainWindow::setGifEngineIndex(int index)
{
    ui->comboBox_Engine_GIF->setCurrentIndex(index);
}

void MainWindow::setVideoEngineIndex(int index)
{
    ui->comboBox_Engine_Video->setCurrentIndex(index);
}

void MainWindow::pushButton_Start_setEnabled_self(bool isEnabled)
{
    ui->pushButton_Start->setEnabled(isEnabled);
    Start_SystemTrayIcon->setEnabled(isEnabled);
    ui->pushButton_Start->setVisible(isEnabled);
}

void MainWindow::pushButton_Stop_setEnabled_self(bool isEnabled)
{
    ui->pushButton_Stop->setEnabled(isEnabled);
    Pause_SystemTrayIcon->setEnabled(isEnabled);
    ui->pushButton_Stop->setVisible(isEnabled);
}

FileMetadataCache MainWindow::getOrFetchMetadata(const QString &filePath) { qDebug() << "STUB: MainWindow::getOrFetchMetadata called for" << filePath; return FileMetadataCache(); }
bool MainWindow::ReplaceOriginalFile(QString original_fullpath, QString output_fullpath) { qDebug() << "STUB: MainWindow::ReplaceOriginalFile called for" << original_fullpath << "and" << output_fullpath; return false; }
void MainWindow::MoveFileToOutputPath(QString Orginal, QString SourceFilePath) { qDebug() << "STUB: MainWindow::MoveFileToOutputPath called for" << Orginal << "and" << SourceFilePath; }
QMap<QString,int> MainWindow::Image_Gif_Read_Resolution(QString SourceFileFullPath) { qDebug() << "STUB: MainWindow::Image_Gif_Read_Resolution called for" << SourceFileFullPath; return QMap<QString,int>(); }
void MainWindow::Delay_sec_sleep(int time) { qDebug() << "STUB: MainWindow::Delay_sec_sleep called with" << time; QThread::sleep(time); }

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // TODO: Add custom event filtering logic if needed
    qDebug() << "STUB: MainWindow::eventFilter called for object" << obj << "event type" << event->type();
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "STUB: MainWindow::closeEvent called";
    // Example: if (isAlreadyClosed.load()) return;
    // Ensure settings are saved if auto-save is on
    // if (ui->checkBox_AutoSaveSettings->isChecked()) {
    //     Settings_Save();
    // }
    // TODO: Implement proper exit procedure (stop threads, save state)
    QMainWindow::closeEvent(event);
}
