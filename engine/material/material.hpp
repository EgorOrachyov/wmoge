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

#pragma once

#include "asset/asset_ref.hpp"
#include "core/array_view.hpp"
#include "core/buffered_vector.hpp"
#include "core/data.hpp"
#include "core/flat_set.hpp"
#include "core/mask.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "io/serialization.hpp"
#include "material/shader.hpp"
#include "math/vec.hpp"
#include "render/texture.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class MaterialFile
     * @brief Represents material file stored in assets folder
     */
    struct MaterialFile {
        /** @brief Param info of a material */
        struct EntryParam {
            Strid       name;
            std::string value;

            WG_IO_DECLARE(EntryParam);
        };

        /** @brief Texture info of a material */
        struct EntryTexture {
            Strid             name;
            AssetRef<Texture> value;

            WG_IO_DECLARE(EntryTexture);
        };

        std::vector<EntryParam>   parameters;
        std::vector<EntryTexture> textures;
        AssetRef<Shader>          shader;

        WG_IO_DECLARE(MaterialFile);
    };

    /**
     * @class Material
     * @brief Controls the rendering of the mesh geometry
     *
     * Material is composed of the shader object and a set of material params.
     * Material shader object defines the set of available params for rendering
     * settings. Material params provide user the ability to easily set params
     * to tweak rendering of the concrete object. User can apply shader to a
     * given mesh geometry and issuer rendering on a GPU.
     *
     * @note Particular shader variation depends on a mesh properties and other settings,
     *       thus huge number of materials with different settings may cause a significant
     *       increase of shader variations count.
     *
     * @see MaterialShader
     */
    class Material final : public Asset {
    public:
        WG_RTTI_CLASS(Material, Asset);

        Material()           = default;
        ~Material() override = default;

        /**
         * @brief Create material using specified shader
         *
         * This method initialized material with specified shader, allocates
         * params storage with default values, setups default textures and
         * prepares material for the rendering.
         *
         * @param shader Valid shader to use for this material
         */
        Material(Ref<Shader> shader);

        /** @brief Set material parameter by name from string value */
        void set_param(const Strid& name, const std::string& value);
        /** @brief Set material int parameter value by name */
        void set_int(const Strid& name, int value);
        /** @brief Set material float parameter value by name */
        void set_float(const Strid& name, float value);
        /** @brief Set material vec2 parameter value by name */
        void set_vec2(const Strid& name, const Vec2f& value);
        /** @brief Set material vec3 parameter value by name */
        void set_vec3(const Strid& name, const Vec3f& value);
        /** @brief Set material vec4 parameter value by name */
        void set_vec4(const Strid& name, const Vec4f& value);
        /** @brief Set material texture parameter value by name */
        void set_texture(const Strid& name, const Ref<Texture>& texture);

        /** @brief Validates GPU state of material, buffer and descriptor set for rendering */
        void validate();

        [[nodiscard]] array_view<const Ref<Texture>> get_textures() const { return m_textures; }
        [[nodiscard]] const Ref<Shader>&             get_shader() const { return m_shader; }
        [[nodiscard]] const Ref<Data>&               get_parameters() const { return m_parameters; }
        [[nodiscard]] const Ref<GfxUniformBuffer>&   get_buffer() const { return m_buffer; }
        [[nodiscard]] const Ref<GfxDescSet>&         get_desc_set() const { return m_desc_set; }

    private:
        void init();

    private:
        enum class DirtyFlag {
            Textures   = 0,
            Parameters = 1
        };

        buffered_vector<Ref<Texture>> m_textures;
        Ref<Shader>                   m_shader;
        Ref<Data>                     m_parameters;
        Ref<GfxUniformBuffer>         m_buffer;
        Ref<GfxDescSet>               m_desc_set;
        Mask<DirtyFlag>               m_dirty = {DirtyFlag::Textures, DirtyFlag::Parameters};

        SpinMutex m_mutex;
    };

    WG_RTTI_CLASS_BEGIN(Material) {
        WG_RTTI_META_DATA(RttiUiHint(""));
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge