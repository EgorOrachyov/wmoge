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

#include "shader.hpp"

#include "core/log.hpp"
#include "grc/shader_builder.hpp"
#include "grc/shader_compiler.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace wmoge {

    Shader::~Shader() {
        if (m_callback) {
            (*m_callback)(this);
        }
    }

    Shader::Shader(ShaderReflection&& reflection) : m_reflection(std::move(reflection)) {
    }

    Status Shader::fill_layout_desc(GfxDescSetLayoutDesc& desc, std::int16_t space) const {
        WG_PROFILE_CPU_GRC("Shader::fill_layout_desc");

        assert(0 <= space && space < m_reflection.spaces.size());

        const ShaderSpace& shader_space = m_reflection.spaces[space];

        std::int16_t binding_id = 0;

        for (const ShaderBinding& binding : shader_space.bindings) {
            GfxDescBinging& gfx_binding = desc.emplace_back();
            gfx_binding.binding         = binding_id++;
            gfx_binding.count           = 1;
            gfx_binding.name            = binding.name;

            switch (binding.binding) {
                case ShaderBindingType::InlineUniformBuffer:
                case ShaderBindingType::UniformBuffer:
                    gfx_binding.type = GfxBindingType::UniformBuffer;
                    break;

                case ShaderBindingType::Sampler2d:
                case ShaderBindingType::Sampler2dArray:
                case ShaderBindingType::SamplerCube:
                    gfx_binding.type = GfxBindingType::SampledTexture;
                    break;

                case ShaderBindingType::StorageBuffer:
                    gfx_binding.type = GfxBindingType::StorageBuffer;
                    break;

                case ShaderBindingType::StorageImage2d:
                    gfx_binding.type = GfxBindingType::StorageImage;
                    break;

                default:
                    return StatusCode::InvalidState;
            }
        }

        return WG_OK;
    }

    Status Shader::fill_compiler_env(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompiler* compiler, ShaderCompilerEnv& compiler_env) {
        WG_PROFILE_CPU_GRC("Shader::fill_compiler_env");

        compiler_env.set_define(SID(GfxShaderPlatformGlslDefines[int(platform)]));
        compiler_env.set_define(SID("TECHNIQUE_IDX"), permutation.technique_idx);
        compiler_env.set_define(SID("PASS_IDX"), permutation.pass_idx);

        permutation.vert_attribs.for_each([&](int idx, GfxVertAttrib) {
            compiler_env.set_define(SID(GfxVertAttribGlslDefines[idx]));
        });

        for (std::int16_t i = 0; i < ShaderOptions::MAX_OPTIONS; i++) {
            if (permutation.options[i]) {
                Strid option, variant;
                WG_CHECKED(fill_option_info(permutation.technique_idx, permutation.pass_idx, i, option, variant));
                compiler_env.set_define(SID(option.str() + "_" + variant.str()));
            }
        }

        WG_CHECKED(fill_declarations(lang, compiler, compiler_env.virtual_includes["generated/declarations.glsl"]));
        WG_CHECKED(fill_vertex_input(lang, compiler, permutation, compiler_env.virtual_includes["generated/input.glsl"]));

        return WG_OK;
    }

    Status Shader::fill_compiler_input(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompiler* compiler, ShaderCompilerInput& compiler_input) {
        WG_PROFILE_CPU_GRC("Shader::fill_compiler_input");

        std::string program_name;
        if (!fill_program_name(lang, platform, permutation, program_name)) {
            WG_LOG_ERROR("failed to get program name to compile " << get_shader_name());
            return StatusCode::Error;
        }

        if (!fill_compiler_env(lang, platform, permutation, compiler, compiler_input.env)) {
            WG_LOG_ERROR("failed to fill env to compile " << program_name);
            return StatusCode::Error;
        }

        for (ShaderSourceFile& source_file : m_reflection.sources) {
            if (source_file.lang == lang) {
                ShaderCompilerInputFile& f = compiler_input.files.emplace_back();
                f.name                     = source_file.file;
                f.file_path                = source_file.file;
                f.module_type              = source_file.module;
                f.entry_point              = "main";
            }
        }

        compiler_input.name     = SID(program_name);
        compiler_input.language = lang;

        return WG_OK;
    }

    Status Shader::fill_program_name(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, std::string& name) {
        WG_PROFILE_CPU_GRC("Shader::fill_program_name");

        std::stringstream stream;

        stream << "shader=" << get_shader_name() << " ";
        stream << "perm=" << permutation.hash() << " ";
        stream << "tech=" << permutation.technique_idx << " ";
        stream << "pass=" << permutation.pass_idx << " ";
        stream << "vf=" << permutation.vert_attribs.to_string() << " ";
        stream << "opts: ";

        for (std::int16_t i = 0; i < ShaderOptions::MAX_OPTIONS; i++) {
            if (permutation.options[i]) {
                Strid option, variant;
                WG_CHECKED(fill_option_info(permutation.technique_idx, permutation.pass_idx, i, option, variant));
                stream << option << "=" << variant << " ";
            }
        }

        name = stream.str();

        return WG_OK;
    }

    Status Shader::fill_option_info(std::int16_t technique_idx, std::int16_t pass_idx, std::int16_t permutation_bit, Strid& option, Strid& variant) {
        ShaderPassInfo& pass = m_reflection.techniques[technique_idx].passes[pass_idx];
        option               = pass.options_remap[permutation_bit];
        variant              = pass.variants_remap[permutation_bit];
        return WG_OK;
    }

    Status Shader::fill_declarations(GfxShaderLang lang, ShaderCompiler* compiler, std::string& out_declarations) {
        WG_PROFILE_CPU_GRC("Shader::fill_declarations");

        assert(lang == GfxShaderLang::GlslVk450);

        auto                                                          builder = compiler->make_builder();
        std::function<void(const Ref<ShaderTypeStruct>& struct_type)> visitor;
        flat_set<Ref<ShaderTypeStruct>>                               visited;

        auto emit_struct = [&](const Ref<ShaderTypeStruct>& struct_type) {
            for (const auto& field : struct_type->fields) {
                if (field.is_array && field.elem_count == 0) {
                    builder->add_field(field.type->name, field.name, std::nullopt);
                }
                if (field.is_array && field.elem_count > 0) {
                    builder->add_field(field.type->name, field.name, field.elem_count);
                }
                if (!field.is_array) {
                    builder->add_field(field.type->name, field.name);
                }
            }
        };

        visitor = [&](const Ref<ShaderTypeStruct>& struct_type) {
            assert(struct_type);

            if (visited.find(struct_type) != visited.end()) {
                return;
            }

            visited.insert(struct_type);

            for (const auto& filed : struct_type->fields) {
                if (filed.type->type == ShaderBaseType::Struct) {
                    visitor(filed.type.cast<ShaderTypeStruct>());
                }
            }

            builder->begin_struct(struct_type->name);
            emit_struct(struct_type);
            builder->end_struct();
        };

        // Collect all declarations and emit them first
        for (std::int16_t space_idx = 0; space_idx < get_num_spaces(); space_idx++) {
            const ShaderSpace& space = m_reflection.spaces[space_idx];

            for (std::int16_t binding_idx = 0; binding_idx < std::int16_t(space.bindings.size()); binding_idx++) {
                const ShaderBinding& binding = space.bindings[binding_idx];

                switch (binding.binding) {
                    case ShaderBindingType::InlineUniformBuffer:
                    case ShaderBindingType::UniformBuffer:
                    case ShaderBindingType::StorageBuffer: {
                        auto s = binding.type.cast<ShaderTypeStruct>();
                        assert(s);
                        for (const auto& filed : s->fields) {
                            if (filed.type->type == ShaderBaseType::Struct) {
                                visitor(filed.type.cast<ShaderTypeStruct>());
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        for (std::int16_t space_idx = 0; space_idx < get_num_spaces(); space_idx++) {
            const ShaderSpace& space = m_reflection.spaces[space_idx];

            for (std::int16_t binding_idx = 0; binding_idx < std::int16_t(space.bindings.size()); binding_idx++) {
                const ShaderBinding& binding = space.bindings[binding_idx];

                switch (binding.binding) {
                    case ShaderBindingType::InlineUniformBuffer:
                    case ShaderBindingType::UniformBuffer:
                        builder->begin_uniform_binding(space_idx, binding_idx, binding.name, binding.qualifiers);
                        emit_struct(binding.type.cast<ShaderTypeStruct>());
                        builder->end_uniform_binding();
                        break;

                    case ShaderBindingType::Sampler2d:
                        builder->add_sampler2d_binding(space_idx, binding_idx, binding.name);
                        break;

                    case ShaderBindingType::Sampler2dArray:
                        builder->add_sampler2dArray_binding(space_idx, binding_idx, binding.name);
                        break;

                    case ShaderBindingType::SamplerCube:
                        builder->add_samplerCube_binding(space_idx, binding_idx, binding.name);
                        break;

                    case ShaderBindingType::StorageImage2d:
                        builder->add_image_binding(space_idx, binding_idx, binding.name, binding.qualifiers);
                        break;

                    case ShaderBindingType::StorageBuffer:
                        builder->begin_storage_binding(space_idx, binding_idx, binding.name, binding.qualifiers);
                        emit_struct(binding.type.cast<ShaderTypeStruct>());
                        builder->end_storage_binding();
                        break;

                    default:
                        return StatusCode::InvalidState;
                }
            }
        }

        out_declarations = builder->emit();

        return WG_OK;
    }

    Status Shader::fill_vertex_input(GfxShaderLang lang, ShaderCompiler* compiler, const ShaderPermutation& permutation, std::string& out_input) {
        WG_PROFILE_CPU_GRC("Shader::fill_vertex_input");

        assert(lang == GfxShaderLang::GlslVk450);

        auto builder = compiler->make_builder();

        int next_location = 0;

        permutation.vert_attribs.for_each([&](int idx, GfxVertAttrib attrib) {
            builder->add_vertex_input(next_location, GfxVertAttribGlslTypes[idx], std::string("in") + Enum::to_str(attrib));
            next_location += 1;
        });

        out_input = builder->emit();

        return WG_OK;
    }

    std::optional<std::int16_t> Shader::find_technique(Strid name) {
        auto r = m_reflection.techniques_map.find(name);
        return r != m_reflection.techniques_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    std::optional<std::int16_t> Shader::find_pass(std::int16_t technique, Strid name) {
        auto r = m_reflection.techniques[technique].passes_map.find(name);
        return r != m_reflection.techniques[technique].passes_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    std::optional<std::int16_t> Shader::find_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) {
        if (m_reflection.techniques.size() <= technique) {
            return false;
        }
        if (m_reflection.techniques[technique].passes.size() <= pass) {
            return false;
        }

        const ShaderOptions& options = m_reflection.techniques[technique].passes[pass].options;

        auto option_query = options.options_map.find(name);
        if (option_query == options.options_map.end()) {
            return std::nullopt;
        }

        auto variant_query = options.options[option_query->second].variants.find(variant);
        if (variant_query == options.options[option_query->second].variants.end()) {
            return std::nullopt;
        }

        return variant_query->second;
    }

    std::optional<ShaderParamInfo*> Shader::find_param(ShaderParamId id) {
        if (id.is_invalid()) {
            return std::nullopt;
        }
        if (id.value >= m_reflection.params_info.size()) {
            return std::nullopt;
        }

        return &m_reflection.params_info[id.value];
    }

    ShaderParamId Shader::find_param_id(Strid name) {
        auto query = m_reflection.params_id.find(name);

        if (query != m_reflection.params_id.end()) {
            return ShaderParamId(query->second);
        }

        return ShaderParamId();
    }

    void Shader::set_shader_callback(CallbackRef callback) {
        m_callback = std::move(callback);
    }

    std::optional<ShaderPermutation> Shader::permutation(Strid technique, Strid pass, buffered_vector<ShaderOptionVariant> options, GfxVertAttribs attribs) {
        auto t = find_technique(technique);
        if (!t) {
            return std::nullopt;
        }
        auto p = find_pass(*t, pass);
        if (!p) {
            return std::nullopt;
        }

        ShaderPermutation res;
        res.technique_idx = *t;
        res.pass_idx      = *p;
        res.vert_attribs  = attribs;

        const ShaderOptions& shader_options = m_reflection.techniques[*t].passes[*p].options;

        for (const auto& option : shader_options.options) {
            res.options.set(option.variants.find(option.base_variant)->second);
        }

        for (const auto& user_option : options) {
            auto option_idx = shader_options.options_map.find(user_option.first);
            if (option_idx == shader_options.options_map.end()) {
                continue;
            }

            const auto& option = shader_options.options[option_idx->second];

            auto variant_idx = option.variants.find(user_option.second);
            if (variant_idx == option.variants.end()) {
                continue;
            }

            res.options.set(variant_idx->second);
        }

        return res;
    }

    bool Shader::has_space(ShaderSpaceType space_type) const {
        for (const ShaderSpace& space : m_reflection.spaces) {
            if (space.type == space_type) {
                return true;
            }
        }
        return false;
    }

    bool Shader::has_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) const {
        if (m_reflection.techniques.size() <= technique) {
            return false;
        }
        if (m_reflection.techniques[technique].passes.size() <= pass) {
            return false;
        }

        const ShaderOptions& options = m_reflection.techniques[technique].passes[pass].options;

        auto option_query = options.options_map.find(name);
        if (option_query == options.options_map.end()) {
            return false;
        }

        auto variant_query = options.options[option_query->second].variants.find(variant);
        return variant_query != options.options[option_query->second].variants.end();
    }

    bool Shader::is_material() const {
        return m_reflection.domain == ShaderDomain::Material;
    }

    bool Shader::is_graphics() const {
        return m_reflection.domain == ShaderDomain::Graphics;
    }

    bool Shader::is_compute() const {
        return m_reflection.domain == ShaderDomain::Compute;
    }

    const std::int16_t Shader::get_num_spaces() const {
        return std::int16_t(m_reflection.spaces.size());
    }

    const std::int16_t Shader::get_num_techniques() const {
        return std::int16_t(m_reflection.techniques.size());
    }

    const std::int16_t Shader::get_num_passes(std::int16_t technique_idx) const {
        return std::int16_t(m_reflection.techniques[technique_idx].passes.size());
    }

}// namespace wmoge