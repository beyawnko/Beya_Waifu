#version 440

layout(location = 0) in vec2 vTexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D sourceTexture;
layout(binding = 1) uniform sampler2D environmentTexture;

float sphereSDF(vec3 p, float r)
{
    return length(p) - r;
}

vec3 calcNormal(vec3 p)
{
    const vec2 e = vec2(0.001, 0.0);
    return normalize(vec3(
        sphereSDF(p + e.xyy, 1.0) - sphereSDF(p - e.xyy, 1.0),
        sphereSDF(p + e.yxy, 1.0) - sphereSDF(p - e.yxy, 1.0),
        sphereSDF(p + e.yyx, 1.0) - sphereSDF(p - e.yyx, 1.0)
    ));
}

void main()
{
    vec2 p = vTexCoord * 2.0 - 1.0;
    vec3 ro = vec3(0.0, 0.0, -2.5);
    vec3 rd = normalize(vec3(p, 1.5));

    float t = 0.0;
    for (int i = 0; i < 64; ++i) {
        vec3 pos = ro + t * rd;
        float d = sphereSDF(pos, 1.0);
        if (abs(d) < 0.001)
            break;
        t += d;
    }

    vec3 pos = ro + t * rd;
    vec3 n = calcNormal(pos);

    vec3 refl = reflect(rd, n);
    vec3 refr = refract(rd, n, 1.0 / 1.33);

    vec3 reflCol = texture(environmentTexture, refl.xy * 0.5 + 0.5).rgb;
    vec3 refrCol = texture(sourceTexture, refr.xy * 0.5 + 0.5).rgb;

    float cShift = 0.003;
    vec3 chroma = vec3(
        texture(environmentTexture, refl.xy * 0.5 + 0.5 + vec2(cShift, 0.0)).r,
        texture(environmentTexture, refl.xy * 0.5 + 0.5).g,
        texture(environmentTexture, refl.xy * 0.5 + 0.5 - vec2(cShift, 0.0)).b
    );

    vec3 color = mix(reflCol, refrCol, 0.5);
    color = mix(color, chroma, 0.3);

    fragColor = vec4(color, 1.0);
}
