/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"

#ifdef AUX_DRAW_GEOM
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_col;
#endif
#ifdef AUX_DRAW_TEXT
layout(location = 0) in vec2 in_pos;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_col;
#endif

LAYOUT_LOCATION(0)
out vec3 out_color;
#ifdef AUX_DRAW_TEXT
LAYOUT_LOCATION(1)
out vec2 out_uv;
#endif

void main() {
    out_color = in_col;

#ifdef AUX_DRAW_GEOM
    gl_Position = clip_proj_view * vec4(in_pos, 1.0f);
#endif

#ifdef AUX_DRAW_TEXT
    out_uv      = unpack_uv(in_uv);
    gl_Position = clip_proj_screen * vec4(in_pos, 0.0f, 1.0f);
#endif
}