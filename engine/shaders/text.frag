/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#version 450 core

#include "common_funcs.glsl"

layout (location = 0) out vec4 out_color;

LAYOUT_LOCATION(0) in vec4 fsCol04f;
LAYOUT_LOCATION(1) in vec2 fsUv02f;

void main() {
    vec4 result_color = vec4(fsCol04f.rgb, fsCol04f.a * texture(FontTexture, fsUv02f).r);

    #ifdef OUT_SRGB
    result_color.rgb = linear_to_srgb(result_color.rgb, inverse_gamma);
    #endif

    out_color = result_color;
}
