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

#include "archive_file.hpp"

namespace wmoge {

    ArchiveWriterFile::ArchiveWriterFile(std::fstream& stream) : m_stream(stream) {
        m_can_read  = false;
        m_can_write = true;
    }
    Status ArchiveWriterFile::nwrite(int num_bytes, const void* bytes) {
        m_stream.write(reinterpret_cast<const char*>(bytes), num_bytes);
        return StatusCode::Ok;
    }
    bool ArchiveWriterFile::is_memory() {
        return false;
    }
    bool ArchiveWriterFile::is_physical() {
        return true;
    }
    std::size_t ArchiveWriterFile::get_size() {
        return m_stream.tellp();
    }

    ArchiveReaderFile::ArchiveReaderFile(std::fstream& stream) : m_stream(stream) {
        m_can_read  = true;
        m_can_write = false;
    }
    Status ArchiveReaderFile::nread(int num_bytes, void* bytes) {
        m_stream.read(reinterpret_cast<char*>(bytes), num_bytes);
        return StatusCode::Ok;
    }
    bool ArchiveReaderFile::is_memory() {
        return false;
    }
    bool ArchiveReaderFile::is_physical() {
        return true;
    }
    std::size_t ArchiveReaderFile::get_size() {
        auto pos = m_stream.tellp();
        m_stream.seekp(0, std::ios_base::end);
        auto size = m_stream.tellp();
        m_stream.seekp(pos, std::ios_base::beg);
        return size;
    }

}// namespace wmoge