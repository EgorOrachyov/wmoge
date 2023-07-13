/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"

layout(location = 0) out vec4 fs_color;

LAYOUT_LOCATION(0)
in vec4 in_color;
LAYOUT_LOCATION(1)
in vec2 in_uv;

void main() {
    #ifdef CANVAS_FONT_BITMAP
    float mask = texture(Texture, in_uv).r;
    fs_color   = vec4(in_color.rgb, in_color.a * mask);
    #else
    vec4 tex_color_srgb   = texture(Texture, in_uv).rgba;
    vec4 tex_color_linear = vec4(srgb_to_linear(tex_color_srgb.rgb, gamma), tex_color_srgb.a);

    fs_color = vec4(in_color.rgba * tex_color_linear);
    #endif
}