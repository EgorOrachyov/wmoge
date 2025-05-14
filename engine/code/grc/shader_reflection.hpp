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
#include "core/simple_id.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/texture.hpp"
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
     * @class ShaderTypeIdx
     * @brief Aux struct to save/load type ref and identify type at runtime
     */
    struct ShaderTypeIdx {
        WG_RTTI_STRUCT(ShaderTypeIdx);

        Strid        name;
        std::int16_t idx = -1;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderTypeIdx) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(idx, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderTypeField
     * @brief Recursive complex shader struct filed
    */
    struct ShaderTypeField {
        WG_RTTI_STRUCT(ShaderTypeField);

        Strid         name;            // field name
        ShaderTypeIdx type;            // base element type (elem type of array)
        std::int16_t  offset     = -1; // offset in a struct from this to next field
        std::int16_t  elem_count = 0;  // count of elem in array (0 if array is unbound)
        std::string   default_value;   // optional default value to set
        bool          is_array = false;// is array field
    };

    WG_RTTI_STRUCT_BEGIN(ShaderTypeField) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(offset, {});
        WG_RTTI_FIELD(elem_count, {});
        WG_RTTI_FIELD(default_value, {});
        WG_RTTI_FIELD(is_array, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderType
     * @brief Recursive complex type for declaring of anything in a shader what has a type
    */
    struct ShaderType : public RttiObject {
        WG_RTTI_CLASS(ShaderType, RttiObject);

        Strid                        name;                       // type name
        ShaderBaseType               type = ShaderBaseType::None;// type of its base
        std::vector<ShaderTypeField> fields;                     // fields of a struct type
        std::int16_t                 n_row        = -1;          // num of rows for vector like types
        std::int16_t                 n_col        = -1;          // num of columns for matrix like types
        std::int16_t                 n_elem       = -1;          // num of elements in vec/mat type
        std::int16_t                 byte_size    = 0;           // raw byte size
        bool                         is_primitive = false;       // is a primitive type, raw value in a memory
        bool                         is_builtin   = false;       // is type pre-defined in the engine (no save/load)
    };

    WG_RTTI_CLASS_BEGIN(ShaderType) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(fields, {});
        WG_RTTI_FIELD(n_row, {});
        WG_RTTI_FIELD(n_col, {});
        WG_RTTI_FIELD(n_elem, {});
        WG_RTTI_FIELD(byte_size, {});
        WG_RTTI_FIELD(is_primitive, {});
        WG_RTTI_FIELD(is_builtin, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderStructRegister
     * @brief Helper to build and register struct type
     */
    class ShaderStructRegister {
    public:
        ShaderStructRegister(Strid name, std::size_t size, class ShaderManager* shader_manager);

        ShaderStructRegister& add_field(Strid name, Strid struct_type);
        ShaderStructRegister& add_field_array(Strid name, Strid struct_type, int n_elements = 0);
        ShaderStructRegister& add_field_array(Strid name, Ref<ShaderType> type, int n_elements = 0, std::string value = std::string());

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
        WG_RTTI_STRUCT(ShaderConstant);

        Strid       name;
        std::string value;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderConstant) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(value, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderSourceFile
     * @brief Single shader module required for compilation (shader stage)
    */
    struct ShaderSourceFile {
        WG_RTTI_STRUCT(ShaderSourceFile);

        Strid           file;
        GfxShaderModule module;
        GfxShaderLang   lang = GfxShaderLang::GlslVk450;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderSourceFile) {
        WG_RTTI_FIELD(file, {});
        WG_RTTI_FIELD(module, {});
        WG_RTTI_FIELD(lang, {});
    }
    WG_RTTI_END;

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
        WG_RTTI_STRUCT(ShaderBinding);

        Strid             name;
        ShaderTypeIdx     type;
        ShaderBindingType binding = ShaderBindingType::None;
        ShaderQualifiers  qualifiers;
        DefaultTexture    default_tex     = DefaultTexture::White;
        DefaultSampler    default_sampler = DefaultSampler::Default;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderBinding) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(binding, {});
        WG_RTTI_FIELD(qualifiers, {});
        WG_RTTI_FIELD(default_tex, {});
        WG_RTTI_FIELD(default_sampler, {});
    }
    WG_RTTI_END;

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
        WG_RTTI_STRUCT(ShaderSpace);

        Strid                      name;
        ShaderSpaceType            type = ShaderSpaceType::Default;
        std::vector<ShaderBinding> bindings;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderSpace) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(bindings, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderOption
     * @brief An user controlled option which affects shader permutation
    */
    struct ShaderOption {
        WG_RTTI_STRUCT(ShaderOption);

        Strid                         name;
        Strid                         base_variant;
        flat_map<Strid, std::int16_t> variants;
        std::string                   ui_name;
        std::string                   ui_hint;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderOption) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(base_variant, {});
        WG_RTTI_FIELD(variants, {});
        WG_RTTI_FIELD(ui_name, {});
        WG_RTTI_FIELD(ui_hint, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderOptions
     * @brief Map of options for a technique or pass
    */
    struct ShaderOptions {
        WG_RTTI_STRUCT(ShaderOptions);

        static constexpr std::int16_t MAX_OPTIONS = 64;
        using Mask                                = std::bitset<MAX_OPTIONS>;

        buffered_vector<ShaderOption> options;
        flat_map<Strid, std::int16_t> options_map;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderOptions) {
        WG_RTTI_FIELD(options, {});
        WG_RTTI_FIELD(options_map, {});
    }
    WG_RTTI_END;

    using ShaderOptionVariant = std::pair<Strid, Strid>;

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

        GfxPrimType       prim_type = GfxPrimType::Triangles;
        RasterState       rs;// = default
        DepthStencilState ds;// = default
        BlendState        bs;// = default;

        void fill(GfxPsoStateGraphics& state) const;
    };

    WG_RTTI_STRUCT_BEGIN(PipelineState) {
        WG_RTTI_FIELD(prim_type, {RttiOptional});
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
        WG_RTTI_STRUCT(ShaderPassInfo);

        Strid                        name;
        PipelineState                state;
        ShaderOptions                options;
        flat_map<Strid, std::string> tags;
        std::string                  ui_name;
        std::string                  ui_hint;
        std::vector<Strid>           options_remap;
        std::vector<Strid>           variants_remap;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderPassInfo) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(state, {});
        WG_RTTI_FIELD(options, {});
        WG_RTTI_FIELD(tags, {});
        WG_RTTI_FIELD(ui_name, {});
        WG_RTTI_FIELD(ui_hint, {});
        WG_RTTI_FIELD(options_remap, {});
        WG_RTTI_FIELD(variants_remap, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderTechniqueInfo
     * @brief Defines single technique as collection of passes for drawing
    */
    struct ShaderTechniqueInfo {
        WG_RTTI_STRUCT(ShaderTechniqueInfo);

        Strid                           name;
        buffered_vector<ShaderPassInfo> passes;
        flat_map<Strid, std::int16_t>   passes_map;
        flat_map<Strid, std::string>    tags;
        std::string                     ui_name;
        std::string                     ui_hint;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderTechniqueInfo) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(passes, {});
        WG_RTTI_FIELD(passes_map, {});
        WG_RTTI_FIELD(tags, {});
        WG_RTTI_FIELD(ui_name, {});
        WG_RTTI_FIELD(ui_hint, {});
    }
    WG_RTTI_END;

    /**
     * @brief Handle to a shader param
    */
    using ShaderParamId = SimpleId<std::int16_t>;

    /**
     * @class ShaderParamInfo
     * @brief Info about an param which can be set from shader or material
    */
    struct ShaderParamInfo {
        WG_RTTI_STRUCT(ShaderParamInfo);

        Strid             name;                                     // fully qualified param name
        ShaderTypeIdx     type;                                     // param base type (in case of array - element type)
        ShaderBindingType binding_type;                             // binding type where param is
        std::int16_t      space      = -1;                          // binding space
        std::int16_t      binding    = -1;                          // binding index in space
        std::int16_t      offset     = -1;                          // byte offset of scalar data in a buffer
        std::int16_t      buffer     = -1;                          // buffer index in space
        std::int16_t      elem_idx   = -1;                          // element index of array element
        std::int16_t      elem_count = 1;                           // count of elements (array size)
        std::int16_t      byte_size  = -1;                          // size in bytes (not for all type of params actual)
        std::string       ui_name;                                  // optional ui name
        std::string       ui_hint;                                  // optional ui hint
        std::string       default_value;                            // optional default scalar value
        DefaultTexture    default_tex     = DefaultTexture::White;  // optional texture
        DefaultSampler    default_sampler = DefaultSampler::Default;// optional sampler
    };

    WG_RTTI_STRUCT_BEGIN(ShaderParamInfo) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(type, {});
        WG_RTTI_FIELD(binding_type, {});
        WG_RTTI_FIELD(space, {});
        WG_RTTI_FIELD(binding, {});
        WG_RTTI_FIELD(offset, {});
        WG_RTTI_FIELD(buffer, {});
        WG_RTTI_FIELD(elem_idx, {});
        WG_RTTI_FIELD(elem_count, {});
        WG_RTTI_FIELD(byte_size, {});
        WG_RTTI_FIELD(ui_name, {});
        WG_RTTI_FIELD(ui_hint, {});
        WG_RTTI_FIELD(default_value, {});
        WG_RTTI_FIELD(default_tex, {});
        WG_RTTI_FIELD(default_sampler, {});
    }
    WG_RTTI_END;

    /**
     * @class ShaderBufferInfo
     * @brief Buffer info for auto packing of scalar params
    */
    struct ShaderBufferInfo {
        WG_RTTI_STRUCT(ShaderBufferInfo);

        Ref<Data>    defaults;
        std::int16_t space   = -1;
        std::int16_t binding = -1;
        std::int16_t size    = 0;
        std::int16_t idx     = 0;
    };

    WG_RTTI_STRUCT_BEGIN(ShaderBufferInfo) {
        WG_RTTI_FIELD(defaults, {});
        WG_RTTI_FIELD(space, {});
        WG_RTTI_FIELD(binding, {});
        WG_RTTI_FIELD(size, {});
        WG_RTTI_FIELD(idx, {});
    }
    WG_RTTI_END;

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
        WG_RTTI_STRUCT(ShaderReflection);

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
        std::vector<ShaderSpace>         spaces;        // binding spaces for descriptor sets creation
        std::vector<ShaderSourceFile>    sources;       // source code modules
        std::vector<ShaderTechniqueInfo> techniques;    // shader techniques info
        flat_map<Strid, std::int16_t>    techniques_map;// mapping techniques name to its id
        flat_set<GfxShaderLang>          languages;     // shader languages, which it provides
        std::vector<ShaderTypeIdx>       type_idxs;     // aux type indices to build type map after loading
        std::vector<Ref<ShaderType>>     type_map;      // aux type map to get type info by its index
    };

    WG_RTTI_STRUCT_BEGIN(ShaderReflection) {
        WG_RTTI_FIELD(shader_name, {});
        WG_RTTI_FIELD(shader_extends, {});
        WG_RTTI_FIELD(domain, {});
        WG_RTTI_FIELD(ui_name, {});
        WG_RTTI_FIELD(ui_hint, {});
        WG_RTTI_FIELD(params_id, {});
        WG_RTTI_FIELD(params_info, {});
        WG_RTTI_FIELD(buffers, {});
        WG_RTTI_FIELD(declarations, {});
        WG_RTTI_FIELD(constants, {});
        WG_RTTI_FIELD(spaces, {});
        WG_RTTI_FIELD(sources, {});
        WG_RTTI_FIELD(techniques, {});
        WG_RTTI_FIELD(techniques_map, {});
        WG_RTTI_FIELD(languages, {});
        WG_RTTI_FIELD(type_idxs, {});
        WG_RTTI_FIELD(type_map, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    void rtti_grc_shader_reflection();

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