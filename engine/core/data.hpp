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

#include "core/ref.hpp"
#include "io/archive.hpp"
#include "io/yaml.hpp"
#include "math/math_utils.hpp"

#include <cstddef>
#include <filesystem>

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

        friend Status archive_write(Archive& archive, const Ref<Data>& data);
        friend Status archive_read(Archive& archive, Ref<Data>& data);

        friend Status yaml_write(YamlNodeRef node, const Ref<Data>& data);
        friend Status yaml_read(const YamlConstNodeRef& node, Ref<Data>& data);

    private:
        std::size_t   m_size   = 0;
        std::uint8_t* m_buffer = nullptr;
    };

}// namespace wmoge