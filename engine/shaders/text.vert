/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#version 450 core

#include "common_funcs.glsl"

//@ in vec3 inPos3f;
//@ in vec4 inCol04f;
//@ in vec2 inUv02f;

#ifndef ATTRIB_Pos3f
#error "Pos attribute must be defined"
#endif

#ifndef ATTRIB_Col04f
#error "Col attribute must be defined"
#endif

#ifndef ATTRIB_Uv02f
#error "Uv attribute must be defined"
#endif

LAYOUT_LOCATION(0) out vec4 fsCol04f;
LAYOUT_LOCATION(1) out vec2 fsUv02f;

void main() {
    fsCol04f = inCol04f;
    fsUv02f = unpack_uv(inUv02f);
    gl_Position = mat_clip_proj_screen * vec4(inPos3f, 1.0f);
}