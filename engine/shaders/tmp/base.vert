/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "inout_attributes.glsl"
#include "vertex_attributes.glsl"
#include "common_defines.glsl"

void main() {
    VertexAttributes vertex = ReadVertexAttributes();
    InoutAttributes result;

    result.worldPos = vertex.pos3;
    result.col[0] = vertex.col[0];
    result.col[1] = vertex.col[1];
    result.col[2] = vertex.col[2];
    result.col[3] = vertex.col[3];
    
    StoreInoutAttributes(result);

    gl_Position = mat_clip_proj_view * vec4(vertex.pos3, 1.0f);
}