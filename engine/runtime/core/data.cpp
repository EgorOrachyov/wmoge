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

#include "data.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "io/base64.hpp"

#include <cstring>
#include <fstream>

namespace wmoge {

    Data::Data(std::size_t size) {
        m_size = size;
        if (size) m_buffer = new std::uint8_t[size];
    }
    Data::Data(const void* buffer, std::size_t size) : Data(size) {
        if (m_buffer) {
            std::memcpy(m_buffer, buffer, size);
        }
    }
    Data::~Data() {
        if (m_size) {
            delete[] m_buffer;
            m_size   = 0;
            m_buffer = nullptr;
        }
    }

    std::string Data::to_string() const {
        return "data 0x" + StringUtils::from_ptr(m_buffer) + " " + StringUtils::from_mem_size(m_size);
    }

    Crc32 Data::to_crc32() const {
        Crc32Builder builder;
        if (m_size > 0) {
            builder.hash(m_buffer, m_size);
        }
        return builder.get();
    }

    Sha256 Data::to_sha256() const {
        Sha256Builder builder;
        if (m_size > 0) {
            builder.hash(m_buffer, m_size);
        }
        return builder.get();
    }

    Status archive_write(IoContext& context, Archive& archive, const Ref<Data>& data) {
        assert(data);
        WG_ARCHIVE_WRITE(context, archive, data->m_size);
        return archive.nwrite(static_cast<int>(data->m_size), data->m_buffer);
    }
    Status archive_read(IoContext& context, Archive& archive, Ref<Data>& data) {
        std::size_t size;
        WG_ARCHIVE_READ(context, archive, size);
        data = make_ref<Data>(size);
        return archive.nread(static_cast<int>(size), data->buffer());
    }

    Status yaml_write(IoContext& context, YamlNodeRef node, const Ref<Data>& data) {
        if (!data) {
            node << "";
            return WG_OK;
        }

        std::string encoded;
        if (Base64::encode(data, encoded)) {
            return yaml_write(context, node, encoded);
        }
        return StatusCode::FailedWrite;
    }
    Status yaml_read(IoContext& context, YamlConstNodeRef node, Ref<Data>& data) {
        std::string encoded;
        if (yaml_read(context, node, encoded)) {
            return Base64::decode(encoded, data);
        }
        return StatusCode::FailedRead;
    }

}// namespace wmoge