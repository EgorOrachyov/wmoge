/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "surface.glsl"

struct GBufferValue {
    vec4 value[3];
};

GBufferValue GetDefaultGBufferValue() {
    GBufferValue v;
    v.value[0] = vec4(0,0,0,0);
    v.value[1] = vec4(0,0,0,0);
    v.value[2] = vec4(0,0,0,0);
    return v;
}

GBufferValue PackGBufferFromSurface(in Surface surface) {
    // Encoded attributes
    // - vec3  baseColor;
    // - vec3  emissiveColor;
    // - float metallic;
    // - float roughness;
    // - float reflectance;
    // - float ao;

    GBufferValue v;
    v.value[0] = vec4(surface.baseColor, 0.0f);
    v.value[1] = vec4(surface.emissiveColor, 0.0f);
    v.value[2] = vec4(surface.metallic, surface.roughness, surface.reflectance, surface.ao);
    return v;
}

Surface UnpackSurfaceFromGBuffer(in GBufferValue gbuffer) {
    // Decoded attributes
    // - vec3  baseColor;
    // - vec3  emissiveColor;
    // - float metallic;
    // - float roughness;
    // - float reflectance;
    // - float ao;

    Surface surface;
    surface.baseColor       = gbuffer.value[0].rgb;
    surface.emissiveColor   = gbuffer.value[1].rgb;
    surface.metallic        = gbuffer.value[2].r;
    surface.roughness       = gbuffer.value[2].g;
    surface.reflectance     = gbuffer.value[2].b;
    surface.ao              = gbuffer.value[2].a;
    return surface;
}