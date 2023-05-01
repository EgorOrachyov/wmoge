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

#include "shader_builder.hpp"

#include "core/engine.hpp"
#include "debug/console.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"
#include "render/render_engine.hpp"
#include "render/shader_cache.hpp"
#include "render/shader_manager.hpp"
#include "resource/shader.hpp"

namespace wmoge {

    ShaderBuilder& ShaderBuilder::set_shader(Shader* shader) {
        m_shader = shader;
        return *this;
    }
    ShaderBuilder& ShaderBuilder::set_key(StringId key) {
        m_key = key;
        return *this;
    }
    ShaderBuilder& ShaderBuilder::set_attribs(MeshAttribs mesh_attribs) {
        m_mesh_attribs = mesh_attribs;
        return *this;
    }
    ShaderBuilder& ShaderBuilder::set_material_bindings(int first_buffer, int first_texture) {
        m_mat_first_buffer  = first_buffer;
        m_mat_first_texture = first_texture;
        return *this;
    }
    ShaderBuilder& ShaderBuilder::add_define(const std::string& define) {
        add_define_vs(define);
        add_define_fs(define);
        return *this;
    }
    ShaderBuilder& ShaderBuilder::add_defines(const fast_vector<std::string>& defines) {
        for (const auto& d : defines) add_define(d);
        return *this;
    }
    ShaderBuilder& ShaderBuilder::add_define_vs(const std::string& define) {
        m_vertex << "#define " << define << "\n";
        return *this;
    }
    ShaderBuilder& ShaderBuilder::add_define_fs(const std::string& define) {
        m_fragment << "#define " << define << "\n";
        return *this;
    }
    ShaderBuilder& ShaderBuilder::add_vertex_module(const std::string& code) {
        m_vertex << code;
        return *this;
    }
    ShaderBuilder& ShaderBuilder::add_fragment_module(const std::string& code) {
        m_fragment << code;
        return *this;
    }
    bool ShaderBuilder::compile() {
        WG_AUTO_PROFILE_RENDER();

        Engine*        engine         = Engine::instance();
        RenderEngine*  render_engine  = engine->render_engine();
        ShaderCache*   shader_cache   = render_engine->get_shader_cache();
        ShaderManager* shader_manager = render_engine->get_shader_manager();

        std::string        source = m_vertex.str() + m_fragment.str();
        ref_ptr<GfxShader> shader = shader_cache->find(source);

        if (!shader) {
            StringId gfx_name(m_shader->get_name().str() + "_" + m_key.str());
            shader = Engine::instance()->gfx_driver()->make_shader(m_vertex.str(), m_fragment.str(), gfx_name);
            shader_cache->cache(source, shader);

            if (shader_manager->get_var_shader_compiler_dump()->as_int()) {
                FileSystem* file_system = engine->file_system();

                const std::string file_common  = "debug://shader_" + m_shader->get_name().str() + "_" + m_key.str();
                const std::string file_path_vs = file_common + "_vs.glsl";
                const std::string file_path_fs = file_common + "_fs.glsl";

                WG_LOG_INFO("try to dump shader " << file_path_vs);
                WG_LOG_INFO("try to dump shader " << file_path_fs);

                if (!file_system->save_file(file_path_vs, m_vertex.str())) {
                    WG_LOG_WARNING("failed to save file " << file_path_vs);
                }
                if (!file_system->save_file(file_path_fs, m_fragment.str())) {
                    WG_LOG_WARNING("failed to save file " << file_path_fs);
                }
            }
        }

        m_variant.m_material_first_buffer  = m_mat_first_buffer;
        m_variant.m_material_first_texture = m_mat_first_texture;
        m_variant.m_owner                  = m_shader;
        m_variant.m_key                    = m_key;
        m_variant.m_hash                   = std::hash<std::string>()(source);
        m_variant.m_gfx_shader             = shader;
        return true;
    }
    ShaderVariant& ShaderBuilder::get_variant() {
        return m_variant;
    }
    MeshAttribs ShaderBuilder::get_mesh_attribs() {
        return m_mesh_attribs;
    }

}// namespace wmoge