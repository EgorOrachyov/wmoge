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

#include "file_mem.hpp"

#include <memory>
#include <string>
#include <utility>

namespace wmoge {

    Status FileMemReader::init(const array_view<std::uint8_t>& buffer) {
        m_buffer   = buffer;
        m_position = 0;
        return WG_OK;
    }

    Status FileMemReader::nread(void* buffer, std::size_t bytes) {
        assert(buffer);
        assert(bytes + m_position <= m_buffer.size());
        std::memcpy(buffer, m_buffer.data() + m_position, bytes);
        m_position += bytes;
        return WG_OK;
    }

    Status FileMemReader::nwrite(const void* buffer, std::size_t bytes) {
        return StatusCode::InvalidState;
    }

    Status FileMemReader::eof(bool& is_eof) {
        is_eof = m_position == m_buffer.size();
        return WG_OK;
    }

    Status FileMemReader::size(std::size_t& out_size) {
        out_size = m_buffer.size();
        return WG_OK;
    }

    Status FileMemWriter::nread(void* buffer, std::size_t bytes) {
        return StatusCode::InvalidState;
    }

    Status FileMemWriter::nwrite(const void* buffer, std::size_t bytes) {
        assert(buffer);
        std::size_t offset = m_buffer.size();
        m_buffer.resize(m_buffer.size() + bytes);
        std::memcpy(m_buffer.data() + offset, buffer, bytes);
        return WG_OK;
    }

    Status FileMemWriter::eof(bool& is_eof) {
        is_eof = true;
        return WG_OK;
    }

    Status FileMemWriter::size(std::size_t& out_size) {
        out_size = m_buffer.size();
        return WG_OK;
    }

}// namespace wmoge