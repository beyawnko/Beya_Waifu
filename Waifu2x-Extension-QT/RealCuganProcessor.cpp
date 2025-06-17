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

#include "RealCuganProcessor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication> // Added for applicationDirPath
#include <QDir>
#include <QSettings>
#include <QtGlobal>
#include <QListWidgetItem>
#include <QDebug>
#include <QtCore5Compat/QTextCodec>

RealCuganProcessor::RealCuganProcessor(MainWindow *parent)
    : QObject(parent), m_mainWindow(parent)
{}

void RealCuganProcessor::preLoadSettings()
{
    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");
    // Fallback to stored values when specific UI widgets are not available.
    QVariant modelVar = settings.value("Model", "models-se");
    if (m_mainWindow->comboBox_Model_RealCUGAN)
        m_mainWindow->comboBox_Model_RealCUGAN->setCurrentText(modelVar.toString());

    QVariant denoiseVar = settings.value("DenoiseLevel", -1);
    if (m_mainWindow->spinBox_DenoiseLevel_RealCUGAN)
        m_mainWindow->spinBox_DenoiseLevel_RealCUGAN->setValue(denoiseVar.toInt());

    QVariant tileVar = settings.value("TileSize", 0);
    if (m_mainWindow->spinBox_TileSize_RealCUGAN)
        m_mainWindow->spinBox_TileSize_RealCUGAN->setValue(tileVar.toInt());

    QVariant ttaVar = settings.value("TTA", false);
    if (m_mainWindow->checkBox_TTA_RealCUGAN)
        m_mainWindow->checkBox_TTA_RealCUGAN->setChecked(ttaVar.toBool());

    QVariant gpuVar = settings.value("GPUID", "0: Default GPU 0");
    if (m_mainWindow->comboBox_GPUID_RealCUGAN)
        m_mainWindow->comboBox_GPUID_RealCUGAN->setCurrentText(gpuVar.toString());

    QVariant multiVar = settings.value("MultiGPUEnabled", false);
    if (m_mainWindow->checkBox_MultiGPU_RealCUGAN)
        m_mainWindow->checkBox_MultiGPU_RealCUGAN->setChecked(multiVar.toBool());

    m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN = settings.value("MultiGPU_List").value<QList_QMap_QStrQStr>();
    if (m_mainWindow->listWidget_GPUList_MultiGPU_RealCUGAN) {
        m_mainWindow->listWidget_GPUList_MultiGPU_RealCUGAN->clear();
        for(const auto &gpuMap : m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN) {
            QListWidgetItem *newItem = new QListWidgetItem();
            newItem->setText(QString("ID: %1, Name: %2, Threads: %3").arg(gpuMap.value("ID"), gpuMap.value("Name"), gpuMap.value("Threads")));
            m_mainWindow->listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
        }
    }
    settings.endGroup();

    QSettings iniSettings(QDir::currentPath() + "/settings.ini", QSettings::IniFormat);
    // iniSettings.setIniCodec(QTextCodec::codecForName("UTF-8")); // Removed for Qt6
    m_mainWindow->setImageEngineIndex(iniSettings.value("/settings/ImageEngine", 0).toInt());
    m_mainWindow->setGifEngineIndex(iniSettings.value("/settings/GIFEngine", 0).toInt());
    m_mainWindow->setVideoEngineIndex(iniSettings.value("/settings/VideoEngine", 0).toInt());
    readSettings();
    qDebug() << "Realcugan_NCNN_Vulkan_PreLoad_Settings completed.";
}

void RealCuganProcessor::readSettings()
{
    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");
    // Use saved settings as defaults when RealCUGAN widgets are absent.

    m_mainWindow->m_realcugan_Model = m_mainWindow->comboBox_Model_RealCUGAN
            ? m_mainWindow->comboBox_Model_RealCUGAN->currentText()
            : settings.value("Model", "models-se").toString();
    m_mainWindow->m_realcugan_DenoiseLevel = m_mainWindow->spinBox_DenoiseLevel_RealCUGAN
            ? m_mainWindow->spinBox_DenoiseLevel_RealCUGAN->value()
            : settings.value("DenoiseLevel", -1).toInt();
    m_mainWindow->m_realcugan_TileSize = m_mainWindow->spinBox_TileSize_RealCUGAN
            ? m_mainWindow->spinBox_TileSize_RealCUGAN->value()
            : settings.value("TileSize", 0).toInt();
    m_mainWindow->m_realcugan_TTA = m_mainWindow->checkBox_TTA_RealCUGAN
            ? m_mainWindow->checkBox_TTA_RealCUGAN->isChecked()
            : settings.value("TTA", false).toBool();

    bool multiEnabled = m_mainWindow->checkBox_MultiGPU_RealCUGAN
            ? m_mainWindow->checkBox_MultiGPU_RealCUGAN->isChecked()
            : settings.value("MultiGPUEnabled", false).toBool();

    QString comboGpu = m_mainWindow->comboBox_GPUID_RealCUGAN
            ? m_mainWindow->comboBox_GPUID_RealCUGAN->currentText()
            : settings.value("GPUID", "0: Default GPU 0").toString();

    if (multiEnabled) {
        if (!m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN.isEmpty()) {
            m_mainWindow->m_realcugan_GPUID =
                m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN.first().value("ID");
        } else {
            m_mainWindow->m_realcugan_GPUID = comboGpu;
        }
    } else {
        m_mainWindow->m_realcugan_GPUID = comboGpu;
    }

    settings.endGroup();

    qDebug() << "Realcugan_NCNN_Vulkan_ReadSettings: Model:" << m_mainWindow->m_realcugan_Model
             << "Denoise:" << m_mainWindow->m_realcugan_DenoiseLevel
             << "Tile:" << m_mainWindow->m_realcugan_TileSize
             << "TTA:" << m_mainWindow->m_realcugan_TTA
             << "GPUID:" << m_mainWindow->m_realcugan_GPUID;
}

