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
#include "ui_mainwindow.h" // Required for UI elements access via m_mainWindow
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QtGlobal>
#include <QListWidgetItem> // For preLoadSettings, though might be better in MainWindow
#include <QDebug>
// #include <QtCore5Compat/QTextCodec> // Not needed if not using QTextCodec specific features directly here

RealCuganProcessor::RealCuganProcessor(MainWindow *parent)
    : QObject(parent), m_mainWindow(parent)
{}

void RealCuganProcessor::preLoadSettings()
{
    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");

    QVariant modelVar = settings.value("RealCUGAN_Model", "models-se");
    if (m_mainWindow->comboBox_Model_RealCUGAN)
        m_mainWindow->comboBox_Model_RealCUGAN->setCurrentText(modelVar.toString());

    QVariant denoiseVar = settings.value("RealCUGAN_DenoiseLevel", -1);
    if (m_mainWindow->spinBox_DenoiseLevel_RealCUGAN)
        m_mainWindow->spinBox_DenoiseLevel_RealCUGAN->setValue(denoiseVar.toInt());

    QVariant tileVar = settings.value("RealCUGAN_TileSize", 0);
    if (m_mainWindow->spinBox_TileSize_RealCUGAN)
        m_mainWindow->spinBox_TileSize_RealCUGAN->setValue(tileVar.toInt());

    QVariant ttaVar = settings.value("RealCUGAN_TTA", false);
    if (m_mainWindow->checkBox_TTA_RealCUGAN)
        m_mainWindow->checkBox_TTA_RealCUGAN->setChecked(ttaVar.toBool());

    QVariant gpuVar = settings.value("RealCUGAN_GPUID", "0: Default GPU 0");
    if (m_mainWindow->comboBox_GPUID_RealCUGAN)
        m_mainWindow->comboBox_GPUID_RealCUGAN->setCurrentText(gpuVar.toString());

    QVariant multiVar = settings.value("RealCUGAN_MultiGPU_Enabled", false);
    if (m_mainWindow->checkBox_MultiGPU_RealCUGAN)
        m_mainWindow->checkBox_MultiGPU_RealCUGAN->setChecked(multiVar.toBool());

    // GPUIDs_List_MultiGPU_RealCUGAN is a member of MainWindow, handle its loading there or pass UI pointers
    // For now, assuming MainWindow handles its own UI list population from settings.
    // m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN = settings.value("RealCUGAN_GPUJobConfig_MultiGPU").value<QList_QMap_QStrQStr>();
    // if (m_mainWindow->listWidget_GPUList_MultiGPU_RealCUGAN) { ... }

    settings.endGroup();

    // These settings are application-wide, might be better placed in MainWindow's own preload logic
    // QSettings iniSettings(QDir::currentPath() + "/settings.ini", QSettings::IniFormat);
    // m_mainWindow->setImageEngineIndex(iniSettings.value("/settings/ImageEngine", 0).toInt());
    // m_mainWindow->setGifEngineIndex(iniSettings.value("/settings/GIFEngine", 0).toInt());
    // m_mainWindow->setVideoEngineIndex(iniSettings.value("/settings/VideoEngine", 0).toInt());

    readSettings(); // Ensure member variables are populated with these settings
    qDebug() << "RealCuganProcessor::preLoadSettings completed (some settings might be MainWindow responsibility).";
}

void RealCuganProcessor::readSettings()
{
    // This function should populate the RealCuganProcessor's own members if it had them,
    // or directly use m_mainWindow's members if they are guaranteed to be up-to-date.
    // The current implementation reads from UI/settings and stores into m_mainWindow->m_realcugan_...
    // This is acceptable as long as m_mainWindow is the central place for these settings.

    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");

    m_mainWindow->m_realcugan_Model = m_mainWindow->comboBox_Model_RealCUGAN
            ? m_mainWindow->comboBox_Model_RealCUGAN->currentText()
            : settings.value("RealCUGAN_Model", "models-se").toString();
    m_mainWindow->m_realcugan_DenoiseLevel = m_mainWindow->spinBox_DenoiseLevel_RealCUGAN
            ? m_mainWindow->spinBox_DenoiseLevel_RealCUGAN->value()
            : settings.value("RealCUGAN_DenoiseLevel", -1).toInt();
    m_mainWindow->m_realcugan_TileSize = m_mainWindow->spinBox_TileSize_RealCUGAN
            ? m_mainWindow->spinBox_TileSize_RealCUGAN->value()
            : settings.value("RealCUGAN_TileSize", 0).toInt();
    m_mainWindow->m_realcugan_TTA = m_mainWindow->checkBox_TTA_RealCUGAN
            ? m_mainWindow->checkBox_TTA_RealCUGAN->isChecked()
            : settings.value("RealCUGAN_TTA", false).toBool();

    bool multiEnabled = m_mainWindow->checkBox_MultiGPU_RealCUGAN
            ? m_mainWindow->checkBox_MultiGPU_RealCUGAN->isChecked()
            : settings.value("RealCUGAN_MultiGPU_Enabled", false).toBool();

    QString comboGpu = m_mainWindow->comboBox_GPUID_RealCUGAN
            ? m_mainWindow->comboBox_GPUID_RealCUGAN->currentText()
            : settings.value("RealCUGAN_GPUID", "0: Default GPU 0").toString();

    if (multiEnabled) {
        // This logic assumes GPUIDs_List_MultiGPU_RealCUGAN is populated correctly in MainWindow
        if (!m_mainWindow->GPUIDs_List_MultiGPU_RealCUGAN.isEmpty()) {
             // Prefer the first one from the job list if multi-GPU is on and list is not empty.
             // Or, rely on RealcuganNcnnVulkan_MultiGPU() to build the correct string for multiGpuJobArgs.
            m_mainWindow->m_realcugan_GPUID = comboGpu; // Keep individual selection as primary for single GPU mode within multi-setup
        } else {
            m_mainWindow->m_realcugan_GPUID = comboGpu; // Fallback if list is empty
        }
    } else {
        m_mainWindow->m_realcugan_GPUID = comboGpu;
    }
    settings.endGroup();

    // qDebug() call removed from here as it's better placed in MainWindow after calling this.
}

