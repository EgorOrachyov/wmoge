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

#include "core/buffered_vector.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "grc/grc_shader_reflection.hpp"
#include "io/serialization.hpp"
#include "platform/file_system.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class GrcShaderScriptFile
     * @brief Serializable representation of a script file
    */
    struct GrcShaderScriptFile {

        /** @brief Compilation option */
        struct Option {
            Strid                  name;
            buffered_vector<Strid> variants;

            WG_IO_DECLARE(Option);
        };

        /** @brief Single param defenition */
        struct Param {
            Strid                name;
            Strid                type;
            int                  elements = 1;
            std::string          value;
            std::string          ui_name;
            std::string          ui_hint;
            GrcShaderBindingType binding = GrcShaderBindingType::None;

            WG_IO_DECLARE(Param);
        };

        /** @brief Params block definition */
        struct ParamBlock {
            Strid                  name;
            GrcShaderSpaceType     type;
            buffered_vector<Param> params;

            WG_IO_DECLARE(ParamBlock);
        };

        /** @brief Single technique pass */
        struct Pass {
            Strid                        name;
            GrcPipelineState             state;
            buffered_vector<Option>      options;
            flat_map<Strid, std::string> tags;
            std::string                  ui_name;
            std::string                  ui_hint;

            WG_IO_DECLARE(Pass);
        };

        /** @brief Technique defenition */
        struct Technique {
            Strid                        name;
            buffered_vector<Option>      options;
            buffered_vector<Pass>        passes;
            flat_map<Strid, std::string> tags;
            std::string                  ui_name;
            std::string                  ui_hint;

            WG_IO_DECLARE(Technique);
        };

        /** @brief Source code of shader per module */
        struct Source {
            std::string     file;
            GfxShaderModule module;

            WG_IO_DECLARE(Source);
        };

        Strid                       name;
        Strid                       extends;
        std::string                 ui_name;
        std::string                 ui_hint;
        buffered_vector<ParamBlock> param_blocks;
        buffered_vector<Technique>  techniques;
        buffered_vector<Source>     sources;

        WG_IO_DECLARE(GrcShaderScriptFile);
    };

    /**
     * @class GrcShaderScript
     * @brief Reprsents a particular shader program script
     * 
     * GrcShaderScript is a high level representation of a shading program.
     * It provides a connection between raw glsl sources code of a shader,
     * material and engine gfx module for runtime usage.
     * 
     * GrcShaderScript provides info about a particular shader type. It provides
     * layout information, parameters and structures layout, defines and
     * compilations options, constants and includes, and provides hot-reloading
     * mechanism for debugging. 
     * 
     * GrcShaderScript is a `template` shader for drawing with pre-defined interface.
     * It is not suitable for rendering. In order to get a concrete instance of 
     * compiled gpu program, pass and options must be provided from GrcShader.
    */
    class GrcShaderScript : public RefCnt {
    public:
        GrcShaderScript(GrcShaderReflection&& reflection);
        ~GrcShaderScript() override = default;

        std::optional<std::int16_t>        find_technique(Strid name);
        std::optional<std::int16_t>        find_pass(std::int16_t technique, Strid name);
        GrcShaderParamId                   get_param_id(Strid name);
        std::optional<GrcShaderParamInfo*> get_param_info(GrcShaderParamId id);
        Status                             reload_sources(const std::string& folder, FileSystem* fs);
        Status                             fill_layout(GfxDescSetLayoutDesc& desc, int space) const;
        bool                               has_dependency(const Strid& dependency) const;
        bool                               has_space(GrcShaderSpaceType space_type) const;

        [[nodiscard]] const GrcShaderReflection& get_reflection() const { return m_reflection; }
        [[nodiscard]] GrcShaderReflection&       get_reflection() { return m_reflection; }
        [[nodiscard]] const Strid&               get_name() const { return m_reflection.shader_name; }

    private:
        GrcShaderReflection m_reflection;
    };

}// namespace wmoge