/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#version 450 core

#include "common_defines.glsl"

//@ in vec3 inPos3f;
//@ in vec3 inCol04f;
//@ in vec3 inCol14f;
//@ in vec3 inCol24f;
//@ in vec3 inCol34f;

#ifdef ATTRIB_Col04f
LAYOUT_LOCATION(0) out vec4 fsCol04f;
#endif

#ifdef ATTRIB_Col14f
LAYOUT_LOCATION(1) out vec4 fsCol14f;
#endif

#ifdef ATTRIB_Col24f
LAYOUT_LOCATION(2) out vec4 fsCol24f;
#endif

#ifdef ATTRIB_Col34f
LAYOUT_LOCATION(3) out vec4 fsCol34f;
#endif

void main() {
    #ifdef ATTRIB_Col04f
    fsCol04f = inCol04f;
    #endif

    #ifdef ATTRIB_Col14f
    fsCol14f = inCol14f;
    #endif

    #ifdef ATTRIB_Col24f
    fsCol24f = inCol24f;
    #endif

    #ifdef ATTRIB_Col34f
    fsCol34f = inCol34f;
    #endif

    gl_Position = mat_clip_proj_view * vec4(inPos3f, 1.0f);
}