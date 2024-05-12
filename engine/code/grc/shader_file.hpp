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
#include "grc/shader_reflection.hpp"
#include "platform/file_system.hpp"
#include "rtti/traits.hpp"

#include <optional>

namespace wmoge {

    /** @brief Compilation option */
    struct ShaderFileOption {
        WG_RTTI_STRUCT(ShaderFileOption);

        Strid                  name;
        buffered_vector<Strid> variants;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFileOption) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(variants, {});
    }
    WG_RTTI_END;

    /** @brief Single param defenition */
    struct ShaderFileParam {
        WG_RTTI_STRUCT(ShaderFileParam);

        Strid             name;
        Strid             type;
        int               elements = 1;
        std::string       value;
        std::string       ui_name;
        std::string       ui_hint;
        ShaderBindingType binding = ShaderBindingType::None;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFileParam) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(elements, {RttiOptional});
        WG_RTTI_FIELD(value, {RttiOptional});
        WG_RTTI_FIELD(ui_name, {RttiOptional});
        WG_RTTI_FIELD(ui_hint, {RttiOptional});
        WG_RTTI_FIELD(binding, {RttiOptional});
    }
    WG_RTTI_END;

    /** @brief Params block definition */
    struct ShaderFileParamBlock {
        WG_RTTI_STRUCT(ShaderFileParamBlock);

        Strid                            name;
        ShaderSpaceType                  type;
        buffered_vector<ShaderFileParam> params;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFileParamBlock) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {RttiOptional});
        WG_RTTI_FIELD(params, {RttiOptional});
    }
    WG_RTTI_END;

    /** @brief Single technique pass */
    struct ShaderFilePass {
        WG_RTTI_STRUCT(ShaderFilePass);

        Strid                             name;
        PipelineState                     state;
        buffered_vector<ShaderFileOption> options;
        flat_map<Strid, std::string>      tags;
        std::string                       ui_name;
        std::string                       ui_hint;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFilePass) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(state, {RttiOptional});
        WG_RTTI_FIELD(options, {RttiOptional});
        WG_RTTI_FIELD(tags, {RttiOptional});
        WG_RTTI_FIELD(ui_name, {RttiOptional});
        WG_RTTI_FIELD(ui_hint, {RttiOptional});
    }
    WG_RTTI_END;

    /** @brief Technique defenition */
    struct ShaderFileTechnique {
        WG_RTTI_STRUCT(ShaderFileTechnique);

        Strid                             name;
        buffered_vector<ShaderFileOption> options;
        buffered_vector<ShaderFilePass>   passes;
        flat_map<Strid, std::string>      tags;
        std::string                       ui_name;
        std::string                       ui_hint;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFileTechnique) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(options, {RttiOptional});
        WG_RTTI_FIELD(passes, {RttiOptional});
        WG_RTTI_FIELD(tags, {RttiOptional});
        WG_RTTI_FIELD(ui_name, {RttiOptional});
        WG_RTTI_FIELD(ui_hint, {RttiOptional});
    }
    WG_RTTI_END;

    /** @brief Source code of shader per module */
    struct ShaderFileSource {
        WG_RTTI_STRUCT(ShaderFileSource);

        Strid           file;
        GfxShaderModule module;
        GfxShaderLang   lang = GfxShaderLang::GlslVk450;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFileSource) {
        WG_RTTI_FIELD(file, {});
        WG_RTTI_FIELD(module, {});
        WG_RTTI_FIELD(lang, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class ShaderFile
     * @brief Serializable representation of a shader file
    */
    struct ShaderFile {
        WG_RTTI_STRUCT(ShaderFile);

        Strid                                 name;
        Strid                                 extends;
        ShaderDomain                          domain;
        std::string                           ui_name;
        std::string                           ui_hint;
        buffered_vector<ShaderFileParamBlock> param_blocks;
        buffered_vector<ShaderFileTechnique>  techniques;
        buffered_vector<ShaderFileSource>     sources;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderFile) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(extends, {RttiOptional});
        WG_RTTI_FIELD(domain, {});
        WG_RTTI_FIELD(ui_name, {RttiOptional});
        WG_RTTI_FIELD(ui_hint, {RttiOptional});
        WG_RTTI_FIELD(param_blocks, {RttiOptional});
        WG_RTTI_FIELD(techniques, {RttiOptional});
        WG_RTTI_FIELD(sources, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge