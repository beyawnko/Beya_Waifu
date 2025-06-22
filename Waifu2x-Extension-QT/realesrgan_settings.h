// file: realesrgan_settings.h
#ifndef REALESRGAN_SETTINGS_H
#define REALESRGAN_SETTINGS_H

#include <QString>

struct RealEsrganSettings {
    QString programPath;
    QString modelName;
    int modelNativeScale = 4; // Most Real-ESRGAN models are 4x
    int targetScale;
    int tileSize;
    bool ttaEnabled;
    QString outputFormat = "png";

    // GPU Settings
    QString singleGpuId;
};

#endif // REALESRGAN_SETTINGS_H
