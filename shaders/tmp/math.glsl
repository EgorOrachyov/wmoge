/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

struct Aabb {
    vec3 center; // Box center (symmetrical)
    vec3 extent; // Half size of the box edges
};

Aabb MakeAabb(in vec3 center, in vec3 extent) {
    Aabb box;
    box.center = center;
    box.extent = extent;
    return box;
}

Aabb MakeAabbDefault() {
    return MakeAabb(0,0);
}

Aabb MakeAabbFromMinMax(in vec3 boxMin, in vec3 boxMax) {
    return MakeAabb((boxMin + boxMax) * 0.5f, (boxMax - boxMin) * 0.5f);
}