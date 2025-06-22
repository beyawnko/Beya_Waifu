// file: waifu2xconverter_settings.h
#ifndef WAIFU2XCONVERTER_SETTINGS_H
#define WAIFU2XCONVERTER_SETTINGS_H

#include <QString>

struct Waifu2xConverterSettings {
    QString programPath;
    int denoiseLevel;
    int targetScale;
    int blockSize;

    bool ttaEnabled;
    bool forceOpenCL;
    bool disableGpu;
    QString targetProcessor; // e.g., "auto", "cpu", "gpu:0"
};

#endif // WAIFU2XCONVERTER_SETTINGS_H
