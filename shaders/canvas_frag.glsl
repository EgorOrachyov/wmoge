#include "generated/declarations.glsl"

#include "common/math.glsl"
#include "common/color.glsl"

layout (location = 0) out vec4 outColor;

layout (location = 0) in vec3 vsWorldPos;
layout (location = 1) in vec4 vsCol;
layout (location = 2) in vec2 vsUv;
layout (location = 3) flat in int vsCmdId;

CanvasDrawCmd GetCmdData(in int idx) {
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
    const CanvasDrawCmd cmdData = GetCmdData(vsCmdId);
    const vec2 pos = vsWorldPos.xy;
    const vec4 clipRect = cmdData.ClipRect;
    const int textureIdx = cmdData.TextureIdx;

    if (ChechClipRect(clipRect, pos)) {
        discard;
    }

    vec4 color = SampleImage(textureIdx, vsUv) * vsCol;    

    #ifdef OUT_MODE_SRGB
        color.rgb = ColorSrgbToLinear(color.rgb, InverseGamma);
    #endif

    outColor = color;
}