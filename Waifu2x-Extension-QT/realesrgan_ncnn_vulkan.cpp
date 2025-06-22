#include "mainwindow.h"
#include <QDebug>
#include <QSettings>

void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int file_list_row_number, bool /*isBatch*/)
{
    /*******************************************************
    *        Real-ESRGAN Image Processing (Refactored Wrapper)
    *******************************************************/

    // Note: Assuming Table_model_image is the correct model for this function.
    // The original code used table_image_item_fullpath.at(file_list_row_number),
    // so we get the full path from column 2 of Table_model_image.
    QString sourceFile = Table_model_image->item(file_list_row_number, 2)->text(); // Col 2 is FullPath
    QString destFile = Generate_Output_Path(sourceFile, "realesrgan");

    RealEsrganSettings settings;
    settings.programPath = Current_Path + "/realesrgan-ncnn-vulkan/realesrgan-ncnn-vulkan.exe"; // Path to the executable

    // UI elements for RealESRGAN settings:
    // ui->comboBox_Model_RealsrNCNNVulkan
    // ui->doubleSpinBox_ScaleRatio_image (assuming this is general image scale, RealESRGAN might have its own or derive from model)
    // ui->spinBox_TileSize_RealsrNCNNVulkan
    // ui->checkBox_TTA_RealsrNCNNVulkan
    // ui->comboBox_GPUID_RealsrNCNNVulkan

    settings.modelName = ui->comboBox_Model_RealsrNCNNVulkan->currentText();

    // Target scale: Use the general image scale ratio for now.
    // The RealEsrganProcessor will handle iterative scaling if targetScale > modelNativeScale.
    if (ui->doubleSpinBox_ScaleRatio_image) {
        settings.targetScale = static_cast<int>(ui->doubleSpinBox_ScaleRatio_image->value());
    } else {
        settings.targetScale = Settings_Read_value("/settings/ImageScaleRatio", 2).toInt(); // Default to 2x if UI element not found
    }

    if (ui->spinBox_TileSize_RealsrNCNNVulkan) {
        settings.tileSize = ui->spinBox_TileSize_RealsrNCNNVulkan->value();
    } else {
        settings.tileSize = 0; // Auto
    }

    if (ui->checkBox_TTA_RealsrNCNNVulkan) {
        settings.ttaEnabled = ui->checkBox_TTA_RealsrNCNNVulkan->isChecked();
    } else {
        settings.ttaEnabled = false;
    }

    if (ui->comboBox_GPUID_RealsrNCNNVulkan) {
        settings.singleGpuId = ui->comboBox_GPUID_RealsrNCNNVulkan->currentText();
    } else {
        settings.singleGpuId = "auto";
    }

    // Determine model's native scale from its name (e.g., "realesrgan-x4plus")
    if (settings.modelName.contains("x4")) {
        settings.modelNativeScale = 4;
    } else if (settings.modelName.contains("x2")) {
        settings.modelNativeScale = 2;
    } else {
        // Fallback or default native scale. Many common RealESRGAN models are x4.
        // If not specified in name, it might be a generic model where -s param is more critical.
        // For safety, let's assume 4 if not specified, or 1 if it's truly unknown and relies on -s from user.
        // The processor's buildArguments uses settings.modelNativeScale for the -s parameter.
        settings.modelNativeScale = 4; // Default to 4x, can be adjusted if behavior is different.
        qDebug() << "RealESRGAN_NCNN_Vulkan_Image: Model native scale not explicitly in name, defaulting to" << settings.modelNativeScale;
    }

    // Set output format from general image settings
    if (ui->comboBox_ImageSaveFormat) {
        settings.outputFormat = ui->comboBox_ImageSaveFormat->currentText().toLower();
        if (settings.outputFormat == "keep original") settings.outputFormat = QFileInfo(sourceFile).suffix().toLower();
        if (settings.outputFormat.isEmpty()) settings.outputFormat = "png"; // Default if "Keep Original" on a file without suffix
    } else {
        settings.outputFormat = "png";
    }


    // The MainWindow's current_File_Row_Number and Processing_Status
    // should be set by the calling loop (e.g., in Waifu2xMainThread or whatever calls this)
    // before this function is invoked. This wrapper just passes the rowNum.

    m_realEsrganProcessor->processImage(file_list_row_number, sourceFile, destFile, settings);
}

static QStringList parseVulkanDeviceList(const QString &output)
{
    QStringList list;
    QRegularExpression re(QStringLiteral("^\\s*(?:GPU device\\s*)?\\[?(\\d+)\\]?[:\\-]?\\s*(.+)$"),
                           QRegularExpression::CaseInsensitiveOption |
                           QRegularExpression::MultilineOption);
    auto it = re.globalMatch(output);
    while (it.hasNext())
    {
        auto m = it.next();
        list << QStringLiteral("%1: %2").arg(m.captured(1).trimmed(), m.captured(2).trimmed());
    }
    return list;
}

