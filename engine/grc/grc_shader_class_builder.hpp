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

#include "grc/grc_shader_class.hpp"
#include "grc/grc_shader_reflection.hpp"

#include <memory>

namespace wmoge {

    /**
     * @class GrcShaderClassBuilder
     * @brief Allows to configure a particular shader class and register in the engine
    */
    class GrcShaderClassBuilder {
    public:
        class StructBuilder;
        class SpaceBuilder;
        class PassBuilder;
        class TechniqueBuilder;

        class StructBuilder {
        public:
            StructBuilder(GrcShaderClassBuilder& owner, Ref<GrcShaderType> struct_type);

            StructBuilder&         add_field(Strid name, Strid struct_type);
            StructBuilder&         add_field(Strid name, Ref<GrcShaderType> type, Var value = Var());
            StructBuilder&         add_field_array(Strid name, Strid struct_type, int n_elements = 0);
            StructBuilder&         add_field_array(Strid name, Ref<GrcShaderType> type, int n_elements = 0, Var value = Var());
            GrcShaderClassBuilder& end_struct();

        private:
            GrcShaderClassBuilder& m_owner;
            Ref<GrcShaderType>     m_struct_type;

            friend class GrcShaderClassBuilder;
        };

        class SpaceBuilder {
        public:
            SpaceBuilder(GrcShaderClassBuilder& owner, GrcShaderSpace& space);

            SpaceBuilder&          add_inline_uniform_buffer(Strid name, Strid type_struct);
            SpaceBuilder&          add_uniform_buffer(Strid name, Strid type_struct);
            SpaceBuilder&          add_texture_2d(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler);
            SpaceBuilder&          add_texture_2d_array(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler);
            SpaceBuilder&          add_texture_cube(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler);
            SpaceBuilder&          add_storage_buffer(Strid name, Strid type_struct);
            SpaceBuilder&          add_storage_image_2d(Strid name);
            GrcShaderClassBuilder& end_space();

        private:
            GrcShaderClassBuilder& m_owner;
            GrcShaderSpace&        m_space;

            friend class GrcShaderClassBuilder;
        };

        class PassBuilder {
        public:
            PassBuilder(GrcShaderClassBuilder& owner, GrcShaderPass& pass, TechniqueBuilder& technique);

            PassBuilder&      add_option(Strid name, const fast_vector<Strid>& variants);
            PassBuilder&      add_state(const GrcPipelineState& state);
            TechniqueBuilder& end_pass();

        private:
            GrcShaderClassBuilder& m_owner;
            GrcShaderPass&         m_pass;
            TechniqueBuilder&      m_technique;
            std::int16_t           m_next_option_idx = 0;

            friend class GrcShaderClassBuilder;
        };

        class TechniqueBuilder {
        public:
            TechniqueBuilder(GrcShaderClassBuilder& owner, GrcShaderTechnique& technique);

            TechniqueBuilder&      add_option(Strid name, const fast_vector<Strid>& variants);
            PassBuilder            add_pass(Strid name);
            GrcShaderClassBuilder& end_technique();

        private:
            GrcShaderClassBuilder& m_owner;
            GrcShaderTechnique&    m_technique;
            std::int16_t           m_next_option_idx  = 0;
            std::int16_t           m_next_variant_idx = 0;

            friend class PassBuilder;
            friend class GrcShaderClassBuilder;
        };

        GrcShaderClassBuilder& set_name(Strid name);
        GrcShaderClassBuilder& add_source(Strid file, GfxShaderModule module);
        GrcShaderClassBuilder& add_constant(Strid name, Var value);
        StructBuilder          add_struct(Strid name, int byte_size);
        SpaceBuilder           add_space(Strid name, GrcShaderSpaceType type);
        TechniqueBuilder       add_technique(Strid name);

        Status finish(std::shared_ptr<GrcShaderClass>& shader_class);

    private:
        GrcShaderReflection m_reflection;
        std::int16_t        m_next_technique_idx = 0;

        friend class StructBuilder;
        friend class SpaceBuilder;
        friend class PassBuilder;
        friend class TechniqueBuilder;
    };

}// namespace wmoge