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

#include "vertex_factories.hpp"

namespace wmoge {

    VertexFactoryStatic::VertexFactoryStatic(const std::array<Ref<GfxVertBuffer>, MAX_BUFFERS>& buffers, const GfxVertAttribsStreams& attribs, const StringId& name) {
        m_buffers = buffers;
        m_attribs = attribs;
        m_name    = name;
    }

    void VertexFactoryStatic::fill_required_attributes(GfxVertAttribs& attribs, VertexInputType input_type) {
        for (const auto& stream : m_attribs) {
            attribs |= stream;
        }
    }

    void VertexFactoryStatic::fill_elements(VertexInputType input_type, GfxVertElements& elements, int& used_buffers) {
        for (int i = 0; i < MAX_BUFFERS; i++) {
            if (m_attribs[i].bits.any()) {
                elements.add_vert_attribs(m_attribs[i], used_buffers, false);
                used_buffers += 1;
            }
        }
    }

    void VertexFactoryStatic::fill_setup(VertexInputType input_type, GfxVertBuffersSetup& setup, int& used_buffers) {
        for (int i = 0; i < MAX_BUFFERS; i++) {
            if (m_attribs[i].bits.any()) {
                setup.buffers[i] = m_buffers[i].get();
                setup.offsets[i] = 0;
            }
        }
    }

    std::string VertexFactoryStatic::get_friendly_name() const {
        return m_name.str();
    }

    const VertexFactoryType& VertexFactoryStatic::get_type_info() const {
        static VertexFactoryType s_type = {
                SID("VertexFactoryStatic"),
                false};

        return s_type;
    }

}// namespace wmoge