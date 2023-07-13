/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_defines.glsl"

vec3 srgb_to_linear(in vec3 color, in float gamma) {
    return pow(color, vec3(gamma));
}

vec3 linear_to_srgb(in vec3 color, in float inverse_gamma) {
    return pow(color, vec3(inverse_gamma));
}

vec2 unpack_uv(in vec2 uv) {
    #ifdef TARGET_VULKAN
    return vec2(uv.x, 1.0f - uv.y);
    #else
    return uv;
    #endif
}