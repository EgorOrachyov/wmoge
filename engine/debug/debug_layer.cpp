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

#include "debug_layer.hpp"

#include "core/engine.hpp"
#include "debug/console.hpp"
#include "debug/profiler.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/render_engine.hpp"

namespace wmoge {

    void DebugLayer::on_start_frame() {
        WG_AUTO_PROFILE_DEBUG("DebugLayer::on_start_frame");

        auto engine           = Engine::instance();
        auto window           = engine->window_manager()->primary_window();
        auto canvas_debug     = engine->canvas_debug();
        auto aux_draw_manager = engine->aux_draw_manager();
    }
    void DebugLayer::on_debug_draw() {
        WG_AUTO_PROFILE_DEBUG("DebugLayer::on_debug_draw");

        auto engine           = Engine::instance();
        auto render_engine    = engine->render_engine();
        auto canvas_debug     = engine->canvas_debug();
        auto aux_draw_manager = engine->aux_draw_manager();
        auto console          = engine->console();

        console->update();
        console->render();

        aux_draw_manager->flush(engine->get_delta_time_game());
        render_engine->render_aux_geom(*aux_draw_manager);

        canvas_debug->compile(true);
        render_engine->render_canvas(*canvas_debug, Vec4f(0.0f, 0.0f, 1280.0f, 720.0f));

        canvas_debug->clear(false);
    }

}// namespace wmoge
