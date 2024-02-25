/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#include "light_grid.glsl"
#include "math.glsl"

#define GROUP_SIZE_DEFAULT 64

layout (local_size_x = GROUP_SIZE_DEFAULT, local_size_y = 1, local_size_z = 1) in;

void main() {
    const uint tid = gl_GlobalInvocationID.x;

    if (tid < GridSize) {
        return;
    }

    const uvec3 coord = GridIndexToGridCoord(tid);
    const GridCell cell = GridCellFromCoord(coord);
    const Aabb cellBox = MakeAabbFromMinMax(cell.boxMin, cell.boxMax);

    const uint numLights = GridNumLights;
    const uint lightsOffset = GetGridCellLightsOffset(tid);

    uint lightCouner = 0;

    for (uint i = 0; i < numLights && lightCouner < GridCellSize; i++) {
        Light light = GetLight(i);

        bool affects = true;

        if (light.type == LIGHT_TYPE_DIR) {

        }
        if (light.type == LIGHT_TYPE_SPOT) {
            
        }
        if (light.type == LIGHT_TYPE_POINT) {
            
        }
        if (light.type == LIGHT_TYPE_AREA) {
            
        }

        if (affects) {
            LightGridCellLights[lightsOffset + lightCouner] = i;
            lightCouner += 1;
        }
    }

    LightGridCellSizes[tid] = lightCouner;
}