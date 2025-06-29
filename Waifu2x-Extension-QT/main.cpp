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

#include "mainwindow.h"
#include "Logger.h"

// RHI_ENABLED will be defined by CMake if RHI is to be compiled
#ifdef RHI_ENABLED
#include "RhiLiquidGlassItem.h"
#endif

#include <QApplication>
// For QT_VERSION_CHECK if needed elsewhere, though RHI logic now uses RHI_ENABLED
#include <QtGlobal>
#include <QQmlApplicationEngine> // For QML
#include <QtQml> // For qmlRegisterType
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStyleFactory>
#include <QDir>

int main(int argc, char *argv[])
{
    // Early file log for debugging purposes
    QFile debugFile("debug_trace.log");
    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : Application main() started. Waifu2x-Extension-QT. Debugging file list population.\n";
        debugFile.close();
    }

    QApplication a(argc, argv);
    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : QApplication object created.\n";
        debugFile.close();
    }
    QStyle *style = QStyleFactory::create("macos");
    if (!style)
        style = QStyleFactory::create("Fusion");
    if (style)
        a.setStyle(style);

    // Load custom stylesheet
    QFile qssFile(":/new/prefix1/styles.qss"); // Updated path based on icon.qrc structure
    if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&qssFile);
        a.setStyleSheet(stream.readAll());
        qssFile.close();
        if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&debugFile);
            out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
                << " : Custom stylesheet loaded from " << qssFile.fileName() << ".\n";
            debugFile.close();
        }
    } else {
        // Fallback for development if resource not compiled yet, try loading from parent of app dir
        QDir appDir(QCoreApplication::applicationDirPath());
        appDir.cdUp(); // Go to the directory containing styles.qss (repo root)
        QString directPath = appDir.filePath("styles.qss");
        qssFile.setFileName(directPath);
        if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&qssFile);
            a.setStyleSheet(stream.readAll());
            qssFile.close();
            if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&debugFile);
                out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
                    << " : Custom stylesheet loaded from (fallback) " << qssFile.fileName() << ".\n";
                debugFile.close();
            }
        } else {
            if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&debugFile);
                out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
                    << " : Failed to load custom stylesheet. Searched: :/new/prefix1/styles.qss and "
                    << directPath <<".\n";
                debugFile.close();
            }
        }
    }

    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : Style created and set.\n";
        debugFile.close();
    }

    QDir logDir(QCoreApplication::applicationDirPath() + "/logs");
    if (!logDir.exists())
        logDir.mkpath(".");

    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : Log directory created.\n";
        debugFile.close();
    }

    QCommandLineParser parser;
    parser.setApplicationDescription("Beya_Waifu");
    parser.addHelpOption();
    QCommandLineOption verboseOpt(QStringLiteral("verbose"),
                                  QStringLiteral("Enable verbose debug logging"));
    parser.addOption(verboseOpt);
    QCommandLineOption maxThreadsOpt(QStringLiteral("max-threads"),
                                     QStringLiteral("Override global thread limit"),
                                     QStringLiteral("count"));
    parser.addOption(maxThreadsOpt);
    parser.process(a);
    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : Command line parser processed.\n";
        debugFile.close();
    }
    bool verbose = parser.isSet(verboseOpt);
    initLogger(logDir.filePath("waifu.log"), verbose);
    int overrideThreads = 0;
    if(parser.isSet(maxThreadsOpt))
        overrideThreads = parser.value(maxThreadsOpt).toInt();

    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : Logger initialized.\n";
        debugFile.close();
    }

    // Register RhiLiquidGlassItem for QML
    // Usage: import com.waifu2x.effects 1.0
    //        RhiLiquidGlass { ... }
#ifdef RHI_ENABLED
    qmlRegisterType<RhiLiquidGlassItem>("com.waifu2x.effects", 1, 0, "RhiLiquidGlass");
#endif

    // a.setQuitOnLastWindowClosed(true); // For testing, maybe don't quit on last window close if only QML test window
    if (debugFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&debugFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
            << " : Creating main window.\n";
        debugFile.close();
    }
    MainWindow w(overrideThreads); // Create and show main window
    w.show();

    // --- Load the test QML file directly for RhiLiquidGlassItem ---
    // QQmlApplicationEngine engine;
    // engine.load(QUrl(QStringLiteral("qrc:/TestRhiLiquidGlass.qml"))); // Path from test_assets.qrc
    // if (engine.rootObjects().isEmpty()) {
    //     qWarning("Failed to load TestRhiLiquidGlass.qml");
    //     // return -1; // Allow to continue without this for now
    // }
    // --- End Test ---


    return a.exec();
}