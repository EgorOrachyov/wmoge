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

#include "asset/asset.hpp"
#include "core/buffered_vector.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_reflection.hpp"
#include "platform/file_system.hpp"
#include "rtti/traits.hpp"

#include <bitset>
#include <optional>

namespace wmoge {

    /**
     * @class ShaderPermutation
     * @brief Defines a particular variant of a compiled shader
    */
    struct ShaderPermutation {
        std::bitset<ShaderOptions::MAX_OPTIONS> options;
        GfxVertAttribs                          vert_attribs;
    };

    /**
     * @class Shader
     * @brief Reprsents a particular shader program script
     * 
     * Shader is a high level representation of a shading program.
     * It provides a connection between raw glsl sources code of a shader,
     * material and engine gfx module for runtime usage.
     * 
     * Shader provides info about a particular shader type. It provides
     * layout information, parameters and structures layout, defines and
     * compilations options, constants and includes, and provides hot-reloading
     * mechanism for debugging. 
     * 
     * Shader is a shader defenition for drawing with pre-defined interface.
     * It is not an compiled instance of a particular glsl shader. In order to get 
     * a concrete instance of compiled gpu program, pass and options must be
     * provided from ShaderInstance or Material / MaterialInstance classes.
    */
    class Shader : public Asset {
    public:
        WG_RTTI_CLASS(Shader, Asset);

        Shader()           = default;
        ~Shader() override = default;

        Status                          from_reflection(ShaderReflection& reflection);
        Status                          from_file(const ShaderFile& file);
        std::optional<std::int16_t>     find_technique(Strid name);
        std::optional<std::int16_t>     find_pass(std::int16_t technique, Strid name);
        ShaderParamId                   get_param_id(Strid name);
        std::optional<ShaderParamInfo*> get_param_info(ShaderParamId id);
        Status                          reload_sources(const std::string& folder, FileSystem* fs);
        Status                          fill_layout(GfxDescSetLayoutDesc& desc, int space) const;
        bool                            has_dependency(const Strid& dependency) const;
        bool                            has_space(ShaderSpaceType space_type) const;
        bool                            has_option(std::int16_t technique, Strid name, Strid variant) const;
        bool                            has_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) const;
        std::int16_t                    get_num_spaces() const;

        [[nodiscard]] const ShaderReflection&      get_reflection() const { return m_reflection; }
        [[nodiscard]] ShaderReflection&            get_reflection() { return m_reflection; }
        [[nodiscard]] const Strid&                 get_name() const { return m_reflection.shader_name; }
        [[nodiscard]] const Ref<GfxDescSetLayout>& get_layout(std::int16_t space) const { return m_layouts[space]; }

    protected:
        ShaderReflection                       m_reflection;
        buffered_vector<Ref<GfxDescSetLayout>> m_layouts;
    };

    WG_RTTI_CLASS_BEGIN(Shader) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge