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

#include "property_tree.hpp"

namespace wmoge {

    Status tree_read(IoContext& context, IoPropertyTree& tree, bool& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const bool& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, int& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const int& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, unsigned int& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const unsigned int& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, float& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const float& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, Strid& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const Strid& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, std::string& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const std::string& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, std::int16_t& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const std::int16_t& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, std::size_t& value) {
        return tree.node_read_value(value);
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const std::size_t& value) {
        return tree.node_write_value(value);
    }

    Status tree_read(IoContext& context, IoPropertyTree& tree, Status& value) {
        return tree_read(context, tree, value.code());
    }
    Status tree_write(IoContext& context, IoPropertyTree& tree, const Status& value) {
        return tree_write(context, tree, value.code());
    }

}// namespace wmoge