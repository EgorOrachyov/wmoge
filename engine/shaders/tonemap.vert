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

void main() {
    const VertexAttributes vertex = ReadVertexAttributes();
    
    InoutAttributes result;
    result.uv[0] = UnpackUv(vertex.uv[0]);

    StoreInoutAttributes(result);

    gl_Position = Clip * vec4(vertex.pos2, 0.0f, 1.0f);
}