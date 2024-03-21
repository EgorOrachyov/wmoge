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

#include "core/fast_map.hpp"
#include "core/fast_set.hpp"
#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "platform/file_system.hpp"
#include "resource/shader.hpp"

#include <cinttypes>
#include <filesystem>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @brief Base (built-in) types for compositing shader interface
    */
    enum class GrcShaderBaseType {
        None,
        Int,
        Float,
        Bool,
        Struct,
        Sampler2d,
        Sampler2dArray,
        SamplerCube,
        Image2d
    };

    /**
     * @brief Sizes of of base types
     */
    static constexpr const int GrcShaderBaseTypeSizes[] = {
            0,
            4,
            4,
            4,
            0,
            0,
            0,
            0,
            0};

    /**
     * @brief Binding types supported by shader pass interface
    */
    enum class GrcShaderBindingType {
        None,
        InlineUniformBuffer,
        UniformBuffer,
        StorageBuffer,
        Sampler2d,
        Sampler2dArray,
        SamplerCube,
        StorageImage2d
    };

    /**
     * @class GrcShaderType
     * @brief Recursive complex type for declaring of anything in a shader what has a type
    */
    struct GrcShaderType : public RefCnt {
        struct Field {
            Strid              name;
            Ref<GrcShaderType> type;
            int                offset       = -1;
            int                n_array_elem = 0;
            bool               is_array     = false;
            Var                default_value;
        };

        Strid              name;
        GrcShaderBaseType  type      = GrcShaderBaseType::None;
        int                n_row     = -1;
        int                n_col     = -1;
        int                n_elem    = -1;
        int                byte_size = 0;
        std::vector<Field> fields;
    };

    /**
     * @class GrcShaderTypes
     * @brief Pre-defined common shader types
    */
    struct GrcShaderTypes {
        static Ref<GrcShaderType> FLOAT;
        static Ref<GrcShaderType> INT;
        static Ref<GrcShaderType> BOOL;
        static Ref<GrcShaderType> VEC2;
        static Ref<GrcShaderType> VEC3;
        static Ref<GrcShaderType> VEC4;
        static Ref<GrcShaderType> IVEC2;
        static Ref<GrcShaderType> IVEC3;
        static Ref<GrcShaderType> IVEC4;
        static Ref<GrcShaderType> BVEC2;
        static Ref<GrcShaderType> BVEC3;
        static Ref<GrcShaderType> BVEC4;
        static Ref<GrcShaderType> MAT2;
        static Ref<GrcShaderType> MAT3;
        static Ref<GrcShaderType> MAT4;
        static Ref<GrcShaderType> SAMPLER2D;
        static Ref<GrcShaderType> SAMPLER2D_ARRAY;
        static Ref<GrcShaderType> SAMPLER_CUBE;
        static Ref<GrcShaderType> IMAGE2D;
    };

    /**
     * @class GrcShaderConstant
     * @brief Declared pass constants inlined as defines into source code
    */
    struct GrcShaderConstant {
        Strid       name;
        Var         value;
        std::string str;
    };

    /**
     * @class GrcShaderInclude
     * @brief Information about single include file of a shader module
    */
    struct GrcShaderInclude {
        Strid           name;
        GfxShaderModule module;
    };

    /**
     * @class GrcShaderSourceFile
     * @brief Single shader module required for compilation (shader stage)
    */
    struct GrcShaderSourceFile {
        Strid           name;
        GfxShaderModule module;
        std::string     content;
    };

    /**
     * @class GrcShaderQualifiers
     * @brief Additional qualifiers for shader interface params
    */
    struct GrcShaderQualifiers {
        bool readonly  = false;
        bool writeonly = false;
        bool std140    = false;
        bool std430    = false;
    };

    /**
     * @class GrcShaderBinding
     * @brief An interface exposed bindable param
    */
    struct GrcShaderBinding {
        Strid                name;
        GrcShaderBindingType binding = GrcShaderBindingType::None;
        Ref<GrcShaderType>   type;
        GrcShaderQualifiers  qualifiers;
        Ref<GfxTexture>      default_tex;
        Ref<GfxSampler>      default_sampler;
    };

    /**
     * @brief Semantics of a space containing parameters
    */
    enum class GrcShaderSpaceType {
        Default,
        Frame,
        Material,
        Draw
    };

    /**
     * @class GrcShaderSpace
     * @brief Contains interface resources for a single descriptor set 
    */
    struct GrcShaderSpace {
        Strid                         name;
        GrcShaderSpaceType            type = GrcShaderSpaceType::Default;
        fast_vector<GrcShaderBinding> bindings;
    };

    /**
     * @class GrcShaderOption
     * @brief An user controlled option which affects shader permutation
    */
    struct GrcShaderOption {
        Strid              name;
        fast_vector<Strid> variants;
        fast_vector<int>   mappings;
    };

    /**
     * @class GrcShaderPass
     * @brief Defines single pass of shader, a functional subset
    */
    struct GrcShaderPass {
        Strid                        name;
        std::vector<GrcShaderOption> options;
    };

    /**
     * @class GrcShaderParamInfo
     * @brief Info about an param which can be set from shader or material
    */
    struct GrcShaderParamInfo {
        Strid                name;
        Ref<GrcShaderType>   type;
        GrcShaderBindingType binding_type = GrcShaderBindingType::None;
        std::int16_t         space        = -1;
        std::int16_t         binding      = -1;
        std::int16_t         offset       = -1;
        std::int16_t         buffer       = -1;
        std::string          ui_name;
        std::string          ui_hint;
        Var                  ui_range_min;
        Var                  ui_range_max;
        Var                  default_var;
        Ref<GfxTexture>      default_tex;
        Ref<GfxSampler>      default_sampler;
        std::string          default_value_str;
    };

    /**
     * @class GrcShaderBufferInfo
     * @brief Buffer info for auto packing of scalar params
    */
    struct GrcShaderBufferInfo {
        std::int16_t space   = -1;
        std::int16_t binding = -1;
        std::int16_t size;
    };

    /**
     * @class GrcShaderReflection
     * @brief Full reflection information of a single shader class
    */
    struct GrcShaderReflection {
        Strid                               shader_name;
        fast_map<Strid, int>                params_id;
        fast_vector<GrcShaderParamInfo>     params_info;
        fast_vector<GrcShaderBufferInfo>    buffers;
        fast_map<Strid, Ref<GrcShaderType>> declarations;
        fast_vector<GrcShaderConstant>      constants;
        fast_vector<GrcShaderInclude>       includes;
        fast_vector<GrcShaderSpace>         spaces;
        fast_vector<GrcShaderSourceFile>    sources;
        fast_vector<GrcShaderPass>          passes;
        fast_vector<GrcShaderOption>        options;
        fast_map<Strid, int>                passes_map;
        fast_map<Strid, int>                options_map;
        fast_map<Strid, int>                variants_map;
        fast_set<Strid>                     dependencies;
    };

}// namespace wmoge