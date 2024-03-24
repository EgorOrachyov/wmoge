#version 450 core

#include "common/funcs.glsl"
#include "common/color.glsl"

layout (location = 0) out vec4 outColor;

LAYOUT_LOCATION(0) vec3 in vsWorldPos;
LAYOUT_LOCATION(1) vec4 in vsCol;
LAYOUT_LOCATION(2) vec2 in vsUv;
LAYOUT_LOCATION(3) int in vsCmdId;

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
        return texture(Image0, uv).rgba;
    }
    if (idx == 1) {
        return texture(Image1, uv).rgba;
    }
    if (idx == 2) {
        return texture(Image2, uv).rgba;
    }
    if (idx == 3) {
        return texture(Image3, uv).rgba;
    }

    return vec4(1.0f,1.0f,1.0f,1.0f);
}

void main() {
    const DrawCmdData cmdData = GetCmdData(vsCmdId);
    const vec2 pos = vsWorldPos.xy;
    const vec4 clipRect = cmdData.ClipRect;
    const int textureIdx = cmdData.TextureIdx;

    if (ChechClipRect(clipRect, pos)) {
        discard;
    }

    vec4 color = SampleImage(textureIdx, vsUv) * vsCol;    

    #if OUT_MODE == OUT_MODE_SRGB
        color.rgb = ColorSrgbToLinear(color.rgb, InverseGamma);
    #endif

    outColor = color;
}