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
#include "material_fs.glsl"

__SHADER_CODE_FRAGMENT__

layout (location = 0) out vec4 gbuffer_color;

void main() {
    ShaderInoutFs shaderInoutFs;
    InitShaderInoutFs(shaderInoutFs);

    Fragment(shaderInoutFs);

    gbuffer_color = shaderInoutFs.result.baseColor;
}