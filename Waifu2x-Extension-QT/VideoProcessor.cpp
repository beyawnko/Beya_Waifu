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

#include "VideoProcessor.h"
#include <QFile>

VideoProcessor::VideoProcessor(QObject *parent)
    : QObject(parent)
{}

QString VideoProcessor::buildOutputArguments(const QString &audioPath,
                                            bool settingsEnabled,
                                            const QString &videoEncoder,
                                            const QString &audioEncoder,
                                            int audioBitrate,
                                            const QString &pixelFormat,
                                            const QString &extraCommand) const
{
    QString output;
    if (settingsEnabled) {
        if (!videoEncoder.trimmed().isEmpty()) {
            output += "-c:v " + videoEncoder.trimmed() + " ";
        }
        if (QFile::exists(audioPath)) {
            if (!audioEncoder.trimmed().isEmpty()) {
                output += "-c:a " + audioEncoder.trimmed() + " ";
            }
            if (audioBitrate > 0) {
                output += "-b:a " + QString::number(audioBitrate) + "k ";
            }
        }
        if (!pixelFormat.trimmed().isEmpty()) {
            output += "-pix_fmt " + pixelFormat.trimmed() + " ";
        } else {
            output += "-pix_fmt yuv420p ";
        }
        if (!extraCommand.trimmed().isEmpty()) {
            output += extraCommand.trimmed() + " ";
        }
    } else {
        output += "-pix_fmt yuv420p ";
    }
    return output;
}


