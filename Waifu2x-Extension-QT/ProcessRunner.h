/*
    Copyright (C) 2025  beyawnko
*/
#pragma once

#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QEventLoop>

/**
 * @brief Launches external processes and collects output.
 */
class ProcessRunner
{
public:
    ProcessRunner() = default;
    bool run(QProcess *process, const QString &cmd, QByteArray *stdOut = nullptr,
             QByteArray *stdErr = nullptr);
};

