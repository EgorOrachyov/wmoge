/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_defines.glsl"

vec3 ColorSrgbToLinear(in vec3 color, in float gamma) {
    return pow(color, vec3(gamma));
}

vec3 ColorLinearToSrgb(in vec3 color, in float inverse_gamma) {
    return pow(color, vec3(inverse_gamma));
}

// Convert rgb to luminance with rgb in linear space 
// with sRGB primaries and D65 white point
float Luminance(in vec3 color) {
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

// Flip optionally UV (for GL or VK to work uniformly)
vec2 UnpackUv(in vec2 uv) {
    #ifdef TARGET_VULKAN
    return vec2(uv.x, 1.0f - uv.y);
    #else
    return uv;
    #endif
}

mat4 GetIdentity4x4() {
    mat4 matrix = mat4(
        vec4(1,0,0,0),
        vec4(0,1,0,0),
        vec4(0,0,1,0),
        vec4(0,0,0,1));
        
    return matrix;
}

vec3 TransformLocalToWorld(in vec3 posLocal, in mat4 localToWorld) {
    return (localToWorld * vec4(posLocal, 1.0f)).xyz;
}

vec3 TransformLocalToWorldNormal(in vec3 normLocal, in mat4 normalMatrix) {
    return (normalMatrix * vec4(normLocal, 0.0f)).xyz;
}