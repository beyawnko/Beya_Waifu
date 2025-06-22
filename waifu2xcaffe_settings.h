// file: waifu2xcaffe_settings.h
#ifndef WAIFU2XCAFFE_SETTINGS_H
#define WAIFU2XCAFFE_SETTINGS_H

#include <QString>

struct Waifu2xCaffeSettings {
    QString programPath;
    QString processMode; // "CPU", "GPU", or "cuDNN"
    QString model; // Path to the model directory
    bool ttaEnabled = false;
    int gpuId = 0;
    int batchSize = 1;
    int splitSize = 128;

    bool multiGpuEnabled = false;
    QString multiGpuConfig; // The pre-formatted string for multi-GPU
};

#endif // WAIFU2XCAFFE_SETTINGS_H
