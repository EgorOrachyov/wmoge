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
#include "core/synchronization.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_reflection.hpp"
#include "rtti/traits.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <utility>

namespace wmoge {

    /**
     * @class Shader
     * @brief An interface to the shader class
     * 
     * Shader provides fundamental interface to the shading algorithms.
     * It provides full information about the the shader, parameters, spaces,
     * techniques, passes, pipeline setup and pre-compilation options.
     * 
     * Shader allows to get or compile a particular variations (permutations)
     * of gpu programs with desierd defines setup. Also it keeps a cache of
     * already compiled programs, and serializes this cahce to load on next run.
    */
    class Shader : public Asset {
    public:
        WG_RTTI_CLASS(Shader, Asset);

        using Callback    = std::function<void(Shader*)>;
        using CallbackRef = std::shared_ptr<Callback>;

        Shader() = default;
        ~Shader() override;

        Shader(ShaderReflection&& reflection);

        virtual Status fill_layout_desc(GfxDescSetLayoutDesc& desc, std::int16_t space) const;
        virtual Status fill_compiler_env(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompiler* compiler, ShaderCompilerEnv& compiler_env);
        virtual Status fill_compiler_input(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompiler* compiler, ShaderCompilerInput& compiler_input);
        virtual Status fill_program_name(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, std::string& name);
        virtual Status fill_option_info(std::int16_t technique_idx, std::int16_t pass_idx, std::int16_t permutation_bit, Strid& option, Strid& variant);
        virtual Status fill_declarations(GfxShaderLang lang, ShaderCompiler* compiler, std::string& out_declarations);
        virtual Status fill_vertex_input(GfxShaderLang lang, ShaderCompiler* compiler, const ShaderPermutation& permutation, std::string& out_input);

        std::optional<std::int16_t>      find_technique(Strid name);
        std::optional<std::int16_t>      find_pass(std::int16_t technique, Strid name);
        std::optional<std::int16_t>      find_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant);
        std::optional<ShaderParamInfo*>  find_param(ShaderParamId id);
        std::optional<Ref<ShaderType>>   find_type(const ShaderTypeIdx idx);
        ShaderParamId                    find_param_id(Strid name);
        void                             set_shader_callback(CallbackRef callback);
        std::optional<ShaderPermutation> permutation(Strid technique, Strid pass, const buffered_vector<ShaderOptionVariant>& options = {}, GfxVertAttribs attribs = GfxVertAttribs());

        [[nodiscard]] bool                    has_space(ShaderSpaceType space_type) const;
        [[nodiscard]] bool                    has_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) const;
        [[nodiscard]] bool                    is_material() const;
        [[nodiscard]] bool                    is_graphics() const;
        [[nodiscard]] bool                    is_compute() const;
        [[nodiscard]] const std::int16_t      get_num_spaces() const;
        [[nodiscard]] const std::int16_t      get_num_techniques() const;
        [[nodiscard]] const std::int16_t      get_num_passes(std::int16_t technique_idx) const;
        [[nodiscard]] const Strid&            get_shader_name() const { return m_reflection.shader_name; }
        [[nodiscard]] const ShaderReflection& get_reflection() const { return m_reflection; }
        [[nodiscard]] ShaderReflection&       get_reflection() { return m_reflection; }

    protected:
        ShaderReflection m_reflection;
        CallbackRef      m_callback;
    };

    WG_RTTI_CLASS_BEGIN(Shader) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_reflection, {});
    }
    WG_RTTI_END;

    void rtti_grc_shader();

}// namespace wmoge