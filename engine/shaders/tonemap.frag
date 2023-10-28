/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"
#include "inout_attributes.glsl"
#include "hdr.glsl"

layout (location = 0) out vec4 out_color;

void main() {
    const InoutAttributes attributes = ReadInoutAttributes();

    const vec3 hdrColor   = texture(Image, attributes.uv[0]).rgb;
    const vec3 ldrColor   = TonemapExp(hdrColor, Exposure);
    const vec3 gammaColor = ColorLinearToSrgb(ldrColor, InverseGamma);

    out_color = vec4(gammaColor, 1.0f);
}