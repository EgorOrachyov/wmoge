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

#ifdef ATTRIB_Col04f
LAYOUT_LOCATION(0) in vec4 fsCol04f;
#endif

#ifdef ATTRIB_Col14f
LAYOUT_LOCATION(1) in vec4 fsCol14f;
#endif

#ifdef ATTRIB_Col24f
LAYOUT_LOCATION(2) in vec4 fsCol24f;
#endif

#ifdef ATTRIB_Col34f
LAYOUT_LOCATION(3) in vec4 fsCol34f;
#endif

void main() {
    vec4 result_color = base_color;

    #ifdef ATTRIB_Col04f
    result_color = fsCol04f;
    #endif

    #ifdef ATTRIB_Col14f
    result_color = mix(result_color, fsCol14f, mix_weight_1);
    #endif

    #ifdef ATTRIB_Col24f
    result_color = mix(result_color, fsCol24f, mix_weight_2);
    #endif

    #ifdef ATTRIB_Col34f
    result_color = mix(result_color, fsCol34f, mix_weight_3);
    #endif

    #ifdef OUT_SRGB
    result_color.rgb = linear_to_srgb(result_color.rgb, inverse_gamma);
    #endif

    #ifdef NO_ALPHA
    result_color.a = 1.0f;
    #endif

    out_color = result_color;
}
