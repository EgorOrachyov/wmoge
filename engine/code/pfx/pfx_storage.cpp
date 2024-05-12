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

#include "pfx_storage.hpp"

#include "core/log.hpp"

namespace wmoge {

    PfxStorage::PfxStorage(PfxAttributes attributes, int capacity) {
        assert(attributes.bits.any());
        assert(capacity > 0);

        m_capacity   = capacity;
        m_attributes = attributes;
        m_stride     = 0;

        for (int i = 0; i < TOTAL_ATTRIBUTES; i++) {
            auto attribute = static_cast<PfxAttribute>(i);

            m_sizes[i]   = 0;
            m_offsets[i] = m_stride;

            if (m_attributes.get(attribute)) {
                switch (attribute) {
                    case PfxAttribute::Pos2d:
                    case PfxAttribute::Vel2d:
                        m_sizes[i] = sizeof(Vec2f);
                        break;
                    case PfxAttribute::Color:
                        m_sizes[i] = sizeof(Color4f);
                        break;
                    case PfxAttribute::Angle:
                    case PfxAttribute::Size:
                    case PfxAttribute::Time:
                    case PfxAttribute::TimeNorm:
                    case PfxAttribute::TimeToLive:
                        m_sizes[i] = sizeof(float);
                        break;
                    default:
                        WG_LOG_ERROR("unknown attribute " << i);
                        break;
                }
            }

            m_stride += m_sizes[i];
        }

        m_data.resize(m_stride * m_capacity);
    }

    PfxView<Vec2f> PfxStorage::get_pos2d() {
        return PfxView<Vec2f>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::Pos2d)]);
    }
    PfxView<Vec2f> PfxStorage::get_vel2d() {
        return PfxView<Vec2f>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::Vel2d)]);
    }
    PfxView<Color4f> PfxStorage::get_color() {
        return PfxView<Color4f>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::Color)]);
    }
    PfxView<float> PfxStorage::get_angle() {
        return PfxView<float>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::Angle)]);
    }
    PfxView<float> PfxStorage::get_size() {
        return PfxView<float>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::Size)]);
    }
    PfxView<float> PfxStorage::get_time() {
        return PfxView<float>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::Time)]);
    }
    PfxView<float> PfxStorage::get_time_norm() {
        return PfxView<float>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::TimeNorm)]);
    }
    PfxView<float> PfxStorage::get_time_to_live() {
        return PfxView<float>(m_stride, m_data.data() + m_offsets[static_cast<int>(PfxAttribute::TimeToLive)]);
    }

    std::vector<std::uint8_t>& PfxStorage::get_data() {
        return m_data;
    }
    int PfxStorage::get_capacity() const {
        return m_capacity;
    }
    int PfxStorage::get_stride() const {
        return m_stride;
    }
    PfxAttributes PfxStorage::get_attributes() const {
        return m_attributes;
    }

}// namespace wmoge