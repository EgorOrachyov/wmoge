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
#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "render/render_engine.hpp"
#include "resource/resource_manager.hpp"
#include "system/engine.hpp"

#include <algorithm>
#include <cstring>
#include <magic_enum.hpp>
#include <sstream>

namespace wmoge {

    WG_IO_BEGIN_NMSP(MaterialFile, EntryParam)
    WG_IO_FIELD(name)
    WG_IO_FIELD(value)
    WG_IO_END_NMSP(MaterialFile, EntryParam)

    WG_IO_BEGIN_NMSP(MaterialFile, EntryTexture)
    WG_IO_FIELD(name)
    WG_IO_FIELD(value)
    WG_IO_END_NMSP(MaterialFile, EntryTexture)

    WG_IO_BEGIN(MaterialFile)
    WG_IO_PROFILE()
    WG_IO_FIELD(shader)
    WG_IO_FIELD_OPT(parameters)
    WG_IO_FIELD_OPT(textures)
    WG_IO_END(MaterialFile)

    Material::Material(Ref<Shader> shader) {
        assert(shader);

        m_shader = std::move(shader);

        init();
    }

    void Material::init() {
        m_textures.resize(m_shader->get_textures_count());

        if (m_shader->get_parameters_size() > 0) {
            const int params_size = m_shader->get_parameters_size();
            m_parameters          = make_ref<Data>(params_size);
        }

        for (const auto& entry : m_shader->get_parameters()) {
            auto& param = entry.second;
            set_param(param.name, param.value);
        }

        for (const auto& entry : m_shader->get_textures()) {
            m_textures[entry.second.id] = entry.second.value;
        }
    }

    void Material::set_param(const Strid& name, const std::string& string_value) {
        assert(m_shader);

        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end()) {
            WG_LOG_ERROR("no such param " << name);
            return;
        }

        std::stringstream value(string_value);
        auto*             mem = m_parameters->buffer() + param->second.offset;

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

        m_dirty.set(DirtyFlag::Parameters);
    }
    void Material::set_int(const Strid& name, int value) {
        assert(m_shader);

        const auto  type   = GfxShaderParam::Int;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters->buffer() + param->second.offset, &value, param->second.size);
        m_dirty.set(DirtyFlag::Parameters);
    }
    void Material::set_float(const Strid& name, float value) {
        assert(m_shader);

        const auto  type   = GfxShaderParam::Float;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters->buffer() + param->second.offset, &value, param->second.size);
        m_dirty.set(DirtyFlag::Parameters);
    }
    void Material::set_vec2(const Strid& name, const Vec2f& value) {
        assert(m_shader);

        const auto  type   = GfxShaderParam::Vec2;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters->buffer() + param->second.offset, &value, param->second.size);
        m_dirty.set(DirtyFlag::Parameters);
    }
    void Material::set_vec3(const Strid& name, const Vec3f& value) {
        assert(m_shader);

        const auto  type   = GfxShaderParam::Vec3;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters->buffer() + param->second.offset, &value, param->second.size);
        m_dirty.set(DirtyFlag::Parameters);
    }
    void Material::set_vec4(const Strid& name, const Vec4f& value) {
        assert(m_shader);

        const auto  type   = GfxShaderParam::Vec4;
        const auto& params = m_shader->get_parameters();
        const auto  param  = params.find(name);

        if (param == params.end() || param->second.type != type) {
            WG_LOG_ERROR("no such param " << name << " of type " << magic_enum::enum_name(type));
            return;
        }

        std::memcpy(m_parameters->buffer() + param->second.offset, &value, param->second.size);
        m_dirty.set(DirtyFlag::Parameters);
    }
    void Material::set_texture(const Strid& name, const Ref<Texture>& texture) {
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
        m_dirty.set(DirtyFlag::Textures);
    }

    void Material::validate() {
        std::lock_guard guard(m_mutex);

        if (!m_dirty.bits.any()) {
            return;
        }

        const bool need_buffer_update   = m_dirty.get(DirtyFlag::Parameters) && m_parameters;
        const bool need_textures_update = m_dirty.get(DirtyFlag::Textures) && m_textures.size() > 0;
        const bool need_desc_update     = !m_buffer && need_buffer_update || need_textures_update;

        m_dirty.bits.reset();

        if (need_buffer_update) {
            Engine* engine  = Engine::instance();
            GfxCtx* gfx_ctx = engine->gfx_ctx();

            if (!m_buffer) {
                m_buffer = engine->gfx_driver()->make_uniform_buffer(int(m_parameters->size()), GfxMemUsage::GpuLocal, get_name());
            }

            gfx_ctx->update_uniform_buffer(m_buffer, 0, m_buffer->size(), m_parameters);
        }

        if (need_desc_update) {
            GfxDescSetResources desc_set_resources;

            int buffer_slot = m_shader->get_start_buffers_slot();
            if (m_buffer) {
                GfxDescSetResource& resource   = desc_set_resources.emplace_back();
                GfxDescBindPoint&   bind_point = resource.first;
                GfxDescBindValue&   bind_value = resource.second;
                bind_point.type                = GfxBindingType::UniformBuffer;
                bind_point.binding             = buffer_slot++;
                bind_point.array_element       = 0;
                bind_value.resource            = m_buffer.as<GfxResource>();
                bind_value.range               = m_buffer->size();
                bind_value.offset              = 0;
            }

            const int textures_slot = m_shader->get_start_buffers_slot();
            for (int i = 0; i < int(m_textures.size()); i++) {
                if (m_textures[i]) {
                    GfxDescSetResource& resource   = desc_set_resources.emplace_back();
                    GfxDescBindPoint&   bind_point = resource.first;
                    GfxDescBindValue&   bind_value = resource.second;
                    bind_point.type                = GfxBindingType::SampledTexture;
                    bind_point.binding             = textures_slot + i;
                    bind_point.array_element       = 0;
                    bind_value.resource            = m_textures[i]->get_texture().as<GfxResource>();
                    bind_value.sampler             = m_textures[i]->get_sampler();
                    bind_value.range               = 0;
                    bind_value.offset              = 0;
                }
            }

            if (!desc_set_resources.empty()) {
                GfxDriver* gfx_driver = Engine::instance()->gfx_driver();
                m_desc_set            = gfx_driver->make_desc_set(desc_set_resources, get_name());
            }
        }
    }

    Status Material::read_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("Material::read_from_yaml");

        MaterialFile material_file;
        WG_YAML_READ(node, material_file);

        m_shader = material_file.shader;

        init();

        for (const auto& param : material_file.parameters) {
            set_param(param.name, param.value);
        }

        for (const auto& texture : material_file.textures) {
            set_texture(texture.name, texture.value);
        }

        return StatusCode::Ok;
    }
    Status Material::copy_to(Object& copy) const {
        Resource::copy_to(copy);
        auto material          = dynamic_cast<Material*>(&copy);
        material->m_shader     = m_shader;
        material->m_parameters = m_parameters;
        material->m_textures   = m_textures;
        return StatusCode::Ok;
    }

    void Material::register_class() {
        auto* cls = Class::register_class<Material>();
    }

}// namespace wmoge
