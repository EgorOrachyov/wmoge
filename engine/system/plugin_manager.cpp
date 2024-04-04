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

#include "plugin_manager.hpp"

#include "core/log.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    void PluginManager::setup() {
        for (auto& plugin : m_plugins) {
            for (auto& dep : plugin->get_requirements()) {
                if (m_plugins_loaded.find(dep) == m_plugins_loaded.end()) {
                    WG_LOG_ERROR("plugin name=" << plugin->get_name() << " dep=" << dep << " not loaded");
                }
            }

            plugin->on_register();
            m_plugins_loaded.insert(plugin->get_name());
        }

        WG_LOG_INFO("register plugins");
    }

    void PluginManager::init() {
        for (auto& plugin : m_plugins) {
            plugin->on_init();
        }

        WG_LOG_INFO("init plugins");
    }

    void PluginManager::shutdown() {
        for (auto& plugin : m_plugins) {
            plugin->on_shutdown();
        }

        WG_LOG_INFO("shutdown plugins");
    }

    void PluginManager::add(std::shared_ptr<Plugin> plugin) {
        m_plugins_id[plugin->get_name()] = int(m_plugins.size());
        m_plugins.push_back(std::move(plugin));
    }

}// namespace wmoge