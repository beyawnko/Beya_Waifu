# Recreating Waifu2x-Extension-QT: Feature List and Coding Architecture

This document outlines the features and coding architecture of the Waifu2x-Extension-QT project, providing a basis for understanding how it could be recreated from scratch. The application is a sophisticated GUI tool for upscaling images, videos, and animated GIFs using a variety of external command-line programs, primarily those based on neural network algorithms.

## Section 1: Feature List

### 1.1. Core Functionality
    *   **Image Upscaling:**
        *   Supports processing of common image formats (e.g., PNG, JPG, WEBP, BMP).
        *   Includes robust alpha channel preservation and separate processing if needed.
    *   **Video Upscaling:**
        *   Supports common video container formats (e.g., MP4, MKV, MOV, AVI).
        *   Involves a pipeline of frame extraction, individual frame upscaling, and re-encoding with audio.
        *   Preserves audio tracks, with options for re-encoding or direct copying.
        *   Attempts to handle Variable Frame Rate (VFR) videos, often by converting to Constant Frame Rate (CFR).
    *   **GIF/Animated PNG (APNG) Upscaling:**
        *   Splits animated files into sequences of frames.
        *   Upscales individual frames.
        *   Reassembles frames into an animated GIF or APNG, maintaining animation properties.
    *   **Batch Processing:**
        *   Allows users to add multiple files (images, videos, GIFs) to a processing queue.
        *   Files are processed sequentially, but individual upscaling operations (especially for video frames) can leverage multi-core CPUs via CLI tool's internal threading or by running multiple CLI tool instances up to a configurable global thread limit.
    *   **Multiple Upscaling Engine Support:**
        *   **Real-ESRGAN (NCNN Vulkan):** For general-purpose image and video upscaling.
        *   **Real-CUGAN (NCNN Vulkan):** Particularly for anime-style content, includes denoising.
        *   **Anime4K (CPP):** Fast upscaler for anime, supports CPU and GPU (OpenCL/CUDA) modes, extensive filter options.
        *   **Waifu2x (NCNN Vulkan):** Various models for anime/manga style content, includes denoising. Supports different NCNN Vulkan implementations (old/new).
        *   **Waifu2x (Converter):** Another variant of Waifu2x.
        *   **Waifu2x (Caffe):** Supports CPU, GPU (CUDA), and cuDNN acceleration for Waifu2x.
        *   **SRMD (NCNN Vulkan, CUDA):** Super-Resolution for Multiple Degradations.
    *   **Frame Interpolation (VFI):**
        *   Increases video frame rates for smoother motion.
        *   Supports engines like RIFE, CAIN, DAIN (NCNN Vulkan implementations).
        *   Users can set the desired multiplication factor for FPS.
    *   **Denoising:**
        *   Integrated with several upscaling engines (e.g., Real-CUGAN denoise levels, Waifu2x denoise options).
        *   Separate audio denoising capability for video files (likely using SoX or FFmpeg filters).

