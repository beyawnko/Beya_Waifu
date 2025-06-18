#pragma once
/*
    Copyright (C) 2025  beyawnko
*/
#include <QFont>
#include <QCheckBox>
#include <QFontComboBox>
#include <QSpinBox>
#include "ui_mainwindow.h"

/**
 * @brief Encapsulates UI related helper routines.
 */
class UiController
{
public:
    UiController() = default;

    void setFontFixed(QCheckBox *enableCheckBox, QFontComboBox *fontComboBox,
                      QSpinBox *sizeSpinBox);

    bool systemPrefersDark() const;

    void applyDarkStyle(int mode);

    void outputSettingsAreaSetEnabled(Ui::MainWindow *ui, bool enabled);

    void updateEngineSettingsVisibility(Ui::MainWindow *ui, const QString& selectedEngineName);
};

