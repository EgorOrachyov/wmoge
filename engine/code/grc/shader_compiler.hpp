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
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/task_manager.hpp"
#include "gfx/gfx_defs.hpp"
#include "grc/shader_reflection.hpp"
#include "rtti/traits.hpp"

#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /** 
     * @class ShaderCompilerInputFile
     * @brief Input file to compile 
     */
    struct ShaderCompilerInputFile {
        Strid           name;
        Strid           file_path;
        std::string     entry_point;
        GfxShaderModule module_type;
    };

    /**
     * @class ShaderCompilerOptions
     * @brief Compiler options for compilation
    */
    struct ShaderCompilerOptions {
        bool generate_degug_info = true;
        bool strip_debug_info    = false;
        bool disable_otimizer    = true;
        bool optimize_size       = false;
        bool validate            = true;
        bool dump_on_failure     = true;
    };

    /**
     * @class ShaderCompilerEnv
     * @brief Compiler enviroment
    */
    struct ShaderCompilerEnv {
        flat_map<std::string, std::string> virtual_includes;
        flat_set<std::string>              path_includes;
        flat_map<Strid, std::string>       defines;

        void set_define(const Strid& def) { defines[def] = ""; }
        void set_define(const Strid& def, std::string val) { defines[def] = std::move(val); }
        void set_define(const Strid& def, int val) { defines[def] = std::to_string(val); }

        void merge(const ShaderCompilerEnv& other) {
            virtual_includes.insert(other.virtual_includes.begin(), other.virtual_includes.end());
            path_includes.insert(other.path_includes.begin(), other.path_includes.end());
            defines.insert(other.defines.begin(), other.defines.end());
        }
    };

    /** 
     * @class ShaderCompilerInputFile
     * @brief Compiler input 
     */
    struct ShaderCompilerInput {
        using File = ShaderCompilerInputFile;

        buffered_vector<File> files;
        ShaderCompilerOptions options;
        ShaderCompilerEnv     env;
        GfxShaderLang         language;
        Strid                 name;
    };

    /** 
     * @class ShaderCompilerOutput
     * @brief Compiler result
    */
    struct ShaderCompilerOutput {
        buffered_vector<Ref<Data>>   bytecode;
        buffered_vector<Sha256>      source_hashes;
        buffered_vector<Sha256>      bytecode_hashes;
        buffered_vector<std::string> errors;
        Status                       status;
        float                        time_sec = 0.0f;
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
     * @class ShaderCodeBuilder
     * @brief Builder to construct shader code file
    */
    class ShaderCodeBuilder {
    public:
        virtual ~ShaderCodeBuilder() = default;

        virtual void set_version(int version, bool core_profile)                                         = 0;
        virtual void set_module(GfxShaderModule module)                                                  = 0;
        virtual void add_define(Strid define)                                                            = 0;
        virtual void add_define(Strid define, const std::string& value)                                  = 0;
        virtual void add_vertex_input(int location, const std::string& type, const std::string& name)    = 0;
        virtual void add_sampler2d_binding(int space, int slot, Strid name)                              = 0;
        virtual void add_sampler2dArray_binding(int space, int slot, Strid name)                         = 0;
        virtual void add_samplerCube_binding(int space, int slot, Strid name)                            = 0;
        virtual void add_image_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers)     = 0;
        virtual void begin_storage_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers) = 0;
        virtual void end_storage_binding()                                                               = 0;
        virtual void begin_uniform_binding(int space, int slot, Strid name, ShaderQualifiers qualifiers) = 0;
        virtual void end_uniform_binding()                                                               = 0;
        virtual void begin_struct(Strid name)                                                            = 0;
        virtual void end_struct()                                                                        = 0;
        virtual void add_field(Strid type_name, Strid field_name)                                        = 0;
        virtual void add_field(Strid type_name, Strid field_name, std::optional<int> num_elements)       = 0;
        virtual void add_source(const std::string& source)                                               = 0;

        virtual std::string emit() const = 0;
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

        /** @brief Constructs code builder for this compiler */
        virtual std::shared_ptr<ShaderCodeBuilder> make_builder() { return nullptr; }

        /** @brief Returns shader platform of this compiler instance */
        virtual GfxShaderPlatform get_platform() { return GfxShaderPlatform::None; }

        /** @brief Returns shader lang of this compiler instance */
        virtual GfxShaderLang get_lang() { return GfxShaderLang::None; }
    };

    WG_RTTI_CLASS_BEGIN(ShaderCompiler) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class ShaderTaskManager
     * @brief Task manager to schedule shader compilation jobs
    */
    class ShaderTaskManager : public TaskManager {
    public:
        ShaderTaskManager(int num_workers) : TaskManager(num_workers, "shader-system") {}
        ~ShaderTaskManager() = default;
    };

}// namespace wmoge