### 1.2. User Interface & User Experience (UI/UX)
    *   **Cross-Platform GUI:** Built with Qt, ensuring functionality on Windows and Linux.
    *   **File Input:**
        *   Drag and drop support for files and folders onto the main window.
        *   Standard file browser dialog for selecting inputs.
        *   Option for recursive scanning of selected folders to add all compatible files.
    *   **File List Management:**
        *   Separate tabs or views for image, video, and GIF processing queues.
        *   Table views display file name, full path, processing status, original/output resolution, and other relevant metadata.
        *   Context menus on file list items for actions like "Open File," "Open Containing Folder," "Remove from List," "Apply/Cancel Custom Resolution."
    *   **Processing Controls:**
        *   Prominent "Start," "Stop" buttons for managing the processing queue.
        *   Pause functionality (often integrated with system tray controls).
        *   "Force Retry" option for files that failed processing.
    *   **Output Configuration:**
        *   User can specify a global output directory or have outputs next to input files.
        *   Option to replace original files (with a sub-option to move originals to Recycle Bin/Trash).
        *   Option to delete original files after successful processing (with a sub-option to move to Recycle Bin/Trash).
        *   Configurable suffix for output file names.
        *   Selection of output image format (PNG, JPG, WEBP) and quality settings (e.g., JPG compression).
        *   Advanced video output settings: choice of video encoder, audio encoder, audio bitrate, pixel format, and a field for custom FFmpeg commands.
    *   **Engine Settings Panels:**
        *   UI sections (often tabs or group boxes) dynamically display relevant settings based on the currently selected upscaling or VFI engine for images, videos, or GIFs.
        *   Parameters include model selection, scale factor, denoise level, tile size, TTA (Test-Time Augmentation) mode, etc.
    *   **Progress Display:**
        *   An overall progress bar indicating the status of the entire batch.
        *   An individual progress bar for the currently processing file.
        *   A detailed text browser/log view showing messages from the application and stdout/stderr from the backend CLI tools.
        *   ETA (Estimated Time Remaining) for the current file and/or the entire batch.
        *   Display of time elapsed for processing.
    *   **Custom Resolution:**
        *   Allows users to specify an exact target width and/or height for the output.
        *   Options to maintain aspect ratio (e.g., Ignore, Keep, Keep by Expanding/Cropping).
    *   **GPU Management UI:**
        *   Dropdown or list to select the GPU ID for processing for engines that support it.
        *   UI for configuring multi-GPU processing: selecting multiple GPU IDs, assigning job parameters (threads, tiles) per GPU.
        *   A "Detect GPU" button to populate the list of available GPUs.
    *   **Theme/Style:**
        *   Support for a dark mode/theme, with options for manual toggling or following system preference.
        *   Ability to change the font size for the text browser log.
    *   **System Tray Integration:**
        *   Option to minimize the application to the system tray.
        *   Control application functions (Start, Pause, Restore, Quit) from the tray icon's context menu.
        *   Option for "Background Mode" where the main window might be hidden while processing continues.
    *   **Localization:**
        *   Interface translated into multiple languages (English, Chinese, Traditional Chinese confirmed).
    *   **Notifications:**
        *   Audible sound notification upon completion of the processing queue.
        *   System tray pop-up notifications for key events (e.g., task completion, errors).
    *   **Other UI Features:**
        *   "Hide Settings Panel" and "Hide Text Browser" toggles to maximize file list visibility.
        *   Quick links to project Wiki, ReadMe, issue reporting page, and donation/support pages.
        *   Display of a "Top Supporters List."
        *   A built-in compatibility test module to check if backend engines are correctly configured and operational.
        *   Manual and automatic "Check for Updates" functionality.
        *   Option to "Turn off screen when idle" during processing.

### 1.3. File Handling & Management
    *   **Input File Validation:** Basic checks to ensure dropped/selected files are of supported types.
    *   **Metadata Reading:**
        *   Utilizes `ffprobe` (part of FFmpeg) and potentially `ImageMagick`'s `identify` to read media metadata like resolution, FPS, duration, bitrate, frame count, and identify animated images.
        *   Implements a cache for frequently accessed metadata to improve performance.
    *   **Temporary File Management:**
        *   Systematic creation of temporary directories (e.g., in `Current_Path + "/temp"`) for storing intermediate files like video frames during processing.
        *   Reliable cleanup of these temporary files and directories after processing is complete or on error.
    *   **Output File Naming Conventions:** Generates output file names by appending suffixes or using user-defined patterns, ensuring no overwrites unless intended.
    *   **Deduplication of Input Files:** Prevents adding the exact same file (by full path) multiple times to the processing queue.
    *   **Directory Operations:** Includes creating output directories if they don't exist and checking for write permissions before starting tasks.
    *   **Move to Trash/Recycle Bin:** Platform-specific implementation for safely moving files instead of permanent deletion.

