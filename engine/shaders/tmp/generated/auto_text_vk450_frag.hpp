/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

// Autogenerated file by 'generator.py' on 2023-11-05 14:43:57.718271, do not modify

#pragma once

#include "core/string_id.hpp"
#include "core/string_utf.hpp"
#include "core/string_utils.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"

namespace wmoge {

    static const char source_text_vk450_frag[] = R"(


layout (set = 0, binding = 1) uniform sampler2D FontTexture;



layout (set = 0, binding = 0, std140) uniform Params {
mat4 mat_clip_proj_screen;
float inverse_gamma;
float _tp_pad0;
float _tp_pad1;
float _tp_pad2;
};


#define TARGET_VULKAN
#if defined(TARGET_VULKAN)
#define LAYOUT_LOCATION(idx) layout(location = idx)
#else
#define LAYOUT_LOCATION(idx)
#endif
#if defined(TARGET_VULKAN)
#define LAYOUT_BUFFER(set_idx, binding_idx, fields_layout) layout(set = set_idx, binding = binding_idx, fields_layout)
#else
#define LAYOUT_BUFFER(set_idx, binding_idx, fields_layout) layout(fields_layout)
#endif
#if defined(TARGET_VULKAN)
#define LAYOUT_SAMPLER(set_idx, binding_idx) layout(set = set_idx, binding = binding_idx)
#else
#define LAYOUT_SAMPLER(set_idx, binding_idx)
#endif
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
vec2 GidToUv(uvec2 gid, ivec2 size) {
    return (vec2(gid) + vec2(0.5f, 0.5f)) / vec2(size);
}
float Saturate(float x) {
    return clamp(x, 0.0f, 1.0f);
}
uint Flatten3D(uvec3 coord, uvec3 dim) {
    return coord.x * dim.y * dim.z + coord.y * dim.z + coord.z;
}
uvec3 Unflatten3D(uint index, uvec3 dim) {
    uvec3 coord;
    coord.x = index / (dim.y * dim.z);
    index -= coord.x * dim.y * dim.z;
    coord.y = index / (dim.z);
    index -= coord.y * dim.z;
    coord.z = index;
    return coord;
}
float DepthToZ(in float depth, in float nearZ, in float farZ) {
    const float pz = depth * 2.0f - 1.0f;
    return (2.0 * nearZ * farZ) / (farZ + nearZ - pz * (farZ - nearZ)); 
}
vec3 ProjectPoint(in vec3 p, in mat4 projection) {
    vec4 projected = projection * vec4(p, 1.0f);
    projected.xyz /= projected.w;
    return projected.xyz;
}
vec3 NdcToView(in vec3 ndc, in mat4 projInv) {
    return ProjectPoint(ndc, projInv);
}
vec3 UvDepthToNdc(in vec2 uv, in float depth) {
    return vec3(uv, depth) * 2.0f - 1.0f;
}
#define EPSILON           0.00001f
#define EPSILON_LUMINANCE 0.001f
#define PI          3.14159265359
#define PI_HALF     1.57079632679
#define PI_QUARTER  0.78539816339
#define EULER_NUM   2.71828182846
#define REFLECTANCE_TABLE_WATER         0.02
#define REFLECTANCE_TABLE_FABRIC        0.04
#define REFLECTANCE_TABLE_LIQUIDS       0.02
#define REFLECTANCE_TABLE_GEMSTONES     0.05
#define REFLECTANCE_TABLE_PLASTIC       0.04
#define REFLECTANCE_TABLE_GLASS         0.04
#define REFLECTANCE_TABLE_DIELECTRIC    0.04
#define REFLECTANCE_TABLE_EYES          0.025
#define REFLECTANCE_TABLE_SKIN          0.028
#define REFLECTANCE_TABLE_HAIR          0.046
#define REFLECTANCE_TABLE_TEETH         0.058
#define REFLECTANCE_TABLE_DEFAULT       0.04
#define IOR_TABLE_WATER         1.33
#define IOR_TABLE_FABRIC        1.5
#define IOR_TABLE_LIQUIDS       1.33
#define IOR_TABLE_GEMSTONES     1.58
#define IOR_TABLE_PLASTIC       1.5
#define IOR_TABLE_GLASS         1.5
#define IOR_TABLE_DIELECTRIC    1.33
#define IOR_TABLE_EYES          1.38
#define IOR_TABLE_SKIN          1.4
#define IOR_TABLE_HAIR          1.55
#define IOR_TABLE_TEETH         1.63
#define IOR_TABLE_DEFAULT       1.5
#define LIGHT_TYPE_DIR      0
#define LIGHT_TYPE_SPOT     1
#define LIGHT_TYPE_POINT    2
#define LIGHT_TYPE_AREA     3
vec3 ColorSrgbToLinear(in vec3 color, in float gamma) {
    return pow(color, vec3(gamma));
}
vec3 ColorLinearToSrgb(in vec3 color, in float inverse_gamma) {
    return pow(color, vec3(inverse_gamma));
}
// Convert rgb to luminance with rgb in linear space 
// with sRGB primaries and D65 white point
float ColorToLuminance(in vec3 color) {
	return dot(color, vec3(0.2126729, 0.7151522, 0.0721750));
}
// Quadratic color thresholding
// curve = (threshold - knee, knee * 2, 0.25 / knee)
vec3 QuadraticThreshold(vec3 color, float threshold, vec3 curve) {
    // Pixel brightness
    float br = max(color.r, max(color.g, color.b));
    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;
    // Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);
    return color;
}
layout (location = 0) out vec4 out_color;
LAYOUT_LOCATION(0) in vec4 fsCol04f;
LAYOUT_LOCATION(1) in vec2 fsUv02f;
void main() {
    vec4 result_color = vec4(fsCol04f.rgb, fsCol04f.a * texture(FontTexture, fsUv02f).r);
    #ifdef OUT_SRGB
        result_color.rgb = ColorLinearToSrgb(result_color.rgb, inverse_gamma);
    #endif
    out_color = result_color;
}


)";
}
