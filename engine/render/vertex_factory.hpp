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

#ifndef WMOGE_VERTEX_FACTORY_HPP
#define WMOGE_VERTEX_FACTORY_HPP

#include "core/fast_vector.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_vert_format.hpp"

#include <array>
#include <string>

namespace wmoge {

    /**
     * @class VertexFactoryType
     * @brief Properties of a particular vertex factory type
     */
    struct VertexFactoryType {
        StringId name;             //< Vertex factory type name
        bool     supports_skinning;//< If supports gpu skinning
    };

    /** 
     * @brief Type of required vertex input
     */
    enum class VertexInputType {
        Default      = 0,//< Default type with full attributes
        PositionOnly = 1,//< Spatial position for depth/shadow rendering
        Total        = 2
    };

    /**
     * @class VertexFactory
     * @brief Encapsulates vertex data source which can be linked to the shader
     * 
     * Vertex factory interface serves for a number of things.
     *  - Getting vertex data to submit to a gpu whene rendering in a draw call
     *  - Gathering required attributes for a particular pass (default, shadow only, etc.)
     *  - Modification of shader env compilation to influence logic of a code
     *  - Caching of GFX vertex formats to reduce frequent driver make call overhead
     * 
     * @note Call `init` factory to prepare for rendering before usage in
     *       any collect mesh batch method.
     * 
     * @note Some methods of a factory may be called in a MT mode, 
     *       safety must be granted by the implementation.
     */
    class VertexFactory {
    public:
        static constexpr int TOTAL_INPUT_TYPES = int(VertexInputType::Total);

        virtual ~VertexFactory() = default;

        virtual void                      init();
        virtual void                      cache_vert_format(class GfxDriver* driver, const GfxVertElements& elements, VertexInputType input_type);
        virtual void                      fill_required_attributes(GfxVertAttribs& attribs, VertexInputType input_type)           = 0;
        virtual void                      fill_elements(VertexInputType input_type, GfxVertElements& elements, int& used_buffers) = 0;
        virtual void                      fill_setup(VertexInputType input_type, GfxVertBuffersSetup& setup, int& used_buffers)   = 0;
        virtual void                      modify_compilation_enviroment(fast_vector<std::string>& defines) {}
        virtual std::string               get_friendly_name() const = 0;
        virtual const Ref<GfxVertFormat>& get_vert_format(VertexInputType input_type) const { return m_gfx_formats[int(input_type)]; }
        virtual const VertexFactoryType&  get_type_info() const;

    protected:
        std::array<Ref<GfxVertFormat>, TOTAL_INPUT_TYPES> m_gfx_formats;
    };

}// namespace wmoge

#endif//WMOGE_VERTEX_FACTORY_HPP