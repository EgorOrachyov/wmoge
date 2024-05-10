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

#include "ecs_core.hpp"

#include "ecs/ecs_registry.hpp"
#include "system/ioc_container.hpp"

#include <sstream>
#include <string>

namespace wmoge {

    std::string EcsArch::to_string() const {
        if (!any()) {
            return "'empty'";
        }

        EcsRegistry* registry = IocContainer::iresolve_v<EcsRegistry>();

        std::stringstream stream;

        const int total_components = int(count());

        stream << "(";
        stream << "count=" << total_components << ":";

        for (int i = 0; i < EcsLimits::MAX_COMPONENTS; i++) {
            if (test(i)) {
                const Strid& name = registry->get_component_info(i).name;
                stream << name << ",";
            }
        }

        stream << ")";
        return stream.str();
    }

    std::string EcsQuery::to_string() const {
        EcsRegistry* registry = IocContainer::iresolve_v<EcsRegistry>();

        if (!read.any() && !write.any()) {
            return "'empty'";
        }

        std::stringstream stream;

        const auto affected       = read | write;
        const int  total_affected = int(affected.count());

        stream << "<";
        stream << "count=" << total_affected << ":";

        for (int i = 0; i < EcsLimits::MAX_COMPONENTS; i++) {
            if (affected.test(i)) {
                const Strid& name = registry->get_component_info(i).name;
                stream << (write.test(i) ? "rw-" : "r-") << name << ",";
            }
        }

        stream << ">";
        return stream.str();
    }

}// namespace wmoge