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

#include "material.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "render/render_engine.hpp"
#include "resource/resource_manager.hpp"

#include <cstring>
#include <magic_enum.hpp>
#include <sstream>

namespace wmoge {

    bool yaml_read(const YamlConstNodeRef& node, MaterialFile::Entry& entry) {
        WG_YAML_READ_AS(node, "name", entry.name);
        WG_YAML_READ_AS(node, "value", entry.value);

        return true;
    }
    bool yaml_write(YamlNodeRef& node, const MaterialFile::Entry& entry) {
        WG_YAML_WRITE_AS(node, "name", entry.name);
        WG_YAML_WRITE_AS(node, "value", entry.value);

        return true;
    }

    bool yaml_read(const YamlConstNodeRef& node, MaterialFile& file) {
        WG_YAML_READ_AS(node, "shader", file.shader);
        WG_YAML_READ_AS_OPT(node, "keywords", file.keywords);
        WG_YAML_READ_AS_OPT(node, "parameters", file.parameters);
        WG_YAML_READ_AS_OPT(node, "textures", file.textures);

        return true;
    }
    bool yaml_write(YamlNodeRef& node, const MaterialFile& file) {
        WG_YAML_WRITE_AS(node, "shader", file.shader);
        WG_YAML_WRITE_AS(node, "keywords", file.keywords);
        WG_YAML_WRITE_AS(node, "parameters", file.parameters);
        WG_YAML_WRITE_AS(node, "textures", file.textures);

        return true;
    }

    void Material::create(Ref<Shader> shader) {
        assert(shader);

        m_shader = std::move(shader);
        m_textures.resize(m_shader->get_textures_count());
        m_parameters.resize(m_shader->get_parameters_size());

        for (const auto& entry : m_shader->get_parameters()) {
            auto& param = entry.second;
            set_param(param.name, param.value);
        }

        auto res_man = Engine::instance()->resource_manager();

        for (const auto& entry : m_shader->get_textures()) {
            auto texture = res_man->load(SID(entry.second.value)).cast<Texture>();

            if (!texture) {
                WG_LOG_ERROR("no loaded texture " << entry.second.value);
                continue;
            }

            m_textures[entry.second.id] = texture;
        }

        m_version = 1;
    }

    bool Material::load_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("Material::load_from_import_options");

        MaterialFile material_file;
        WG_YAML_READ(node, material_file);

        auto res_man = Engine::instance()->resource_manager();

        auto material_shader = res_man->load(SID(material_file.shader)).cast<Shader>();
        if (!material_shader) {
            WG_LOG_ERROR("not found shader " << material_file.shader << " for " << get_name());
            return false;
        }

        create(material_shader);

        for (const auto& param : material_file.parameters) {
            set_param(param.name, param.value);
        }

        for (const auto& texture : material_file.textures) {
            auto texture_res = res_man->load(SID(texture.value)).cast<Texture>();
            if (!texture_res) {
                WG_LOG_ERROR("not found texture " << texture.value << " for " << get_name());
                continue;
            }

            set_texture(texture.name, texture_res);
        }

        for (auto& keyword : material_file.keywords) {
            m_keywords.insert(std::move(keyword));
        }

        return true;
    }
    void Material::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto material          = dynamic_cast<Material*>(&copy);
        material->m_shader     = m_shader;
        material->m_parameters = m_parameters;
        material->m_textures   = m_textures;
    }

    const Ref<Shader>& Material::get_shader() {
        return m_shader;
    }
    const fast_vector<std::uint8_t>& Material::get_parameters() {
        return m_parameters;
    }
    const fast_vector<Ref<Texture>>& Material::get_textures() {
        return m_textures;
    }
    const fast_set<StringId>& Material::get_keywords() {
        return m_keywords;
    }

    void Material::set_param(const StringId& name, const std::string& string_value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end()) {
            WG_LOG_ERROR("no such param " << name);
            return;
        }

        std::stringstream value(string_value);
        auto*             mem = m_parameters.data() + param->second.offset;

        switch (param->second.type) {
            case GfxShaderParam::Int:
                value >> *(reinterpret_cast<int*>(mem));
                break;
            case GfxShaderParam::Float:
                value >> *(reinterpret_cast<float*>(mem));
                break;
            case GfxShaderParam::Vec2:
                value >> *(reinterpret_cast<float*>(mem) + 0);
                value >> *(reinterpret_cast<float*>(mem) + 1);
                break;
            case GfxShaderParam::Vec3:
                value >> *(reinterpret_cast<float*>(mem) + 0);
                value >> *(reinterpret_cast<float*>(mem) + 1);
                value >> *(reinterpret_cast<float*>(mem) + 2);
                break;
            case GfxShaderParam::Vec4:
                value >> *(reinterpret_cast<float*>(mem) + 0);
                value >> *(reinterpret_cast<float*>(mem) + 1);
                value >> *(reinterpret_cast<float*>(mem) + 2);
                value >> *(reinterpret_cast<float*>(mem) + 3);
                break;
            default:
                WG_LOG_ERROR("invalid shader parameter type");
                return;
        }
    }
    void Material::set_int(const StringId& name, int value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = GfxShaderParam::Int;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;
    }
    void Material::set_float(const StringId& name, float value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = GfxShaderParam::Float;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;
    }
    void Material::set_vec2(const StringId& name, const Vec2f& value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = GfxShaderParam::Vec2;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;
    }
    void Material::set_vec3(const StringId& name, const Vec3f& value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = GfxShaderParam::Vec3;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;
    }
    void Material::set_vec4(const StringId& name, const Vec4f& value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = GfxShaderParam::Vec4;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;
    }
    void Material::set_texture(const StringId& name, const Ref<Texture>& texture) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        if (!texture) {
            WG_LOG_ERROR("an attempt to set null " << name << " texture");
            return;
        }

        const auto& textures      = m_shader->get_textures();
        const auto  texture_param = textures.find(name);

        if (texture_param == textures.end() || texture_param->second.type != texture->get_tex_type()) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(texture->get_tex_type()));
            return;
        }

        m_textures[texture_param->second.id] = texture;
        m_version += 1;
    }

    void Material::register_class() {
        auto* cls = Class::register_class<Material>();
    }

}// namespace wmoge
