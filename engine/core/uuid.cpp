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

#include "uuid.hpp"

#include "core/random.hpp"
#include "core/string_utils.hpp"

namespace wmoge {

    UUID::UUID(std::uint64_t value) : m_value(value) {
    }

    std::string UUID::to_str() const {
        return StringUtils::from_uint64(m_value);
    }

    UUID UUID::generate() {
        return {Random::next_uint64()};
    }

    Status yaml_read(const YamlConstNodeRef& node, UUID& id) {
        node >> id.m_value;
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const UUID& id) {
        node << id.m_value;
        return StatusCode::Ok;
    }

    Status archive_read(Archive& archive, UUID& id) {
        return archive.nread(sizeof(id.m_value), &id.m_value);
    }
    Status archive_write(Archive& archive, const UUID& id) {
        return archive.nwrite(sizeof(id.m_value), &id.m_value);
    }

}// namespace wmoge
