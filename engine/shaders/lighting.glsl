/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "light.glsl"
#include "light_grid.glsl"
#include "brdf.glsl"
#include "surface.glsl"

float CalcF0(float IOR) {
    return ((IOR - 1) * (IOR - 1)) / ((IOR + 1) * (IOR + 1));
}

vec3 CalcF0(vec3 baseColor, float reflectance, float metallic) {
    return 0.16 * reflectance * reflectance * (1.0 - metallic) + baseColor * metallic;
}

vec3 CalcF90() {
    return vec3(1,1,1);
}

vec3 CaclDiffuseColor(vec3 baseColor, float metallic) {
    return baseColor * (1.0f - metallic);
}

float RemapPerceptualRoughness(float roughness) {
    return roughness * roughness;
}

BrdfParams GetBrdfParamsFromSurface(in Surface surface) {
    BrdfParams params;
    params.diffuseColor = CaclDiffuseColor(surface.baseColor, surface.metallic);
    params.f0 = CalcF0(surface.baseColor, surface.reflectance, surface.metallic);
    params.roughness = RemapPerceptualRoughness(surface.roughness);
    return params;
}

struct LightResult {
    vec3 irradiance;
};

LightResult CalcLight(in Light light, in BrdfParams brdfParams, in vec3 P, in vec3 N, in vec3 V, in float NdotV) {
    const float d = length(light.WorldPos - P);
    const vec3 L = normalize(light.WorldPos - P);
    const float NdotL = Saturate(dot(N, L));

    BrdfResult brdf = CalcBrdf(BrdfParams, N, V, L, NdotV, NdotL);

    LightResult result;
    result.irradiance = (brdf.diffuse +  brdf.specular) * NdotL * pow(1.0f - pow(d/5, 2.0f), 2.0f);
    
    return result;
}

LightResult CalcLight(in vec2 pixel, in vec3 ndcPos, in vec3 worldPos, in Surface surface, in vec3 V) {
    const BrdfParams brdfParams = GetBrdfParamsFromSurface(surface);

    const vec3 P = surface.worldPos;
    const vec3 N = surface.worldNorm;
    const float NdotV = Saturate(abs(dot(N, V)) + 1e-5);

    const uvec gridIndex = NdcToGridIndex(ndcPos);
    const uint numLights = GetGridCellSize(gridIndex);
    const uint lightsOffset = GetGridCellLightsOffset(gridIndex);

    LightResult totalResult;
    totalResult.irradiance = 0;

    for (uint i = 0; i < numLights; i++) {
        const Light light = GetGridCellLight(lightsOffset, i);
        totalResult.irradiance += CalcLight(light, brdfParams, P, N, V, NdotV);
    }

    return totalResult;
}