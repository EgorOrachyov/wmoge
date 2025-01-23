#include "common/consts.glsl"

// Convert color in srgb space to linear space
vec3 ColorSrgbToLinear(in vec3 color, in float gamma)
{
    return pow(color, vec3(gamma));
}

// Convert color in linear space to gamma space
vec3 ColorLinearToSrgb(in vec3 color, in float inverse_gamma)
{
    return pow(color, vec3(inverse_gamma));
}

// Convert rgb to luminance with rgb in linear space
// with sRGB primaries and D65 white point
float ColorToLuminance(in vec3 color)
{
    return dot(color, vec3(0.2126729, 0.7151522, 0.0721750));
}

// Quadratic color thresholding
// curve = (threshold - knee, knee * 2, 0.25 / knee)
vec3 QuadraticThreshold(vec3 color, float threshold, vec3 curve)
{
    // Pixel brightness
    float br = max(color.r, max(color.g, color.b));

    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;

    // Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);

    return color;
}