#include "generated/input.glsl"
#include "generated/declarations.glsl"

#include "common/defines.glsl"
#include "common/math.glsl"
#include "common/vertex.glsl"

layout(location = 0) out vec4 vsCol;
layout(location = 1) out vec2 vsUv;

void main()
{
    const VertexAttributes vertex = ReadVertexAttributes();

    vsCol = vertex.col[0];
    vsUv = vertex.uv[0];

    gl_Position = ClipProjView * vec4(vertex.pos3, 1.0f);
}