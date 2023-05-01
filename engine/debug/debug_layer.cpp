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
#include "render/aux_draw_canvas.hpp"
#include "render/aux_draw_manager.hpp"

namespace wmoge {

    void DebugLayer::on_start_frame() {
        WG_AUTO_PROFILE_DEBUG();

        auto engine           = Engine::instance();
        auto window           = engine->window_manager()->primary_window();
        auto canvas_debug     = engine->canvas_2d_debug();
        auto aux_draw_manager = engine->aux_draw_manager();

        canvas_debug->set_window(window);
        canvas_debug->set_viewport(Rect2i{0, 0, window->fbo_width(), window->fbo_height()});
        canvas_debug->set_screen_size(Vec2f(1280, 720));

        aux_draw_manager->set_window(window);
    }
    void DebugLayer::on_debug_draw() {
        WG_AUTO_PROFILE_DEBUG();

        auto engine           = Engine::instance();
        auto canvas_debug     = engine->canvas_2d_debug();
        auto aux_draw_manager = engine->aux_draw_manager();
        auto console          = engine->console();

        console->update();

        aux_draw_manager->render();
        console->render();
    }

}// namespace wmoge
