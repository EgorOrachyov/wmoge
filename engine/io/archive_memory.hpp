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

#ifndef WMOGE_ARCHIVE_MEMORY_HPP
#define WMOGE_ARCHIVE_MEMORY_HPP

#include <vector>

#include "core/data.hpp"
#include "io/archive.hpp"

namespace wmoge {

    /**
     * @class ArchiveWriterMemory
     * @brief An archive for binary serialization into a memory buffer
     */
    class ArchiveWriterMemory final : public Archive {
    public:
        ArchiveWriterMemory();
        ~ArchiveWriterMemory() override = default;

        bool nwrite(int num_bytes, const void* bytes) override;

        bool        is_memory() override;
        bool        is_physical() override;
        std::size_t get_size() override;

        std::vector<std::uint8_t>& get_data() { return m_data; }
        std::size_t                get_pos() { return m_pos; }

    private:
        std::vector<std::uint8_t> m_data;
        std::size_t               m_pos = 0;
    };

    /**
     * @class ArchiveReaderMemory
     * @brief An archive for binary deserialization from a memory buffer
     */
    class ArchiveReaderMemory final : public Archive {
    public:
        ArchiveReaderMemory(const std::uint8_t* data, std::size_t size);
        ~ArchiveReaderMemory() override = default;

        bool nread(int num_bytes, void* bytes) override;

        bool        is_memory() override;
        bool        is_physical() override;
        std::size_t get_size() override;

        std::size_t get_pos() { return m_pos; }

    private:
        const std::uint8_t* m_data;
        std::size_t         m_size = 0;
        std::size_t         m_pos  = 0;
    };

}// namespace wmoge

#endif//WMOGE_ARCHIVE_MEMORY_HPP
