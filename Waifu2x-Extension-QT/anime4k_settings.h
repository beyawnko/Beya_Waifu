/*
    Copyright (C) 2025  beyawnko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
