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

/**
 * @brief Helper for building command arguments for video processing.
 */
class VideoProcessor : public QObject
{
    Q_OBJECT
public:
    explicit VideoProcessor(QObject *parent = nullptr);

    /**
     * @brief Build ffmpeg argument string for output video.
     */
    QString buildOutputArguments(const QString &audioPath,
                                 bool settingsEnabled,
                                 const QString &videoEncoder,
                                 const QString &audioEncoder,
                                 int audioBitrate,
                                 const QString &pixelFormat,
                                 const QString &extraCommand) const;
};


