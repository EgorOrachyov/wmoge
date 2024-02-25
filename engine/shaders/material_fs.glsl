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
#include "surface.glsl"

struct ShaderInoutFs {
    InoutAttributes attributes;
    Surface result;
};

void InitShaderInoutFs(inout ShaderInoutFs fs) {
    fs.attributes = ReadInoutAttributes();
    fs.result = GetDefaultSurface();
}