/*
    Copyright (C) 2025  beyawnko
*/
#include "UiController.h"
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
        QFile f(":/style/styles/dark.qss");
        if (f.open(QIODevice::ReadOnly))
            qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
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

