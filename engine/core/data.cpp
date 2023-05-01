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

#include "data.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"

#include <cstring>
#include <fstream>

namespace wmoge {

    Data::Data(std::size_t size) {
        m_size = size;
        if (size) m_buffer = new std::uint8_t[size];
    }
    Data::Data(const void* buffer, std::size_t size) : Data(size) {
        if (m_buffer) {
            std::memcpy(m_buffer, buffer, size);
        }
    }
    Data::~Data() {
        if (m_size) {
            delete[] m_buffer;
            m_size   = 0;
            m_buffer = nullptr;
        }
    }

    std::string Data::to_string() const {
        return "data 0x" + StringUtils::from_ptr(m_buffer) + " " + StringUtils::from_mem_size(m_size);
    }

    Archive& operator<<(Archive& archive, const ref_ptr<Data>& data) {
        archive << data->m_size;
        archive.nwrite(static_cast<int>(data->m_size), data->m_buffer);
        return archive;
    }
    Archive& operator>>(Archive& archive, ref_ptr<Data>& data) {
        std::size_t size;
        archive >> size;
        data = make_ref<Data>(size);
        archive.nread(static_cast<int>(size), data->buffer());
        return archive;
    }

}// namespace wmoge