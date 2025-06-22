// file: realesrganprocessor.h
#ifndef REALESRGANPROCESSOR_H
#define REALESRGANPROCESSOR_H

#include <QObject>
#include <QProcess>
#include <QQueue>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "realesrgan_settings.h"

class RealEsrganProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RealEsrganProcessor(QObject *parent = nullptr);
    ~RealEsrganProcessor();

    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings);
    void processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings);

signals:
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);
    void fileProgress(int rowNum, int percent);

private:
    enum class State {
        Idle,
        ProcessingImage,
        SplittingVideo,
        ProcessingVideoFrames,
        AssemblingVideo
    };

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput(); // Assuming this is for m_process, might need a new one for m_ffmpegProcess std err
    void onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFfmpegError(QProcess::ProcessError error);
    void onFfmpegStdErr();


private:
    void startNextPass();
    QStringList buildArguments(const QString &inputFile, const QString &outputFile);
    void cleanup(); // General cleanup for m_process related things
    void startNextVideoFrame();
    void startVideoAssembly();
    void cleanupVideo(); // Cleanup for video specific resources

    QProcess *m_process = nullptr; // For RealESRGAN executable
    QProcess *m_ffmpegProcess = nullptr; // For FFmpeg operations

    // State for the current job
    int m_currentRowNum;
    QString m_originalSourceFile; // Original source, could be image or video
    QString m_finalDestinationFile; // Final output file path
    RealEsrganSettings m_settings;
    State m_state = State::Idle;

    // State for iterative scaling passes (used for both images and video frames)
    QQueue<int> m_scaleSequence;
    QString m_currentPassInputFile;
    QString m_currentPassOutputFile;

    // Video processing members
    QString m_video_tempPath;
    QString m_video_inputFramesPath;
    QString m_video_outputFramesPath;
    QString m_video_audioPath;
    QQueue<QString> m_video_frameQueue;
    int m_video_totalFrames = 0;
    int m_video_processedFrames = 0;
};

#endif // REALESRGANPROCESSOR_H
