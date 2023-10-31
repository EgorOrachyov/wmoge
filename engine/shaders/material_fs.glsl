/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "inout_attributes.glsl"
#include "scene_data.glsl"
#include "color.glsl"

struct MaterialAttributes {
    vec4 baseColor;
    vec3 worldNorm;
    vec3 emissiveColor;
    float metallic;
    float roughness;
    float reflectance;
    float ao;
};

struct ShaderInoutFs {
    InoutAttributes attributes;
    MaterialAttributes result;
};

MaterialAttributes GetDefaultMaterialAttributes() {
    MaterialAttributes attributes;
    attributes.baseColor = vec4(1,1,1,1);
    attributes.worldNorm = vec3(0,0,1);
    attributes.emissiveColor = vec3(0,0,0);
    attributes.metallic = 0;
    attributes.roughness = 0;
    attributes.reflectance = 0;
    attributes.ao = 0;

    return attributes;
}

void InitShaderInoutFs(inout ShaderInoutFs fs) {
    fs.attributes = ReadInoutAttributes();
    fs.result = GetDefaultMaterialAttributes();
}