/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"
#include "inout_attributes.glsl

layout (location = 0) out vec4 out_color;

DrawCmdData GetCmdData(in int idx) {
    return DrawCmds[idx];
}

bool ChechClipRect(in vec4 clipRect, in vec2 pos) {
    const vec2 clipMin = clipRect.xy;
    const vec2 clipMax = clipRect.zw;

    return pos.x < clipMin.x || pos.x > clipMax.x ||
           pos.y < clipMin.y || pos.y > clipMax.y;
}

vec4 SampleImage(in int idx, in vec2 uv) {
    if (idx == 0) {
        return texture(CanvasImage0, uv).rgba;
    }
    if (idx == 1) {
        return texture(CanvasImage1, uv).rgba;
    }
    if (idx == 2) {
        return texture(CanvasImage2, uv).rgba;
    }
    if (idx == 3) {
        return texture(CanvasImage3, uv).rgba;
    }

    return vec4(1,1,1,1);
}

void main() {
    const InoutAttributes attributes = ReadInoutAttributes();
    const DrawCmdData cmdData = GetCmdData(attributes.primitiveId);
    const vec2 pos = attributes.worldPos.xy;
    const vec4 clipRect = cmdData.ClipRect;
    const int textureIdx = cmdData.TextureIdx;

    if (ChechClipRect(clipRect, pos)) {
        discard;
    }

    vec4 color = SampleImage(textureIdx, attributes.uv[0]) * attributes.col[0];    

    #ifdef OUT_SRGB
        color.rgb = ColorSrgbToLinear(color.rgb, InverseGamma);
    #endif

    out_color = color;
}