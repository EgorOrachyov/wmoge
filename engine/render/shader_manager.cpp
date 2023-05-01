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

#include "shader_manager.hpp"

#include "core/engine.hpp"
#include "debug/console.hpp"
#include "shaders/generated/auto_aux_canvas_fs.hpp"
#include "shaders/generated/auto_aux_canvas_vs.hpp"
#include "shaders/generated/auto_aux_geom_fs.hpp"
#include "shaders/generated/auto_aux_geom_vs.hpp"
#include "shaders/generated/auto_aux_text_fs.hpp"
#include "shaders/generated/auto_aux_text_vs.hpp"

namespace wmoge {

    ShaderManager::ShaderManager() {
        m_shader_aux_geom = make_ref<Shader>();
        m_shader_aux_geom->set_name(SID("aux-geom"));
        m_shader_aux_geom->create_from_source(source_aux_geom_vs, source_aux_geom_fs);

        m_shader_aux_text = make_ref<Shader>();
        m_shader_aux_text->set_name(SID("aux-text"));
        m_shader_aux_text->create_from_source(source_aux_text_vs, source_aux_text_fs);

        m_shader_canvas = make_ref<Shader>();
        m_shader_canvas->set_name(SID("canvas"));
        m_shader_canvas->create_from_source(source_aux_canvas_vs, source_aux_canvas_fs);

        m_var_shader_compiler_dump = Engine::instance()->console()->register_var(SID("shader.compiler.dump"), 1, "dump compiled shaders text after per-processing to debug directory");
    }

}// namespace wmoge
