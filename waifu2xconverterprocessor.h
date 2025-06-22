// file: waifu2xconverterprocessor.h
#ifndef WAIFU2XCONVERTERPROCESSOR_H
#define WAIFU2XCONVERTERPROCESSOR_H

#include <QObject>
#include <QProcess>
#include "waifu2xconverter_settings.h"

class Waifu2xConverterProcessor : public QObject
{
    Q_OBJECT
public:
    explicit Waifu2xConverterProcessor(QObject *parent = nullptr);
    ~Waifu2xConverterProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const Waifu2xConverterSettings &settings);

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
    QStringList buildArguments(const QString &sourceFile, const QString &destinationFile);
    void cleanup();

    QProcess *m_process;
    int m_currentRowNum;
    QString m_destinationFile;
    Waifu2xConverterSettings m_settings;
};

#endif // WAIFU2XCONVERTERPROCESSOR_H
