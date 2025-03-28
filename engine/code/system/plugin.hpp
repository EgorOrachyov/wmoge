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

#include "core/ioc_container.hpp"
#include "core/status.hpp"

#include <functional>
#include <memory>

namespace wmoge {

    /**
     * @class Plugin
     * @brief Base class for any engine plug-in
    */
    class Plugin {
    public:
        virtual ~Plugin() = default;

        virtual Status on_register(IocContainer* ioc) { return WG_OK; }
        virtual Status on_init() { return WG_OK; }
        virtual Status on_shutdown() { return WG_OK; }
    };

    using PluginPtr = std::shared_ptr<Plugin>;

    using PluginFactory = std::function<PluginPtr()>;

}// namespace wmoge

#define WG_PLUGIN_FACTORY(plugin_class) \
    plugin_class##Factory()

#define WG_PLUGIN_ID(plugin_class) \
    SID(#plugin_class)

#define WG_PLUGIN_DECL(plugin_class)                              \
    inline PluginFactory plugin_class##Factory() {                \
        return []() { return std::make_shared<plugin_class>(); }; \
    }
