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

#include "gfx_vert_format.hpp"

#include "core/crc32.hpp"
#include "io/enum.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

    GfxVertElements::GfxVertElements() {
        std::memset(&m_elements, 0, sizeof(m_elements));
    }
    bool GfxVertElements::operator==(const GfxVertElements& other) const {
        return std::memcmp(&m_elements, &other.m_elements, sizeof(m_elements)) == 0;
    }
    std::size_t GfxVertElements::hash() const {
        return Crc32Util::hash(&m_elements, sizeof(m_elements));
    }
    void GfxVertElements::add_element(Strid name, GfxFormat format, int buffer, int offset, int stride, bool instanced) {
        assert(buffer < GfxLimits::MAX_VERT_BUFFERS);
        assert(m_elements_count < GfxLimits::MAX_VERT_ATTRIBUTES);

        m_elements_names[m_elements_count]     = std::move(name);
        m_elements[m_elements_count].format    = format;
        m_elements[m_elements_count].buffer    = buffer;
        m_elements[m_elements_count].stride    = stride;
        m_elements[m_elements_count].offset    = offset;
        m_elements[m_elements_count].instanced = instanced;
        m_elements_count += 1;
    }
    void GfxVertElements::add_vert_attribs(GfxVertAttribs attribs, GfxVertAttribs layout, int buffer, bool instanced) {
        assert(buffer < GfxLimits::MAX_VERT_BUFFERS);
        assert(layout.bits.any());
        assert(attribs.bits.any());
        assert((attribs.bits & layout.bits) == attribs.bits);

        int stride = 0;

        layout.for_each([&](int i, GfxVertAttrib) {
            stride += GfxVertAttribSizes[i];
        });

        int offset = 0;

        layout.for_each([&](int i, GfxVertAttrib attrib) {
            if (attribs.get(attrib)) {
                assert(m_elements_count < GfxLimits::MAX_VERT_ATTRIBUTES);

                m_elements_names[m_elements_count]     = Enum::to_sid(attrib);
                m_elements[m_elements_count].format    = GfxVertAttribFormats[i];
                m_elements[m_elements_count].buffer    = buffer;
                m_elements[m_elements_count].stride    = stride;
                m_elements[m_elements_count].offset    = offset;
                m_elements[m_elements_count].instanced = instanced;
                m_elements_count += 1;
            }

            offset += GfxVertAttribSizes[i];
        });
    }
    void GfxVertElements::add_vert_attribs(wmoge::GfxVertAttribs attribs, int buffer, bool instanced) {
        add_vert_attribs(attribs, attribs, buffer, instanced);
    }

}// namespace wmoge