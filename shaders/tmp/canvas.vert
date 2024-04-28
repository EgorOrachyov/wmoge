/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_funcs.glsl"
#include "vertex_attributes.glsl"
#include "inout_attributes.glsl

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
    
    InoutAttributes result;
    result.worldPos = transform * vec3(vertex.pos2, 1.0f);
    result.col[0] = vertex.col[0];
    result.uv[0] = UnpackUv(vertex.uv[0]);
    result.primitiveId = vertex.primitiveId;

    StoreInoutAttributes(result);

    gl_Position = ClipProjView * vec4(result.worldPos, 1.0f);
}