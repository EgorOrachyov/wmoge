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

#include "io/compression.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

    ArchiveFile::ArchiveFile() {
        m_can_read  = false;
        m_can_write = false;
    }

    ArchiveFile::~ArchiveFile() {
        assert(m_compression_region == 0);
    }

    Status ArchiveFile::set(Ref<File> file, FileOpenModeFlags flags) {
        assert(file);

        m_file = std::move(file);

        m_can_read  = flags.get(FileOpenMode::In);
        m_can_write = flags.get(FileOpenMode::Out);

        assert(m_can_read && !m_can_write || m_can_write && !m_can_read);

        return WG_OK;
    }

    Status ArchiveFile::open(const std::string& file_path, FileOpenModeFlags flags) {
        Ref<File>   file;
        FileSystem* file_system = IocContainer::iresolve_v<FileSystem>();

        WG_CHECKED(file_system->open_file(file_path, file, flags));

        return set(std::move(file), flags);
    }

    Status ArchiveFile::nwrite(std::size_t num_bytes, const void* bytes) {
        assert(m_file);
        assert(m_can_write);

        if (is_compressed()) {
            return append_raw(num_bytes, bytes);
        } else {
            return append_file(num_bytes, bytes);
        }

        return WG_OK;
    }

    Status ArchiveFile::nread(std::size_t num_bytes, void* bytes) {
        assert(m_file);
        assert(m_can_read);

        if (is_compressed()) {
            return fetch_raw(num_bytes, bytes);
        } else {
            return fecth_file(num_bytes, bytes);
        }

        return WG_OK;
    }

    Status ArchiveFile::begin_compressed() {
        assert(m_compression_region >= 0);

        if (m_compression_region == 0) {
            if (m_can_read) {
                WG_CHECKED(fetch_decompressed());
            }
        }

        m_compression_region += 1;
        return WG_OK;
    }

    Status ArchiveFile::end_compressed() {
        assert(m_compression_region > 0);

        m_compression_region -= 1;

        if (m_compression_region == 0) {
            if (m_can_write) {
                WG_CHECKED(flush_compressed());
            }
        }

        return WG_OK;
    }

    Status ArchiveFile::append_raw(std::size_t num_bytes, const void* bytes) {
        if (m_buffer_raw.size() < m_offset + num_bytes) {
            m_buffer_raw.resize(m_offset + num_bytes);
        }

        std::memcpy(m_buffer_raw.data() + m_offset, bytes, num_bytes);
        m_offset += num_bytes;

        return WG_OK;
    }

    Status ArchiveFile::append_file(std::size_t num_bytes, const void* bytes) {
        return m_file->nwrite(bytes, num_bytes);
    }

    Status ArchiveFile::fetch_raw(std::size_t num_bytes, void* bytes) {
        assert(m_buffer_size >= m_offset + num_bytes);

        std::memcpy(bytes, m_buffer_raw.data() + m_offset, num_bytes);
        m_offset += num_bytes;

        return WG_OK;
    }

    Status ArchiveFile::fecth_file(std::size_t num_bytes, void* bytes) {
        return m_file->nread(bytes, num_bytes);
    }

    Status ArchiveFile::flush_compressed() {
        WG_AUTO_PROFILE_IO("ArchiveFile::flush_compressed");

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

    Status ArchiveFile::fetch_decompressed() {
        WG_AUTO_PROFILE_IO("ArchiveFile::fetch_decompressed");

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

    bool ArchiveFile::is_compressed() {
        return m_compression_region > 0;
    }

}// namespace wmoge