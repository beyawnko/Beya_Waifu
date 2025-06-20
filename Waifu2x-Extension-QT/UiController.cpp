/*
    Copyright (C) 2025  beyawnko
*/
#include "UiController.h"
#include "ui_mainwindow.h" // Make sure this is included if not already
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QStyle>

void UiController::setFontFixed(QCheckBox *enableCheckBox, QFontComboBox *fontComboBox,
                                QSpinBox *sizeSpinBox)
{
    QFont font;
    if (enableCheckBox->isChecked()) {
        font = fontComboBox->currentFont();
        font.setPixelSize(sizeSpinBox->value());
    } else {
        font = qApp->font();
        font.setPixelSize(15);
    }
    qApp->setFont(font);
}

bool UiController::systemPrefersDark() const
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                       QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#else
    QColor bg = qApp->palette().color(QPalette::Window);
    return bg.lightness() < 128;
#endif
}

void UiController::applyDarkStyle(int mode)
{
    bool enable = false;
    if (mode == 1)
        enable = true;
    else if (mode == 2)
        enable = systemPrefersDark();

    if (enable) {
        QString style;
        QFile f(":/style/styles/dark.qss");
        if (f.open(QIODevice::ReadOnly))
            style += QString::fromUtf8(f.readAll());

        QFile f2(":/style/styles/button.qss");
        if (f2.open(QIODevice::ReadOnly))
            style += QString::fromUtf8(f2.readAll());

        qApp->setStyleSheet(style);
    } else {
        qApp->setStyleSheet("");
    }

    for (QWidget *w : QApplication::allWidgets()) {
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    }
}

void UiController::outputSettingsAreaSetEnabled(Ui::MainWindow *ui, bool enabled)
{
    ui->scrollArea_outputPathSettings->setEnabled(enabled);
    ui->lineEdit_outputPath->setClearButtonEnabled(enabled);
    ui->lineEdit_outputPath->setFocusPolicy(enabled ? Qt::StrongFocus : Qt::NoFocus);
}

void UiController::updateEngineSettingsVisibility(Ui::MainWindow *ui, const QString& selectedEngineName)
{
    if (!ui || !ui->tabWidget_Engines) return;

    QWidget *targetTab = nullptr;

    // Map engine names (as they appear in comboboxes) to tab objectNames
    if (selectedEngineName.contains("waifu2x-ncnn-vulkan", Qt::CaseInsensitive)) {
        targetTab = ui->tab_W2xNcnnVulkan;
    } else if (selectedEngineName.contains("waifu2x-converter", Qt::CaseInsensitive)) {
        targetTab = ui->tab_W2xConverter;
    } else if (selectedEngineName.contains("Anime4K", Qt::CaseInsensitive)) {
        targetTab = ui->tab_A4k;
    } else if (selectedEngineName.contains("srmd-ncnn-vulkan", Qt::CaseInsensitive)) {
        targetTab = ui->tab_SrmdNcnnVulkan;
    } else if (selectedEngineName.contains("waifu2x-caffe", Qt::CaseInsensitive)) {
        targetTab = ui->tab_W2xCaffe;
    } else if (selectedEngineName.contains("realsr-ncnn-vulkan", Qt::CaseInsensitive) || selectedEngineName.contains("RealSR-ncnn-vulkan", Qt::CaseInsensitive)) { // Original name from UI
        targetTab = ui->tab_RealesrganNcnnVulkan; // The tab is named RealesrganNcnnVulkan
    } else if (selectedEngineName.contains("RealESRGAN-NCNN-Vulkan", Qt::CaseInsensitive)) {
        targetTab = ui->tab_RealesrganNcnnVulkan; // Assuming RealESRGAN uses the same tab as RealSR or has its own. UI shows tab_RealesrganNcnnVulkan.
    } else if (selectedEngineName.contains("RealCUGAN-NCNN-Vulkan", Qt::CaseInsensitive)) {
        // RealCUGAN settings are in a hidden widget, not a main engine tab.
        // This function might not be the place to toggle its visibility,
        // or it needs a different mechanism. For now, do nothing or log.
        qDebug() << "UiController: RealCUGAN selected, settings are not in a dedicated tab in tabWidget_Engines.";
        // Potentially, if RealCUGAN has its own tab, map it here.
        // Based on UI, it does not. It uses a hidden group box.
        // Let's assume for now it should default to a general tab or do nothing.
        // targetTab = ui->tab_W2xNcnnVulkan; // Fallback example
    }
    // Add other mappings as necessary

    if (targetTab) {
        ui->tabWidget_Engines->setCurrentWidget(targetTab);
    } else {
        qDebug() << "UiController: Could not find a tab for engine:" << selectedEngineName;
        // Optionally, set to a default tab if no match
        // ui->tabWidget_Engines->setCurrentIndex(0);
    }
}