### 1.4. Video Processing Specifics
    *   **Frame Splitting:** Uses FFmpeg commands to accurately extract all frames from a video into a sequence of image files (typically PNGs for quality) in a temporary folder.
    *   **Audio Extraction:** Uses FFmpeg to demux the audio stream from the source video into a separate temporary audio file.
    *   **Frame Reassembly:** After individual frames are upscaled, FFmpeg is used to combine the sequence of processed image frames and the (potentially modified) audio track into a new video file, respecting settings like output encoder and pixel format.
    *   **Segment-Based Processing:** For very long videos or to manage system resources, offers an option to split the video into time-based segments, process each segment (frames and audio), and then concatenate the processed segments.
    *   **Video Configuration Management:** Allows saving and loading of complex video processing settings (e.g., codec choices, bitrate, custom FFmpeg commands).
    *   **Bitrate Calculation/Suggestion:** May offer automatic suggestions for video bitrate based on output resolution to maintain quality.

### 1.5. Configuration & Settings
    *   **Persistent Settings:**
        *   Application settings (window size/position, last used paths, default engine choices, UI states) are saved persistently, likely using `QSettings` (to an INI file on Windows or platform-appropriate location).
        *   Auto-save settings option to persist changes automatically.
        *   A "Reset Settings" button to revert all configurations to their default values.
    *   **Engine-Specific Settings Persistence:** Parameters for each upscaling and VFI engine are saved and reloaded with the application.
    *   **Global Thread Limit Configuration:** Users can specify the maximum number of concurrent processing threads (often mapping to files being processed or concurrent CLI tool instances for video frames).
    *   **Command-line Options:**
        *   `--verbose`: Enables more detailed debug logging to the console and/or log file.
        *   `--max-threads`: Allows overriding the global thread limit from the command line.

### 1.6. Advanced & Technical Features
    *   **Multi-threading/Concurrency:**
        *   `QThreadPool` is used to manage a pool of worker threads for concurrent file processing tasks, typically launched via `QtConcurrent::run`.
        *   `QProcess` is used for running all external CLI tools asynchronously, preventing the GUI from freezing.
    *   **External CLI Tool Integration:** The core of the application involves robustly wrapping and managing various command-line executables, handling their input/output streams, and monitoring their execution.
    *   **GPU Acceleration Leverage:** Relies on the GPU capabilities of the backend engines (NCNN Vulkan, CUDA versions). The application provides UI to configure GPU usage.
    *   **Engine Compatibility Testing Framework:** A module that executes each backend tool with simple test parameters to verify its operational status and compatibility with the current system environment (e.g., GPU drivers, libraries).
    *   **Logging System:**
        *   Comprehensive logging to a file (e.g., `waifu.log` in a `logs` subdirectory).
        *   Timestamped messages, including application events and output from CLI tools.
        *   Verbose logging option for debugging.
    *   **Shader Pre-Compilation:** The `LiquidGlassWidget` uses GLSL shaders. The build process includes a step to compile these `.frag` shaders into `.frag.qsb` (Qt Shader Binary) files using Qt's `qsb` tool.
    *   **Cross-Platform Build System:**
        *   Utilizes `qmake` for managing Qt project files (`.pro`).
        *   Employs `CMake` for building the C++ based CLI upscaling tools (Real-ESRGAN, Real-CUGAN) from their submodules.
        *   Shell scripts (`build_projects.sh`) and PowerShell (`build_windows.ps1`) automate the entire build process, including dependency fetching, CLI tool compilation, and Qt application compilation for Linux and Windows.
    *   **CI/CD (Continuous Integration/Continuous Deployment):**
        *   GitHub Actions workflows are defined for automated builds, static analysis, and testing on pushes and pull requests for both Ubuntu and Windows environments.
    *   **Static Analysis:** `cppcheck` is integrated into the build process and CI pipeline to identify potential issues in the C++ codebase.
    *   **Unit/Integration Testing Framework:**
        *   A Python-based test suite using `pytest`.
        *   `PySide6` is used for testing GUI components and interactions.
        *   Tests cover various aspects like binary execution, default settings, GPU detection, UI loading, and core functionalities.

