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

    My Github homepage: https://github.com/beyawnko
*/

#include "Logger.h"
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include <QLoggingCategory>

QLoggingCategory beyaLogCategory("beya.waifu");

static QFile g_logFile;
static QMutex g_logMutex;
static bool g_verbose = false;

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    if (type == QtDebugMsg && !g_verbose)
        return;
    fprintf(stderr, "%s\n", msg.toLocal8Bit().constData());
    if (g_logFile.isOpen()) {
        QMutexLocker locker(&g_logMutex);
        QTextStream ts(&g_logFile);
        ts << QDateTime::currentDateTime().toString(Qt::ISODate) << " - " << msg << Qt::endl;
    }
}

void initLogger(const QString &filePath, bool verbose)
{
    g_verbose = verbose;
    g_logFile.setFileName(filePath);
    g_logFile.open(QIODevice::Append | QIODevice::Text);
    qInstallMessageHandler(messageHandler);
    const QString rules = QStringLiteral("beya.waifu.debug=%1\n").arg(verbose ? "true" : "false");
    QLoggingCategory::setFilterRules(rules);
}
