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

#ifndef WMOGE_AL_BUS_HPP
#define WMOGE_AL_BUS_HPP

#include "audio/audio_bus.hpp"
#include "audio/audio_defs.hpp"
#include "audio/openal/al_playback.hpp"
#include "core/flat_set.hpp"

#include <atomic>

namespace wmoge {

    /**
     * @class ALAudioBus
     * @brief OpenAL implementation for audio bus
     */
    class ALAudioBus final : public AudioBus {
    public:
        ALAudioBus(Strid name, class ALAudioEngine& engine);
        ~ALAudioBus() override;

        void add_playback(ALAudioPlayback* playback);
        void remove_playback(ALAudioPlayback* playback);
        void make_active() override;
        void make_inactive() override;
        void set_gain_scale(float value) override;
        void set_pitch_scale(float value) override;
        void get_playbacks(std::vector<Ref<AudioPlayback>>& playbacks) override;

        flat_set<ALAudioPlayback*>& get_playbacks() { return m_playbacks; }
        AudioBusState               get_state() { return m_state; }
        float                       get_gain_scale() { return m_gain_scale; }
        float                       get_pitch_scale() { return m_pitch_scale; }
        class ALAudioEngine&        get_engine() { return m_engine; }

    private:
        flat_set<ALAudioPlayback*> m_playbacks;
        AudioBusState              m_state       = AudioBusState::Active;
        float                      m_gain_scale  = 1.0f;
        float                      m_pitch_scale = 1.0f;
        class ALAudioEngine&       m_engine;
    };

}// namespace wmoge

#endif//WMOGE_AL_BUS_HPP
