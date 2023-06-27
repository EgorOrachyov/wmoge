/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#ifndef WMOGE_ECS_COMPONENT_HPP
#define WMOGE_ECS_COMPONENT_HPP

#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "ecs/ecs_core.hpp"
#include "math/mat.hpp"

#include <functional>
#include <string>

namespace wmoge {

    /**
     * @class EcsComponent
     * @brief Base class for any engine ecs component
     */
    struct EcsComponent {};

    /**
     * @class EcsComponentInfo
     * @brief Holds information required to work with components
     */
    struct EcsComponentInfo {
        StringId                                                name;
        int                                                     idx  = -1;
        int                                                     size = -1;
        std::function<void(EcsComponent*)>                      create;
        std::function<void(EcsComponent*)>                      destroy;
        std::function<void(EcsComponent*, const EcsComponent*)> copy;
    };

#define WG_ECS_COMPONENT(ecs_component_class, ecs_idx)                                                         \
    static_assert(ecs_idx < EcsLimits::MAX_COMPONENTS, "Index for " #ecs_component_class " is out of limits"); \
    static constexpr int        IDX    = ecs_idx;                                                              \
    static constexpr const char NAME[] = #ecs_component_class;

}// namespace wmoge

#endif//WMOGE_ECS_COMPONENT_HPP