void RealCuganProcessor::readSettingsVideoGif(int ThreadNum)
{
    Q_UNUSED(ThreadNum); // ThreadNum might be used for per-thread GPU cycling if implemented
    readSettings(); // Read the base settings first
    // Specific adjustments for Video/GIF can be made here if necessary
    // The current implementation of readSettings already updates m_mainWindow->m_realcugan_...
    // which are then used by MainWindow when calling prepareArguments.
    // qDebug() call removed from here.
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
                                                 bool experimental,
                                                 const QString &jobsStr,
                                                 const QString &syncGapStr,
                                                 bool verboseLog)
{
    QStringList arguments;
    arguments << "-i" << inputFile;
    arguments << "-o" << outputFile;
    arguments << "-s" << QString::number(currentPassScale);
    arguments << "-n" << QString::number(denoiseLevel);

    // Tile Size (-t): 0 for auto. The executable handles "0" as auto.
    arguments << "-t" << QString::number(tileSize);

    // Model (-m): Resolved by modelPath()
    arguments << "-m" << modelPath(modelName, experimental);

    // GPU ID (-g)
    if (isMultiGPU && !multiGpuJobArgs.isEmpty()) {
        arguments << "-g" << multiGpuJobArgs;
    } else if (!gpuId.isEmpty() && gpuId.toLower() != "auto") {
        QString actualGpuId = gpuId.split(":")[0];
        arguments << "-g" << actualGpuId;
    }
    // If gpuId is empty or "auto", the -g switch is omitted, letting the executable use its default.

    // Jobs (-j): load:proc:save thread configuration
    if (!jobsStr.isEmpty()) {
        arguments << "-j" << jobsStr;
    }
    // If jobsStr is empty, the executable's default job configuration will be used.

    // SyncGap (-c): sync gap mode
    bool syncGapOk;
    int syncgap_val = syncGapStr.toInt(&syncGapOk);
    if (syncGapOk && syncgap_val >= 0 && syncgap_val <= 3) {
        arguments << "-c" << syncGapStr;
    } else {
        arguments << "-c" << "3"; // Default to 3 if invalid or empty
        if (!syncGapStr.isEmpty() && syncGapStr != "3") { // Log only if an invalid non-empty string (not already "3") was provided
            qDebug() << "[RealCuganProcessor] Invalid syncgap value provided:" << syncGapStr << ". Defaulting to 3.";
        }
    }

    // TTA Mode (-x)
    if (ttaEnabled) {
        arguments << "-x";
    }

    // Output Format (-f)
    if (!outputFormat.isEmpty()) {
        arguments << "-f" << outputFormat.toLower();
    } else {
        arguments << "-f" << "png"; // Default to png
    }

    // Verbose Log (-v)
    if (verboseLog) {
        arguments << "-v";
    }

    // Removed the central qDebug message for arguments from here.
    // It's better to log this in the calling function in MainWindow if needed.
    return arguments;
}

QString RealCuganProcessor::executablePath(bool experimental) const
{
    Q_UNUSED(experimental);
    #ifdef Q_OS_WIN
        return QCoreApplication::applicationDirPath() + "/realcugan-ncnn-vulkan.exe";
    #else
        return QCoreApplication::applicationDirPath() + "/realcugan-ncnn-vulkan";
    #endif
}

QString RealCuganProcessor::modelPath(const QString &modelName, bool experimental) const
{
    Q_UNUSED(experimental);
    return QCoreApplication::applicationDirPath() + "/" + modelName;
}

RealCuganProcessor::~RealCuganProcessor()
{
    // qDebug() << "RealCuganProcessor destroyed"; // Avoid qDebug in destructor
}
