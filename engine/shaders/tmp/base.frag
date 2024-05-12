/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "inout_attributes.glsl"
#include "common_funcs.glsl"
#include "color.glsl"

layout (location = 0) out vec4 out_color;

void main() {
    InoutAttributes attributes = ReadInoutAttributes();

    vec4 result_color = base_color;

    #ifdef ATTRIB_Col04f
        result_color = attributes.col[0];
    #endif

    #ifdef ATTRIB_Col14f
        result_color = mix(result_color, attributes.col[1], mix_weight_1);
    #endif

    #ifdef ATTRIB_Col24f
        result_color = mix(result_color, attributes.col[2], mix_weight_2);
    #endif

    #ifdef ATTRIB_Col34f
        result_color = mix(result_color, attributes.col[3], mix_weight_3);
    #endif

    #ifdef OUT_SRGB
        result_color.rgb = ColorLinearToSrgb(result_color.rgb, inverse_gamma);
    #endif

    #ifdef NO_ALPHA
        result_color.a = 1.0f;
    #endif

    out_color = result_color;
}
