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

#include "glsl_builder.hpp"

namespace wmoge {

    void GlslBuilder::set_version(int version, bool core_profile) {
        m_stream << "#version " << version;
        if (core_profile) {
            m_stream << " core";
        }
        m_stream << "\n\n";
    }

    void GlslBuilder::set_module(GfxShaderModule module) {
        m_stream << "#define " << GfxShaderModuleGlslDefines[int(module)] << "\n";
        m_module = module;
    }

    void GlslBuilder::add_define(Strid define) {
        m_stream << "#define " << define.str() << "\n";
    }

    void GlslBuilder::add_define(Strid define, const std::string& value) {
        m_stream << "#define " << define.str() << " " << value << "\n";
    }

    void GlslBuilder::add_vertex_input(int location, const std::string& type, const std::string& name) {
        m_stream << "layout (location=" << location << ") in " << type << " " << name << ";\n";
    }

    void GlslBuilder::add_sampler2d_binding(int space, int slot, Strid name) {
        m_stream << "layout (set=" << space << ", binding=" << slot << ") uniform sampler2D " << name.str() << ";\n";
    }

    void GlslBuilder::add_sampler2dArray_binding(int space, int slot, Strid name) {
        m_stream << "layout (set=" << space << ", binding=" << slot << ") uniform sampler2DArray " << name.str() << ";\n";
    }

    void GlslBuilder::add_samplerCube_binding(int space, int slot, Strid name) {
        m_stream << "layout (set=" << space << ", binding=" << slot << ") uniform samplerCube " << name.str() << ";\n";
    }

    void GlslBuilder::add_image_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers) {
        m_stream << "layout (set=" << space << ", binding=" << slot << ", ";

        if (qualifiers.get(ShaderQualifier::Rgba16f)) {
            m_stream << "rgba16f";
        }

        m_stream << ") uniform ";

        if (qualifiers.get(ShaderQualifier::Readonly)) {
            m_stream << "readonly";
        }
        if (qualifiers.get(ShaderQualifier::Writeonly)) {
            m_stream << "writeonly";
        }

        m_stream << " image2D " << name.str() << ";\n";
    }

    void GlslBuilder::begin_storage_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers) {
        m_stream << "layout (set=" << space << ", binding=" << slot << ", ";

        if (qualifiers.get(ShaderQualifier::Std140)) {
            m_stream << "std140";
        }
        if (qualifiers.get(ShaderQualifier::Std430)) {
            m_stream << "std430";
        }

        m_stream << ") ";

        if (qualifiers.get(ShaderQualifier::Readonly)) {
            m_stream << "readonly";
        }
        if (qualifiers.get(ShaderQualifier::Writeonly)) {
            m_stream << "writeonly";
        }

        m_stream << " buffer " << name.str() << " {\n";
    }

    void GlslBuilder::end_storage_binding() {
        m_stream << "};\n";
    }

    void GlslBuilder::begin_uniform_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers) {
        m_stream << "layout (set=" << space << ", binding=" << slot << ", ";

        if (qualifiers.get(ShaderQualifier::Std140)) {
            m_stream << "std140";
        }
        if (qualifiers.get(ShaderQualifier::Std430)) {
            m_stream << "std430";
        }

        m_stream << ") uniform " << name.str() << " {\n";
    }

    void GlslBuilder::end_uniform_binding() {
        m_stream << "};\n";
    }

    void GlslBuilder::begin_struct(Strid name) {
        m_stream << "struct " << name.str() << " {\n";
    }

    void GlslBuilder::end_struct() {
        m_stream << "};\n";
    }

    void GlslBuilder::add_field(Strid type_name, Strid field_name) {
        m_stream << m_spacing << type_name.str() << " " << field_name.str() << ";\n";
    }

    void GlslBuilder::add_field(Strid type_name, Strid field_name, std::optional<int> num_elements) {
        m_stream << m_spacing << type_name.str() << " " << field_name.str() << "[";

        if (num_elements) {
            m_stream << num_elements.value();
        }

        m_stream << "];\n";
    }

    void GlslBuilder::add_source(const std::string& source) {
        m_stream << source << "\n";
    }

    std::string GlslBuilder::emit() const {
        return m_stream.str();
    }

}// namespace wmoge