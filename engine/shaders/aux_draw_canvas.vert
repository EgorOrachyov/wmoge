/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"

layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_col;

LAYOUT_LOCATION(0) out vec4 out_color;
LAYOUT_LOCATION(1) out vec2 out_uv;

void main() {
    out_color   = in_col;
    out_uv      = UnpackUv(in_uv);
    gl_Position = clip_proj_screen * vec4(in_pos, 0.0f, 1.0f);
}