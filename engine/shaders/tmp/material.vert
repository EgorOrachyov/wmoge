/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_defines.glsl"
#include "common_funcs.glsl"
#include "material_vs.glsl"

__SHADER_CODE_VERTEX__

void main() {
    ShaderInoutVs shaderInoutVs;
    InitShaderInoutVs(shaderInoutVs);

    Vertex(shaderInoutVs);

    StoreInoutAttributes(shaderInoutVs.result);

    gl_Position = ClipProjView * vec4(shaderInoutVs.result.worldPos, 1.0f);
}