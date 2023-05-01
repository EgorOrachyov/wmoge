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

#ifndef WMOGE_AUDIO_LISTENER_2D_HPP
#define WMOGE_AUDIO_LISTENER_2D_HPP

#include "components/canvas_item.hpp"

namespace wmoge {

    /**
     * @class AudioListener2d
     * @brief 2d-space listener for incoming audio
     */
    class AudioListener2d : public CanvasItem {
    public:
        WG_OBJECT(AudioListener2d, CanvasItem)

        void set_position(Vec2f value);
        void set_velocity(Vec2f value);
        void set_direction(Vec2f value);

        Vec2f get_position() const;
        Vec2f get_velocity() const;
        Vec2f get_direction() const;

        bool on_load_from_yaml(const YamlConstNodeRef& node) override;

    private:
        Vec2f m_position;
        Vec2f m_velocity;
        Vec2f m_direction;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_LISTENER_2D_HPP
