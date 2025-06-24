// file: realesrgan_settings.h
#pragma once

#include <QString>

struct RealEsrganSettings {
    QString programPath;
    QString modelName;
    int modelNativeScale = 4; // Most Real-ESRGAN models are 4x
    int targetScale;
    int tileSize;
    bool ttaEnabled;
    QString outputFormat = "png";

    // Logging and general paths
    bool verboseLog = false;
    QString sourceFile;
    QString ffmpegPath;
    QString ffprobePath;
    QString modelPath;
    double videoFps = 0.0;

    // GPU Settings
    QString singleGpuId;
};
