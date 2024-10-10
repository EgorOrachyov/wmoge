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

#include "stream_file.hpp"

#include "io/compression.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

    IoStreamFile::IoStreamFile() {
        m_can_read  = false;
        m_can_write = false;
    }

    IoStreamFile::~IoStreamFile() {
        assert(m_compression_region == 0);
    }

    Status IoStreamFile::set(Ref<File> file, FileOpenModeFlags flags) {
        assert(file);

        m_file = std::move(file);

        m_can_read  = flags.get(FileOpenMode::In);
        m_can_write = flags.get(FileOpenMode::Out);

        assert(m_can_read && !m_can_write || m_can_write && !m_can_read);

        return WG_OK;
    }

    Status IoStreamFile::open(FileSystem* file_system, const std::string& file_path, FileOpenModeFlags flags) {
        Ref<File> file;
        WG_CHECKED(file_system->open_file(file_path, file, flags));
        return set(std::move(file), flags);
    }

    Status IoStreamFile::nwrite(std::size_t num_bytes, const void* bytes) {
        assert(m_file);
        assert(m_can_write);

        if (is_compressed()) {
            return append_raw(num_bytes, bytes);
        } else {
            return append_file(num_bytes, bytes);
        }

        return WG_OK;
    }

    Status IoStreamFile::nread(std::size_t num_bytes, void* bytes) {
        assert(m_file);
        assert(m_can_read);

        if (is_compressed()) {
            return fetch_raw(num_bytes, bytes);
        } else {
            return fecth_file(num_bytes, bytes);
        }

        return WG_OK;
    }

    Status IoStreamFile::begin_compressed() {
        assert(m_compression_region >= 0);

        if (m_compression_region == 0) {
            if (m_can_read) {
                WG_CHECKED(fetch_decompressed());
            }
        }

        m_compression_region += 1;
        return WG_OK;
    }

    Status IoStreamFile::end_compressed() {
        assert(m_compression_region > 0);

        m_compression_region -= 1;

        if (m_compression_region == 0) {
            if (m_can_write) {
                WG_CHECKED(flush_compressed());
            }
        }

        return WG_OK;
    }

    Status IoStreamFile::append_raw(std::size_t num_bytes, const void* bytes) {
        if (m_buffer_raw.size() < m_offset + num_bytes) {
            m_buffer_raw.resize(m_offset + num_bytes);
        }

        std::memcpy(m_buffer_raw.data() + m_offset, bytes, num_bytes);
        m_offset += num_bytes;

        return WG_OK;
    }

    Status IoStreamFile::append_file(std::size_t num_bytes, const void* bytes) {
        return m_file->nwrite(bytes, num_bytes);
    }

    Status IoStreamFile::fetch_raw(std::size_t num_bytes, void* bytes) {
        assert(m_buffer_size >= m_offset + num_bytes);

        std::memcpy(bytes, m_buffer_raw.data() + m_offset, num_bytes);
        m_offset += num_bytes;

        return WG_OK;
    }

    Status IoStreamFile::fecth_file(std::size_t num_bytes, void* bytes) {
        return m_file->nread(bytes, num_bytes);
    }

    Status IoStreamFile::flush_compressed() {
        WG_PROFILE_CPU_IO("IoStreamFile::flush_compressed");

        IoContext dummy;

        int source_size = int(m_offset);
        WG_ARCHIVE_WRITE(dummy, *this, source_size);

        if (source_size > 0) {
            int requirement = 0;
            WG_CHECKED(Compression::estimate_lz4(m_buffer_raw.data(), source_size, requirement));

            if (m_buffer_compressed.size() < requirement) {
                m_buffer_compressed.resize(requirement);
            }

            int compressed_size = 0;
            WG_CHECKED(Compression::compress_lz4(m_buffer_raw.data(), source_size, requirement, compressed_size, m_buffer_compressed.data()));

            WG_ARCHIVE_WRITE(dummy, *this, compressed_size);
            WG_CHECKED(append_file(compressed_size, m_buffer_compressed.data()));
        }

        m_offset = 0;

        return WG_OK;
    }

    Status IoStreamFile::fetch_decompressed() {
        WG_PROFILE_CPU_IO("IoStreamFile::fetch_decompressed");

        IoContext dummy;

        int source_size = 0;
        WG_ARCHIVE_READ(dummy, *this, source_size);

        if (source_size > 0) {
            int compressed_size;
            WG_ARCHIVE_READ(dummy, *this, compressed_size);

            if (m_buffer_compressed.size() < compressed_size) {
                m_buffer_compressed.resize(compressed_size);
            }

            WG_CHECKED(fecth_file(compressed_size, m_buffer_compressed.data()));

            if (m_buffer_raw.size() < source_size) {
                m_buffer_raw.resize(source_size);
            }

            WG_CHECKED(Compression::decompress_lz4(m_buffer_compressed.data(), compressed_size, source_size, m_buffer_raw.data()));
        }

        m_offset      = 0;
        m_buffer_size = source_size;

        return WG_OK;
    }

    bool IoStreamFile::is_compressed() {
        return m_compression_region > 0;
    }

}// namespace wmoge