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

#include "core/array_view.hpp"
#include "platform/file.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class FileMemReader
     * @brief File interface implementation for reading from memory buffer
    */
    class FileMemReader : public File {
    public:
        FileMemReader()           = default;
        ~FileMemReader() override = default;

        Status init(const array_view<std::uint8_t>& buffer);

        Status nread(void* buffer, std::size_t bytes) override;
        Status nwrite(const void* buffer, std::size_t bytes) override;
        Status eof(bool& is_eof) override;
        Status size(std::size_t& out_size) override;

        [[nodiscard]] array_view<std::uint8_t>&       get_buffer() { return m_buffer; }
        [[nodiscard]] const array_view<std::uint8_t>& get_buffer() const { return m_buffer; }
        [[nodiscard]] std::size_t                     get_position() const { return m_position; }

    private:
        array_view<std::uint8_t> m_buffer;
        std::size_t              m_position = 0;
    };

    /**
     * @class FileMemWriter
     * @brief File interface implementation for writing into dynamic memory buffer
    */
    class FileMemWriter : public File {
    public:
        FileMemWriter()           = default;
        ~FileMemWriter() override = default;

        Status nread(void* buffer, std::size_t bytes) override;
        Status nwrite(const void* buffer, std::size_t bytes) override;
        Status eof(bool& is_eof) override;
        Status size(std::size_t& out_size) override;

        [[nodiscard]] std::vector<std::uint8_t>&       get_buffer() { return m_buffer; }
        [[nodiscard]] const std::vector<std::uint8_t>& get_buffer() const { return m_buffer; }

    private:
        std::vector<std::uint8_t> m_buffer;
    };

}// namespace wmoge