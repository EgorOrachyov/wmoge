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
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "platform/file_system.hpp"
#include "rtti/traits.hpp"

#include <bitset>
#include <cinttypes>
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @brief Base (built-in) types for compositing shader interface
    */
    enum class ShaderBaseType {
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
    static constexpr const std::int16_t ShaderBaseTypeSizes[] = {
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
    enum class ShaderBindingType {
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
     * @class ShaderType
     * @brief Recursive complex type for declaring of anything in a shader what has a type
    */
    struct ShaderType : public RefCnt {
        struct Field {
            Strid           name;              // field name
            Ref<ShaderType> type;              // base element type (elem type of array)
            std::int16_t    offset     = -1;   // offset in a struct from this to next field
            std::int16_t    elem_count = 0;    // count of elem in array (0 if array is unbound)
            bool            is_array   = false;// is array field
            Var             default_value;     // optional default value to set
        };

        Strid                  name;                            // type name
        ShaderBaseType         type      = ShaderBaseType::None;// type of its base
        std::int16_t           n_row     = -1;                  // num of rows for vector like types
        std::int16_t           n_col     = -1;                  // num of columns for matrix like types
        std::int16_t           n_elem    = -1;                  // num of elements in vec/mat type
        std::int16_t           byte_size = 0;                   // raw byte size
        buffered_vector<Field> fields;                          // fields of a struct type
        bool                   is_primitive = false;            // is a primitive type, raw value in a memory
    };

    /**
     * @class ShaderStructRegister
     * @brief Helper to build and register struct type
     */
    class ShaderStructRegister {
    public:
        ShaderStructRegister(Strid name, std::size_t size);

        ShaderStructRegister& add_field(Strid name, Strid struct_type);
        ShaderStructRegister& add_field(Strid name, Ref<ShaderType> type, Var value = Var());
        ShaderStructRegister& add_field_array(Strid name, Strid struct_type, int n_elements = 0);
        ShaderStructRegister& add_field_array(Strid name, Ref<ShaderType> type, int n_elements = 0, Var value = Var());

        Status finish();

    private:
        Ref<ShaderType>      m_struct_type;
        class ShaderManager* m_manager;
    };

    /**
     * @class ShaderTypes
     * @brief Pre-defined common shader types
    */
    struct ShaderTypes {
        static Ref<ShaderType> FLOAT;
        static Ref<ShaderType> INT;
        static Ref<ShaderType> BOOL;
        static Ref<ShaderType> VEC2;
        static Ref<ShaderType> VEC3;
        static Ref<ShaderType> VEC4;
        static Ref<ShaderType> IVEC2;
        static Ref<ShaderType> IVEC3;
        static Ref<ShaderType> IVEC4;
        static Ref<ShaderType> BVEC2;
        static Ref<ShaderType> BVEC3;
        static Ref<ShaderType> BVEC4;
        static Ref<ShaderType> MAT2;
        static Ref<ShaderType> MAT3;
        static Ref<ShaderType> MAT4;
        static Ref<ShaderType> SAMPLER2D;
        static Ref<ShaderType> SAMPLER2D_ARRAY;
        static Ref<ShaderType> SAMPLER_CUBE;
        static Ref<ShaderType> IMAGE2D;

        static std::vector<Ref<ShaderType>> builtin();
    };

    /**
     * @class ShaderConstant
     * @brief Declared pass constants inlined as defines into source code
    */
    struct ShaderConstant {
        Strid       name;
        Var         value;
        std::string str;
    };

    /**
     * @class ShaderInclude
     * @brief Information about single include file of a shader module
    */
    struct ShaderInclude {
        Strid           file;
        GfxShaderModule module;
    };

    /**
     * @class ShaderSourceFile
     * @brief Single shader module required for compilation (shader stage)
    */
    struct ShaderSourceFile {
        Strid           file;
        GfxShaderModule module;
        GfxShaderLang   lang = GfxShaderLang::GlslVk450;
    };

    /**
     * @brief Additional qualifiers for shader interface params
    */
    enum class ShaderQualifier {
        Readonly,
        Writeonly,
        Std140,
        Std430,
        Rgba16f
    };

    /**
     * @brief Additional qualifiers mask for shader interface params
    */
    using ShaderQualifiers = Mask<ShaderQualifier>;

    /**
     * @class ShaderBinding
     * @brief An interface exposed bindable param
    */
    struct ShaderBinding {
        Strid             name;
        Ref<ShaderType>   type;
        ShaderBindingType binding = ShaderBindingType::None;
        ShaderQualifiers  qualifiers;
        Ref<GfxTexture>   default_tex;
        Ref<GfxSampler>   default_sampler;
    };

    /**
     * @brief Semantics of a space containing parameters
    */
    enum class ShaderSpaceType {
        Default,
        Frame,
        Material,
        Draw
    };

    /**
     * @class ShaderSpace
     * @brief Contains interface assets for a single descriptor set 
    */
    struct ShaderSpace {
        Strid                          name;
        ShaderSpaceType                type = ShaderSpaceType::Default;
        buffered_vector<ShaderBinding> bindings;
    };

    /**
     * @class ShaderOption
     * @brief An user controlled option which affects shader permutation
    */
    struct ShaderOption {
        Strid                         name;
        Strid                         base_variant;
        flat_map<Strid, std::int16_t> variants;
        std::string                   ui_name;
        std::string                   ui_hint;
    };

    /**
     * @class ShaderOptions
     * @brief Map of options for a technique or pass
    */
    struct ShaderOptions {
        static constexpr std::int16_t MAX_OPTIONS = 64;
        using Mask                                = std::bitset<MAX_OPTIONS>;

        buffered_vector<ShaderOption> options;
        flat_map<Strid, std::int16_t> options_map;
    };

    /**
     * @brief Pipeline raster state overrides
    */
    struct RasterState {
        WG_RTTI_STRUCT(RasterState);

        GfxPolyMode      poly_mode  = GfxPolyMode::Fill;
        GfxPolyCullMode  cull_mode  = GfxPolyCullMode::Disabled;
        GfxPolyFrontFace front_face = GfxPolyFrontFace::CounterClockwise;
    };

    WG_RTTI_STRUCT_BEGIN(RasterState) {
        WG_RTTI_FIELD(poly_mode, {RttiOptional});
        WG_RTTI_FIELD(cull_mode, {RttiOptional});
        WG_RTTI_FIELD(front_face, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @brief Pipeline depth stencil state overrides
    */
    struct DepthStencilState {
        WG_RTTI_STRUCT(DepthStencilState);

        bool        depth_enable      = false;
        bool        depth_write       = true;
        GfxCompFunc depth_func        = GfxCompFunc::Less;
        bool        stencil_enable    = false;
        int         stencil_wmask     = 0;
        int         stencil_rvalue    = 0;
        int         stencil_cmask     = 0;
        GfxCompFunc stencil_comp_func = GfxCompFunc::Never;
        GfxOp       stencil_sfail     = GfxOp::Keep;
        GfxOp       stencil_dfail     = GfxOp::Keep;
        GfxOp       stencil_dpass     = GfxOp::Keep;
    };

    WG_RTTI_STRUCT_BEGIN(DepthStencilState) {
        WG_RTTI_FIELD(depth_enable, {RttiOptional});
        WG_RTTI_FIELD(depth_write, {RttiOptional});
        WG_RTTI_FIELD(depth_func, {RttiOptional});
        WG_RTTI_FIELD(stencil_enable, {RttiOptional});
        WG_RTTI_FIELD(stencil_wmask, {RttiOptional});
        WG_RTTI_FIELD(stencil_rvalue, {RttiOptional});
        WG_RTTI_FIELD(stencil_cmask, {RttiOptional});
        WG_RTTI_FIELD(stencil_comp_func, {RttiOptional});
        WG_RTTI_FIELD(stencil_sfail, {RttiOptional});
        WG_RTTI_FIELD(stencil_dfail, {RttiOptional});
        WG_RTTI_FIELD(stencil_dpass, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @brief Pipeline blend state overrides
    */
    struct BlendState {
        WG_RTTI_STRUCT(BlendState);

        bool blending = false;
    };

    WG_RTTI_STRUCT_BEGIN(BlendState) {
        WG_RTTI_FIELD(blending, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class PipelineState
     * @brief Rendering settings provided in a pass
    */
    struct PipelineState {
        WG_RTTI_STRUCT(PipelineState);

        RasterState       rs;// = default
        DepthStencilState ds;// = default
        BlendState        bs;// = default;

        void fill(GfxPsoStateGraphics& state) const;
    };

    WG_RTTI_STRUCT_BEGIN(PipelineState) {
        WG_RTTI_META_DATA();
        WG_RTTI_FIELD(rs, {RttiOptional});
        WG_RTTI_FIELD(ds, {RttiOptional});
        WG_RTTI_FIELD(bs, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class ShaderPassInfo
     * @brief Defines single pass of shader, a functional subset
    */
    struct ShaderPassInfo {
        Strid                name;
        PipelineState        state;
        ShaderOptions        options;
        flat_map<Strid, Var> tags;
        std::string          ui_name;
        std::string          ui_hint;
    };

    /**
     * @class ShaderTechniqueInfo
     * @brief Defines single technique as collection of passes for drawing
    */
    struct ShaderTechniqueInfo {
        Strid                           name;
        ShaderOptions                   options;
        buffered_vector<ShaderPassInfo> passes;
        flat_map<Strid, std::int16_t>   passes_map;
        flat_map<Strid, Var>            tags;
        std::string                     ui_name;
        std::string                     ui_hint;
        std::vector<Strid>              options_remap;
        std::vector<Strid>              variants_remap;
    };

    /**
     * @class ShaderParamId
     * @brief Handle to a shader param
    */
    struct ShaderParamId {
        ShaderParamId() = default;
        ShaderParamId(std::int16_t index) : index(index) {}

        operator std::int16_t() const { return index; }

        [[nodiscard]] bool is_valid() const { return index != -1; }
        [[nodiscard]] bool is_invalid() const { return index == -1; }

        std::int16_t index = -1;
    };

    /**
     * @class ShaderParamInfo
     * @brief Info about an param which can be set from shader or material
    */
    struct ShaderParamInfo {
        Strid             name;             // fully qualified param name
        Ref<ShaderType>   type;             // param base type (in case of array - element type)
        ShaderBindingType binding_type;     // binding type where param is
        std::int16_t      space      = -1;  // binding space
        std::int16_t      binding    = -1;  // binding index in space
        std::int16_t      offset     = -1;  // byte offset of scala data in a buffer
        std::int16_t      buffer     = -1;  // buffer index in space
        std::int16_t      elem_idx   = -1;  // element index of array element
        std::int16_t      elem_count = 1;   // count of elements (array size)
        std::int16_t      byte_size  = 1;   // count of elements (array size)
        std::string       ui_name;          // optional ui name
        std::string       ui_hint;          // optional ui hint
        Var               ui_range_min;     // optional min range for scalar value
        Var               ui_range_max;     // optional max range for scalar value
        Var               default_var;      // optional default scalar value
        Ref<GfxTexture>   default_tex;      // optional texture
        Ref<GfxSampler>   default_sampler;  // optional sampler
        std::string       default_value_str;// optional display string of default value
    };

    /**
     * @class ShaderBufferInfo
     * @brief Buffer info for auto packing of scalar params
    */
    struct ShaderBufferInfo {
        Ref<Data>    defaults;
        std::int16_t space   = -1;
        std::int16_t binding = -1;
        std::int16_t size    = 0;
        std::int16_t idx     = 0;
    };

    /**
     * @brief Describes how this shader will be used
    */
    enum class ShaderDomain {
        Material,// Shader to use with materials
        Compute, // Shader for in-egnine compute dispatches (used without material)
        Graphics // Shader for in-egnine graphics dispatches (used without material)
    };

    /**
     * @class ShaderPermutation
     * @brief Defines a particular variant of a compiled shader
    */
    struct ShaderPermutation {
        WG_RTTI_STRUCT(ShaderPermutation);

        bool        operator==(const ShaderPermutation& other) const;
        std::size_t hash() const;

        ShaderOptions::Mask options;          // = []
        GfxVertAttribs      vert_attribs;     // = {}
        std::int16_t        technique_idx = 0;// = 0
        std::int16_t        pass_idx      = 0;// = 0
    };

    WG_RTTI_STRUCT_BEGIN(ShaderPermutation) {
        WG_RTTI_META_DATA();
        WG_RTTI_FIELD(options, {});
        WG_RTTI_FIELD(vert_attribs, {});
        WG_RTTI_FIELD(technique_idx, {});
        WG_RTTI_FIELD(pass_idx, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderReflection
     * @brief Full reflection information of a single shader class
    */
    struct ShaderReflection {
        Strid                            shader_name;   // shader script global unique name
        Strid                            shader_extends;// shader script which we extend in this one
        ShaderDomain                     domain;        // shader domain
        std::string                      ui_name;       // optional ui name
        std::string                      ui_hint;       // optional ui hint
        flat_map<Strid, std::int16_t>    params_id;     // mapping of full param name to its id
        std::vector<ShaderParamInfo>     params_info;   // id to param info
        std::vector<ShaderBufferInfo>    buffers;       // buffer info for scalar params packing
        flat_map<Strid, Ref<ShaderType>> declarations;  // shader defined struct types
        std::vector<ShaderConstant>      constants;     // shader defined constanst
        std::vector<ShaderInclude>       includes;      // shader includes per module
        std::vector<ShaderSpace>         spaces;        // binding spaces for descriptor sets creation
        std::vector<ShaderSourceFile>    sources;       // source code modules
        std::vector<ShaderTechniqueInfo> techniques;    // shader techniques info
        flat_map<Strid, std::int16_t>    techniques_map;// mapping techniques name to its id
        flat_set<Strid>                  dependencies;  // shader files dependencies for hot-reload
        flat_set<GfxShaderLang>          languages;     // shader languages, which it provides
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::ShaderPermutation> {
    public:
        std::size_t operator()(const wmoge::ShaderPermutation& permutation) const {
            return permutation.hash();
        }
    };

}// namespace std