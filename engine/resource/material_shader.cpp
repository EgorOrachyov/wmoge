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

#include "material_shader.hpp"

#include "core/class.hpp"
#include "debug/profiler.hpp"
#include "render/draw_cmd.hpp"
#include "render/shader_builder.hpp"

#include <magic_enum.hpp>
#include <sstream>

namespace wmoge {

    bool MaterialShader::load_from_import_options(const YamlTree& tree) {
        WG_AUTO_PROFILE_RESOURCE();

        if (!Shader::load_from_import_options(tree)) {
            return false;
        }

        auto        params = tree["params"];
        std::string domain;
        std::string render_queue;
        std::string poly_mode;
        std::string cull_mode;
        std::string front_face;
        std::string depth_func;

        params["domain"] >> domain;
        params["render_queue"] >> render_queue;
        params["poly_mode"] >> poly_mode;
        params["cull_mode"] >> cull_mode;
        params["front_face"] >> front_face;
        params["depth_enable"] >> m_depth_enable;
        params["depth_write"] >> m_depth_write;
        params["depth_func"] >> depth_func;

        m_domain       = SID(domain);
        m_render_queue = SID(render_queue);
        m_poly_mode    = magic_enum::enum_cast<GfxPolyMode>(poly_mode).value();
        m_cull_mode    = magic_enum::enum_cast<GfxPolyCullMode>(cull_mode).value();
        m_front_face   = magic_enum::enum_cast<GfxPolyFrontFace>(front_face).value();
        m_depth_func   = magic_enum::enum_cast<GfxCompFunc>(depth_func).value();

        auto keywords = params["keywords"];
        for (auto entry = keywords.first_child(); entry.valid(); entry = entry.next_sibling()) {
            std::string keyword;
            entry >> keyword;
            m_keywords.emplace(SID(keyword));
        }

        auto parameters = params["parameters"];
        for (auto entry = parameters.first_child(); entry.valid(); entry = entry.next_sibling()) {
            std::string name;
            std::string type;
            std::string value;

            entry["name"] >> name;
            entry["type"] >> type;
            entry["value"] >> value;

            auto& shader_parameter = m_parameters[SID(name)];
            shader_parameter.name  = SID(name);
            shader_parameter.type  = magic_enum::enum_cast<ShaderParamType>(type).value();
            shader_parameter.value = value;
        }

        auto textures = params["textures"];
        for (auto entry = textures.first_child(); entry.valid(); entry = entry.next_sibling()) {
            std::string name;
            std::string type;
            std::string value;

            entry["name"] >> name;
            entry["type"] >> type;
            entry["value"] >> value;

            auto& shader_texture = m_textures[SID(name)];
            shader_texture.name  = SID(name);
            shader_texture.type  = magic_enum::enum_cast<GfxTex>(type).value();
            shader_texture.value = value;
        }

        if (!generate_params_layout()) return false;
        if (!generate_textures_layout()) return false;

        return true;
    }
    void MaterialShader::copy_to(Resource& copy) {
        Shader::copy_to(copy);
        auto shader            = dynamic_cast<MaterialShader*>(&copy);
        shader->m_domain       = m_domain;
        shader->m_render_queue = m_render_queue;
        shader->m_keywords     = m_keywords;
        shader->m_parameters   = m_parameters;
        shader->m_textures     = m_textures;
    }

    const StringId& MaterialShader::get_domain() {
        return m_domain;
    }
    const StringId& MaterialShader::get_render_queue() {
        return m_render_queue;
    }
    const fast_set<StringId>& MaterialShader::get_keywords() {
        return m_keywords;
    }
    const fast_map<StringId, ShaderParameter>& MaterialShader::get_parameters() {
        return m_parameters;
    }
    const fast_map<StringId, ShaderTexture>& MaterialShader::get_textures() {
        return m_textures;
    }
    GfxPolyMode MaterialShader::get_poly_mode() {
        return m_poly_mode;
    }
    GfxPolyCullMode MaterialShader::get_cull_mode() {
        return m_cull_mode;
    }
    GfxPolyFrontFace MaterialShader::get_front_face() {
        return m_front_face;
    }
    bool MaterialShader::get_depth_enable() {
        return m_depth_enable;
    }
    bool MaterialShader::get_depth_write() {
        return m_depth_write;
    }
    GfxCompFunc MaterialShader::get_depth_func() {
        return m_depth_func;
    }
    int MaterialShader::get_parameters_size() {
        return m_parameters_size;
    }
    int MaterialShader::get_parameters_count() {
        return int(m_parameters.size());
    }
    int MaterialShader::get_textures_count() {
        return int(m_textures.size());
    }

