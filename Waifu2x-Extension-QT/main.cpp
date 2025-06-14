/*
    Copyright (C) 2025  beyawnko

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/beyawnko
*/

#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);//High-resolution screen support
    QApplication a(argc,argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("Beya_Waifu");
    parser.addHelpOption();
    QCommandLineOption maxThreadsOpt(QStringLiteral("max-threads"),
                                     QStringLiteral("Override global thread limit"),
                                     QStringLiteral("count"));
    parser.addOption(maxThreadsOpt);
    parser.process(a);
    int overrideThreads = 0;
    if(parser.isSet(maxThreadsOpt))
        overrideThreads = parser.value(maxThreadsOpt).toInt();
    a.setQuitOnLastWindowClosed(false);//Keep running when window is hidden
    MainWindow *w = new MainWindow(overrideThreads);
    w->show();
    return a.exec();
}

/*
Change log:

****************** Complete Chinese translation ***********************

v3.41.02-beta:
- Fix bug: Unable to assemble APNG when there are "%" in the path.

v3.41.01-beta:
- New Feature: APNG(Animated PNG) support.
- Improve GUI.
- Some other improvements.

v3.31.21-beta:
- Performance optimization.
- Update FFmpeg, ImageMagick.
- Some other improvements.

--------------------------------------------------------------------------
To do:
- APNG plugin compatibility test
- Find a better performing apng assembly plugin (try ffmpeg)
- Add support for "image folders", process one folder at a time.
- Improve automatic engine setting adjustment
- Avoid black frame confusion and transition confusion during frame interpolation (frame number after interpolation = N*2-1)
- Automatically convert GIF to mp4
- After SRMD and REALSR officially add cpu support, add [* "-1" is CPU, others are GPUs *] to the GPU ID tooltip
- CRF parameter adjustment support
- Automatic folder monitoring
---------------------------------------------------------------------------
Integrated component:
- gifsicle version 1.92
- SoX 14.4.2-win32
- waifu2x-caffe 1.2.0.4
- Waifu2x-converter 9e0284ae23d43c990efb6320253ff0f1e3776854
- Waifu2x-NCNN-Vulkan 20210210
- SRMD-NCNN-Vulkan 20210210
- RealSR-NCNN-Vulkan 20210210
- ImageMagick 7.0.11-8-portable-Q16-x64
- Anime4KCPP v2.5.0
- FFmpeg 2021-04-20-git-718e03e5f2-full_build
- NirCmd v2.86
- Ghostscript 9.53.3
- GNU Wget 1.20.3
- rife-ncnn-vulkan 20210227
- cain-ncnn-vulkan 20210210
---------------------------------------------------------------------------
Icons made by :
Freepik (https://www.flaticon.com/authors/freepik) From Flaticon : https://www.flaticon.com/
Roundicons (https://www.flaticon.com/authors/roundicons) From Flaticon : https://www.flaticon.com/
Icongeek26 (https://www.flaticon.com/authors/Icongeek26) From Flaticon : https://www.flaticon.com/
*/
