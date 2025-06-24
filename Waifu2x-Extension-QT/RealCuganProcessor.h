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
    void onFfmpegFinished(int exitCode, QProcess::ExitStatus exitStatus); // Used by old video method
    void onFfmpegError(QProcess::ProcessError error); // Used by old video method
    void onFfmpegStdErr(); // Used by old video method for progress

    // --- New slots for piped video processing ---
    // Decoder (FFmpeg)
    void onPipeDecoderReadyReadStandardOutput();
    void onPipeDecoderReadyReadStandardError();
    void onPipeDecoderFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPipeDecoderError(QProcess::ProcessError error);

    // SR Engine (RealCUGAN via pipe)
    // Reusing: onReadyReadStandardOutput for SR engine output
    // Reusing: onProcessFinished for SR engine finish
    // Reusing: onProcessError for SR engine error

    // Encoder (FFmpeg)
    void onPipeEncoderReadyReadStandardError();
    void onPipeEncoderFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPipeEncoderError(QProcess::ProcessError error);


private:
    // --- State Management ---
    enum class State {
        Idle,
        ProcessingImage,
        SplittingVideo,          // Old video method
        ProcessingVideoFrames,   // Old video method
        AssemblingVideo,         // Old video method
        PipeDecodingVideo,       // New video method
        PipeProcessingSR,        // New video method (frame being processed by SR engine)
        PipeEncodingVideo        // New video method
    };
    State m_state = State::Idle;
    void cleanup();
    void cleanupPipeProcesses();


    // --- Core Processes ---
    QProcess* m_process = nullptr;       // For the RealCUGAN executable (image or piped video frame)
    QProcess* m_ffmpegProcess = nullptr; // For splitting/assembling video (old method)

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

    QString m_tempVideoJobPath; // Path to the unique temporary directory for a video job (contains audio, etc.)
    QString m_tempAudioPath;    // Path to extracted audio file (inside m_tempVideoJobPath)

    int m_framesProcessedPipe = 0;
    int m_totalFramesEstimatePipe = 0; // From ffprobe initially
    bool m_allFramesDecoded = false;
    bool m_allFramesSentToEncoder = false;


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
    QStringList buildArguments(const QString &inputFile, const QString &outputFile); // For image processing and old video frame processing
    void startNextVideoFrame(); // Old video method
    void startVideoAssembly();  // Old video method

    // --- New helper methods for piped video processing ---
    bool getVideoInfo(const QString& inputFile); // Uses ffprobe
    void startPipeDecoder();
    void processDecodedFrameBuffer();
    void startRealCuganPipe(const QByteArray& frameData);
    void processSROutputBuffer();
    void startPipeEncoder();
    void pipeFrameToEncoder();
    void finalizePipedVideoProcessing(bool success);

    // Constants for buffer limits
    static const qint64 MAX_DECODED_BUFFER_SIZE = 100 * 1024 * 1024; // 100MB limit for decoded frames buffer
    static const qint64 MAX_UPSCALED_BUFFER_SIZE = 100 * 1024 * 1024; // 100MB limit for upscaled frames buffer
};

#endif // REALCUGANPROCESSOR_H
