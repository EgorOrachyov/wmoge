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

#ifndef WMOGE_RENDER_ENGINE_HPP
#define WMOGE_RENDER_ENGINE_HPP

#include "core/callback_queue.hpp"
#include "render/objects/render_camera_2d.hpp"
#include "render/objects/render_canvas_item.hpp"
#include "render/objects/render_canvas_text.hpp"
#include "render/objects/render_particles_2d.hpp"
#include "render/objects/render_sprite_instance.hpp"

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace wmoge {

    /**
     * @class RenderEngine
     * @brief Engine is responsible for high-level rendering management
     */
    class RenderEngine {
    public:
        RenderEngine();
        ~RenderEngine();

        void update();

        /** Object creation api */
        Ref<RenderCamera2d>       make_camera_2d();
        Ref<RenderCanvasText>     make_canvas_text();
        Ref<RenderSpriteInstance> make_sprite_instance();
        Ref<RenderParticles2d>    make_particles_2d();

        /** Mem management for persistent draw cmds (must be deleted manually) */
        struct DrawCmd* allocate_draw_cmd();
        void            free_draw_cmd(struct DrawCmd* cmd);

        class ShaderManager* get_shader_manager();
        class ShaderCache*   get_shader_cache();
        CallbackQueue*       get_queue();

    private:
        CallbackQueue m_queue;

        std::unique_ptr<class ShaderManager> m_shader_manager;
        std::unique_ptr<class ShaderCache>   m_shader_cache;
        std::unique_ptr<class MemPool>       m_pool_draw_cmd;

        std::mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_ENGINE_HPP
