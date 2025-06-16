/*
    Copyright (C) 2025  beyawnko
*/
#include "GpuManager.h"
#include <QDebug> // For stub implementation

QStringList GpuManager::detectGPUs(const QString &engine)
{
    Q_UNUSED(engine); // For stub implementation
    m_availableGPUs.clear();
    // In absence of platform specific detection, return a default list.
    // This can be expanded later with engine-specific logic.
    qDebug() << "GpuManager::detectGPUs called for engine:" << engine << "(stub)";
    m_availableGPUs << "-1: CPU" << "0: Default GPU 0"; // Example default
    return m_availableGPUs;
}

bool GpuManager::isCompatible(const QString &engine)
{
    Q_UNUSED(engine); // For stub implementation
    // Stub: assume all engines are compatible for now.
    // This can be expanded later.
    qDebug() << "GpuManager::isCompatible called for engine:" << engine << "(stub - returning true)";
    return true;
}
