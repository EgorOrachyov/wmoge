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

#ifndef WMOGE_AL_PLAYBACK_HPP
#define WMOGE_AL_PLAYBACK_HPP

#include "audio/audio_defs.hpp"
#include "audio/audio_playback.hpp"
#include "audio/openal/al_defs.hpp"
#include "core/fast_vector.hpp"

namespace wmoge {

    /**
     * @class ALAudioPlayback
     * @brief OpenAL audio playback inmpleemntation
     */
    class ALAudioPlayback final : public AudioPlayback {
    public:
        ALAudioPlayback(Ref<AudioStream> stream, Strid bus, const Strid& name, class ALAudioEngine& engine);
        ~ALAudioPlayback() override;

        void play() override;
        void stop() override;
        void pause() override;
        void resume() override;

        void bus_pause();
        void bus_resume();
        void bus_set_gain();
        void bus_set_pitch_scale();

        void set_position(Vec3f value) override;
        void set_velocity(Vec3f value) override;
        void set_direction(Vec3f value) override;
        void set_pitch_scale(float value) override;
        void set_gain(float value) override;
        void set_min_gain(float value) override;
        void set_max_gain(float value) override;
        void set_max_distance(float value) override;
        void set_loop(bool value) override;

        fast_vector<ALuint>& get_buffers() { return m_buffers; }
        ALuint               get_source() { return m_source; }
        AudioPlaybackState   get_state() { return m_state; }
        float                get_pitch_scale() { return m_pitch_scale; }
        float                get_gain() { return m_gain; }
        class ALAudioBus*    get_bus() { return m_bus; }
        class ALAudioEngine& get_engine() { return m_engine; }

    private:
        fast_vector<ALuint>  m_buffers;
        ALuint               m_source      = AL_NONE;
        AudioPlaybackState   m_state       = AudioPlaybackState::Stopped;
        float                m_pitch_scale = 1.0f;
        float                m_gain        = 1.0f;
        class ALAudioBus*    m_bus         = nullptr;
        class ALAudioEngine& m_engine;
    };

}// namespace wmoge

#endif//WMOGE_AL_PLAYBACK_HPP
