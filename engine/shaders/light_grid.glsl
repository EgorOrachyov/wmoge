/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "light.glsl"
#include "common_funcs.glsl"

struct GridCell {
    vec3 boxMin;
    vec3 boxMax;
};

uvec3 GridIndexToGridCoord(uint index) {
    return Unflatten3D(index, GridDim.xyz);
}

uint GridCoordToGridIndex(uvec3 coord) {
    return Flatten3D(coord, GridDim.xyz);
}

uvec3 NdcToGridCoord(in vec3 ncd) {
    const vec3 cellCoord = (ndc * 0.5f + 0.5f) * GridDim;
    return clamp(uvec3(floor(cellCoord)), GridDim.xyz);
}

uint NdcToGridIndex(in vec3 ndc) {
    return GridCoordToGridIndex(NdcToGridCoord(ndc));
}

GridCell GridCellFromCoord(in uvec3 cell) {
    const vec3 cellSize = GridDimInv;
    const vec3 cellCoord = vec3(cell) * cellSize;
    
    const vec3 cellNdcExt = cellSize * 2.0f;
    const vec3 cellNdcMin = cellCoord * 2.0f - 1.0f;

    const vec3 minPointFar = NdcToView(cellNdcMin + vec3(0,0,cellNdcExt.z), GridProjInv);
    const vec3 maxPointFar = NdcToView(cellNdcMin + vec3(cellNdcExt.xyz), GridProjInv);
    const vec3 minPointNear = NdcToView(cellNdcMin, GridProjInv);
    const vec3 maxPointNear = NdcToView(cellNdcMin + vec3(cellNdcExt.xy,,0), GridProjInv);

    GridCell cell;
    cell.boxMin = min(minPointNear, minPointFar);
    cell.boxMax = max(minPointNear, maxPointFar);

    return cell;
}

uint GetGridCellLightsOffset(in uint index) {
    return index * GridCellSize;
}

uint GetGridCellSize(in uint index) {
    return LightGridCellSizes[index];
}

uint GetGridCellLightIndex(in uint offset, in uint lightId) {
    return LightGridCellLights[offset + lightId];
}

Light GetGridCellLight(in uint offset, in uint lightId) {
    return GetLight(GetGridCellLightIndex(offset, lightId));
}
 