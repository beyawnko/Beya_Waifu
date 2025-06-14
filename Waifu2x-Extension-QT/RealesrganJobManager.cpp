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

#include "RealesrganJobManager.h"

RealesrganJobManager::RealesrganJobManager(QObject *parent)
    : QObject(parent)
{}

QString RealesrganJobManager::buildGpuJobString(bool multiGpuEnabled,
                                               const QList_QMap_QStrQStr &gpuList,
                                               const QString &fallbackGpuId) const
{
    if (multiGpuEnabled) {
        if (!gpuList.isEmpty()) {
            QStringList ids;
            QStringList jobs;
            for (const auto &gpu : gpuList) {
                ids << gpu.value("ID");
                QString threads = gpu.value("Threads", "1");
                jobs << QString("1:%1:1").arg(threads);
            }
            return QString("-g %1 -j %2").arg(ids.join(','), jobs.join(','));
        }
        if (fallbackGpuId == "-1") {
            return "-g -1";
        }
        return QString("-g %1").arg(fallbackGpuId);
    }
    if (fallbackGpuId == "-1") {
        return "-g -1";
    }
    return QString("-g %1").arg(fallbackGpuId);
}


