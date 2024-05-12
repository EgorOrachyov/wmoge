/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "inout_attributes.glsl"

layout (location = 0) out vec4 out_color;

void main() {
    const InoutAttributes attributes = ReadInoutAttributes();
    const vec2 uv = attributes.uv[0];
    
    const vec4 ldrColor = texture(Color, uv).rgba;

    out_color = ldrColor;
}