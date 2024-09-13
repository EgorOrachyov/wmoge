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

#pragma once

#include "io/stream.hpp"
#include "platform/file.hpp"

#include <fstream>
#include <vector>

namespace wmoge {

    /**
     * @class IoStreamFile
     * @brief An stream to read/write data to a file with compression
     */
    class IoStreamFile final : public IoStream {
    public:
        IoStreamFile();
        ~IoStreamFile() override;

        Status set(Ref<File> file, FileOpenModeFlags flags);
        Status open(const std::string& file_path, FileOpenModeFlags flags);
        Status nwrite(std::size_t num_bytes, const void* bytes) override;
        Status nread(std::size_t num_bytes, void* bytes) override;
        Status begin_compressed() override;
        Status end_compressed() override;

    private:
        Status             append_raw(std::size_t num_bytes, const void* bytes);
        Status             append_file(std::size_t num_bytes, const void* bytes);
        Status             fetch_raw(std::size_t num_bytes, void* bytes);
        Status             fecth_file(std::size_t num_bytes, void* bytes);
        Status             flush_compressed();
        Status             fetch_decompressed();
        [[nodiscard]] bool is_compressed();

    private:
        std::vector<std::uint8_t> m_buffer_raw;
        std::vector<std::uint8_t> m_buffer_compressed;
        std::size_t               m_offset             = 0;
        std::size_t               m_buffer_size        = 0;
        std::int32_t              m_compression_region = 0;
        Ref<File>                 m_file;
    };

}// namespace wmoge