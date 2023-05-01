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

#include "canvas_layer.hpp"

namespace wmoge {

    void CanvasLayer::hide() {
        m_is_visible = false;
    }
    void CanvasLayer::show() {
        m_is_visible = true;
    }
    void CanvasLayer::set_layer_id(int id) {
        m_id = id;
    }

    Vec2f CanvasLayer::get_offset() const {
        return m_offset;
    }
    float CanvasLayer::get_rotation() const {
        return m_rotation;
    }
    int CanvasLayer::get_id() const {
        return m_id;
    }
    bool CanvasLayer::get_is_visible() const {
        return m_is_visible;
    }

    bool CanvasLayer::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneComponent::on_load_from_yaml(node)) {
            return false;
        }

        if (!node.has_child("layer_id")) {
            WG_LOG_ERROR("layer id not specified");
            return false;
        }

        m_id = Yaml::read_int(node["layer_id"]);
        return true;
    }

}// namespace wmoge