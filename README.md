# Waifu2x-Extension-GUI

Waifu2x-Extension-GUI is a Qt based graphical interface for upscaling and denoising images or
videos. The project integrates the RealCUGAN and RealESRGAN upscalers and presents them in a
single GUI.

## Features

- Simple Qt interface supporting image, GIF and video files
- Queue based processing with per-file progress information
- Multi-GPU configuration and compatibility tests
- System tray integration and drag & drop of files

RealCUGAN and RealESRGAN are currently the only supported upscaling engines.

## Dependencies

- **Qt 5 or Qt 6** development environment (Core, GUI, Widgets, Multimedia modules)
- **C++11** compatible compiler
- **FFmpeg** for handling video input/output
- Included **RealCUGAN** and **RealESRGAN** executables built with ncnn Vulkan

Ensure your GPU drivers support Vulkan since both engines rely on it.

## Build and Run

```bash
# build the main GUI
cd Waifu2x-Extension-QT
qmake Waifu2x-Extension-QT.pro
make

# optional launcher
cd ../Waifu2x-Extension-QT-Launcher
qmake Waifu2x-Extension-QT-Launcher.pro
make
```

After compilation run `Waifu2x-Extension-GUI` (or the launcher) from its build
folder. Place the realcugan-ncnn-vulkan and realesrgan-ncnn-vulkan executables in
the same directory so the GUI can invoke them.

### Thread control

By default the application limits its internal thread pool to twice the number
of detected CPU cores. You can override this via the command line:

```bash
Waifu2x-Extension-GUI --max-threads 8
```
or by editing `settings.ini` and setting `MaxThreadCount`.
