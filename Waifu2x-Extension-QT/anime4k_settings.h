// file: anime4k_settings.h
#ifndef ANIME4K_SETTINGS_H
#define ANIME4K_SETTINGS_H

#include <QString>

struct Anime4kSettings {
    // General Settings
    bool preserveAlpha;
    QString programPath;
    int numOfGpus;

    // Anime4K Specific UI Settings
    bool acNet;
    bool hdnMode;
    bool fastMode;
    int passes;
    int pushColorCount;
    double pushColorStrength;
    double pushGradientStrength;

    // Pre/Post Processing
    bool preProcessing;
    bool postProcessing;
    QString preFilters;  // e.g., "MedianBlur:MeanBlur:CAS"
    QString postFilters; // e.g., "GaussianBlurWeak:BilateralFilter"

    // GPU Settings
    bool gpuMode;
    QString gpgpuModel; // "OpenCL" or "CUDA"
    bool specifyGpu;
    QString gpuString;
    int commandQueues;
    bool parallelIo;
};

#endif // ANIME4K_SETTINGS_H
