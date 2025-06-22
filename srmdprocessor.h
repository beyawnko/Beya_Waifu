// file: srmdprocessor.h
#ifndef SRMDPROCESSOR_H
#define SRMDPROCESSOR_H

#include <QObject>
#include <QProcess>
#include "srmd_settings.h"

class SrmdProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SrmdProcessor(QObject *parent = nullptr);
    ~SrmdProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const SrmdSettings &settings);

signals:
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);
    void fileProgress(int rowNum, int percent);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();

private:
    QStringList buildArguments();
    void cleanup();

    QProcess *m_process;

    // State for the current asynchronous job
    int m_currentRowNum;
    QString m_destinationFile;
    SrmdSettings m_settings;
};

#endif // SRMDPROCESSOR_H
