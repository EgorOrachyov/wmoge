#version 450 core

#include "common/defines.glsl"
#include "common/funcs.glsl"
#include "common/vertex.glsl"

LAYOUT_LOCATION(0) vec3 out vsWorldPos;
LAYOUT_LOCATION(1) vec4 out vsCol;
LAYOUT_LOCATION(2) vec2 out vsUv;
LAYOUT_LOCATION(3) int out vsCmdId;

DrawCmdData GetCmdData(in int idx) {
    return DrawCmds[idx];
}

mat3 GetDrawCmdTransform(in int idx) {
    const DrawCmdData data = GetCmdData(idx);
    return mat3(data.Transform0.xyz,
                data.Transform1.xyz,
                data.Transform2.xyz);
}

void main() {
    const VertexAttributes vertex = ReadVertexAttributes();
    const mat3 transform = GetDrawCmdTransform(vertex.primitiveId);
    
    vsWorldPos = transform * vec3(vertex.pos2, 1.0f);
    vsCol = vertex.col[0];
    vsUv = UnpackUv(vertex.uv[0]);
    vsCmdId = vertex.primitiveId;

    gl_Position = ClipProjView * vec4(vsWorldPos, 1.0f);
}