### 1.7. Launcher Application Features
    *   **Splash Screen Display:** Shows a branded image (`NuTitleImageDark.png`) while the main application is loading.
    *   **Single Instance Check:** Ensures that only one instance of the main `Beya_Waifu.exe` application can run at a time. If an instance is already running, the launcher informs the user and exits.
    *   **Administrator Privileges Elevation (Windows):**
        *   Checks if the main application's directory is writable.
        *   If not (suggesting restricted permissions), it attempts to relaunch the main application with administrator privileges using `ShellExecuteW` with the `runas` verb.

## Section 2: Coding Architecture Analysis

### 2.1. Overview
Waifu2x-Extension-QT is a C++/Qt desktop application that acts as a sophisticated front-end for a suite of command-line upscaling and video processing tools. Its architecture emphasizes modularity in integrating these external tools, providing a unified user experience for complex media processing tasks. The application is event-driven, leveraging Qt's signals and slots mechanism for communication between components, and uses asynchronous operations extensively to keep the GUI responsive during long-running processing tasks.

### 2.2. Key Architectural Components

*   **A. GUI Layer (Qt-based):**
    *   **Role:** Manages all user interaction, displays application state, and provides controls for configuring and initiating processing tasks.
    *   **Key Classes:**
        *   `MainWindow` (`mainwindow.h/.cpp`): The primary QMainWindow subclass. Orchestrates almost all application logic, including UI event handling, file list management, settings persistence, initiation of processing tasks, and communication with backend processors.
        *   UI Forms (`.ui` files): XML files defining the structure and static properties of UI elements, loaded dynamically by Qt. `mainwindow.ui` is central.
        *   `UiController` (`UiController.h/.cpp`): A helper class responsible for UI-specific logic like theme application (dark mode), dynamic font changes, and managing the visibility of engine-specific settings panels within `MainWindow`.
        *   Dialogs: Various custom and standard Qt dialogs for specific functions (e.g., `CustomResolution.cpp`, `Donate.cpp`, `TopSupportersList`).
    *   **Interactions:** Captures user inputs (mouse clicks, keyboard, drag-drop). Modifies its internal state and triggers backend operations. Updates views based on data models (`QStandardItemModel` for file lists) and signals from other components.

*   **B. Processing Core (CLI Tool Wrappers):**
    *   **Role:** Provides C++ abstractions for each external command-line tool, managing their execution and communication.
    *   **Key Classes:**
        *   `RealCuganProcessor`, `RealEsrganProcessor`, `Anime4KProcessor`, (and potentially `Waifu2xConverterProcessor`, `Waifu2xCaffeProcessor`, `SrmdProcessor`): Each class is dedicated to a specific upscaling engine.
    *   **Functionality:**
        *   Constructs command-line arguments based on passed-in settings objects (e.g., `RealCuganSettings`).
        *   Manages a `QProcess` instance for launching and monitoring the respective CLI tool.
        *   Connects to `QProcess` signals (`finished`, `errorOccurred`, `readyReadStandardOutput`, `readyReadStandardError`) to handle process lifecycle and I/O.
        *   Parses stdout/stderr for progress information and log messages.
        *   Emits signals (`logMessage`, `processingFinished`, `statusChanged`, `fileProgress`) to `MainWindow` to update the UI and application state.
        *   For video tasks, these processors also manage calls to FFmpeg for frame splitting and reassembly.

*   **C. Video Processing Pipeline:**
    *   **Role:** Encapsulates the multi-stage process of upscaling video files. This logic is primarily distributed within the video-capable Processor classes (e.g., `RealEsrganProcessor`).
    *   **Key Functionality:**
        1.  **Splitting:** Use FFmpeg (`QProcess`) to extract individual frames from the source video into a temporary directory and demux the audio track.
        2.  **Frame Processing:** Iterate through the extracted frames, invoking the selected image upscaling logic (e.g., `RealEsrganProcessor::processImage` for each frame).
        3.  **Assembly:** Use FFmpeg (`QProcess`) to re-combine the upscaled frames and the original (or processed) audio into the final output video.
    *   **Coordination:** The Processor class manages the state of this pipeline (e.g., `SplittingVideo`, `ProcessingVideoFrames`, `AssemblingVideo`).

