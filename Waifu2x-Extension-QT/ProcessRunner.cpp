/*
    Copyright (C) 2025  beyawnko
*/
#include "ProcessRunner.h"

bool ProcessRunner::run(QProcess *process, const QString &cmd, QByteArray *stdOut,
                        QByteArray *stdErr)
{
    QEventLoop loop;
    if (stdOut)
        stdOut->clear();
    if (stdErr)
        stdErr->clear();

    if (stdOut)
        QObject::connect(process, &QProcess::readyReadStandardOutput,
                         [&]() { stdOut->append(process->readAllStandardOutput()); });
    if (stdErr)
        QObject::connect(process, &QProcess::readyReadStandardError,
                         [&]() { stdErr->append(process->readAllStandardError()); });

    QObject::connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
                     &loop, &QEventLoop::quit);
    QObject::connect(process, &QProcess::errorOccurred, &loop, &QEventLoop::quit);

    const int timeoutDuration = 600000; // 10 minutes in milliseconds
    bool timedOut = false;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(&timer, &QTimer::timeout, [&]() {
        timedOut = true;
        qWarning() << "Process execution timed out for command:" << cmd;
        if (process->state() == QProcess::Running) {
            process->kill(); // Forcefully kill the process
        }
        loop.quit(); // Ensure the event loop is quit
    });

    process->start(cmd);

    if (!process->waitForStarted(5000)) { // 5-second timeout for process startup
        qWarning() << "Process failed to start or started too slow for command:" << cmd << "Error:" << process->errorString();
        // Ensure timer doesn't fire if we exit early
        timer.stop(); // Though it's not started yet, good practice
        return false;
    }

    timer.start(timeoutDuration);
    loop.exec();
    timer.stop(); // Stop the timer: either loop finished normally or timer triggered loop.quit()

    if (timedOut) {
        return false;
    }

    return process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0;
}

