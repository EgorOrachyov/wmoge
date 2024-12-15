#include "generated/input.glsl"
#include "generated/declarations.glsl"

#include "common/defines.glsl"
#include "common/math.glsl"
#include "common/vertex.glsl"

layout(location = 0) out vec3 vsWorldPos;
layout(location = 1) out vec4 vsCol;
layout(location = 2) out vec2 vsUv;
layout(location = 3) flat out int vsCmdId;

GpuCanvasDrawCmd GetCmdData(in int idx)
{
    return DrawCmds[idx];
}

mat3 GetDrawCmdTransform(in int idx)
{
    const GpuCanvasDrawCmd data = GetCmdData(idx);
    return mat3(data.Transform0.xyz,
                data.Transform1.xyz,
                data.Transform2.xyz);
}

void main()
{
    const VertexAttributes vertex = ReadVertexAttributes();
    const mat3 transform = GetDrawCmdTransform(vertex.objectId);

    vsWorldPos = transform * vec3(vertex.pos2, 1.0f);
    vsCol = vertex.col[0];
    vsUv = vertex.uv[0];
    vsCmdId = vertex.objectId;

    gl_Position = ClipProjView * vec4(vsWorldPos, 1.0f);
}