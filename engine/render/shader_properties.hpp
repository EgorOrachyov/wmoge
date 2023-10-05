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

#ifndef WMOGE_SHADER_PROPERTIES_HPP
#define WMOGE_SHADER_PROPERTIES_HPP

#include "core/array_view.hpp"
#include "core/data.hpp"
#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "resource/material.hpp"
#include "resource/shader.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class ShaderProperties
     * @brief Struct to hold properties and material params required for rendering
     * 
     * Shader properties may be created for a particular shader in order to hold params,
     * buffers and textures, required for rendering of any geometry in a scene.
     * 
     * Shader properties hold all textures, buffers, params, allows to access them and modify.
     * On redering properties allows to automatically access descriptor set with all resources,
     * which can be directly passed to a redner command or may be bound to the gfx context.
     * 
     * Each instance of properties must be accessed only from single thread at once.
     * Use multiple instance if you need some variations in params. But, a large amount
     * of unique shader params will lead to a drop in a performance. Use with care.
     * 
     * @see Material
     * @see Shader
    */
    class ShaderProperties {
    public:
        ShaderProperties(const Ref<Shader>& shader, const StringId& name);

        /** @brief Set material parameter by name from string value */
        void set_param(const StringId& name, const std::string& value);
        /** @brief Set material int parameter value by name */
        void set_int(const StringId& name, int value);
        /** @brief Set material float parameter value by name */
        void set_float(const StringId& name, float value);
        /** @brief Set material vec2 parameter value by name */
        void set_vec2(const StringId& name, const Vec2f& value);
        /** @brief Set material vec3 parameter value by name */
        void set_vec3(const StringId& name, const Vec3f& value);
        /** @brief Set material vec4 parameter value by name */
        void set_vec4(const StringId& name, const Vec4f& value);
        /** @brief Set material texture parameter value by name */
        void set_texture(const StringId& name, const Ref<Texture>& texture);

        void from(const Ref<Material>& material);
        void validate();

        [[nodiscard]] ArrayView<const Ref<Texture>> get_textures() const { return m_textures; }
        [[nodiscard]] const Ref<Shader>&            get_shader() const { return m_shader; }
        [[nodiscard]] const Ref<Data>&              get_parameters() const { return m_parameters; }
        [[nodiscard]] const Ref<GfxUniformBuffer>&  get_buffer() const { return m_buffer; }
        [[nodiscard]] const Ref<GfxDescSet>&        get_desc_set() const { return m_desc_set; }
        [[nodiscard]] const StringId&               get_name() const { return m_name; }

    private:
        enum class DirtyFlag {
            Textures   = 0,
            Parameters = 1
        };

        fast_vector<Ref<Texture>> m_textures;
        Ref<Shader>               m_shader;
        Ref<Data>                 m_parameters;
        Ref<GfxUniformBuffer>     m_buffer;
        Ref<GfxDescSet>           m_desc_set;
        StringId                  m_name;
        Mask<DirtyFlag>           m_dirty = {DirtyFlag::Textures, DirtyFlag::Parameters};
    };

}// namespace wmoge

#endif//WMOGE_SHADER_PROPERTIES_HPP