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

#pragma once

#include "core/buffered_vector.hpp"
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
    template<typename Component>
    struct EcsComponent {
        static int   IDX;
        static Strid NAME;

        static bool is(int id) { return id == IDX; }
        static bool is(const std::string& name) { return name == NAME.str(); }
        static bool is(const Strid& name) { return name == NAME; }

        static void bind(int id, const Strid& name) {
            IDX  = id;
            NAME = name;
        }
    };

    template<typename Component>
    int EcsComponent<Component>::IDX = -1;

    template<typename Component>
    Strid EcsComponent<Component>::NAME;

    /**
     * @class EcsComponentInfo
     * @brief Holds information required to work with components
     */
    struct EcsComponentInfo {
        Strid                             name;
        int                               idx  = -1;
        int                               size = -1;
        std::function<void(void*)>        create;
        std::function<void(void*)>        destroy;
        std::function<void(void*, void*)> swap;
    };

}// namespace wmoge
