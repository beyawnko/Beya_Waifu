// file: realesrganprocessor.h
#ifndef REALESRGANPROCESSOR_H
#define REALESRGANPROCESSOR_H

#include <QObject>
#include <QProcess>
#include <QQueue>
#include "realesrgan_settings.h"

class RealEsrganProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RealEsrganProcessor(QObject *parent = nullptr);
    ~RealEsrganProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings);

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
    void startNextPass();
    QStringList buildArguments(const QString &inputFile, const QString &outputFile);
    void cleanup();

    QProcess *m_process;

    // State for the current job
    int m_currentRowNum;
    QString m_originalSourceFile;
    QString m_finalDestinationFile;
    RealEsrganSettings m_settings;

    // State for iterative scaling passes
    QQueue<int> m_scaleSequence;
    QString m_currentPassInputFile;
    QString m_currentPassOutputFile;
};

#endif // REALESRGANPROCESSOR_H
