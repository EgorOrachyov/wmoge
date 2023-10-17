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
#include "scene_data.glsl"

struct ShaderInoutVs {
    VertexAttributes attributes;
    InoutAttributes result;
};

void InitShaderInoutVs(inout ShaderInoutVs vs) {
    vs.attributes = ReadVertexAttributes();
}