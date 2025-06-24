// file: realesrganprocessor.h
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
    void onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus); // Old file-based method
    void onFfmpegError(QProcess::ProcessError error); // Old file-based method
    void onFfmpegStdErr(); // Old file-based method

    // --- New slots for piped video processing ---
    // Decoder (FFmpeg)
    void onPipeDecoderReadyReadStandardOutput();
    void onPipeDecoderReadyReadStandardError();
    void onPipeDecoderFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPipeDecoderError(QProcess::ProcessError error);

    // SR Engine (RealESRGAN via pipe) - Reusing existing m_process slots:
    // onReadyReadStandardOutput (will need to check state)
    // onProcessFinished (will need to check state)
    // onProcessError (will need to check state)

    // Encoder (FFmpeg)
    void onPipeEncoderReadyReadStandardError();
    void onPipeEncoderFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPipeEncoderError(QProcess::ProcessError error);

private:
    // --- State Management ---
    enum class State {
        Idle,
        ProcessingImage,          // Top-level image task OR processing a single video frame (old)
        SplittingVideo,           // Old file-based video method
        ProcessingVideoFrames,    // Old file-based video method (meta-state)
        AssemblingVideo,          // Old file-based video method
        PipeDecodingVideo,        // New video method
        PipeProcessingSR,         // New video method (frame being processed by SR engine)
        PipeEncodingVideo         // New video method
    };
    State m_state = State::Idle;
    void cleanup(); // General cleanup for the processor state
    void cleanupPipeProcesses(); // Cleanup for new pipe-related processes and temp files

    // --- Core Processes ---
    QProcess* m_process = nullptr;       // For the Real-ESRGAN executable (image or piped video frame)
    QProcess* m_ffmpegProcess = nullptr; // For splitting/assembling video (old file-based method)

    // --- New members for piped video processing ---
    QProcess* m_ffmpegDecoderProcess = nullptr;
    QProcess* m_ffmpegEncoderProcess = nullptr;
    // m_process will be reused for SR engine in pipe mode.

    QByteArray m_currentDecodedFrameBuffer;
    QByteArray m_currentUpscaledFrameBuffer;

    QSize m_inputFrameSize;
    int m_inputFrameChannels = 0;
    QString m_inputPixelFormat = "bgr24"; // For FFmpeg rawvideo output and SR engine input

    QSize m_outputFrameSize; // After scaling by SR engine

    QString m_tempAudioPathPipe; // Path to extracted audio file for pipe method (to distinguish from m_video_audioPath)

    int m_framesProcessedPipe = 0;
    int m_totalFramesEstimatePipe = 0;
    bool m_allFramesDecoded = false;
    bool m_allFramesSentToEncoder = false;

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
    QStringList buildArguments(const QString &inputFile, const QString &outputFile); // For image & old video frame method
    void startNextPass(); // For multi-pass image processing
    void startNextVideoFrame(); // Old file-based video method
    void startVideoAssembly();  // Old file-based video method

    // --- New helper methods for piped video processing ---
    bool getVideoInfoPipe(const QString& inputFile); // Uses ffprobe, suffixed to avoid name clash if an old getVideoInfo exists
    void startPipeDecoder();
    void processDecodedFrameBuffer();
    void startRealEsrganPipe(const QByteArray& frameData);
    // void processSROutputBuffer(); // SR output directly handled by onReadyReadStandardOutput of m_process
    void startPipeEncoder();
    void pipeFrameToEncoder(const QByteArray& upscaledFrameData); // Common method, could be base class
    void finalizePipedVideoProcessing(bool success); // Common method, could be base class
};

#endif // REALESRGANPROCESSOR_H
