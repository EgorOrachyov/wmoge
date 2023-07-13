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
in vec3 in_color;
#ifdef AUX_DRAW_TEXT
LAYOUT_LOCATION(1)
in vec2 in_uv;
#endif

void main() {
    #ifdef AUX_DRAW_GEOM
    fs_color = vec4(linear_to_srgb(in_color, inverse_gamma), 1.0f);
    #endif
    #ifdef AUX_DRAW_TEXT
    fs_color = vec4(linear_to_srgb(in_color, inverse_gamma), texture(FontBitmap, in_uv).r);
    #endif
}