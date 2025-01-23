#include "generated/input.glsl"
#include "generated/declarations.glsl"

#include "common/defines.glsl"
#include "common/math.glsl"
#include "common/vertex.glsl"

layout(location = 0) out vec2 vsUv;

void main()
{
    const VertexAttributes vertex = ReadVertexAttributes();

    vec2 positions[3] = vec2[3](vec2(-1, -1), vec2(3, -1), vec2(-1, 3));
    vec2 pos = positions[vertex.vertexId];

    vsUv = UnpackUv(0.5 * pos + vec2(0.5));

    gl_Position = GetClipMatrix() * vec4(pos, 0.0f, 1.0f);
}