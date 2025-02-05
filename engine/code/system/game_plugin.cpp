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

#include "game_plugin.hpp"

#include "system/engine.hpp"
#include "system/engine_signals.hpp"

namespace wmoge {

    Status GamePlugin::on_register(IocContainer* ioc) {
        m_ioc    = ioc;
        m_engine = m_ioc->resolve_value<Engine>();

        EngineSignals* engine_signals = m_ioc->resolve_value<EngineSignals>();

        engine_signals->pre_update.bind([this]() {
            on_pre_update();
        });
        engine_signals->update.bind([this]() {
            on_update();
        });
        engine_signals->post_update.bind([this]() {
            on_post_update();
        });
        engine_signals->render.bind([this]() {
            on_render();
        });
        engine_signals->debug_draw.bind([this]() {
            on_debug_draw();
        });

        return WG_OK;
    }

    Status GamePlugin::on_init() {
        return WG_OK;
    }

    Status GamePlugin::on_shutdown() {
        return WG_OK;
    }

}// namespace wmoge