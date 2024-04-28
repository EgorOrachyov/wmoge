/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/**********************************************************************************/

#define TARGET_VULKAN

#if defined(TARGET_VULKAN)
#define LAYOUT_LOCATION(idx) layout(location = idx)
#else
#define LAYOUT_LOCATION(idx)
#endif

#if defined(TARGET_VULKAN)
    #define LAYOUT_BUFFER(set_idx, binding_idx, fields_layout) layout(set = set_idx, binding = binding_idx, fields_layout)
#else
    #define LAYOUT_BUFFER(set_idx, binding_idx, fields_layout) layout(fields_layout)
#endif

#if defined(TARGET_VULKAN)
#define LAYOUT_SAMPLER(set_idx, binding_idx) layout(set = set_idx, binding = binding_idx)
#else
#define LAYOUT_SAMPLER(set_idx, binding_idx)
#endif