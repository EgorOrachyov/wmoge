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

#include "al_bus.hpp"

#include "audio/openal/al_engine.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    ALAudioBus::ALAudioBus(StringId name, class ALAudioEngine& engine) : m_engine(engine) {
        assert(!name.empty());
        m_name = std::move(name);
    }
    ALAudioBus::~ALAudioBus() {
        assert(m_playbacks.empty());

        if (!m_playbacks.empty()) {
            WG_LOG_ERROR("some playbacks of bus " << get_name() << " were not released correctly");
        }

        m_playbacks.clear();
        m_state = AudioBusState::None;
    }

    void ALAudioBus::add_playback(ALAudioPlayback* playback) {
        std::lock_guard guard(m_engine.get_mutex());

        assert(m_playbacks.find(playback) == m_playbacks.end());
        m_playbacks.emplace(playback);
    }
    void ALAudioBus::remove_playback(ALAudioPlayback* playback) {
        std::lock_guard guard(m_engine.get_mutex());

        assert(m_playbacks.find(playback) != m_playbacks.end());
        m_playbacks.erase(playback);
    }
    void ALAudioBus::make_active() {
        WG_AUTO_PROFILE_OPENAL("ALAudioBus::make_active");

        std::lock_guard guard(m_engine.get_mutex());

        if (m_state != AudioBusState::Active) {
            m_state = AudioBusState::Active;
            for (auto playback : m_playbacks) {
                playback->bus_resume();
            }
        }
    }
    void ALAudioBus::make_inactive() {
        WG_AUTO_PROFILE_OPENAL("ALAudioBus::make_inactive");

        std::lock_guard guard(m_engine.get_mutex());

        if (m_state != AudioBusState::Inactive) {
            m_state = AudioBusState::Inactive;
            for (auto playback : m_playbacks) {
                playback->bus_pause();
            }
        }
    }
    void ALAudioBus::set_gain_scale(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        m_gain_scale = value;
        for (auto playback : m_playbacks) {
            playback->bus_set_gain();
        }
    }
    void ALAudioBus::set_pitch_scale(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        m_pitch_scale = value;
        for (auto playback : m_playbacks) {
            playback->bus_set_pitch_scale();
        }
    }
    void ALAudioBus::get_playbacks(std::vector<Ref<AudioPlayback>>& playbacks) {
        WG_AUTO_PROFILE_OPENAL("ALAudioBus::get_playbacks");

        std::lock_guard guard(m_engine.get_mutex());

        playbacks.clear();
        playbacks.reserve(m_playbacks.size());

        for (auto playback : m_playbacks) {
            playbacks.emplace_back(playback);
        }
    }

}// namespace wmoge