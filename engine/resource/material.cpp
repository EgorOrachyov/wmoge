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

    void Material::create(ref_ptr<MaterialShader> shader) {
        assert(shader);

        m_shader = std::move(shader);
        m_textures.resize(m_shader->get_textures_count());
        m_parameters.resize(m_shader->get_parameters_size());

        for (const auto& entry : m_shader->get_parameters()) {
            auto&         param = entry.second;
            std::uint8_t* mem   = m_parameters.data() + param.offset;
            set_parameter_from_string(param.type, param.value, mem);
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

        m_version         = 1;
        m_render_material = make_ref<RenderMaterial>(this, m_textures.size(), m_parameters.size());
        m_render_material->ensure_version();
    }

    bool Material::load_from_import_options(const YamlTree& tree) {
        WG_AUTO_PROFILE_RESOURCE("Material::load_from_import_options");

        if (!Resource::load_from_import_options(tree)) return false;

        auto params  = tree["params"];
        auto res_man = Engine::instance()->resource_manager();

        std::string shader;
        params["shader"] >> shader;
        if (shader.empty()) {
            WG_LOG_ERROR("passed empty shader for " << get_name());
            return false;
        }

        auto material_shader = res_man->load(SID(shader)).cast<MaterialShader>();
        if (!material_shader) {
            WG_LOG_ERROR("not loaded shader " << shader << ". shader must be loaded");
            return false;
        }

        create(material_shader);

        auto parameters = params["parameters"];
        for (auto it = parameters.first_child(); it.valid(); it = it.next_sibling()) {
            std::string name;
            std::string value;

            it["name"] >> name;
            it["value"] >> value;

            auto param = m_shader->get_parameters().find(SID(name));
            if (param == m_shader->get_parameters().end()) {
                WG_LOG_ERROR("no such param in shader " << name);
                continue;
            }

            set_parameter_from_string(param->second.type, value, m_parameters.data() + param->second.offset);
        }

        auto textures = params["textures"];
        for (auto it = textures.first_child(); it.valid(); it = it.next_sibling()) {
            std::string name;
            std::string value;

            it["name"] >> name;
            it["value"] >> value;

            auto texture = res_man->load(SID(value)).cast<Texture>();
            if (!texture) {
                WG_LOG_ERROR("no loaded texture " << value);
                continue;
            }

            set_texture(SID(name), texture);
        }

        m_render_material->ensure_version();

        return true;
    }
    void Material::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto material          = dynamic_cast<Material*>(&copy);
        material->m_shader     = m_shader;
        material->m_parameters = m_parameters;
        material->m_textures   = m_textures;
    }

    const ref_ptr<MaterialShader>& Material::get_shader() {
        return m_shader;
    }
    const ref_ptr<RenderMaterial>& Material::get_render_material() {
        return m_render_material;
    }
    const fast_vector<std::uint8_t>& Material::get_parameters() {
        return m_parameters;
    }
    const fast_vector<ref_ptr<Texture>>& Material::get_textures() {
        return m_textures;
    }

    void Material::set_int(const StringId& name, int value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = ShaderParamType::Int;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;

        request_update();
    }
    void Material::set_float(const StringId& name, float value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = ShaderParamType::Float;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;

        request_update();
    }
    void Material::set_vec2(const StringId& name, const Vec2f& value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = ShaderParamType::Vec2;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;

        request_update();
    }
    void Material::set_vec3(const StringId& name, const Vec3f& value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = ShaderParamType::Vec3;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;

        request_update();
    }
    void Material::set_vec4(const StringId& name, const Vec4f& value) {
        std::lock_guard guard(m_mutex);

        assert(m_shader);

        const auto  type   = ShaderParamType::Vec4;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters.data() + param->second.offset, &value, param->second.size);
        m_version += 1;

        request_update();
    }
    void Material::set_texture(const StringId& name, const ref_ptr<Texture>& texture) {
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

        request_update();
    }

    bool Material::copy_state(std::size_t& version, ref_ptr<GfxTexture>* textures, ref_ptr<GfxSampler>* samplers, ref_ptr<Data>& data) {
        std::lock_guard guard(m_mutex);

        if (version != m_version) {
            version = m_version;

            for (int i = 0; i < int(m_textures.size()); i++) {
                assert(m_textures[i]);
                assert(m_textures[i]->get_texture());
                assert(m_textures[i]->get_sampler());

                textures[i] = m_textures[i]->get_texture();
                samplers[i] = m_textures[i]->get_sampler();
            }

            assert(m_parameters.size() > 0);
            data = make_ref<Data>(m_parameters.size());
            std::memcpy(data->buffer(), m_parameters.data(), m_parameters.size());

            return true;
        }

        return false;
    }
    void Material::set_parameter_from_string(ShaderParamType type, const std::string& str_value, void* mem) {
        std::stringstream value(str_value);

        switch (type) {
            case ShaderParamType::Int:
                value >> *(reinterpret_cast<int*>(mem));
                break;
            case ShaderParamType::Float:
                value >> *(reinterpret_cast<float*>(mem));
                break;
            case ShaderParamType::Vec2:
                value >> *(reinterpret_cast<float*>(mem) + 0);
                value >> *(reinterpret_cast<float*>(mem) + 1);
                break;
            case ShaderParamType::Vec3:
                value >> *(reinterpret_cast<float*>(mem) + 0);
                value >> *(reinterpret_cast<float*>(mem) + 1);
                value >> *(reinterpret_cast<float*>(mem) + 2);
                break;
            case ShaderParamType::Vec4:
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
    void Material::request_update() {
        assert(m_render_material);

        Engine::instance()->render_engine()->get_queue()->push([mat = m_render_material]() {
            mat->ensure_version();
        });
    }

    void Material::register_class() {
        auto* cls = Class::register_class<Material>();
    }

}// namespace wmoge
