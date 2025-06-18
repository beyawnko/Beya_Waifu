#version 450 core

// Inputs from vertex shader
layout(location = 0) in vec2 vTexCoord; // Normalized texture coordinates (0-1)

// Output color
layout(location = 0) out vec4 fragColor;

// Uniforms from C++
layout(binding = 0) uniform sampler2D sourceTexture; // Background texture

// Uniform Buffer Object for settings
layout(std140, binding = 1) uniform SettingsBlock { // UBOs usually start at binding = 1 if sampler2D is at 0
    vec2 resolution;         // Viewport/widget resolution (pixels)
    float time;              // Time for animations
    vec2 mouse;              // Mouse coordinates (pixels, relative to widget)
    float ior;               // Index of Refraction (e.g., 1.5)
    float borderRadius;      // Radius for rounded corners (pixels)
    float chromaticAberrationOffset; // Offset for chromatic aberration
} Settings; // Instance name

// --- SDF Function for Rounded Rectangle ---
// p: current fragment's coordinate (scaled, centered)
// b: half-size of the box (width/2, height/2)
// r: border radius
float sdRoundedBox(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

// --- Normal Calculation from SDF ---
// p: current fragment's coordinate (scaled, centered)
// boxSize: full size of the box (width, height)
// radius: border radius for the sdf
vec3 calcNormalSDF(vec2 p, vec2 boxSize, float radius) {
    vec2 e = vec2(1.0, 0.0) / Settings.resolution.y; // Small epsilon scaled by pixel size for consistent thickness
    float sdf_center = sdRoundedBox(p, boxSize * 0.5, radius); // SDF at current point
    vec2 grad = vec2(
        sdRoundedBox(p + e.xy, boxSize * 0.5, radius) - sdf_center,
        sdRoundedBox(p + e.yx, boxSize * 0.5, radius) - sdf_center
    ) / e.x; // Approximate gradient

    // Simulate a Z component based on proximity to edge (stronger curve near edge)
    // This is a simplified approach for a 2.5D effect.
    float z_comp = sqrt(max(0.0, 0.01 - pow(length(grad),2.0) )); // Adjust 0.01 for desired 'depth'
    return normalize(vec3(grad, z_comp));
}


void main() {
    // Normalize fragment coordinates to be -0.5 to 0.5 (or similar, depending on aspect handling)
    // This makes (0,0) the center of the screen/widget.
    vec2 uv = (gl_FragCoord.xy - 0.5 * Settings.resolution.xy) / Settings.resolution.y;

    // Define rounded box properties (can be made dynamic with more uniforms)
    // For now, let's make a box that's a fraction of the screen height
    float boxHeight = Settings.resolution.y * 0.4; // Box is 40% of widget height
    float boxWidth = boxHeight * 1.6;     // Box width (e.g., 1.6:1 aspect)
    vec2 boxHalfSize = vec2(boxWidth / 2.0, boxHeight / 2.0) / Settings.resolution.y; // Scale to UV space
    float actualBorderRadius = Settings.borderRadius / Settings.resolution.y; // Scale radius to UV space

    // Center the box (or use mouse.xy / resolution.xy - 0.5 to position it with mouse)
    vec2 boxCenter = vec2(0.0, 0.0); // Centered
    // vec2 boxCenter = (Settings.mouse - 0.5 * Settings.resolution.xy) / Settings.resolution.y; // Mouse controlled

    float d = sdRoundedBox(uv - boxCenter, boxHalfSize, actualBorderRadius);

    if (d > 0.0) { // Outside the shape
        // Sample background directly or make transparent
        fragColor = texture(sourceTexture, vTexCoord);
        // discard; // Alternative: make it fully transparent
        return;
    }

    // Inside the shape or on its border
    vec3 normal = calcNormalSDF(uv - boxCenter, boxHalfSize * 2.0, actualBorderRadius);

    // View direction (assuming orthographic or simple perspective)
    vec3 viewDir = vec3(0.0, 0.0, 1.0); // From camera towards screen

    // --- Refraction ---
    // Refract view vector to find where to sample background
    vec3 refractedDir = refract(-viewDir, normal, 1.0 / Settings.ior);
    // Use vTexCoord as base, perturb by refraction. Scale effect.
    vec2 refractedUV = vTexCoord + refractedDir.xy * 0.1; // Adjust 0.1 for refraction strength
    vec3 refractedColor = texture(sourceTexture, refractedUV).rgb;

    // --- Reflection / Specular Highlight (Simplified) ---
    // A simple rim light effect or fake specular
    float fresnelFactor = pow(1.0 - max(0.0, dot(normal, viewDir)), 3.0); // Basic Fresnel-like term
    vec3 specularColor = vec3(0.8) * fresnelFactor; // White highlight

    // --- Chromatic Aberration ---
    vec2 caOffsetVec = normalize(uv - boxCenter) * Settings.chromaticAberrationOffset; // Offset outwards
    float r_channel = texture(sourceTexture, refractedUV - caOffsetVec).r;
    float b_channel = texture(sourceTexture, refractedUV + caOffsetVec).b;
    vec3 finalColor = vec3(r_channel, refractedColor.g, b_channel);

    // Blend effects
    // Mix refraction with specular highlights
    finalColor = mix(finalColor, specularColor, fresnelFactor * 0.5); // Adjust 0.5 for specular intensity

    // Add a subtle border to the glass shape
    float borderThickness = 2.0 / Settings.resolution.y; // 2 pixels thick border
    if (d > -borderThickness) {
        finalColor = mix(vec3(0.6, 0.6, 0.7), finalColor, smoothstep(-borderThickness, 0.0, d)); // Semi-transparent white border
    }

    fragColor = vec4(finalColor, 1.0); // Fully opaque glass for now
    // For transparency on the glass itself (if d < 0):
    // float alpha = smoothstep(-0.01, 0.0, d); // Make edges slightly smoother
    // fragColor = vec4(finalColor, 1.0 - alpha * 0.2); // Example: slightly transparent glass
}
