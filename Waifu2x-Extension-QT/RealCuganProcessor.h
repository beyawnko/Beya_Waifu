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
#include <QStringList>

class MainWindow;

class RealCuganProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RealCuganProcessor(MainWindow *parent);

    void preLoadSettings();
    void readSettings();
    void readSettingsVideoGif(int threadNum);
    QStringList prepareArguments(const QString &inputFile,
                                 const QString &outputFile,
                                 int currentPassScale,
                                 const QString &modelName,
                                 int denoiseLevel,
                                 int tileSize,
                                 const QString &gpuId,
                                 bool ttaEnabled,
                                 const QString &outputFormat,
                                 bool isMultiGPU,
                                 const QString &multiGpuJobArgs);

private:
    MainWindow *m_mainWindow;
};

