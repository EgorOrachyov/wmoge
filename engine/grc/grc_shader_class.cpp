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

#include "grc_shader_class.hpp"

#include "glsl/glsl_include_processor.hpp"

#include <cassert>

namespace wmoge {

    GrcShaderClass::GrcShaderClass(GrcShaderReflection&& reflection)
        : m_reflection(std::move(reflection)) {
    }

    Status GrcShaderClass::reload_sources(const std::string& folder, FileSystem* fs) {
        fast_vector<GrcShaderInclude>    new_includes;
        fast_vector<GrcShaderSourceFile> new_sources;
        fast_set<Strid>                  new_dependencies;

        for (GrcShaderSourceFile& source_file : m_reflection.sources) {
            GlslIncludeProcessor include_processor(folder, fs);

            if (!include_processor.parse_file(source_file.name)) {
                WG_LOG_ERROR("failed parse file " << source_file.name);
                return StatusCode::FailedParse;
            }

            for (const Strid& include : include_processor.get_includes()) {
                GrcShaderInclude& new_include = new_includes.emplace_back();
                new_include.module            = source_file.module;
                new_include.name              = include;
            }

            GrcShaderSourceFile& new_source_file = new_sources.emplace_back();
            new_source_file.name                 = source_file.name;
            new_source_file.module               = source_file.module;
            new_source_file.content              = std::move(include_processor.get_result());
        }

        for (GrcShaderInclude& new_include_file : new_includes) {
            new_dependencies.insert(new_include_file.name);
        }

        for (GrcShaderSourceFile& new_source_file : new_sources) {
            new_dependencies.insert(new_source_file.name);
        }

        std::swap(m_reflection.includes, new_includes);
        std::swap(m_reflection.sources, new_sources);
        std::swap(m_reflection.dependencies, new_dependencies);

        return StatusCode::Ok;
    }

    Status GrcShaderClass::fill_layout(GfxDescSetLayoutDesc& desc, int space) const {
        assert(0 <= space && space < m_reflection.spaces.size());

        const GrcShaderSpace& shader_space = m_reflection.spaces[space];
        assert(shader_space.type != GrcShaderSpaceType::Material);

        std::int16_t binding_id = 0;

        for (const GrcShaderBinding& binding : shader_space.bindings) {
            GfxDescBinging& gfx_binding = desc.emplace_back();
            gfx_binding.binding         = binding_id++;
            gfx_binding.count           = 1;
            gfx_binding.name            = binding.name;

            switch (binding.binding) {
                case GrcShaderBindingType::InlineUniformBuffer:
                case GrcShaderBindingType::UniformBuffer:
                    gfx_binding.type = GfxBindingType::UniformBuffer;
                    break;

                case GrcShaderBindingType::Sampler2d:
                case GrcShaderBindingType::Sampler2dArray:
                case GrcShaderBindingType::SamplerCube:
                    gfx_binding.type = GfxBindingType::SampledTexture;
                    break;

                case GrcShaderBindingType::StorageBuffer:
                    gfx_binding.type = GfxBindingType::StorageBuffer;
                    break;

                case GrcShaderBindingType::StorageImage2d:
                    gfx_binding.type = GfxBindingType::StorageImage;
                    break;

                default:
                    return StatusCode::InvalidState;
            }
        }

        return StatusCode::Ok;
    }

    bool GrcShaderClass::has_dependency(const Strid& dependency) const {
        return m_reflection.dependencies.find(dependency) != m_reflection.dependencies.end();
    }

    bool GrcShaderClass::has_space(GrcShaderSpaceType space_type) const {
        for (const GrcShaderSpace& space : m_reflection.spaces) {
            if (space.type == space_type) {
                return true;
            }
        }
        return false;
    }

}// namespace wmoge