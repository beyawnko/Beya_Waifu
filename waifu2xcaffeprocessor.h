// file: waifu2xcaffeprocessor.h
#ifndef WAIFU2XCAFFEPROCESSOR_H
#define WAIFU2XCAFFEPROCESSOR_H

#include <QObject>
#include <QProcess>
#include "waifu2xcaffe_settings.h"

class Waifu2xCaffeProcessor : public QObject
{
    Q_OBJECT
public:
    explicit Waifu2xCaffeProcessor(QObject *parent = nullptr);
    ~Waifu2xCaffeProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Waifu2xCaffeSettings &settings);

signals:
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    QStringList buildArguments(const QString &sourceFile, const QString &destinationFile);
    void cleanup();

    QProcess *m_process;
    int m_currentRowNum;
    QString m_destinationFile;
    Waifu2xCaffeSettings m_settings;
};

#endif // WAIFU2XCAFFEPROCESSOR_H
