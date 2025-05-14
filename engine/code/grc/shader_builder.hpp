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

#include "grc/shader_reflection.hpp"

#include <memory>

namespace wmoge {

    /**
     * @class ShaderBuilder
     * @brief Allows to configure a particular shader and register in the engine
    */
    class ShaderBuilder {
    public:
        class StructBuilder;
        class SpaceBuilder;
        class PassBuilder;
        class TechniqueBuilder;

        friend StructBuilder;
        friend SpaceBuilder;
        friend PassBuilder;
        friend TechniqueBuilder;

        class StructBuilder {
        public:
            StructBuilder(ShaderBuilder& owner, Ref<ShaderType> struct_type);

            StructBuilder& add_field(Strid name, Strid type, std::string value = std::string());
            StructBuilder& add_field_array(Strid name, Strid type, int n_elements = 0, std::string value = std::string());
            ShaderBuilder& end_struct();

        private:
            ShaderBuilder&  m_owner;
            Ref<ShaderType> m_struct_type;

            friend class ShaderBuilder;
        };

        class SpaceBuilder {
        public:
            SpaceBuilder(ShaderBuilder& owner, ShaderSpace& space);

            SpaceBuilder&  add_inline_uniform_buffer(Strid name, Strid type_struct);
            SpaceBuilder&  add_uniform_buffer(Strid name, Strid type_struct);
            SpaceBuilder&  add_texture_2d(Strid name, DefaultTexture texture, DefaultSampler sampler);
            SpaceBuilder&  add_texture_2d_array(Strid name, DefaultTexture texture, DefaultSampler sampler);
            SpaceBuilder&  add_texture_cube(Strid name, DefaultTexture texture, DefaultSampler sampler);
            SpaceBuilder&  add_storage_buffer(Strid name, Strid type_struct);
            SpaceBuilder&  add_storage_image_2d(Strid name, ShaderQualifiers qualifiers);
            ShaderBuilder& end_space();

        private:
            ShaderBuilder& m_owner;
            ShaderSpace&   m_space;

            friend class ShaderBuilder;
        };

        class PassBuilder {
        public:
            PassBuilder(ShaderBuilder& owner, ShaderPassInfo& pass, TechniqueBuilder& technique);

            PassBuilder&      add_option(Strid name, const buffered_vector<Strid>& variants);
            PassBuilder&      add_ui_info(const std::string& name, const std::string& hint);
            PassBuilder&      add_state(const PipelineState& state);
            PassBuilder&      add_tag(Strid name, std::string value);
            TechniqueBuilder& end_pass();

        private:
            ShaderBuilder&    m_owner;
            ShaderPassInfo&   m_pass;
            TechniqueBuilder& m_technique;
            std::int16_t      m_next_option_idx  = 0;
            std::int16_t      m_next_variant_idx = 0;

            friend class ShaderBuilder;
        };

        class TechniqueBuilder {
        public:
            TechniqueBuilder(ShaderBuilder& owner, ShaderTechniqueInfo& technique);

            TechniqueBuilder& add_tag(Strid name, std::string value);
            TechniqueBuilder& add_ui_info(const std::string& name, const std::string& hint);
            PassBuilder       add_pass(Strid name);
            ShaderBuilder&    end_technique();

        private:
            ShaderBuilder&       m_owner;
            ShaderTechniqueInfo& m_technique;
            std::int16_t         m_next_pass_idx = 0;

            friend class PassBuilder;
            friend class ShaderBuilder;
        };

        ShaderBuilder&   set_name(Strid name);
        ShaderBuilder&   set_domain(ShaderDomain domain);
        ShaderBuilder&   add_ui_info(const std::string& name, const std::string& hint);
        ShaderBuilder&   add_source(Strid file, GfxShaderModule module, GfxShaderLang lang);
        ShaderBuilder&   add_constant(Strid name, std::string value);
        ShaderBuilder&   add_type(const Ref<ShaderType>& type);
        StructBuilder    add_struct(Strid name, int byte_size);
        SpaceBuilder     add_space(Strid name, ShaderSpaceType type);
        TechniqueBuilder add_technique(Strid name);

        Status            finish();
        ShaderReflection& get_reflection();

    private:
        void                                 add_type_idx(const Ref<ShaderType>& type, bool allow_duplicates = false);
        [[nodiscard]] ShaderTypeIdx          get_or_add_type_idx(const Ref<ShaderType>& type);
        [[nodiscard]] ShaderTypeIdx          get_type_idx(Strid type);
        [[nodiscard]] const Ref<ShaderType>& get_type(const ShaderTypeIdx& idx);

    private:
        ShaderReflection              m_reflection;
        std::int16_t                  m_next_technique_idx = 0;
        std::int16_t                  m_next_type_idx      = 0;
        flat_map<Strid, std::int16_t> m_name_idx_map;
        std::vector<Ref<ShaderType>>  m_types_list;

        friend class StructBuilder;
        friend class SpaceBuilder;
        friend class PassBuilder;
        friend class TechniqueBuilder;
    };

}// namespace wmoge