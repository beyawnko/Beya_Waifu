#ifndef REALCUGAN_SETTINGS_H
#define REALCUGAN_SETTINGS_H

#include <QString>

struct RealCuganSettings {
    QString programPath;
    QString modelName;
    double targetScale = 2.0; // Default value
    int denoiseLevel = 1;     // Default value
    int tileSize = 0;         // Default value (0 often means auto)
    bool ttaEnabled = false;  // Default value
    QString singleGpuId = "0"; // Default value
    QString outputFormat = "png"; // Default value
};

#endif // REALCUGAN_SETTINGS_H
