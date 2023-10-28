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

#ifdef MESH_PASS_GBUFFER
    layout (location = 0) out vec4 out_baseColor_dummy;
    layout (location = 1) out vec4 out_worldNorm_dummy;
    layout (location = 2) out vec4 out_metallic_roughness_reflectance_ao;
    layout (location = 3) out int  out_primitiveId;
#endif

#if !defined(MESH_PASS_GBUFFER)
    #error "Must be specified at least one pass"
#endif

void main() {
    ShaderInoutFs shaderInoutFs;
    InitShaderInoutFs(shaderInoutFs);

    Fragment(shaderInoutFs);

    #ifdef MESH_PASS_GBUFFER
        const MaterialAttributes result = shaderInoutFs.result;

        out_baseColor_dummy = result.baseColor;
        out_worldNorm_dummy = vec4(result.worldNorm, 0);
        out_metallic_roughness_reflectance_ao = vec4(result.metallic, result.roughness, result.reflectance, result.ao);
        out_primitiveId = shaderInoutFs.attributes.primitiveId;
    #endif
}