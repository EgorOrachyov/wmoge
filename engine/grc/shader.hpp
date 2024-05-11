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
#include "core/async.hpp"
#include "core/buffered_vector.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_file.hpp"
#include "grc/shader_reflection.hpp"
#include "platform/file_system.hpp"
#include "rtti/traits.hpp"

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

        Shader() = default;
        ~Shader() override;

        virtual Status init(ShaderReflection& reflection);
        virtual Status fill_layout_desc(GfxDescSetLayoutDesc& desc, std::int16_t space) const;
        virtual Status fill_layouts(GfxDescSetLayouts& layouts) const;
        virtual Status fill_compiler_env(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompilerEnv& compiler_env);
        virtual Status fill_compiler_input(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompilerInput& compiler_input);
        virtual Status fill_program_name(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, std::string& name);
        virtual Status fill_option_info(std::int16_t technique_idx, std::int16_t permutation_bit, Strid& option, Strid& variant);
        virtual Status fill_declarations(GfxShaderLang lang, std::string& out_declarations);
        virtual Status fill_vertex_input(GfxShaderLang lang, const ShaderPermutation& permutation, std::string& out_input);
        virtual Status reanalyse_includes();
        virtual Status load(const ShaderFile& file);

        Ref<GfxShaderProgram>           get_or_create_program(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        Ref<GfxShaderProgram>           find_program(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        Async                           precache_program(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        Async                           compile_program(GfxShaderPlatform platform, const ShaderPermutation& permutation, Ref<ShaderCompilerRequest>& request, Async depends_on = Async());
        Status                          load_cache(GfxShaderPlatform platform, bool allow_missing = true);
        Status                          save_cache(GfxShaderPlatform platform);
        std::optional<ShaderStatus>     find_program_status(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        std::optional<std::int16_t>     find_technique(Strid name);
        std::optional<std::int16_t>     find_pass(std::int16_t technique, Strid name);
        std::optional<ShaderParamInfo*> find_param(ShaderParamId id);
        ShaderParamId                   find_param_id(Strid name);

        [[nodiscard]] bool                         has_dependency(const Strid& dependency) const;
        [[nodiscard]] bool                         has_space(ShaderSpaceType space_type) const;
        [[nodiscard]] bool                         has_option(std::int16_t technique, Strid name, Strid variant) const;
        [[nodiscard]] bool                         has_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) const;
        [[nodiscard]] const std::int16_t           get_num_spaces() const;
        [[nodiscard]] const ShaderReflection&      get_reflection() const { return m_reflection; }
        [[nodiscard]] const Strid&                 get_shader_name() const { return m_reflection.shader_name; }
        [[nodiscard]] const Ref<GfxDescSetLayout>& get_layout(std::int16_t space) const { return m_layouts[space]; }
        [[nodiscard]] const Ref<GfxPsoLayout>&     get_pso_layout() const { return m_pso_layout; }

    protected:
        ShaderReflection                     m_reflection;
        ShaderCache                          m_cache;
        ShaderCompilerEnv                    m_env;
        GfxDescSetLayouts                    m_layouts;
        GfxPsoLayoutRef                      m_pso_layout;
        flat_map<GfxShaderLang, std::string> m_cached_declarations;

        mutable RwMutexReadPrefer m_mutex;
    };

    WG_RTTI_CLASS_BEGIN(Shader) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge