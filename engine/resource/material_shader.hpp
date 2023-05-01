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

#ifndef WMOGE_MATERIAL_SHADER_HPP
#define WMOGE_MATERIAL_SHADER_HPP

#include "core/fast_map.hpp"
#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "render/shader_builder.hpp"
#include "resource/shader.hpp"

#include <unordered_map>
#include <unordered_set>

namespace wmoge {

    /** @brief Data parameters which can be exposed by a shader */
    enum class ShaderParamType {
        Int,
        Float,
        Vec2,
        Vec3,
        Vec4
    };

    /** @brief Shader parameter info */
    struct ShaderParameter {
        StringId        name;
        ShaderParamType type;
        int             offset = -1;
        int             size   = -1;
        std::string     value;
    };

    /** @brief Shader texture info */
    struct ShaderTexture {
        StringId    name;
        GfxTex      type;
        int         id = -1;
        std::string value;
    };

    /**
     * @class MaterialShader
     * @brief Base class for any shader which can be used with material
     *
     * Material shader allows user to write custom shaders to draw mesh
     * geometry to the screen. Material shader has a number of built-in
     * features. It automates hardware shader creation, allows to select
     * queue and domain for rendering, provides mechanism to simplify user
     * params exposure in form of data and texture values.
     */
    class MaterialShader : public Shader {
    public:
        WG_OBJECT(MaterialShader, Shader);

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

        const StringId&                            get_domain();
        const StringId&                            get_render_queue();
        const fast_set<StringId>&                  get_keywords();
        const fast_map<StringId, ShaderParameter>& get_parameters();
        const fast_map<StringId, ShaderTexture>&   get_textures();
        GfxPolyMode                                get_poly_mode();
        GfxPolyCullMode                            get_cull_mode();
        GfxPolyFrontFace                           get_front_face();
        bool                                       get_depth_enable();
        bool                                       get_depth_write();
        GfxCompFunc                                get_depth_func();
        int                                        get_parameters_size();
        int                                        get_parameters_count();
        int                                        get_textures_count();

    protected:
        virtual void on_build_add_defines(ShaderBuilder& builder){};
        virtual void on_build_add_params(ShaderBuilder& builder){};
        virtual void on_build_add_main(ShaderBuilder& builder){};
        void         on_build(ShaderBuilder& builder) override;

        bool generate_params_layout();
        bool generate_textures_layout();

    private:
        fast_map<StringId, ShaderParameter> m_parameters;
        fast_map<StringId, ShaderTexture>   m_textures;
        fast_set<StringId>                  m_keywords;
        std::string                         m_include_textures;
        std::string                         m_include_parameters;
        StringId                            m_domain;
        StringId                            m_render_queue;
        GfxPolyMode                         m_poly_mode       = GfxPolyMode::Fill;
        GfxPolyCullMode                     m_cull_mode       = GfxPolyCullMode::Disabled;
        GfxPolyFrontFace                    m_front_face      = GfxPolyFrontFace::CounterClockwise;
        bool                                m_depth_enable    = false;
        bool                                m_depth_write     = true;
        GfxCompFunc                         m_depth_func      = GfxCompFunc::Less;
        int                                 m_parameters_size = -1;
    };

}// namespace wmoge

#endif//WMOGE_MATERIAL_SHADER_HPP
