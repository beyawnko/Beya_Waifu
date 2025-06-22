// file: realcuganprocessor.h
#ifndef REALCUGANPROCESSOR_H
#define REALCUGANPROCESSOR_H

#include <QObject>
#include <QProcess>
#include <QQueue>
#include "realcugan_settings.h" // Assuming this file exists from a previous refactor

class RealCuganProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RealCuganProcessor(QObject *parent = nullptr);
    ~RealCuganProcessor();

    // --- Public Methods ---
    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealCuganSettings &settings);
    void processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealCuganSettings &settings);

signals:
    // --- Standard Signals ---
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);
    void fileProgress(int rowNum, int percent);

private slots:
    // --- Slots for RealCUGAN process ---
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();

    // --- Slots for FFmpeg process (video tasks) ---
    void onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFfmpegError(QProcess::ProcessError error);
    void onFfmpegStdErr();

private:
    // --- State Management ---
    enum class State {
        Idle,
        ProcessingImage,
        SplittingVideo,
        ProcessingVideoFrames,
        AssemblingVideo
    };
    State m_state = State::Idle;
    void cleanup();

    // --- Core Processes ---
    QProcess* m_process = nullptr;       // For the RealCUGAN executable
    QProcess* m_ffmpegProcess = nullptr; // For splitting/assembling video

    // --- Job State ---
    int m_currentRowNum = -1;
    QString m_finalDestinationFile;
    RealCuganSettings m_settings;

    // --- Video Processing State ---
    QString m_video_tempPath;
    QString m_video_inputFramesPath;
    QString m_video_outputFramesPath;
    QString m_video_audioPath;
    QQueue<QString> m_video_frameQueue;
    int m_video_totalFrames = 0;
    int m_video_processedFrames = 0;
    void cleanupVideo();

    // --- Helper Methods ---
    QStringList buildArguments(const QString &inputFile, const QString &outputFile);
    void startNextVideoFrame();
    void startVideoAssembly();
};

#endif // REALCUGANPROCESSOR_H
