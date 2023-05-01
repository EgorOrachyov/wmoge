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

#include "audio_listener_2d.hpp"

namespace wmoge {

    void AudioListener2d::set_position(Vec2f value) {
        m_position = value;
    }
    void AudioListener2d::set_velocity(Vec2f value) {
        m_velocity = value;
    }
    void AudioListener2d::set_direction(Vec2f value) {
        m_direction = value;
    }

    Vec2f AudioListener2d::get_position() const {
        return m_position;
    }
    Vec2f AudioListener2d::get_velocity() const {
        return m_velocity;
    }
    Vec2f AudioListener2d::get_direction() const {
        return m_direction;
    }

    bool AudioListener2d::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!CanvasItem::on_load_from_yaml(node)) {
            return false;
        }

        if (node.has_child("position")) {
            Yaml::read(node["position"], m_position);
        }
        if (node.has_child("direction")) {
            Yaml::read(node["direction"], m_direction);
        }
        if (node.has_child("velocity")) {
            Yaml::read(node["velocity"], m_velocity);
        }

        return true;
    }

}// namespace wmoge
