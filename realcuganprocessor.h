// file: realcuganprocessor.h
#ifndef REALCUGANPROCESSOR_H
#define REALCUGANPROCESSOR_H

#include <QObject>
#include <QProcess>
#include "realcugan_settings.h"

class RealCuganProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RealCuganProcessor(QObject *parent = nullptr);
    ~RealCuganProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealCuganSettings &settings);
    // Video/GIF processing can be added as stubs for a future refactor
    void processVideo(int rowNum);

signals:
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);
    void fileProgress(int rowNum, int percent); // For progress updates during scaling

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();

private:
    void startNextPass();
    QStringList buildArguments(const QString &inputFile, const QString &outputFile, int currentPassScale);
    void cleanup();

    QProcess *m_process;

    // --- State for the current asynchronous, iterative job ---
    int m_currentRowNum;
    QString m_originalSourceFile;
    QString m_finalDestinationFile;
    RealCuganSettings m_settings;

    // --- State for iterative scaling passes ---
    int m_targetScale;
    int m_currentScale;
    QString m_currentPassInputFile;
    QString m_currentPassOutputFile;
    QList<int> m_scaleSequence;
};

#endif // REALCUGANPROCESSOR_H
