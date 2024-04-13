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
#include "debug/profiler.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "platform/file_system.hpp"
#include "render/shader_manager.hpp"
#include "system/engine.hpp"

namespace wmoge {

    WG_IO_BEGIN(ShaderParameter)
    WG_IO_FIELD(name)
    WG_IO_FIELD(type)
    WG_IO_FIELD_OPT(offset)
    WG_IO_FIELD_OPT(size)
    WG_IO_FIELD_OPT(value)
    WG_IO_END(ShaderParameter)

    WG_IO_BEGIN(ShaderTexture)
    WG_IO_FIELD(name)
    WG_IO_FIELD(type)
    WG_IO_FIELD_OPT(id)
    WG_IO_FIELD_OPT(value)
    WG_IO_END(ShaderTexture)

    WG_IO_BEGIN(ShaderPipelineState)
    WG_IO_FIELD_OPT(poly_mode)
    WG_IO_FIELD_OPT(cull_mode)
    WG_IO_FIELD_OPT(front_face)
    WG_IO_FIELD_OPT(depth_enable)
    WG_IO_FIELD_OPT(depth_write)
    WG_IO_FIELD_OPT(depth_func)
    WG_IO_END(ShaderPipelineState)

    WG_IO_BEGIN(ShaderFile)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(parameters)
    WG_IO_FIELD_OPT(textures)
    WG_IO_FIELD_OPT(keywords)
    WG_IO_FIELD_OPT(vertex)
    WG_IO_FIELD_OPT(fragment)
    WG_IO_FIELD_OPT(compute)
    WG_IO_FIELD(domain)
    WG_IO_FIELD_OPT(state)
    WG_IO_END(ShaderFile)

    Status Shader::read_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_ASSET("Shader::read_from_yaml");

        ShaderFile shader_file;
        WG_YAML_READ(node, shader_file);

        m_domain         = shader_file.domain;
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

        if (!generate_params_layout()) return StatusCode::Error;
        if (!generate_textures_layout()) return StatusCode::Error;

        return StatusCode::Ok;
    }

    Status Shader::copy_to(Object& copy) const {
        Asset::copy_to(copy);
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
        shader->m_pipeline_state     = m_pipeline_state;
        shader->m_keywords           = m_keywords;
        return StatusCode::Ok;
    }

    Ref<GfxShader> Shader::create_variant(const buffered_vector<std::string>& defines) {
        return create_variant({}, defines);
    }
    Ref<GfxShader> Shader::create_variant(const GfxVertAttribs& attribs, const buffered_vector<std::string>& defines) {
        WG_AUTO_PROFILE_ASSET("Shader::create_variant");

        Engine*        engine         = Engine::instance();
        ShaderManager* shader_manager = engine->shader_manager();

        Ref<GfxShader> shader_variant = shader_manager->get_shader(m_domain, attribs, defines, this);
        if (shader_variant) {
            return shader_variant;
        }

        WG_LOG_ERROR("failed to create shader variant " << shader_manager->make_shader_key(m_domain, attribs, defines, this));
        return Ref<GfxShader>();
    }

    void Shader::fill_layout(GfxDescSetLayoutDesc& layout) const {
        if (!m_parameters.empty()) {
            GfxDescBinging& binding = layout.emplace_back();
            binding.binding         = get_start_buffers_slot();
            binding.count           = 1;
            binding.type            = GfxBindingType::UniformBuffer;
            binding.name            = SID("MaterialParameters");
        }
        if (!m_textures.empty()) {
            for (const auto& texture : m_textures) {
                GfxDescBinging& binding = layout.emplace_back();
                binding.binding         = get_start_textures_slot() + texture.second.id;
                binding.count           = 1;
                binding.type            = GfxBindingType::SampledTexture;
                binding.name            = texture.second.name;
            }
        }
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

    const Strid& Shader::get_domain() const {
        return m_domain;
    }
    const flat_set<Strid>& Shader::get_keywords() const {
        return m_keywords;
    }
    const flat_map<Strid, ShaderParameter>& Shader::get_parameters() const {
        return m_parameters;
    }
    const flat_map<Strid, ShaderTexture>& Shader::get_textures() const {
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
    int Shader::get_start_textures_slot() const {
        return 1;
    }
    int Shader::get_start_buffers_slot() const {
        return 0;
    }
    const std::string& Shader::get_include_parameters() const {
        return m_include_parameters;
    }
    const std::string& Shader::get_include_textures() const {
        return m_include_textures;
    }

    Status Shader::generate_params_layout() {
        if (m_parameters.empty()) {
            return StatusCode::Ok;
        }

        int               total_size = 0;
        int               pad_count  = 0;
        std::stringstream params_declaration;

        params_declaration << "LAYOUT_BUFFER(MATERIAL_SET, 0, std140) uniform MaterialParameters {\n";
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
                    params_declaration << "vec4 " << parameter.name.str() << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = 4 * sizeof(float);
                    total_size += 4 * sizeof(float);
                    break;
                default:
                    WG_LOG_ERROR("unknown type of parameter");
                    return StatusCode::InvalidParameter;
            }
        }
        params_declaration << "};\n\n";

        m_include_parameters = params_declaration.str();
        m_parameters_size    = total_size;
        return StatusCode::Ok;
    }
    Status Shader::generate_textures_layout() {
        if (m_textures.empty()) {
            return StatusCode::Ok;
        }

        int               total_count = 0;
        std::stringstream tex_declaration;

        for (auto& entry : m_textures) {
            auto& texture = entry.second;

            tex_declaration << "LAYOUT_SAMPLER(MATERIAL_SET, 1 + " << total_count << ") uniform ";

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
                    return StatusCode::InvalidParameter;
            }

            tex_declaration << ";\n";
            texture.id = total_count;
            total_count += 1;
        }

        tex_declaration << "\n";

        m_include_textures = tex_declaration.str();
        return StatusCode::Ok;
    }

    void Shader::register_class() {
        auto* cls = Class::register_class<Shader>();
    }

}// namespace wmoge
