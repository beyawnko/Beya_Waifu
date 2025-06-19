# UI Repair Plan Part 2 for Waifu2x-Extension-QT

This document outlines the layout fixes for the "Video settings", "Additional settings", and "Compatibility test" tabs in `Waifu2x-Extension-QT/mainwindow.ui`.

## 1. "Video settings" Tab (`tab_VideoSettings`)

1.  **Apply a `QVBoxLayout` to the `tab_VideoSettings` widget.**
2.  **Top Section:**
    *   Create a `QHBoxLayout`.
    *   Add `groupBox_AudioDenoise` and `groupBox_ProcessVideoBySegment` to it.
    *   Add this `QHBoxLayout` as the first item in the main `QVBoxLayout`.
3.  **Frame Interpolation Section:**
    *   Select `groupBox_FrameInterpolation` and apply a `QGridLayout` to its contents.
    *   Arrange all labels, inputs, and buttons within the grid cells for proper alignment.
    *   Add the group box to the main `QVBoxLayout`.
4.  **Main Video Settings:**
    *   Select `groupBox_video_settings` and apply a `QHBoxLayout`.
    *   Add `groupBox_OutputVideoSettings` and `groupBox_ToMp4VideoSettings` to this layout.
    *   For `groupBox_OutputVideoSettings`, apply a `QFormLayout` to its contents.
    *   For `groupBox_ToMp4VideoSettings`, apply a `QFormLayout` to its contents.
    *   Add `groupBox_video_settings` to the main `QVBoxLayout`.
5.  **Bottom Buttons:**
    *   Create a `QHBoxLayout`. Add a horizontal spacer, then `pushButton_ResetVideoSettings` and `pushButton_encodersList`.
    *   Add this button layout to the main `QVBoxLayout`.

## 2. "Additional settings" Tab (`tab_AdditionalSettings`)

1.  **Apply a `QHBoxLayout` to the `tab_AdditionalSettings` widget.**
2.  **Create three `QVBoxLayouts`, one for each column.**
3.  **Left Column Layout:**
    *   Add `groupBox_InputExt`, the "Custom Font Settings" group box, and the buttons group box to the first `QVBoxLayout`.
4.  **Middle Column Layout:**
    *   Add all checkboxes from the middle column to the second `QVBoxLayout`.
    *   Add a vertical spacer to the bottom.
5.  **Right Column Layout:**
    *   Add all checkboxes from the right column to the third `QVBoxLayout`.
    *   Add a vertical spacer to the bottom.
6.  **Add all three `QVBoxLayouts` to the main `QHBoxLayout`.**

## 3. "Compatibility test" Tab (`tab_CompatibilityTest`)

1.  **Apply a `QVBoxLayout` to the `tab_CompatibilityTest` widget.**
2.  **Results GroupBox:**
    *   Select `groupBox_CompatibilityTestRes` and apply a `QGridLayout`.
    *   Place the labels ("Super-Resolution Engines:", etc.) in the first column, spanning multiple columns if needed.
    *   Arrange all the compatibility `QCheckBox` widgets within the grid cells to align them.
3.  **Add all main widgets (`groupBox_CompatibilityTestRes`, the description `QLabel`, `progressBar_CompatibilityTest`, `pushButton_compatibilityTest`) sequentially to the main `QVBoxLayout`.**
