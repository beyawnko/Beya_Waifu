# Memory: Added QTimer include

Included `#include <QTimer>` at the top of `RealCuganProcessor.cpp` and `realesrganprocessor.cpp`. These files call
`QTimer::singleShot` so the explicit include prevents missing header errors with certain build configurations. All
tests passed after the change.

Related memories:
- [RealCUGAN Qt Multimedia Pipeline Verification](2025-06-24T093757Z-realcugan-qt-media-check.md)
- [Anime4K HDN passes enable fix](2025-06-18T195718Z-hdn-passes-fix.md)
