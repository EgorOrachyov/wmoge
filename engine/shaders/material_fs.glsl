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

struct MaterialAttributes {
    vec4 baseColor;
};

struct ShaderInoutFs {
    InoutAttributes attributes;
    MaterialAttributes result;
};

void InitShaderInoutFs(inout ShaderInoutFs fs) {
    fs.attributes = ReadInoutAttributes();
}