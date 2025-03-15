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

#include "edt_app.hpp"

#include "core/ioc_container.hpp"
#include "system/edt_editor.hpp"
#include "system/engine_signals.hpp"

namespace wmoge {

    static void bind_globals(IocContainer* ioc) {
        ioc->bind_by_ioc<EdtEditor>();
    }

    static void unbind_globals(IocContainer* ioc) {
    }

    static void bind_rtti(IocContainer* ioc) {
    }

    EdtApplication::EdtApplication(EdtApplicationConfig& config)
        : EngineApplication(*config.app_config),
          m_edt_config(config) {

        if (config.game_plugin) {
            config.app_config->plugins.push_back(config.game_plugin);
        }
        for (auto& plugin : config.plugins) {
            config.app_config->plugins.push_back(plugin);
        }

        EngineSignals* signals = m_engine_config.signals;

        signals->setup.bind([this]() {
            bind_globals(m_config.ioc);
            bind_rtti(m_config.ioc);

            m_editor = m_config.ioc->resolve_value<EdtEditor>();
            m_editor->setup();
        });

        signals->init.bind([this]() {
            m_editor->init();
        });

        signals->shutdown.bind([this]() {
            m_editor->shutdown();
            unbind_globals(m_config.ioc);
        });
    }

}// namespace wmoge