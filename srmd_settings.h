// file: srmd_settings.h
#ifndef SRMD_SETTINGS_H
#define SRMD_SETTINGS_H

#include <QString>

struct SrmdSettings {
    QString programPath;
    int targetScale; // 2, 3, or 4
    int denoiseLevel; // -1 to 3
    int tileSize;
    bool ttaEnabled;
    QString outputFormat = "png";

    // GPU Settings
    QString singleGpuId;
};

#endif // SRMD_SETTINGS_H
