#pragma once
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

#include <QObject>
#include <QString>
#include <QStringList>
#include "mainwindow.h"

/**
 * @brief Manage GPU job configuration for RealESRGAN tasks.
 */
class RealesrganJobManager : public QObject
{
    Q_OBJECT
public:
    explicit RealesrganJobManager(QObject *parent = nullptr);

    /**
     * @brief Build GPU job string used by the RealESRGAN binary.
     */
    QString buildGpuJobString(bool multiGpuEnabled,
                              const QList_QMap_QStrQStr &gpuList,
                              const QString &fallbackGpuId) const;
};


