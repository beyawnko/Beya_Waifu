// file: realcugan_settings.h
#ifndef REALCUGAN_SETTINGS_H
#define REALCUGAN_SETTINGS_H

#include <QString>
#include <QList>
#include <QMap>

struct RealCuganSettings {
    QString programPath;
    QString modelName;
    int targetScale;
    int denoiseLevel;
    int tileSize;
    bool ttaEnabled;
    bool experimental; // For alpha channel version
    QString outputFormat = "png";

    // GPU Settings
    QString singleGpuId;
    bool multiGpuEnabled;
    QList<QMap<QString, QString>> multiGpuConfig;
};

#endif // REALCUGAN_SETTINGS_H