void RealCuganProcessor::readSettingsVideoGif(int ThreadNum)
{
    Q_UNUSED(ThreadNum);
    readSettings();
    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan"); // Needed when widgets are missing
    QString fallbackId = m_mainWindow->m_realcugan_GPUID.split(":").first();
    QString gpuJobConfig = m_jobManager.buildGpuJobString(
                m_mainWindow->checkBox_MultiGPU_RealCUGAN
                    ? m_mainWindow->checkBox_MultiGPU_RealCUGAN->isChecked()
                    : settings.value("MultiGPUEnabled", false).toBool(),
                m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN,
                fallbackId);
    m_mainWindow->m_realcugan_gpuJobConfig_temp = m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN;
    qDebug() << "Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF for ThreadNum" << ThreadNum
             << "GPU/Job Config:" << gpuJobConfig;
    settings.endGroup();
}

QStringList RealCuganProcessor::prepareArguments(const QString &inputFile,
                                                 const QString &outputFile,
                                                 int currentPassScale,
                                                 const QString &modelName,
                                                 int denoiseLevel,
                                                 int tileSize,
                                                 const QString &gpuId,
                                                 bool ttaEnabled,
                                                 const QString &outputFormat,
                                                 bool isMultiGPU,
                                                 const QString &multiGpuJobArgs,
                                                 bool experimental)
{
    QString Current_Path = QDir::currentPath();
    QStringList arguments;
    arguments << "-i" << inputFile
              << "-o" << outputFile
              << "-s" << QString::number(currentPassScale)
              << "-n" << QString::number(denoiseLevel)
              << "-t" << QString::number(tileSize)
              << "-m" << modelPath(modelName, experimental);
    if (isMultiGPU) {
        // Assuming multiGpuJobArgs is the already formatted string for the -g option (e.g., "0,1,2")
        // or a special job string if the underlying executable handles that.
        // For standard realcugan-ncnn-vulkan, -g expects a comma-separated list of IDs.
        // If multiGpuJobArgs is "id1,id2,id3", it should be passed as a single argument to -g.
        // If it's a more complex job string for a custom wrapper, it might also be a single arg.
        // The .split(" ") was likely incorrect unless the job string itself had space-separated flags.
        arguments << "-g" << multiGpuJobArgs;
    } else {
        QString actualGpuId = gpuId.split(":")[0]; // Extracts "0" from "0: Default GPU 0"
        arguments << "-g" << actualGpuId;
    }
    if (ttaEnabled) {
        arguments << "-x";
    }
    arguments << "-f" << outputFormat.toLower();
    return arguments;
}

QString RealCuganProcessor::executablePath(bool experimental) const
{
    // experimental flag is no longer used to determine path structure here,
    // as executables are expected to be directly in applicationDirPath.
    // If different executables are needed for experimental, they should have distinct names.
    Q_UNUSED(experimental);
    #ifdef Q_OS_WIN
        return QCoreApplication::applicationDirPath() + "/realcugan-ncnn-vulkan.exe";
    #else
        return QCoreApplication::applicationDirPath() + "/realcugan-ncnn-vulkan";
    #endif
}

QString RealCuganProcessor::modelPath(const QString &modelName, bool experimental) const
{
    // experimental flag is no longer used to determine path structure here.
    // Models are expected to be in subdirectories relative to the application path.
    // e.g. appDir/models-se, appDir/models-pro
    Q_UNUSED(experimental);
    return QCoreApplication::applicationDirPath() + "/" + modelName;
}

RealCuganProcessor::~RealCuganProcessor()
{
    // Destructor logic, if any, or leave empty
    qDebug() << "RealCuganProcessor destroyed";
}
