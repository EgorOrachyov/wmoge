/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_consts.glsl"
#include "common_defines.glsl"
#include "common_funcs.glsl"
#include "gbuffer.glsl"
#include "material_fs.glsl"

__SHADER_CODE_FRAGMENT__

#ifdef MESH_PASS_FORWARD
    layout (location = 0) out vec4 out_HdrColor;
    layout (location = 1) out vec4 out_Norm;
    layout (location = 2) out int  out_primitiveId;
#endif

#ifdef MESH_PASS_GBUFFER
    layout (location = 0) out vec4 out_GBuffer0;
    layout (location = 1) out vec4 out_GBuffer1;
    layout (location = 2) out vec4 out_GBuffer2;
    layout (location = 3) out int  out_primitiveId;
#endif

#if !defined(MESH_PASS_GBUFFER) && !defined(MESH_PASS_FORWARD)
    #error "Must be specified at least one pass"
#endif

void main() {
    ShaderInoutFs shaderInoutFs;
    InitShaderInoutFs(shaderInoutFs);

    Fragment(shaderInoutFs);

    #ifdef MESH_PASS_FORWARD
        // todo
    #endif

    #ifdef MESH_PASS_GBUFFER
        const Surface result = shaderInoutFs.result;
        const GBufferValue gbuffer = PackGBufferFromSurface(result);

        out_GBuffer0 = gbuffer.value[0];
        out_GBuffer1 = gbuffer.value[1];
        out_GBuffer2 = gbuffer.value[2];
        out_primitiveId = shaderInoutFs.attributes.primitiveId;
    #endif
}