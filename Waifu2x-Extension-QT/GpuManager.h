#pragma once
/*
    Copyright (C) 2025  beyawnko
*/
#include <QStringList>

/**
 * @brief Provides GPU enumeration utilities.
 */
class GpuManager
{
public:
    GpuManager() = default;

    /**
     * @brief Detect available GPUs.
     * @return A list of GPU description strings.
     */
    QStringList detectGPUs();

    /**
     * @brief Get the cached GPU list after detection.
     */
    const QStringList &availableGPUs() const { return m_availableGPUs; }

private:
    QStringList m_availableGPUs;
};

