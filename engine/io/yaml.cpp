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

#include "yaml.hpp"

#include "platform/file_system.hpp"
#include "system/ioc_container.hpp"

#include <cassert>
#include <sstream>
#include <string>

namespace wmoge {

    YamlTree yaml_parse(const std::vector<std::uint8_t>& data) {
        auto str_view = ryml::csubstr(reinterpret_cast<const char*>(data.data()), data.size());
        return std::move(ryml::parse_in_arena(str_view));
    }

    YamlTree yaml_parse_file(const std::string& file_path) {
        std::vector<std::uint8_t> file;

        if (!IocContainer::iresolve_v<FileSystem>()->read_file(file_path, file)) {
            WG_LOG_ERROR("failed to read content of file " << file_path);
            return {};
        }

        return yaml_parse(file);
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, bool& value) {
        node >> value;
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const bool& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, int& value) {
        node >> value;
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const int& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, unsigned int& value) {
        node >> value;
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const unsigned int& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, float& value) {
        node >> value;
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const float& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, Strid& value) {
        std::string string;
        node >> string;
        value = SID(string);
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const Strid& value) {
        node << value.str();
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::string& value) {
        if (node.has_val()) {
            node >> value;
        }
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::string& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::int16_t& value) {
        node >> value;
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::int16_t& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, std::size_t& value) {
        node >> value;
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const std::size_t& value) {
        node << value;
        return StatusCode::Ok;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, Status& value) {
        return yaml_read(context, node, value.code());
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const Status& value) {
        return yaml_write(context, node, value.code());
    }

}// namespace wmoge