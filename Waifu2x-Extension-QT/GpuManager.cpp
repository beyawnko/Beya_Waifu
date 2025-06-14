/*
    Copyright (C) 2025  beyawnko
*/
#include "GpuManager.h"

QStringList GpuManager::detectGPUs()
{
    m_availableGPUs.clear();
    // In absence of platform specific detection, return a default list.
    m_availableGPUs << "-1: CPU" << "0: Default GPU 0";
    return m_availableGPUs;
}

