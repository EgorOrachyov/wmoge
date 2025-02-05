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

#include "tree_yaml.hpp"

#include "core/ioc_container.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cassert>
#include <sstream>
#include <string>

namespace wmoge {

    Status IoYamlTree::create_tree() {
        assert(m_stack.empty());
        m_tree      = Tree();
        m_can_read  = false;
        m_can_write = true;
        m_stack.push_back(m_tree.rootref());
        return WG_OK;
    }

    Status IoYamlTree::save_tree(std::string& data) {
        ryml::emitrs_yaml(m_tree, 0, &data);
        return WG_OK;
    }

    Status IoYamlTree::save_tree_json(std::string& data) {
        ryml::emitrs_json(m_tree, 0, &data);
        return WG_OK;
    }

    Status IoYamlTree::parse_data(const array_view<const std::uint8_t>& data) {
        WG_PROFILE_CPU_IO("IoYamlTree::parse_data");
        assert(m_stack.empty());
        auto str_view = ryml::csubstr(reinterpret_cast<const char*>(data.data()), data.size());
        m_tree        = std::move(ryml::parse_in_arena(str_view));
        if (m_tree.empty()) {
            return StatusCode::FailedParse;
        }
        m_can_read  = true;
        m_can_write = false;
        m_stack.push_back(m_tree.rootref());
        return WG_OK;
    }

    Status IoYamlTree::parse_file(FileSystem* fs, const std::string& path) {
        std::vector<std::uint8_t> data;
        WG_CHECKED(fs->read_file(path, data));
        return parse_data(data);
    }

#define TOP    m_stack.back()
#define STR(s) ryml::csubstr(s.data(), s.length())

    bool IoYamlTree::node_is_empty() {
        return TOP.empty();
    }
    bool IoYamlTree::node_has_child(const std::string_view& name) {
        return TOP.has_child(STR(name));
    }
    Status IoYamlTree::node_find_child(const std::string_view& name) {
        assert(can_read());
        if (TOP.has_child(STR(name))) {
            m_stack.push_back(TOP[STR(name)]);
            return WG_OK;
        }
        return StatusCode::Error;
    }
    Status IoYamlTree::node_append_child() {
        assert(can_write());
        m_stack.push_back(TOP.append_child());
        return WG_OK;
    }
    void IoYamlTree::node_find_first_child() {
        TOP = TOP.first_child();
    }
    bool IoYamlTree::node_is_valid() {
        return TOP.valid();
    }
    void IoYamlTree::node_next_sibling() {
        TOP = TOP.next_sibling();
    }
    void IoYamlTree::node_pop() {
        assert(m_stack.size() > 1);
        m_stack.pop_back();
    }
    std::size_t IoYamlTree::node_num_children() {
        return TOP.num_children();
    }

    Status IoYamlTree::node_write_key(const std::string_view& key) {
        assert(can_write());
        auto key_str = STR(key);
        TOP << ryml::key(key_str);
        return WG_OK;
    }

    Status IoYamlTree::node_write_value(const bool& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const int& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const unsigned int& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const float& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const std::string& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const Strid& value) {
        assert(can_write());
        TOP << value.str();
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const std::int16_t& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }
    Status IoYamlTree::node_write_value(const std::size_t& value) {
        assert(can_write());
        TOP << value;
        return WG_OK;
    }

    Status IoYamlTree::node_read_value(bool& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(int& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(unsigned int& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(float& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(std::string& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(Strid& value) {
        assert(can_read());
        std::string content;
        TOP >> content;
        value = Strid(content);
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(std::int16_t& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }
    Status IoYamlTree::node_read_value(std::size_t& value) {
        assert(can_read());
        TOP >> value;
        return WG_OK;
    }

    void IoYamlTree::node_as_map() {
        TOP |= ryml::MAP;
    }
    void IoYamlTree::node_as_list(std::size_t) {
        TOP |= ryml::SEQ;
    }

}// namespace wmoge