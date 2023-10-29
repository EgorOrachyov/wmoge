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
    const vec2 uv = attributes.uv[0];
    
    const vec3 hdrSrcColor  = texture(Image, uv).rgb;
    const vec3 hdrBloom     = texture(Bloom, uv).rgb * BloomIntensity;
    const vec3 hdrBloomDirt = texture(BloomDirtMask, uv).rgb * BloomDirtMaskIntensity;
    const vec3 hdrColor     = hdrSrcColor + hdrBloom + hdrBloom * hdrBloomDirt;

    vec3 ldrColor = hdrColor;
    const int tonemapMode = int(Mode);

    switch (tonemapMode) {
        case TONEMAP_MODE_EXP: 
            ldrColor = TonemapExp(hdrColor, Exposure);
            break;
        case TONEMAP_MODE_REINH: 
            ldrColor = TonemapReinhard(hdrColor);
            break;
        case TONEMAP_MODE_REINH_EXT: 
            ldrColor = TonemapReinhardExtended(hdrColor, WhitePoint);
            break;
        case TONEMAP_MODE_ACES: 
            ldrColor = TonemapACES(hdrColor);
            break;
        case TONEMAP_MODE_UN2: 
            ldrColor = TonemapUncharted2(hdrColor);
            break;     
        default:
            break;
    }

    const vec3 gammaColor = ColorLinearToSrgb(ldrColor, InverseGamma);

    out_color = vec4(gammaColor, 1.0f);
}