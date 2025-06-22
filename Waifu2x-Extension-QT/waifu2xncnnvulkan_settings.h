// file: waifu2xncnnvulkan_settings.h
#ifndef WAIFU2XNCNNVULKAN_SETTINGS_H
#define WAIFU2XNCNNVULKAN_SETTINGS_H

#include <QString>
#include <QList>
#include <QMap>

struct Waifu2xNcnnVulkanSettings {
    QString programPath;
    int targetScale;
    int denoiseLevel;
    int tileSize;
    bool ttaEnabled;

    QString model; // "upconv_7" or "cunet"
    QString imageStyle; // "2D Anime" or "3D Real-life" to determine model subfolder

    // GPU Settings
    QString singleGpuId;
    bool multiGpuEnabled;
    QList<QMap<QString, QString>> multiGpuConfig;
};

#endif // WAIFU2XNCNNVULKAN_SETTINGS_H
