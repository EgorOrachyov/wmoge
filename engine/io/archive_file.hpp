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

#ifndef WMOGE_ARCHIVE_FILE_HPP
#define WMOGE_ARCHIVE_FILE_HPP

#include "archive.hpp"

#include <fstream>

namespace wmoge {

    /**
     * @class ArchiveWriterFile
     * @brief An archive to write data to a platform file
     */
    class ArchiveWriterFile final : public Archive {
    public:
        ArchiveWriterFile(std::fstream& stream);

        bool nwrite(int num_bytes, const void* bytes) override;

        [[nodiscard]] bool   is_memory() override;
        [[nodiscard]] bool   is_physical() override;
        [[nodiscard]] size_t get_size() override;

    private:
        std::fstream& m_stream;
    };

    /**
     * @class ArchiveReaderFile
     * @brief An archive to read data from a platform file
     */
    class ArchiveReaderFile final : public Archive {
    public:
        ArchiveReaderFile(std::fstream& stream);
        ~ArchiveReaderFile() override = default;

        bool nread(int num_bytes, void* bytes) override;

        [[nodiscard]] bool        is_memory() override;
        [[nodiscard]] bool        is_physical() override;
        [[nodiscard]] std::size_t get_size() override;

    private:
        std::fstream& m_stream;
    };

}// namespace wmoge

#endif//WMOGE_ARCHIVE_FILE_HPP
