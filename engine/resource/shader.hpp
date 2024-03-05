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

#include "core/fast_map.hpp"
#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "io/serialization.hpp"
#include "resource/resource.hpp"
#include "resource/resource_ref.hpp"
#include "resource/texture.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class ShaderParameter
     * @brief Shader parameter info
     */
    struct ShaderParameter {
        Strid          name;
        GfxShaderParam type;
        int            offset = -1;
        int            size   = -1;
        std::string    value;

        WG_IO_DECLARE(ShaderParameter);
    };

    /**
     * @class ShaderTexture
     * @brief Shader texture info
     */
    struct ShaderTexture {
        Strid           name;
        GfxTex          type;
        int             id = -1;
        ResRef<Texture> value;

        WG_IO_DECLARE(ShaderTexture);
    };

    /**
     * @class ShaderPipelineState
     * @brief Shader pipeline settings
     */
    struct ShaderPipelineState {
        GfxPolyMode      poly_mode    = GfxPolyMode::Fill;
        GfxPolyCullMode  cull_mode    = GfxPolyCullMode::Disabled;
        GfxPolyFrontFace front_face   = GfxPolyFrontFace::CounterClockwise;
        bool             depth_enable = false;
        bool             depth_write  = true;
        GfxCompFunc      depth_func   = GfxCompFunc::Less;

        WG_IO_DECLARE(ShaderPipelineState);
    };

    /**
     * @class ShaderFile
     * @brief Shader file to describe a shader and store as a resource
     */
    struct ShaderFile {
        std::vector<ShaderParameter> parameters;
        std::vector<ShaderTexture>   textures;
        std::vector<Strid>           keywords;
        std::string                  vertex;
        std::string                  fragment;
        std::string                  compute;
        Strid                        domain;
        ShaderPipelineState          state{};

        WG_IO_DECLARE(ShaderFile);
    };

    /**
     * @class Shader
     * @brief Base class for any shader which can be used with material
     *
     * Shader allows user to write custom shaders to draw mesh
     * geometry to the screen. Material shader has a number of built-in
     * features. It automates hardware shader creation, allows to select
     * domain (base shader) for rendering, provides mechanism to simplify user
     * params exposure in a form of data and texture values.
     *
     * Shader is special resource which consists of optionally a vertex, fragment
     * compute code written using glsl language. This code if followed by a special
     * declaration, which defines the shader domain, keywords, and
     * set of data and texture parameters, which are exposed by this shader
     * to the end (material) user.
     *
     * @note For actual rendering shader can produce one or more gfx shader variants.
     *       Single variant is an actual gfx shader item. Variants share common behaviour,
     *       but differ in a set of defines. Variants created on demand, when they are requested.
     *
     * @note Variants creation is optimized by usage of the shader cache. If
     *       an item was compiled once and its byte code for current platform
     *       was cached, then its byte code reused.
     * 
     * @node Large amound of unique shaders and large amound of variants for compilation
     *       can lead to performance penalties, since variats compilation may take
     *       a long time to first compile. Also each variant produces unique PSO objects,
     *       which compilation also may take a long time. Additional RAM is also required
     *       for greater amound of shaders, variants and PSOs.
     */
    class Shader : public Resource {
    public:
        WG_OBJECT(Shader, Resource);

        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status copy_to(Object& other) const override;

        [[nodiscard]] Ref<GfxShader> create_variant(const fast_vector<std::string>& defines);
        [[nodiscard]] Ref<GfxShader> create_variant(const GfxVertAttribs& attribs, const fast_vector<std::string>& defines);
        void                         fill_layout(GfxDescSetLayoutDesc& layout) const;

        [[nodiscard]] const std::string&                      get_vertex() const;
        [[nodiscard]] const std::string&                      get_fragment() const;
        [[nodiscard]] const std::string&                      get_compute() const;
        [[nodiscard]] const Strid&                            get_domain() const;
        [[nodiscard]] const fast_set<Strid>&                  get_keywords() const;
        [[nodiscard]] const fast_map<Strid, ShaderParameter>& get_parameters() const;
        [[nodiscard]] const fast_map<Strid, ShaderTexture>&   get_textures() const;
        [[nodiscard]] const ShaderPipelineState&              get_pipeline_state() const;
        [[nodiscard]] int                                     get_parameters_size() const;
        [[nodiscard]] int                                     get_parameters_count() const;
        [[nodiscard]] int                                     get_textures_count() const;
        [[nodiscard]] int                                     get_start_textures_slot() const;
        [[nodiscard]] int                                     get_start_buffers_slot() const;
        [[nodiscard]] const std::string&                      get_include_textures() const;
        [[nodiscard]] const std::string&                      get_include_parameters() const;

    protected:
        Status generate_params_layout();
        Status generate_textures_layout();

    private:
        fast_map<Strid, ShaderParameter> m_parameters;
        fast_map<Strid, ShaderTexture>   m_textures;
        fast_set<Strid>                  m_keywords;
        ShaderPipelineState              m_pipeline_state{};
        Strid                            m_domain;
        std::string                      m_vertex;
        std::string                      m_fragment;
        std::string                      m_compute;
        std::string                      m_include_textures;
        std::string                      m_include_parameters;
        int                              m_parameters_size = 0;
    };

}// namespace wmoge