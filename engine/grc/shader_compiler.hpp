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

#include "core/async.hpp"
#include "core/buffered_vector.hpp"
#include "core/data.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "rtti/traits.hpp"

#include <vector>

namespace wmoge {

    /** 
     * @class ShaderCompilerInputFile
     * @brief Input file to compile 
     */
    struct ShaderCompilerInputFile {
        std::string     source_code;
        std::string     entry_point;
        GfxShaderModule module_type;
    };

    /** 
     * @class ShaderCompilerInputFile
     * @brief Compiler input 
     */
    struct ShaderCompilerInput {
        using File = ShaderCompilerInputFile;

        buffered_vector<File> files;
        Strid                 name;
        bool                  disable_otimizer = false;
        bool                  optimize_size    = false;
        bool                  validate         = true;
        GfxShaderLang         language;
    };

    /** 
     * @class ShaderCompilerOutput
     * @brief Compiler result
    */
    struct ShaderCompilerOutput {
        buffered_vector<Ref<Data>>   bytecode;
        buffered_vector<std::string> errors;
    };

    /**
     * @class ShaderCompilerRequest
     * @brief Request to compile a shader program
    */
    class ShaderCompilerRequest : public RefCnt {
    public:
        ~ShaderCompilerRequest() override = default;

        ShaderCompilerInput  input;
        ShaderCompilerOutput output;
    };

    /**
     * @class ShaderCompiler
     * @brief Interface to a shader compiler
    */
    class ShaderCompiler : public RttiObject {
    public:
        WG_RTTI_CLASS(ShaderCompiler, RttiObject);

        ShaderCompiler()           = default;
        ~ShaderCompiler() override = default;

        /**
         * @brief Submit for async shader program compilation
         * 
         * @param request Request with program data for compilation
         * @param depends_on Optional sync dependency to wait before compilation
         * 
         * @return Async to track compilation
        */
        virtual Async compile(const Ref<ShaderCompilerRequest>& request, const Async& depends_on = Async()) { return Async(); }

        /**
         * @brief Returns shader platform of this compiler instance
        */
        virtual GfxShaderPlatform get_platform() { return GfxShaderPlatform::None; }
    };

    WG_RTTI_CLASS_BEGIN(ShaderCompiler) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge