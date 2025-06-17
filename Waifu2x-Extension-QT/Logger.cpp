#include "Logger.h"
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

static QFile g_logFile;
static QMutex g_logMutex;

static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    fprintf(stderr, "%s\n", msg.toLocal8Bit().constData());
    if (g_logFile.isOpen()) {
        QMutexLocker locker(&g_logMutex);
        QTextStream ts(&g_logFile);
        ts << QDateTime::currentDateTime().toString(Qt::ISODate) << " - " << msg << Qt::endl;
    }
}

void initLogger(const QString &filePath)
{
    g_logFile.setFileName(filePath);
    g_logFile.open(QIODevice::Append | QIODevice::Text);
    qInstallMessageHandler(messageHandler);
}
