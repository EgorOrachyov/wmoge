/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "common_consts.glsl"
#include "common_funcs.glsl"
#include "common_structs.glsl"

float D_GGX(float NdotH, float roughness) {
    const float a = NdotH * roughness;
    const float k = roughness / (1.0 - NdotH * NdotH + a * a);
    return k * k * (1.0 / PI);
}

float K_SchlickGGXDirect(float roughness) {
    return (roughness + 1) * (roughness + 1) / 8;
}

float K_SchlickGGXIBL(float roughness) {
    return roughness * roughness / 2;
}

float G_SchlickGGX(float NdotX, float k) {
    return (NdotX) / (NdotX * (1 - k) + k);
}

float G_SmithGGX(vec3 NdotV, vec3 NdotL, float k) {
    const float GGX1 = G_SchlickGGX(NdotV, k);
    const float GGX2 = G_SchlickGGX(NdotL, k);
    return GGX1 * GGX2;
}

float V_SmithGGXCorrelated(float NdotV, float NdotL, float roughness) {
    const float a2 = roughness * roughness;
    const float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - a2) + a2);
    const float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - a2) + a2);
    return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(float VdotH, vec3 f0) {
    const float f = pow(1.0f - VdotH, 5.0f);
    return f + f0 * (1.0f - f);
}

float F_Schlick(float VdotH, vec3 f0, vec3 f90) {
    return f0 + (f90 - f0) * pow(1.0f - VdotH, 5.0f);
}

float Fd_Lambert() {
    return 1.0 / PI;
}

float Fd_Burley(float NdotV, float NdotL, float LdotH, float roughness) {
    const float f90 = 0.5 + 2.0 * roughness * LdotH * LdotH;
    const float lightScatter = F_Schlick(NdotL, 1.0, f90);
    const float viewScatter = F_Schlick(NdotV, 1.0, f90);
    return lightScatter * viewScatter * (1.0 / PI);
}

struct BrdfParams {
    vec3 diffuseColor;
    vec3 f0;
    float roughness;
};

struct BrdfResult {
    vec3 diffuse;
    vec3 specular;
};

BrdfResult CalcBrdf(in BrdfParams params, vec3 N, vec3 V, vec3 L, float NdotV, float NdotL) {
    BrdfResult result;
    result.diffuse = 0.0f;
    result.specular = 0.0f;

    const vec3  H     = normalize(V + L);
    const float NdotH = Saturate(dot(N, H));
    const float VdotH = Saturate(dot(V, H));

    const float D = D_GGX(NdotH, params.roughness);
    const float V = V_SmithGGXCorrelated(NdotV, NdotL, params.roughness);
    const vec3  F = F_Schlick(VdotH, params.f0);

    const vec3 Es = F;
    const vec3 Ed = (1 - Es);

    const vec3 Fs = (D * V) * F;
    const vec3 Fd = params.diffuseColor * Fd_Lambert();

    result.diffuse = Fd * Ed;
    result.specular = Fs * Es;

    return result;
}
