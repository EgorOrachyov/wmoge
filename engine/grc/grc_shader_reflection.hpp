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
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "io/serialization.hpp"
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
    static constexpr const std::int16_t GrcShaderBaseTypeSizes[] = {
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
            Strid              name;              // field name
            Ref<GrcShaderType> type;              // base element type (elem type of array)
            std::int16_t       offset     = -1;   // offset in a struct from this to next field
            std::int16_t       elem_count = 0;    // count of elem in array (0 if array is unbound)
            bool               is_array   = false;// is array field
            Var                default_value;     // optional default value to set
        };

        Strid              name;                               // type name
        GrcShaderBaseType  type      = GrcShaderBaseType::None;// type of its base
        std::int16_t       n_row     = -1;                     // num of rows for vector like types
        std::int16_t       n_col     = -1;                     // num of columns for matrix like types
        std::int16_t       n_elem    = -1;                     // num of elements in vec/mat type
        std::int16_t       byte_size = 0;                      // raw byte size
        fast_vector<Field> fields;                             // fields of a struct type
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
        Ref<GrcShaderType>   type;
        GrcShaderBindingType binding = GrcShaderBindingType::None;
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
        Strid                         name;
        fast_map<Strid, std::int16_t> variants;
        std::string                   ui_name;
        std::string                   ui_hint;
    };

    /**
     * @class GrcShaderOptions
     * @brief Map of options for a technique or pass
    */
    struct GrcShaderOptions {
        fast_vector<GrcShaderOption>  options;
        fast_map<Strid, std::int16_t> options_map;
    };

    /**
     * @class GrcPipelineState
     * @brief Rendering settings provided in a pass
    */
    struct GrcPipelineState {
        GfxPolyMode     poly_mode         = GfxPolyMode::Fill;
        GfxPolyCullMode cull_mode         = GfxPolyCullMode::Disabled;
        int             depth_enable      = false;
        int             depth_write       = true;
        GfxCompFunc     depth_func        = GfxCompFunc::Less;
        int             stencil_enable    = false;
        int             stencil_wmask     = 0;
        int             stencil_rvalue    = 0;
        int             stencil_cmask     = 0;
        GfxCompFunc     stencil_comp_func = GfxCompFunc::Never;
        GfxOp           stencil_sfail     = GfxOp::Keep;
        GfxOp           stencil_dfail     = GfxOp::Keep;
        GfxOp           stencil_dpass     = GfxOp::Keep;
        int             blending          = false;
    };

    /**
     * @class GrcShaderPass
     * @brief Defines single pass of shader, a functional subset
    */
    struct GrcShaderPass {
        Strid            name;
        GrcPipelineState state;
        GrcShaderOptions options;
        std::string      ui_name;
        std::string      ui_hint;
    };

    /**
     * @class GrcShaderTechnique
     * @brief Defines single technique as collection of passes for drawing
    */
    struct GrcShaderTechnique {
        Strid                      name;
        GrcShaderOptions           options;
        fast_vector<GrcShaderPass> passes;
        std::string                ui_name;
        std::string                ui_hint;
    };

    /**
     * @class GrcShaderParamId
     * @brief Handle to a shader param
    */
    struct GrcShaderParamId {
        GrcShaderParamId() = default;
        GrcShaderParamId(std::int16_t index) : index(index) {}

        operator std::int16_t() const { return index; }

        [[nodiscard]] bool is_valid() const { return index != -1; }
        [[nodiscard]] bool is_invalid() const { return index == -1; }

        std::int16_t index = -1;
    };

    /**
     * @class GrcShaderParamInfo
     * @brief Info about an param which can be set from shader or material
    */
    struct GrcShaderParamInfo {
        Strid                name;             // fully qualified param name
        Ref<GrcShaderType>   type;             // param base type (in case of array - element type)
        GrcShaderBindingType binding_type;     // binding type where param is
        std::int16_t         space      = -1;  // binding space
        std::int16_t         binding    = -1;  // binding index in space
        std::int16_t         offset     = -1;  // byte offset of scala data in a buffer
        std::int16_t         buffer     = -1;  // buffer index in space
        std::int16_t         elem_idx   = -1;  // element index of array element
        std::int16_t         elem_count = 1;   // count of elements (array size)
        std::int16_t         byte_size  = 1;   // count of elements (array size)
        std::string          ui_name;          // optional ui name
        std::string          ui_hint;          // optional ui hint
        Var                  ui_range_min;     // optional min range for scalar value
        Var                  ui_range_max;     // optional max range for scalar value
        Var                  default_var;      // optional default scalar value
        Ref<GfxTexture>      default_tex;      // optional texture
        Ref<GfxSampler>      default_sampler;  // optional sampler
        std::string          default_value_str;// optional display string of default value
    };

    /**
     * @class GrcShaderBufferInfo
     * @brief Buffer info for auto packing of scalar params
    */
    struct GrcShaderBufferInfo {
        Ref<Data>    defaults;
        std::int16_t space   = -1;
        std::int16_t binding = -1;
        std::int16_t size    = 0;
        std::int16_t idx     = 0;
    };

    /**
     * @class GrcShaderReflection
     * @brief Full reflection information of a single shader class
    */
    struct GrcShaderReflection {
        Strid                               shader_name;   // shader class global unique name
        fast_map<Strid, std::int16_t>       params_id;     // mapping of full param name to its id
        fast_vector<GrcShaderParamInfo>     params_info;   // id to param info
        fast_vector<GrcShaderBufferInfo>    buffers;       // buffer info for scalar params packing
        fast_map<Strid, Ref<GrcShaderType>> declarations;  // shader defined struct types
        fast_vector<GrcShaderConstant>      constants;     // shader defined constanst
        fast_vector<GrcShaderInclude>       includes;      // shader includes per module
        fast_vector<GrcShaderSpace>         spaces;        // binding spaces for descriptor sets creation
        fast_vector<GrcShaderSourceFile>    sources;       // source code modules
        fast_vector<GrcShaderTechnique>     techniques;    // shader techniques info
        fast_map<Strid, std::int16_t>       techniques_map;// mapping techniques name to its id
        fast_set<Strid>                     dependencies;  // shader files dependencies for hot-reload
    };

}// namespace wmoge