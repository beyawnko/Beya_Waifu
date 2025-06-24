#ifndef REALCUGAN_SETTINGS_H
#define REALCUGAN_SETTINGS_H

#include <QString>

struct RealCuganSettings {
    QString sourceFile; // Source file path, used for logging & context
    QString programPath;
    QString ffprobePath;
    QString ffmpegPath;
    QString modelName;    // Used for -m in image mode (model directory)
    QString modelPath;    // Used for -m in pipe mode (direct model files like up2x-conservative)
    int targetScale = 2;  // Changed from double to int as SR engines usually take int
    int denoiseLevel = -1; // Common default for "auto" or "conservative"
    int tileSize = 0;
    bool ttaEnabled = false;
    QString singleGpuId = "auto"; // Changed default to "auto"
    QString outputFormat = "png";
    bool verboseLog = false;

    // Video specific settings
    double videoFps = 0; // 0 for auto from source, or user override

    // New FFmpeg encoder settings
    QString videoEncoderCodec = "libx264";
    QString videoEncoderPreset = "medium";
    int videoEncoderCRF = 23;
    QString videoOutputPixFmt = "yuv420p";
};

#endif // REALCUGAN_SETTINGS_H