*   **D. Job and Task Management:**
    *   **Role:** Manages the queue of files to be processed and controls the concurrent execution of tasks.
    *   **Key Functionality:**
        *   `MainWindow` holds the master lists of files (images, videos, GIFs) in `QStandardItemModel`s.
        *   When processing starts, jobs are created (e.g., `ProcessJob` struct) and queued.
        *   `MainWindow` uses `QtConcurrent::run` and `QThreadPool::globalInstance()` to dispatch tasks to worker threads, respecting a user-configurable maximum thread count. Each task typically involves processing one file.
        *   Individual Processor classes manage the asynchronous execution of their `QProcess` for a single operation (one image, one video frame).
        *   `MainWindow` tracks overall progress, errors, and manages the start/stop state of the entire batch.

*   **E. File Management:**
    *   **Role:** Handles all file system interactions, including input selection, output path generation, and temporary file handling.
    *   **Key Functionality:**
        *   `MainWindow` contains extensive logic for:
            *   Drag & drop (`dragEnterEvent`, `dropEvent`).
            *   File/folder browser interactions.
            *   Recursive directory scanning.
            *   Deduplicating file entries.
            *   Generating output file paths (`Generate_Output_Path`) based on user settings (suffix, output directory).
            *   Creating and managing temporary directories (`TempDir_Path`) for video processing.
            *   Moving files to the system's Recycle Bin/Trash.
        *   `FileManager` (`FileManager.h/.cpp`): A helper class intended to encapsulate common file system operations, though much of this logic also resides directly in `MainWindow`.
        *   Uses Qt classes like `QFile`, `QDir`, `QFileInfo`, `QUrl`, `QDesktopServices`.

*   **F. Settings Management:**
    *   **Role:** Persists and retrieves application and engine-specific settings.
    *   **Key Functionality:**
        *   `MainWindow` is central to reading (`Settings_Read_value`, `Settings_Read_Apply`) and saving (`Settings_Save`) settings.
        *   `QSettings` is the likely Qt mechanism used for storing settings in a platform-appropriate way (e.g., INI file, registry).
        *   Engine-specific parameters are often defined in dedicated structs or header files (e.g., `realcugan_settings.h`, `realesrgan_settings.h`, `anime4k_settings.h`). These are populated from UI controls in `MainWindow` and passed to Processor classes.
        *   UI elements (QComboBox, QSpinBox, etc.) in `MainWindow` are bound to these settings.

*   **G. GPU Management:**
    *   **Role:** Facilitates the detection and selection of GPUs for hardware-accelerated upscaling.
    *   **Key Classes:**
        *   `GpuManager` (`GpuManager.h/.cpp`): Designed to detect available GPUs by interacting with CLI tools (e.g., `realesrgan-ncnn-vulkan -`). Populates UI elements with detected GPUs. (Implementation was largely stubbed in reviewed code).
        *   `RealcuganJobManager`, `RealesrganJobManager`: Helper classes that construct the specific command-line argument strings (`-g`, `-j` flags) for configuring GPU usage (single/multi-GPU, job distribution) for Real-CUGAN and Real-ESRGAN.
    *   **Interactions:** `MainWindow` uses `GpuManager` results to update GPU selection UI. These selections are then used by Processor classes, potentially via JobManagers, to set CLI arguments.

*   **H. Launcher Application (`Waifu2x-Extension-QT-Launcher`):**
    *   **Role:** A separate small Qt application that acts as a startup wrapper for the main application.
    *   **Key Functionality:** Displays a splash screen. Checks if an instance of `Beya_Waifu.exe` is already running (using `tasklist` on Windows) to enforce a single instance. On Windows, it checks directory writability and attempts to relaunch the main app with administrator privileges (`ShellExecuteW` with `runas`) if necessary.
    *   **Interaction:** It's a standalone executable that launches the main `Beya_Waifu.exe`.