    void MaterialShader::on_build(ShaderBuilder& builder) {
        std::stringstream g_defines;
        g_defines << "#define DRAW_SET_PER_PASS " << DrawPassConsts::DRAW_SET_PER_PASS << "\n";
        g_defines << "#define DRAW_SET_PER_MATERIAL " << DrawPassConsts::DRAW_SET_PER_MATERIAL << "\n";
        g_defines << "#define DRAW_SET_PER_DRAW " << DrawPassConsts::DRAW_SET_PER_DRAW << "\n\n";
        std::string g_defines_str = g_defines.str();

        static const char* attrib_types[] = {"vec3", "vec3", "vec3", "vec4", "vec4", "vec4", "vec4", "vec4", "vec4", "vec2", "vec2", "vec2", "vec2"};
        static const char* attrib_names[] = {"position", "normal", "tangent", "bone_ids", "bone_weights", "color0", "color1", "color2", "color3", "uv0", "uv1", "uv2", "uv3"};
        MeshAttribs        mest_attribs   = builder.get_mesh_attribs();
        std::stringstream  g_vertex_inputs;
        int                location = 0;

        for (int i = 0; i < int(MeshAttrib::Max); i++) {
            MeshAttrib attrib = static_cast<MeshAttrib>(i);

            if (mest_attribs.get(attrib)) {
                g_vertex_inputs << "layout (location = " << location << ") in " << attrib_types[i] << " in_" << attrib_names[i] << ";\n";
                location += 1;
            }
        }

        on_build_add_defines(builder);

        builder
                .add_vertex_module(g_defines_str)
                .add_fragment_module(g_defines_str)
                .add_vertex_module(g_vertex_inputs.str());

        on_build_add_params(builder);

        builder
                .add_vertex_module(m_include_parameters)
                .add_fragment_module(m_include_parameters)
                .add_fragment_module(m_include_textures)
                .add_vertex_module(m_vertex)
                .add_fragment_module(m_fragment);

        on_build_add_main(builder);
    }
    bool MaterialShader::generate_params_layout() {
        int               total_size = 0;
        int               pad_count  = 0;
        std::stringstream params_declaration;

        params_declaration << "layout (set = DRAW_SET_PER_MATERIAL, binding = MAT_BINDING_PARAMS) uniform MaterialParameters {\n";
        for (auto& entry : m_parameters) {
            auto& parameter = entry.second;

            params_declaration << "    ";

            switch (parameter.type) {
                case ShaderParamType::Int:
                    params_declaration << "int " << parameter.name.str() << "; int3 _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = sizeof(int);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case ShaderParamType::Float:
                    params_declaration << "float " << parameter.name.str() << "; vec3 _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = sizeof(float);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case ShaderParamType::Vec2:
                    params_declaration << "vec2 " << parameter.name.str() << "; vec2 _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = 2 * sizeof(float);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case ShaderParamType::Vec3:
                    params_declaration << "vec3 " << parameter.name.str() << "; float _wg_pad" << pad_count << ";\n";
                    parameter.offset = total_size;
                    parameter.size   = 3 * sizeof(float);
                    pad_count += 1;
                    total_size += 4 * sizeof(float);
                    break;
                case ShaderParamType::Vec4:
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
    bool MaterialShader::generate_textures_layout() {
        int               total_count = 0;
        std::stringstream tex_declaration;

        for (auto& entry : m_textures) {
            auto& texture = entry.second;

            tex_declaration << "layout (set = DRAW_SET_PER_MATERIAL, binding = MAT_BINDING_TEX + " << total_count << ") uniform ";

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

    void MaterialShader::register_class() {
        auto* cls = Class::register_class<MaterialShader>();
    }

}// namespace wmoge