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

#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_resource.hpp"
#include "rtti/traits.hpp"

#include <array>
#include <utility>

namespace wmoge {

    /**
     * @class GfxVertElement
     * @brief Structure describing single attribute vertex input
     */
    struct GfxVertElement {
        WG_RTTI_STRUCT(GfxVertElement);

        GfxFormat format    = GfxFormat::RGB32F;
        int       buffer    = -1;
        int       stride    = -1;
        int       offset    = -1;
        int       instanced = false;
    };

    WG_RTTI_STRUCT_BEGIN(GfxVertElement) {
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(buffer, {});
        WG_RTTI_FIELD(stride, {});
        WG_RTTI_FIELD(offset, {});
        WG_RTTI_FIELD(instanced, {});
    }
    WG_RTTI_END;

    /**
     * @class GfxVertElements
     * @brief Complete set of input elements for vertex shader
     */
    class GfxVertElements {
    public:
        GfxVertElements();
        bool        operator==(const GfxVertElements& other) const;
        std::size_t hash() const;

        void add_element(Strid name, GfxFormat format, int buffer, int offset, int stride, bool instanced = false);
        void add_vert_attribs(GfxVertAttribs attribs, GfxVertAttribs layout, int buffer, bool instanced = false);
        void add_vert_attribs(GfxVertAttribs attribs, int buffer, bool instanced = false);

        [[nodiscard]] Strid                                                             to_name() const;
        [[nodiscard]] const std::array<GfxVertElement, GfxLimits::MAX_VERT_ATTRIBUTES>& elements() const { return m_elements; }
        [[nodiscard]] const std::array<Strid, GfxLimits::MAX_VERT_ATTRIBUTES>&          elements_names() const { return m_elements_names; }
        [[nodiscard]] int                                                               elements_count() const { return m_elements_count; }

        static GfxVertElements make(GfxVertAttribs attribs);

    private:
        std::array<GfxVertElement, GfxLimits::MAX_VERT_ATTRIBUTES> m_elements;
        std::array<Strid, GfxLimits::MAX_VERT_ATTRIBUTES>          m_elements_names;
        int                                                        m_elements_count = 0;
    };

    /**
     * @class GfxVertFormat
     * @brief Class describing the format of the input to the vertex shader
     */
    class GfxVertFormat : public GfxResource {
    public:
        ~GfxVertFormat() override                       = default;
        virtual const GfxVertElements& elements() const = 0;
    };

    /**
     * @brief Vert Format: [pos2, uv2, col3,]
    */
    struct GfxVF_Pos2Uv2Col3 {
        Vec2f pos;
        Vec2f uv;
        Vec3f col;
    };

    /**
     * @brief Vert Format: [pos3, col3,]
    */
    struct GfxVF_Pos3Col3 {
        Vec3f pos;
        Vec3f col;
    };

    /**
     * @brief Vert Format: []
    */
    struct GfxVF_Pos2Uv2 {
        Vec2f pos;
        Vec2f uv;
    };

    /**
     * @brief Vert Format: []
    */
    struct GfxVF_Pos2Col3 {
        Vec2f pos;
        Vec3f col;
    };

    /**
     * @brief Vert Format: []
    */
    struct GfxVF_Pos3Col4 {
        Vec3f pos;
        Vec4f col;
    };

    /**
     * @brief Vert Format: []
    */
    struct GfxVF_Pos3Col4Uv2 {
        Vec3f pos;
        Vec4f col;
        Vec2f uv;
    };

    static_assert(sizeof(GfxVF_Pos2Uv2Col3) == 7 * sizeof(float), "unexpected size");
    static_assert(sizeof(GfxVF_Pos3Col3) == 6 * sizeof(float), "unexpected size");
    static_assert(sizeof(GfxVF_Pos2Uv2) == 4 * sizeof(float), "unexpected size");
    static_assert(sizeof(GfxVF_Pos2Col3) == 5 * sizeof(float), "unexpected size");
    static_assert(sizeof(GfxVF_Pos3Col4) == 7 * sizeof(float), "unexpected size");
    static_assert(sizeof(GfxVF_Pos3Col4Uv2) == 9 * sizeof(float), "unexpected size");

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::GfxVertElements> {
        std::size_t operator()(const wmoge::GfxVertElements& desc) const {
            return desc.hash();
        }
    };

}// namespace std