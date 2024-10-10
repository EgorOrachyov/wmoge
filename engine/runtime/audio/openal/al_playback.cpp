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

#include "al_playback.hpp"

#include "audio/openal/al_engine.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cstring>

namespace wmoge {

    ALAudioPlayback::ALAudioPlayback(Ref<AudioStream> stream, Strid bus, const Strid& name, class ALAudioEngine& engine)
        : m_engine(engine) {

        WG_PROFILE_CPU_OPENAL("ALAudioPlayback::ALAudioPlayback");

        assert(stream);
        assert(!bus.empty());

        m_stream   = std::move(stream);
        m_bus_name = bus;
        m_name     = name;
        m_bus      = m_engine.get_bus(bus);

        m_bus->add_playback(this);

        bool   is_stereo    = m_stream->is_stereo();
        int    num_channels = is_stereo ? 2 : 1;
        int    samples_rate = m_stream->get_samples_rate();
        int    num_samples  = m_stream->get_num_samples();
        ALuint buffer;
        ALenum format = is_stereo ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;

        WG_AL_CHECK(alGenBuffers(1, &buffer));

        if (is_stereo) {
            std::vector<float> buffer_data(num_channels * num_samples);

            auto         data_left     = m_stream->get_channel_data(0);
            auto         data_right    = m_stream->get_channel_data(1);
            const float* samples_left  = reinterpret_cast<const float*>(data_left->buffer());
            const float* samples_right = reinterpret_cast<const float*>(data_right->buffer());

            int current = 0;

            for (int i = 0; i < num_samples; i++) {
                buffer_data[current++] = samples_left[i];
                buffer_data[current++] = samples_right[i];
            }

            WG_AL_CHECK(alBufferData(buffer, format, buffer_data.data(), ALsizei(buffer_data.size() * sizeof(float)), samples_rate));
        } else {
            auto data = m_stream->get_channel_data(0);
            WG_AL_CHECK(alBufferData(buffer, format, data->buffer(), ALsizei(data->size()), samples_rate));
        }

        WG_AL_CHECK(alGenSources(1, &m_source));
        WG_AL_CHECK(alSourcei(m_source, AL_BUFFER, buffer));
        WG_AL_CHECK(alSourcei(m_source, AL_LOOPING, 0));
        WG_AL_CHECK(alSourcef(m_source, AL_GAIN, m_bus->get_gain_scale()));
        WG_AL_CHECK(alSourcef(m_source, AL_PITCH, m_bus->get_pitch_scale()));
        WG_AL_CHECK(alSource3f(m_source, AL_POSITION, 0.0f, 0.0f, 0.0f));
        WG_AL_CHECK(alSource3f(m_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f));

        m_buffers.push_back(buffer);
    }
    ALAudioPlayback::~ALAudioPlayback() {
        std::lock_guard guard(m_engine.get_mutex());

        m_bus->remove_playback(this);
        m_bus = nullptr;

        if (m_source) {
            WG_AL_CHECK(alDeleteSources(1, &m_source));
            WG_AL_CHECK(alDeleteBuffers(ALsizei(m_buffers.size()), m_buffers.data()));
            m_buffers.clear();
            m_source = AL_NONE;
        }
    }

    void ALAudioPlayback::play() {
        std::lock_guard guard(m_engine.get_mutex());

        m_state = AudioPlaybackState::Playing;
        if (m_bus->get_state() == AudioBusState::Active) {
            WG_AL_CHECK(alSourcePlay(m_source));
        }
    }
    void ALAudioPlayback::stop() {
        std::lock_guard guard(m_engine.get_mutex());

        m_state = AudioPlaybackState::Stopped;
        WG_AL_CHECK(alSourceStop(m_source));
    }
    void ALAudioPlayback::pause() {
        std::lock_guard guard(m_engine.get_mutex());

        m_state = AudioPlaybackState::Paused;
        WG_AL_CHECK(alSourcePause(m_source));
    }
    void ALAudioPlayback::resume() {
        std::lock_guard guard(m_engine.get_mutex());

        m_state = AudioPlaybackState::Playing;
        if (m_bus->get_state() == AudioBusState::Active) {
            WG_AL_CHECK(alSourcePlay(m_source));
        }
    }

    void ALAudioPlayback::bus_pause() {
        WG_AL_CHECK(alSourcePause(m_source));
    }
    void ALAudioPlayback::bus_resume() {
        if (m_state == AudioPlaybackState::Playing) {
            WG_AL_CHECK(alSourcePlay(m_source));
        }
    }
    void ALAudioPlayback::bus_set_gain() {
        WG_AL_CHECK(alSourcef(m_source, AL_GAIN, m_gain * m_bus->get_gain_scale()));
    }
    void ALAudioPlayback::bus_set_pitch_scale() {
        WG_AL_CHECK(alSourcef(m_source, AL_PITCH, m_pitch_scale * m_bus->get_pitch_scale()));
    }

    void ALAudioPlayback::set_position(Vec3f value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSource3f(m_source, AL_POSITION, value.x(), value.y(), value.z()));
    }
    void ALAudioPlayback::set_velocity(Vec3f value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSource3f(m_source, AL_VELOCITY, value.x(), value.y(), value.z()));
    }
    void ALAudioPlayback::set_direction(Vec3f value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSource3f(m_source, AL_DIRECTION, value.x(), value.y(), value.z()));
    }
    void ALAudioPlayback::set_pitch_scale(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        m_pitch_scale = value;
        bus_set_pitch_scale();
    }
    void ALAudioPlayback::set_gain(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        m_gain = value;
        bus_set_gain();
    }
    void ALAudioPlayback::set_min_gain(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSourcef(m_source, AL_MIN_GAIN, value));
    }
    void ALAudioPlayback::set_max_gain(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSourcef(m_source, AL_MAX_GAIN, value));
    }
    void ALAudioPlayback::set_max_distance(float value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSourcef(m_source, AL_MAX_DISTANCE, value));
    }
    void ALAudioPlayback::set_loop(bool value) {
        std::lock_guard guard(m_engine.get_mutex());
        WG_AL_CHECK(alSourcei(m_source, AL_LOOPING, value));
    }

}// namespace wmoge