*   **I. Utility Components:**
    *   **Role:** Provide shared, reusable helper functionalities across the application.
    *   **Key Classes:**
        *   `ProcessRunner` (`ProcessRunner.h/.cpp`): Executes external commands synchronously using `QEventLoop`, capturing their output. Useful for tasks that need to block and return a result, like compatibility tests or initial GPU detection. Includes a timeout.
        *   `VideoProcessor` (`VideoProcessor.h/.cpp`): A small helper class focused on building FFmpeg command-line argument strings for video output settings (encoders, bitrate, pixel format).
        *   `Logger` (`Logger.h/.cpp`): Configures application-wide logging to a file, initialized in `main.cpp`.
        *   `LiquidGlassWidget` (`LiquidGlassWidget.h/.cpp`): A custom QOpenGLWidget that renders a "liquid glass" visual effect, presumably using GLSL shaders compiled via `qsb`.

### 2.3. Interactions and Data Flow

The application operates on an event-driven model, primarily orchestrated by `MainWindow`.

*   **User Interaction -> `MainWindow`:** User actions on the GUI (button clicks, drag-drop, menu selections) trigger slots in `MainWindow`.
*   **`MainWindow` -> Processor Classes:**
    *   For processing tasks, `MainWindow` gathers settings from the UI, creates a settings object (e.g., `RealEsrganSettings`), and calls the appropriate method on a Processor class instance (e.g., `m_realEsrganProcessor->processImage(...)`). This is often done via `QtConcurrent::run` to offload from the GUI thread.
*   **Processor Class -> CLI Tool:** The Processor class constructs command-line arguments and launches the external CLI tool using `QProcess`.
*   **CLI Tool -> Processor Class:** The Processor class listens to `QProcess` signals for `finished`, `errorOccurred`, and `readyReadStandardOutput/Error`. It parses output for progress and messages.
*   **Processor Class -> `MainWindow` (Feedback):** The Processor class emits signals (`logMessage`, `fileProgress`, `statusChanged`, `processingFinished`) which are connected to slots in `MainWindow`.
*   **`MainWindow` -> UI Update:** Slots in `MainWindow` update the GUI elements (progress bars, table status, text log) in response to signals from Processor classes.
*   **Settings Flow:** UI controls in `MainWindow` modify internal variables. These are used to populate settings objects for Processors. `QSettings` is used by `MainWindow` to load/save these settings persistently.
*   **Concurrency:** The main GUI runs on the Qt main thread. Processing tasks are launched in separate threads using `QThreadPool` (managed by `QtConcurrent::run`). Each external CLI tool invoked by `QProcess` also runs in its own process, allowing for asynchronous operation.

**Simplified Image Upscaling Workflow:**
1.  User adds image file via GUI -> `MainWindow` updates its `QStandardItemModel`.
2.  User configures settings (engine, scale, etc.) in `MainWindow` UI.
3.  User clicks "Start" -> `MainWindow` creates a job, dispatches it via `QtConcurrent::run`.
4.  Worker thread in `MainWindow` calls `SelectedProcessor::processImage(params)`.
5.  `SelectedProcessor` starts `QProcess` for the CLI tool.
6.  `QProcess` output (progress, logs) -> `SelectedProcessor` signals -> `MainWindow` slots update UI.
7.  `QProcess` finishes -> `SelectedProcessor` signals completion/error -> `MainWindow` updates UI and starts next job.

### 2.4. Build System and Dependencies

*   **Build Tools:**
    *   `qmake`: Qt's build system, used for the main application and launcher (`.pro` files).
    *   `make` (Linux/macOS) / `mingw32-make` (Windows with MinGW): Used by `qmake`.
    *   `CMake`: Used to build the third-party C++ CLI upscaling tools (Real-ESRGAN, Real-CUGAN) from their Git submodules.
