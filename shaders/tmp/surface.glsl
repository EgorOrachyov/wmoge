/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

struct Surface {
    vec3 worldPos;
    vec3 worldNorm;
    vec3 baseColor;
    vec3 emissiveColor;
    float metallic;
    float roughness;
    float reflectance;
    float ao;
};

Surface GetDefaultSurface() {
    Surface s;
    s.worldPos = vec3(0,0,0);
    s.worldNorm = vec3(0,1,0);
    s.baseColor = vec3(1,1,1);
    s.emissiveColor = vec3(0,0,0);
    s.metallic = 0;
    s.roughness = 0;
    s.reflectance = 0;
    s.ao = 0;
    return s;
}