#pragma once

#include <QVector2D> // For QVector2D

// C++ struct mirroring the GLSL UBO structure with std140 layout
// This should be kept in sync with the shader.
// GLSL:
// layout(std140, binding = 1) uniform SettingsBlock {
//     vec2 resolution; // Viewport/widget resolution (pixels)
//     float time;       // Time for animations
//     vec2 mouse;       // Mouse coordinates (pixels, relative to widget, Y-up from bottom-left)
//     float ior;        // Index of Refraction (e.g., 1.5)
//     float borderRadius; // Radius for rounded corners (pixels)
//     float chromaticAberrationOffset; // Offset for chromatic aberration
// } Settings;
struct LiquidGlassParams {
    // Ensure std140 layout: vec2 is 8 bytes, float is 4 bytes.
    // Alignments: float (4), vec2 (8).
    // A vec2 followed by a float will have the float at offset 8.
    // A float followed by a vec2, the vec2 starts at the next multiple of 8.
    QVector2D resolution; // offset 0, size 8
    float time;           // offset 8, size 4
    // mouse must start at a multiple of 8 (vec2 alignment)
    // so, float_padding1 makes offset of mouse = 16
    float float_padding1[1]; // offset 12, size 4 (padding to align mouse)
    QVector2D mouse;      // offset 16, size 8
    float ior;            // offset 24, size 4
    float borderRadius;   // offset 28, size 4
    float chromaticAberrationOffset; // offset 32, size 4
    // Total size needs to be a multiple of 16 (max base alignment, often UBO block alignment)
    // Current size is 36. Next multiple of 16 is 48. Need 12 bytes padding.
    float float_padding2[3]; // offset 36, size 12 (padding to make total size 48)
}; // Total size: 48 bytes
