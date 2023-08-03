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
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "platform/file_system.hpp"
#include "render/shader_manager.hpp"

namespace wmoge {

    bool yaml_read(const YamlConstNodeRef& node, ShaderParameter& parameter) {
        WG_YAML_READ_AS(node, "name", parameter.name);
        WG_YAML_READ_AS(node, "type", parameter.type);
        WG_YAML_READ_AS_OPT(node, "offset", parameter.offset);
        WG_YAML_READ_AS_OPT(node, "size", parameter.size);
        WG_YAML_READ_AS_OPT(node, "value", parameter.value);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const ShaderParameter& parameter) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "name", parameter.name);
        WG_YAML_WRITE_AS(node, "type", parameter.type);
        WG_YAML_WRITE_AS(node, "offset", parameter.offset);
        WG_YAML_WRITE_AS(node, "size", parameter.size);
        WG_YAML_WRITE_AS(node, "value", parameter.value);

        return true;
    }

    bool yaml_read(const YamlConstNodeRef& node, ShaderTexture& texture) {
        WG_YAML_READ_AS(node, "name", texture.name);
        WG_YAML_READ_AS(node, "type", texture.type);
        WG_YAML_READ_AS_OPT(node, "id", texture.id);
        WG_YAML_READ_AS_OPT(node, "value", texture.value);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const ShaderTexture& texture) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "name", texture.name);
        WG_YAML_WRITE_AS(node, "type", texture.type);
        WG_YAML_WRITE_AS(node, "id", texture.id);
        WG_YAML_WRITE_AS(node, "value", texture.value);

        return true;
    }

    bool yaml_read(const YamlConstNodeRef& node, ShaderPipelineState& state) {
        WG_YAML_READ_AS_OPT(node, "poly_mode", state.poly_mode);
        WG_YAML_READ_AS_OPT(node, "cull_mode", state.cull_mode);
        WG_YAML_READ_AS_OPT(node, "front_face", state.front_face);
        WG_YAML_READ_AS_OPT(node, "depth_enable", state.depth_enable);
        WG_YAML_READ_AS_OPT(node, "depth_write", state.depth_write);
        WG_YAML_READ_AS_OPT(node, "depth_func", state.depth_func);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const ShaderPipelineState& state) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "poly_mode", state.poly_mode);
        WG_YAML_WRITE_AS(node, "cull_mode", state.cull_mode);
        WG_YAML_WRITE_AS(node, "front_face", state.front_face);
        WG_YAML_WRITE_AS(node, "depth_enable", state.depth_enable);
        WG_YAML_WRITE_AS(node, "depth_write", state.depth_write);
        WG_YAML_WRITE_AS(node, "depth_func", state.depth_func);

        return true;
    }

    bool yaml_read(const YamlConstNodeRef& node, ShaderFile& file) {
        WG_YAML_READ_AS_OPT(node, "parameters", file.parameters);
        WG_YAML_READ_AS_OPT(node, "textures", file.textures);
        WG_YAML_READ_AS_OPT(node, "keywords", file.keywords);
        WG_YAML_READ_AS_OPT(node, "vertex", file.vertex);
        WG_YAML_READ_AS_OPT(node, "fragment", file.fragment);
        WG_YAML_READ_AS_OPT(node, "compute", file.compute);
        WG_YAML_READ_AS(node, "domain", file.domain);
        WG_YAML_READ_AS_OPT(node, "render_queue", file.render_queue);
        WG_YAML_READ_AS_OPT(node, "state", file.state);

        return true;
    }
    bool yaml_write(YamlNodeRef node, const ShaderFile& file) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "parameters", file.parameters);
        WG_YAML_WRITE_AS(node, "textures", file.textures);
        WG_YAML_WRITE_AS(node, "keywords", file.keywords);
        WG_YAML_WRITE_AS(node, "vertex", file.vertex);
        WG_YAML_WRITE_AS(node, "fragment", file.fragment);
        WG_YAML_WRITE_AS(node, "compute", file.compute);
        WG_YAML_WRITE_AS(node, "domain", file.domain);
        WG_YAML_WRITE_AS(node, "render_queue", file.render_queue);
        WG_YAML_WRITE_AS(node, "state", file.state);

        return true;
    }

    bool Shader::load_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("Shader::load_from_import_options");

        ShaderFile shader_file;
        WG_YAML_READ(node, shader_file);

        m_domain         = shader_file.domain;
        m_render_queue   = shader_file.render_queue;
        m_vertex         = shader_file.vertex;
        m_fragment       = shader_file.fragment;
        m_compute        = shader_file.compute;
        m_pipeline_state = shader_file.state;

        for (auto& keyword : shader_file.keywords) {
            m_keywords.insert(std::move(keyword));
        }
        for (auto& parameter : shader_file.parameters) {
            m_parameters[parameter.name] = std::move(parameter);
        }
        for (auto& texture : shader_file.textures) {
            m_textures[texture.name] = std::move(texture);
        }

        if (!generate_params_layout()) return false;
        if (!generate_textures_layout()) return false;

        return true;
    }

    void Shader::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto shader                  = dynamic_cast<Shader*>(&copy);
        shader->m_vertex             = m_vertex;
        shader->m_fragment           = m_fragment;
        shader->m_compute            = m_compute;
        shader->m_include_parameters = m_include_parameters;
        shader->m_include_textures   = m_include_textures;
        shader->m_parameters_size    = m_parameters_size;
        shader->m_parameters         = m_parameters;
        shader->m_textures           = m_textures;
        shader->m_domain             = m_domain;
        shader->m_render_queue       = m_render_queue;
        shader->m_pipeline_state     = m_pipeline_state;
        shader->m_keywords           = m_keywords;
    }

    bool Shader::has_variant(const StringId& key) {
        std::lock_guard lock(m_mutex);
        auto            query = m_variants.find(key);
        return query != m_variants.end();
    }
    Ref<GfxShader> Shader::find_variant(const StringId& shader_key) {
        std::lock_guard lock(m_mutex);
        auto            query = m_variants.find(shader_key);
        return query != m_variants.end() ? query->second : Ref<GfxShader>{};
    }
    Ref<GfxShader> Shader::create_variant(const fast_vector<std::string>& defines) {
        return create_variant({}, defines);
    }
    Ref<GfxShader> Shader::create_variant(const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines) {
        WG_AUTO_PROFILE_RESOURCE("Shader::create_variant");

        Engine*        engine         = Engine::instance();
        ShaderManager* shader_manager = engine->shader_manager();

        StringId shader_key = shader_manager->make_shader_key(m_domain, streams, defines, this);

        Ref<GfxShader> shader_variant = find_variant(shader_key);
        if (shader_variant) {
            return shader_variant;
        }

        shader_variant = shader_manager->find(shader_key);
        if (shader_variant) {
            std::lock_guard lock(m_mutex);
            m_variants[shader_key] = shader_variant;

            return shader_variant;
        }

        shader_variant = shader_manager->get_shader(m_domain, streams, defines, this);
        if (shader_variant) {
            std::lock_guard lock(m_mutex);
            m_variants[shader_key] = shader_variant;

            return shader_variant;
        }

        WG_LOG_ERROR("failed to create shader variant " << shader_key);
        return Ref<GfxShader>();
    }

    const std::string& Shader::get_vertex() const {
        return m_vertex;
    }
    const std::string& Shader::get_fragment() const {
        return m_fragment;
    }
    const std::string& Shader::get_compute() const {
        return m_fragment;
    }

    const StringId& Shader::get_domain() const {
        return m_domain;
    }
    int Shader::get_render_queue() const {
        return m_render_queue;
    }
    const fast_set<StringId>& Shader::get_keywords() const {
        return m_keywords;
    }
    const fast_map<StringId, ShaderParameter>& Shader::get_parameters() const {
        return m_parameters;
    }
    const fast_map<StringId, ShaderTexture>& Shader::get_textures() const {
        return m_textures;
    }
    const ShaderPipelineState& Shader::get_pipeline_state() const {
        return m_pipeline_state;
    }
    int Shader::get_parameters_size() const {
        return m_parameters_size;
    }
    int Shader::get_parameters_count() const {
        return int(m_parameters.size());
    }
    int Shader::get_textures_count() const {
        return int(m_textures.size());
    }
    const std::string& Shader::get_include_parameters() const {
        return m_include_parameters;
    }
    const std::string& Shader::get_include_textures() const {
        return m_include_textures;
    }

    bool Shader::generate_params_layout() {
        if (m_parameters.empty()) {
            return true;
        }

        int               total_size = 0;
        int               pad_count  = 0;
        std::stringstream params_declaration;

        params_declaration << "LAYOUT_BUFFER(DRAW_SET_PER_MATERIAL, MAT_BINDING_PARAMS) uniform MaterialParameters {\n";
        for (auto& entry : m_parameters) {
            auto& parameter = entry.second;

            params_declaration << "    ";

            switch (parameter.type) {
                case GfxShaderParam::Int:
                    params_declaration << "int " << parameter.name.str() << "; int3 _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = sizeof(int);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case GfxShaderParam::Float:
                    params_declaration << "float " << parameter.name.str() << "; vec3 _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = sizeof(float);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case GfxShaderParam::Vec2:
                    params_declaration << "vec2 " << parameter.name.str() << "; vec2 _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = 2 * sizeof(float);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case GfxShaderParam::Vec3:
                    params_declaration << "vec3 " << parameter.name.str() << "; float _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = 3 * sizeof(float);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case GfxShaderParam::Vec4:
                    params_declaration << "vec4 " << parameter.name.str() << "\n";
                    parameter.offset = total_size;
                    parameter.size   = 4 * sizeof(float);
                    total_size += 4 * sizeof(float);
                    break;
                default:
                    WG_LOG_ERROR("unknown type of parameter");
                    return false;
            }
        }
        params_declaration << "};\n\n";

        m_include_parameters = params_declaration.str();
        m_parameters_size    = total_size;
        return true;
    }
    bool Shader::generate_textures_layout() {
        if (m_textures.empty()) {
            return true;
        }

        int               total_count = 0;
        std::stringstream tex_declaration;

        for (auto& entry : m_textures) {
            auto& texture = entry.second;

            tex_declaration << "LAYOUT_SAMPLER(DRAW_SET_PER_MATERIAL, MAT_BINDING_TEX + " << total_count << ") uniform ";

            switch (texture.type) {
                case GfxTex::Tex2d:
                    tex_declaration << "sampler2D " << texture.name.str();
                    break;
                case GfxTex::Tex2dArray:
                    tex_declaration << "sampler2DArray " << texture.name.str();
                    break;
                case GfxTex::TexCube:
                    tex_declaration << "samplerCube " << texture.name.str();
                    break;
                default:
                    WG_LOG_ERROR("unknown texture type");
                    return false;
            }

            tex_declaration << ";\n";
            texture.id = total_count;
            total_count += 1;
        }

        tex_declaration << "\n";

        m_include_textures = tex_declaration.str();
        return true;
    }

    void Shader::register_class() {
        auto* cls = Class::register_class<Shader>();
    }

}// namespace wmoge
