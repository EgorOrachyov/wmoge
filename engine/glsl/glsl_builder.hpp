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

#include "core/string_id.hpp"
#include "grc/shader_reflection.hpp"

#include <sstream>
#include <string>

namespace wmoge {

    /**
     * @class GlslBuilder
     * @brief Builder to construct glsl code file
    */
    class GlslBuilder {
    public:
        GlslBuilder() = default;

        void set_version(int version, bool core_profile);
        void set_module(GfxShaderModule module);
        void add_define(Strid define);
        void add_define(Strid define, const std::string& value);
        void add_vertex_input(int location, const std::string& type, const std::string& name);
        void add_sampler2d_binding(int space, int slot, Strid name);
        void add_sampler2dArray_binding(int space, int slot, Strid name);
        void add_samplerCube_binding(int space, int slot, Strid name);
        void add_image_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers);
        void begin_storage_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers);
        void end_storage_binding();
        void begin_uniform_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers);
        void end_uniform_binding();
        void begin_struct(Strid name);
        void end_struct();
        void add_field(Strid type_name, Strid field_name);
        void add_field(Strid type_name, Strid field_name, std::optional<int> num_elements);
        void add_source(const std::string& source);

        [[nodiscard]] std::string emit() const;

    private:
        GfxShaderModule   m_module;
        GfxShaderPlatform m_platform;
        GfxType           m_driver_type;
        std::stringstream m_stream;
        std::string       m_spacing = "    ";
    };

}// namespace wmoge