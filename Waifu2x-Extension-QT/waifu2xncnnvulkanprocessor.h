// file: waifu2xncnnvulkanprocessor.h
#ifndef WAIFU2XNCNNVULKANPROCESSOR_H
#define WAIFU2XNCNNVULKANPROCESSOR_H

#include <QObject>
#include <QProcess>
#include "waifu2xncnnvulkan_settings.h"

class Waifu2xNcnnVulkanProcessor : public QObject
{
    Q_OBJECT
public:
    explicit Waifu2xNcnnVulkanProcessor(QObject *parent = nullptr);
    ~Waifu2xNcnnVulkanProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Waifu2xNcnnVulkanSettings &settings);

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
    QStringList buildArguments(const QString &inputFile, const QString &outputFile, int currentPassScale);
    void cleanup();

    QProcess *m_process;

    // State for the current asynchronous, iterative job
    int m_currentRowNum;
    QString m_originalSourceFile;
    QString m_finalDestinationFile;
    Waifu2xNcnnVulkanSettings m_settings;
    QList<int> m_scaleSequence;
    QString m_currentPassInputFile;
    QString m_currentPassOutputFile;
};

#endif // WAIFU2XNCNNVULKANPROCESSOR_H
