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
#include "core/log.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "io/property_tree.hpp"

#include <ryml.hpp>
#include <ryml_std.hpp>

#include <vector>

namespace wmoge {

    /**
     * @class IoYamlTree
     * @brief Yaml tree implementation for serialization and de-serialization
     */
    class IoYamlTree : public IoPropertyTree {
    public:
        IoYamlTree()  = default;
        ~IoYamlTree() = default;

        Status create_tree();
        Status parse_data(const array_view<std::uint8_t>& data);
        Status parse_file(const std::string& path);

        bool        node_is_empty() override;
        bool        node_has_child(const std::string_view& name) override;
        Status      node_find_child(const std::string_view& name) override;
        Status      node_append_child() override;
        void        node_find_first_child() override;
        bool        node_is_valid() override;
        void        node_next_sibling() override;
        void        node_pop() override;
        std::size_t node_num_children() override;

        Status node_write_key(const std::string_view& key) override;

        Status node_write_value(const bool& value) override;
        Status node_write_value(const int& value) override;
        Status node_write_value(const unsigned int& value) override;
        Status node_write_value(const float& value) override;
        Status node_write_value(const std::string& value) override;
        Status node_write_value(const Strid& value) override;
        Status node_write_value(const std::int16_t& value) override;
        Status node_write_value(const std::size_t& value) override;

        Status node_read_value(bool& value) override;
        Status node_read_value(int& value) override;
        Status node_read_value(unsigned int& value) override;
        Status node_read_value(float& value) override;
        Status node_read_value(std::string& value) override;
        Status node_read_value(Strid& value) override;
        Status node_read_value(std::int16_t& value) override;
        Status node_read_value(std::size_t& value) override;

        void node_as_map() override;
        void node_as_list(std::size_t length) override;

    private:
        using Tree = ryml::Tree;
        using Node = ryml::NodeRef;

        Tree              m_tree;
        std::vector<Node> m_stack;
    };

}// namespace wmoge