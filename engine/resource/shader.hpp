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

#ifndef WMOGE_SHADER_HPP
#define WMOGE_SHADER_HPP

#include "core/fast_map.hpp"
#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "io/yaml.hpp"
#include "resource/resource.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class ShaderParameter
     * @brief Shader parameter info
     */
    struct ShaderParameter {
        StringId       name;
        GfxShaderParam type;
        int            offset = -1;
        int            size   = -1;
        std::string    value;

        friend bool yaml_read(const YamlConstNodeRef& node, ShaderParameter& parameter);
        friend bool yaml_write(YamlNodeRef& node, const ShaderParameter& parameter);
    };

    /**
     * @class ShaderTexture
     * @brief Shader texture info
     */
    struct ShaderTexture {
        StringId    name;
        GfxTex      type;
        int         id = -1;
        std::string value;

        friend bool yaml_read(const YamlConstNodeRef& node, ShaderTexture& texture);
        friend bool yaml_write(YamlNodeRef& node, const ShaderTexture& texture);
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

        friend bool yaml_read(const YamlConstNodeRef& node, ShaderPipelineState& state);
        friend bool yaml_write(YamlNodeRef& node, const ShaderPipelineState& state);
    };

    /**
     * @class ShaderFile
     * @brief Shader file to describe a shader and store as a resource
     */
    struct ShaderFile {
        std::vector<ShaderParameter> parameters;
        std::vector<ShaderTexture>   textures;
        std::vector<StringId>        keywords;
        std::string                  vertex;
        std::string                  fragment;
        std::string                  compute;
        StringId                     domain;
        int                          render_queue = 0;
        ShaderPipelineState          state{};

        friend bool yaml_read(const YamlConstNodeRef& node, ShaderFile& file);
        friend bool yaml_write(YamlNodeRef& node, const ShaderFile& file);
    };

    /**
     * @class Shader
     * @brief Base class for any shader which can be used with material
     *
     * Shader allows user to write custom shaders to draw mesh
     * geometry to the screen. Material shader has a number of built-in
     * features. It automates hardware shader creation, allows to select
     * queue and domain for rendering, provides mechanism to simplify user
     * params exposure in a form of data and texture values.
     *
     * Shader is special resource which consists of optionally a vertex, fragment
     * compute code written using glsl language. This code if followed by a special
     * declaration, which defines the shader domain, render queue type and
     * set of data and texture parameters, which are exposed by this shader
     * to the end (material) user.
     *
     * @note For actual rendering shader can produce one or more gfx shader variants.
     *       Single variant is an actual gfx shader item. Variants share common behaviour,
     *       but differ in a set of defines. Variants  created on demand, when they are requested.
     *
     * @note Variants creation is optimized by usage of the shader cache. If
     *       an item was compiled once and its byte code for current platform
     *       was cached, then its byte code reused.
     */
    class Shader : public Resource {
    public:
        WG_OBJECT(Shader, Resource);

        bool load_from_yaml(const YamlConstNodeRef& node) override;
        void copy_to(Resource& copy) override;

        [[nodiscard]] bool           has_variant(const StringId& shader_key);
        [[nodiscard]] Ref<GfxShader> find_variant(const StringId& shader_key);
        [[nodiscard]] Ref<GfxShader> create_variant(const fast_vector<std::string>& defines);
        [[nodiscard]] Ref<GfxShader> create_variant(const GfxVertAttribsStreams& streams, const fast_vector<std::string>& defines);

        [[nodiscard]] const std::string&                         get_vertex() const;
        [[nodiscard]] const std::string&                         get_fragment() const;
        [[nodiscard]] const std::string&                         get_compute() const;
        [[nodiscard]] const StringId&                            get_domain() const;
        [[nodiscard]] int                                        get_render_queue() const;
        [[nodiscard]] const fast_set<StringId>&                  get_keywords() const;
        [[nodiscard]] const fast_map<StringId, ShaderParameter>& get_parameters() const;
        [[nodiscard]] const fast_map<StringId, ShaderTexture>&   get_textures() const;
        [[nodiscard]] const ShaderPipelineState&                 get_pipeline_state() const;
        [[nodiscard]] int                                        get_parameters_size() const;
        [[nodiscard]] int                                        get_parameters_count() const;
        [[nodiscard]] int                                        get_textures_count() const;
        [[nodiscard]] const std::string&                         get_include_textures() const;
        [[nodiscard]] const std::string&                         get_include_parameters() const;

    protected:
        bool generate_params_layout();
        bool generate_textures_layout();

    private:
        // Compiled and cached variants of the shader
        // This is stored here also to reduce look-ups count into global manager when doing material rendering
        fast_map<StringId, Ref<GfxShader>> m_variants;

        std::string m_vertex;
        std::string m_fragment;
        std::string m_compute;
        std::string m_include_textures;
        std::string m_include_parameters;
        int         m_parameters_size = -1;

        fast_map<StringId, ShaderParameter> m_parameters;
        fast_map<StringId, ShaderTexture>   m_textures;
        fast_set<StringId>                  m_keywords;
        ShaderPipelineState                 m_pipeline_state{};
        StringId                            m_domain;
        int                                 m_render_queue;

        std::mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SHADER_HPP
