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

#include "core/crc32.hpp"
#include "core/ref.hpp"
#include "core/sha256.hpp"
#include "core/string_utils.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"
#include "math/math_utils.hpp"

#include <cstddef>
#include <filesystem>
#include <ostream>

namespace wmoge {

    /**
     * @class Data
     * @brief Safe, automatic and reference-counted raw binary data buffer
     */
    class Data : public RefCnt {
    public:
        Data() = default;

        /**
         * @brief Make empty data with required size buffer
         *
         * @param size Size of buffer to allocate
         */
        Data(std::size_t size);

        /**
         * @brief Make data buffer from provided memory buffer
         *
         * @param buffer Buffer to copy to data
         * @param size Size of provided buffer
         */
        Data(const void* buffer, std::size_t size);

        ~Data();

        [[nodiscard]] std::uint8_t* buffer() const { return m_buffer; }
        [[nodiscard]] std::size_t   size() const { return m_size; }
        [[nodiscard]] std::size_t   size_as_kib() const { return Math::max(m_size / 1024, std::size_t{1}); }
        [[nodiscard]] std::string   to_string() const;
        [[nodiscard]] Crc32         to_crc32() const;
        [[nodiscard]] Sha256        to_sha256() const;

        friend Status stream_write(IoContext& context, IoStream& stream, const Ref<Data>& data);
        friend Status stream_read(IoContext& context, IoStream& stream, Ref<Data>& data);

        friend Status tree_write(IoContext& context, IoTree& tree, const Ref<Data>& data);
        friend Status tree_read(IoContext& context, IoTree& tree, Ref<Data>& data);

    private:
        std::size_t   m_size   = 0;
        std::uint8_t* m_buffer = nullptr;
    };

    inline std::ostream& operator<<(std::ostream& stream, const Ref<Data>& data) {
        if (!data || !data->buffer()) {
            stream << "nil";
        } else {
            stream << data->buffer() << " size=" << StringUtils::from_mem_size(data->size());
        }
        return stream;
    }

}// namespace wmoge