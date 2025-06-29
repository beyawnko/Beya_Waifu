#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QByteArray>

class ProcessExecutor : public QObject
{
    Q_OBJECT
public:
    explicit ProcessExecutor(QObject *parent = nullptr);

signals:
    void commandFinished(const QString& engineName, bool success, const QString& output, QCheckBox* checkBox);

public slots:
    void executeCommand(const QString& engineName, const QString& executablePath, const QStringList& arguments, QCheckBox* checkBox);

private:
    QProcess *m_process;
};
