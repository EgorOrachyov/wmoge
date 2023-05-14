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

#include "render_engine.hpp"

#include "memory/mem_pool.hpp"
#include "render/draw_cmd.hpp"
#include "render/draw_primitive.hpp"
#include "render/shader_cache.hpp"
#include "render/shader_manager.hpp"

namespace wmoge {

    RenderEngine::RenderEngine() {
        m_shader_manager = std::make_unique<ShaderManager>();
        m_shader_cache   = std::make_unique<ShaderCache>();
        m_pool_draw_cmd  = std::make_unique<MemPool>(sizeof(DrawCmd), MemPool::EXPAND_SIZE);
    }

    RenderEngine::~RenderEngine() = default;

    void RenderEngine::update() {
        m_queue.flush();
    }

    Ref<RenderCamera2d> RenderEngine::make_camera_2d() {
        return make_ref<RenderCamera2d>();
    }
    Ref<RenderCanvasText> RenderEngine::make_canvas_text() {
        return make_ref<RenderCanvasText>();
    }
    Ref<RenderSpriteInstance> RenderEngine::make_sprite_instance() {
        return make_ref<RenderSpriteInstance>();
    }
    Ref<RenderParticles2d> RenderEngine::make_particles_2d() {
        return make_ref<RenderParticles2d>();
    }

    DrawCmd* RenderEngine::allocate_draw_cmd() {
        std::lock_guard guard(m_mutex);
        return new (m_pool_draw_cmd->allocate()) DrawCmd();
    }
    void RenderEngine::free_draw_cmd(DrawCmd* cmd) {
        std::lock_guard guard(m_mutex);
        cmd->~DrawCmd();
        m_pool_draw_cmd->free(cmd);
    }

    class ShaderManager* RenderEngine::get_shader_manager() {
        return m_shader_manager.get();
    }
    class ShaderCache* RenderEngine::get_shader_cache() {
        return m_shader_cache.get();
    }
    CallbackQueue* RenderEngine::get_queue() {
        return &m_queue;
    }

}// namespace wmoge