*   **Key Libraries & Frameworks:**
    *   **Qt (version 6.5.2 specified):** Core, GUI, Widgets, Concurrent, Multimedia, OpenGL (for LiquidGlassWidget), OpenGLWidgets, ShaderTools (for `qsb`), Core5Compat.
    *   **Bundled/External CLI Tools:** Real-ESRGAN-NCNN-Vulkan, Real-CUGAN-NCNN-Vulkan, Anime4KCPP, Waifu2x variants, SRMD, FFmpeg, ImageMagick, Gifsicle, SoX.
    *   **Vulkan SDK:** Required for compiling NCNN-Vulkan tools (for `glslc.exe`).
*   **Build Scripts:**
    *   `build_projects.sh`: A shell script for Linux and MSYS2 (on Windows). It handles:
        *   Git submodule initialization/updates.
        *   Installation of some dependencies (e.g., `glslangValidator`).
        *   Building/copying CLI upscaler tools (builds from source on Linux, prefers prebuilt on Windows but can build from source).
        *   Compiling the Qt applications (`Waifu2x-Extension-QT`, `Waifu2x-Extension-QT-Launcher`) using `qmake` and `make`.
        *   Compiling shaders (`liquidglass.frag`) using `qsb`.
        *   Deploying Qt runtime libraries on Windows using `windeployqt.exe`.
    *   `build_windows.ps1`: A PowerShell script for setting up the Windows build environment. It automates:
        *   Installation of Chocolatey, MSYS2, CMake, Git, Python.
        *   Installation of the MinGW toolchain within MSYS2.
        *   Installation of a specific Qt version and modules using `aqtinstall`.
        *   Installation of the Vulkan SDK.
        *   Then, it executes `build_projects.sh` within the configured MSYS2 environment.
*   **Python Dependencies (`requirements.txt`):** `Pillow`, `pytest`, `pytest-subtests`, `PySide6`, `lxml` – primarily for the test suite.

### 2.5. Cross-Platform Aspects
*   **Qt Framework:** The primary enabler for cross-platform GUI and application logic.
*   **Build System:** `build_projects.sh` contains OS-specific logic (Linux vs. Windows/MSYS2) for paths, compiler choices, and how CLI tools are acquired (built from source vs. using prebuilt binaries). `build_windows.ps1` is Windows-specific.
*   **CLI Tools:** Most core CLI upscaling tools (NCNN Vulkan versions, FFmpeg) are themselves cross-platform.
*   **Code Practices:** Use of `QDir::toNativeSeparators()` for path handling. Conditional compilation (`#ifdef Q_OS_WIN`) is used for platform-specific API calls (e.g., `ShellExecuteW` in Launcher, `tasklist`).

### 2.6. Backend CLI Tool Integration
*   **Wrapper Classes:** Each CLI tool is typically managed by a dedicated C++ "Processor" class (e.g., `RealEsrganProcessor`).
*   **Asynchronous Execution:** `QProcess` is used to launch CLI tools asynchronously, preventing the GUI from freezing. Signals like `finished()`, `errorOccurred()`, `readyReadStandardOutput()`, and `readyReadStandardError()` are used to monitor and interact with the process.
*   **Argument Construction:** Processor classes dynamically assemble command-line arguments based on user-selected settings and input/output file paths.
*   **I/O Handling:**
    *   `stdin`: Not typically used for these tools.
    *   `stdout`/`stderr`: Read by the Processor class to capture log messages, error details, and sometimes progress indicators (e.g., percentage values).
*   **State Management:** Processor classes maintain state related to the execution of the CLI tool (e.g., idle, running, splitting video frames, error).
*   **Error Handling:** Based on the exit code of the `QProcess` and messages parsed from `stderr`.
*   **Configuration:** Settings objects (e.g., `RealEsrganSettings`) are passed to Processor methods, which then translate these into appropriate CLI arguments.
