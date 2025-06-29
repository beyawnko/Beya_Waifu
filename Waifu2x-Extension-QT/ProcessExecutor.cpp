#include "ProcessExecutor.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QCheckBox>

ProcessExecutor::ProcessExecutor(QObject *parent) : QObject(parent)
{
    m_process = new QProcess(this);
}

void ProcessExecutor::executeCommand(const QString& engineName, const QString& executablePath, const QStringList& arguments, QCheckBox* checkBox)
{
    QByteArray stdOutput;
    QByteArray stdError;

    m_process->setProcessChannelMode(QProcess::MergedChannels);

    QEventLoop loop;
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &loop, &QEventLoop::quit);
    connect(m_process, &QProcess::errorOccurred, &loop, &QEventLoop::quit);
    connect(m_process, &QProcess::readyReadStandardOutput, [&]() { stdOutput.append(m_process->readAllStandardOutput()); });
    connect(m_process, &QProcess::readyReadStandardError, [&]() { stdError.append(m_process->readAllStandardError()); });

    m_process->start(executablePath, arguments);

    bool success = m_process->waitForStarted(5000); // 5 seconds to start
    if (!success) {
        QString errorMsg = tr("%1: Failed to start process. Error: %2").arg(engineName).arg(m_process->errorString());
        emit commandFinished(engineName, false, errorMsg, checkBox);
        return;
    }

    const int timeoutDuration = 15000; // 15 seconds for execution
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutDuration);

    loop.exec(); // This will block until process finishes or timer times out

    if (timer.isActive()) { // Process finished before timeout
        timer.stop();
    } else { // Timeout occurred
        m_process->kill();
        m_process->waitForFinished(1000); // Give it a moment to die
        QString errorMsg = tr("%1: Test timed out or crashed.").arg(engineName);
        emit commandFinished(engineName, false, errorMsg, checkBox);
        return;
    }

    QString output = QString::fromLocal8Bit(stdOutput + stdError).trimmed();
    bool isCompatible = (m_process->exitStatus() == QProcess::NormalExit && m_process->exitCode() == 0);
    QString statusMsg;

    if (isCompatible) {
        statusMsg = tr("%1: Compatible (Exit code 0).").arg(engineName);
    } else {
        statusMsg = tr("%1: Not compatible or error (Exit code %2, Status %3). Output: %4")
                      .arg(engineName)
                      .arg(m_process->exitCode())
                      .arg(m_process->exitStatus())
                      .arg(output);
    }

    emit commandFinished(engineName, isCompatible, statusMsg, checkBox);
}
