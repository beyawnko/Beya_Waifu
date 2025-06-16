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
     * @brief Detect available GPUs for a specific engine.
     * @param engine Name of the engine (e.g., "realcugan").
     * @return A list of GPU description strings.
     */
    QStringList detectGPUs(const QString &engine);

    /**
     * @brief Check if a specific engine is considered compatible.
     * @param engine Name of the engine.
     * @return True if compatible, false otherwise.
     */
    bool isCompatible(const QString &engine);

    /**
     * @brief Get the cached GPU list after detection.
     */
    const QStringList &availableGPUs() const { return m_availableGPUs; }

private:
    QStringList m_availableGPUs;
};

