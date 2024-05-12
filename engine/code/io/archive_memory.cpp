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

#include <cstring>

#include "archive_memory.hpp"

namespace wmoge {

    ArchiveWriterMemory::ArchiveWriterMemory() {
        m_can_write = true;
    }
    Status ArchiveWriterMemory::nwrite(int num_bytes, const void* bytes) {
        auto pos = m_data.size();
        m_data.resize(pos + num_bytes);
        m_pos += num_bytes;
        std::memcpy(m_data.data() + pos, bytes, num_bytes);
        return WG_OK;
    }
    bool ArchiveWriterMemory::is_memory() {
        return true;
    }
    bool ArchiveWriterMemory::is_physical() {
        return false;
    }
    std::size_t ArchiveWriterMemory::get_size() {
        return m_data.size();
    }

    ArchiveReaderMemory::ArchiveReaderMemory(const std::uint8_t* data, std::size_t size) {
        m_data     = data;
        m_size     = size;
        m_can_read = true;
    }
    Status ArchiveReaderMemory::nread(int num_bytes, void* bytes) {
        assert(m_pos + num_bytes <= get_size());
        std::memcpy(bytes, m_data + m_pos, num_bytes);
        m_pos += num_bytes;
        return WG_OK;
    }
    bool ArchiveReaderMemory::is_memory() {
        return true;
    }
    bool ArchiveReaderMemory::is_physical() {
        return false;
    }
    std::size_t ArchiveReaderMemory::get_size() {
        return m_size;
    }
}// namespace wmoge