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

#include "shader.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"
#include "render/shader_builder.hpp"

namespace wmoge {

    bool Shader::create_from_source(const std::string& vertex, const std::string& fragment) {
        WG_AUTO_PROFILE_RESOURCE("Shader::create_from_source");

        assert(!vertex.empty());
        assert(!fragment.empty());

        m_vertex   = vertex;
        m_fragment = fragment;
        return true;
    }

    bool Shader::load_from_import_options(const YamlTree& tree) {
        WG_AUTO_PROFILE_RESOURCE("Shader::load_from_import_options");

        auto        params = tree["params"];
        std::string vertex_code_path;
        std::string fragment_code_path;

        if (params.has_child("vertex_code")) params["vertex_code"] >> vertex_code_path;
        if (params.has_child("fragment_code")) params["fragment_code"] >> fragment_code_path;

        FileSystem* file_system = Engine::instance()->file_system();

        if (!vertex_code_path.empty() && !file_system->read_file(vertex_code_path, m_vertex)) {
            WG_LOG_ERROR("failed to read shader " << vertex_code_path);
            return false;
        }
        if (!fragment_code_path.empty() && !file_system->read_file(fragment_code_path, m_fragment)) {
            WG_LOG_ERROR("failed to read shader " << fragment_code_path);
            return false;
        }

        if (m_vertex.empty()) {
            params["vertex"] >> m_vertex;
        }
        if (m_fragment.empty()) {
            params["fragment"] >> m_fragment;
        }

        return true;
    }
    void Shader::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto shader        = dynamic_cast<Shader*>(&copy);
        shader->m_vertex   = m_vertex;
        shader->m_fragment = m_fragment;
    }

    bool Shader::has_variant(const StringId& key) {
        std::lock_guard lock(m_mutex);
        auto            query = m_variants.find(key);
        return query != m_variants.end();
    }
    ShaderVariant* Shader::find_variant(const StringId& key) {
        std::lock_guard lock(m_mutex);
        auto            query = m_variants.find(key);
        return query != m_variants.end() ? &query->second : nullptr;
    }
    ShaderVariant* Shader::create_variant(const fast_vector<std::string>& defines) {
        return Shader::create_variant({}, defines);
    }
    ShaderVariant* Shader::create_variant(MeshAttribs mesh_attribs, fast_vector<std::string> defines) {
        WG_AUTO_PROFILE_RESOURCE("Shader::create_variant");

        if (mesh_attribs.get(MeshAttrib::Position))
            defines.push_back("MESH_POSITION");
        if (mesh_attribs.get(MeshAttrib::Normal))
            defines.push_back("MESH_NORMAL");
        if (mesh_attribs.get(MeshAttrib::Tangent))
            defines.push_back("MESH_TANGENT");
        if (mesh_attribs.get(MeshAttrib::BoneIds))
            defines.push_back("MESH_BONEIDS");
        if (mesh_attribs.get(MeshAttrib::BoneWeights))
            defines.push_back("MESH_BONEWEIGHTS");
        if (mesh_attribs.get(MeshAttrib::Color0))
            defines.push_back("MESH_COLOR0");
        if (mesh_attribs.get(MeshAttrib::Color0))
            defines.push_back("MESH_COLOR1");
        if (mesh_attribs.get(MeshAttrib::Color0))
            defines.push_back("MESH_COLOR2");
        if (mesh_attribs.get(MeshAttrib::Color0))
            defines.push_back("MESH_COLOR3");
        if (mesh_attribs.get(MeshAttrib::Uv0))
            defines.push_back("MESH_UV0");
        if (mesh_attribs.get(MeshAttrib::Uv1))
            defines.push_back("MESH_UV1");
        if (mesh_attribs.get(MeshAttrib::Uv2))
            defines.push_back("MESH_UV2");
        if (mesh_attribs.get(MeshAttrib::Uv3))
            defines.push_back("MESH_UV3");

        std::stringstream key_builder;
        for (const auto& def : defines) key_builder << def << "_";
        StringId key(key_builder.str());

        std::lock_guard lock(m_mutex);

        auto query = m_variants.find(key);
        if (query != m_variants.end()) {
            return &query->second;
        }

        ShaderBuilder builder;
        builder
                .set_shader(this)
                .set_key(key)
                .set_attribs(mesh_attribs)
                .add_vertex_module("#version 450 core\n")
                .add_fragment_module("#version 450 core\n")
                .add_defines(defines);

        on_build(builder);

        if (!builder.compile()) {
            WG_LOG_ERROR("failed to compile variant of " << get_name() << " key: " << key);
            return nullptr;
        }

        m_variants[key] = std::move(builder.get_variant());
        return &m_variants[key];
    }

    const std::string& Shader::get_vertex() {
        return m_vertex;
    }
    const std::string& Shader::get_fragment() {
        return m_fragment;
    }

    void Shader::on_build(ShaderBuilder& builder) {
        builder
                .add_vertex_module(m_vertex)
                .add_fragment_module(m_fragment);
    }

    void Shader::register_class() {
        auto* cls = Class::register_class<Shader>();
    }

}// namespace wmoge
