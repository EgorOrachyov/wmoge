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

#include "vk_vert_format.hpp"

#include <bitset>

namespace wmoge {

    VKVertFormat::VKVertFormat(const GfxVertElements& elements, const Strid& name) {
        m_name             = name;
        m_attributes_count = 0;
        m_buffers_count    = 0;

        auto& items = elements.elements();

        std::bitset<GfxLimits::MAX_VERT_BUFFERS> used_buffers;

        for (int location = 0; location < elements.elements_count(); location++) {
            auto& element = items[location];

            if (!used_buffers[element.buffer]) {
                m_buffers[m_buffers_count].binding   = element.buffer;
                m_buffers[m_buffers_count].stride    = element.stride;
                m_buffers[m_buffers_count].inputRate = element.instanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
                m_buffers_count += 1;

                used_buffers.set(element.buffer);
            }

            m_attributes[m_attributes_count].location = location;
            m_attributes[m_attributes_count].binding  = element.buffer;
            m_attributes[m_attributes_count].offset   = element.offset;
            m_attributes[m_attributes_count].format   = VKDefs::get_format(element.format);
            m_attributes_count += 1;
        }
    }
    const GfxVertElements& VKVertFormat::elements() const {
        return m_elements;
    }

}// namespace wmoge