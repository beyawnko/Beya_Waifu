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

    process->start(cmd);
    loop.exec();

    return process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0;
}

