// file: RealEsrganProcessor.h
#ifndef REALESRGANPROCESSOR_H
#define REALESRGANPROCESSOR_H

#include <QObject>
#include <QProcess>
#include <QQueue>
#include <QStringList>
#include <QTime> // For temporary file naming
#include "realesrgan_settings.h" // Assuming this file exists or will be created

class RealEsrganProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RealEsrganProcessor(QObject *parent = nullptr);
    ~RealEsrganProcessor();

    // --- Public Methods ---
    void processImage(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings);
    void processVideo(int rowNum, const QString &sourceFile, const QString &destinationFile, const RealEsrganSettings &settings);

signals:
    // --- Standard Signals ---
    void logMessage(const QString &message);
    void processingFinished(int rowNum, bool success);
    void statusChanged(int rowNum, const QString &status);
    void fileProgress(int rowNum, int percent); // Overall progress for the item

private slots:
    // --- Slots for Real-ESRGAN process ---
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    // void onProcessStdErr(); // If RealESRGAN outputs useful error info to stderr separately

    // --- Slots for FFmpeg process (video tasks) ---
    void onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFfmpegError(QProcess::ProcessError error);
    void onFfmpegStdErr();

private:
    // --- State Management ---
    enum class State {
        Idle,
        ProcessingImage,          // Top-level image task OR processing a single video frame
        SplittingVideo,
        ProcessingVideoFrames,    // Meta-state for managing frame-by-frame processing
        AssemblingVideo
    };
    State m_state = State::Idle;
    void cleanup(); // General cleanup for the processor state

    // --- Core Processes ---
    QProcess* m_process = nullptr;       // For the Real-ESRGAN executable
    QProcess* m_ffmpegProcess = nullptr; // For splitting/assembling video

    // --- Job State ---
    int m_currentRowNum = -1;            // Row in the UI table
    RealEsrganSettings m_settings;       // Settings for the current job

    // --- Multi-Pass Scaling State for current image/frame ---
    QQueue<int> m_scaleSequence;        // Sequence of scale factors for multi-pass
    QString m_originalSourceFile;       // Path to the original input image/video
    QString m_finalDestinationFile;     // Path to the final output for the current image/video frame
    QString m_currentPassInputFile;     // Input for the current scaling pass
    QString m_currentPassOutputFile;    // Output for the current scaling pass

    // --- Video Processing State ---
    QString m_video_tempPath;           // Root temp directory for current video job
    QString m_video_inputFramesPath;    // Directory for extracted raw frames
    QString m_video_outputFramesPath;   // Directory for upscaled frames
    QString m_video_audioPath;          // Path to extracted audio
    QQueue<QString> m_video_frameQueue; // Queue of input frame filenames to process
    int m_video_totalFrames = 0;
    int m_video_processedFrames = 0;
    void cleanupVideo(); // Specific cleanup for video temp files and state

    // --- Helper Methods ---
    QStringList buildArguments(const QString &inputFile, const QString &outputFile);
    void startNextPass();
    void startNextVideoFrame();
    void startVideoAssembly();
};

#endif // REALESRGANPROCESSOR_H
