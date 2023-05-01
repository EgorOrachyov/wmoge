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

#ifndef WMOGE_AUDIO_SOURCE_2D_HPP
#define WMOGE_AUDIO_SOURCE_2D_HPP

#include "audio/audio_playback.hpp"
#include "components/canvas_item.hpp"

namespace wmoge {

    /**
     * @brief AudioSource2d
     * @class 2d-space positional source for playing sound on the scene
     */
    class AudioSource2d : public CanvasItem {
    public:
        WG_OBJECT(AudioSource2d, CanvasItem)

        void create(ref_ptr<AudioStream> stream, StringId bus = StringId());
        void play();
        void stop();
        bool has_playback();

        void set_position(Vec2f value);
        void set_velocity(Vec2f value);
        void set_direction(Vec2f value);
        void set_pitch_scale(float value);
        void set_gain(float value);
        void set_min_gain(float value);
        void set_max_gain(float value);
        void set_max_distance(float value);

        Vec2f get_position() const;
        Vec2f get_velocity() const;
        Vec2f get_direction() const;
        float get_pitch_scale() const;
        float get_gain() const;
        float get_min_gain() const;
        float get_max_gain() const;
        float get_max_distance() const;

        bool on_load_from_yaml(const YamlConstNodeRef& node) override;
        void on_transform_updated() override;
        void on_scene_enter() override;

    private:
        ref_ptr<AudioPlayback> m_playback;
        Vec2f                  m_position;
        Vec2f                  m_velocity;
        Vec2f                  m_direction;
        float                  m_pitch_scale  = 1.0f;
        float                  m_gain         = 1.0;
        float                  m_min_gain     = 0;
        float                  m_max_gain     = 1.0;
        float                  m_max_distance = 20000.0;
        bool                   m_loop         = false;
        bool                   m_autoplay     = false;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_SOURCE_2D_HPP
