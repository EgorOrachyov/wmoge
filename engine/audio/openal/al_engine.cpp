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

#include "al_engine.hpp"

#include "debug/profiler.hpp"

namespace wmoge {

    ALAudioEngine::ALAudioEngine() {
        WG_AUTO_PROFILE_OPENAL();

        init_device();
        init_context();
        init_caps();

        m_device_name = SID(m_caps.device_specifier);
        m_driver_name = SID("OpenAL Soft Audio Engine");
        m_engine_name = SID("openal");
        m_default_bus = SID("master");

        m_bus[m_default_bus] = make_ref<ALAudioBus>(m_default_bus, *this);

        WG_LOG_INFO("init openal audio engine");
        WG_LOG_INFO("device: " << m_caps.device_specifier << " " << m_caps.major_version << "." << m_caps.minor_version);
    }
    ALAudioEngine::~ALAudioEngine() {
        shutdown();
    }

    void ALAudioEngine::update() {
        WG_AUTO_PROFILE_OPENAL();
    }
    void ALAudioEngine::shutdown() {
        WG_AUTO_PROFILE_OPENAL();

        if (m_device) {
            m_bus.clear();

            WG_ALC_CHECK(alcMakeContextCurrent(nullptr));
            WG_ALC_CHECK(alcDestroyContext(m_context));
            WG_ALC_CHECK(alcCloseDevice(m_device));

            m_device  = nullptr;
            m_context = nullptr;
        }

        WG_LOG_INFO("shutdown openal audio engine");
    }

    ref_ptr<AudioPlayback> ALAudioEngine::make_playback(ref_ptr<AudioStream> stream, const StringId& bus, const StringId& name) {
        std::lock_guard guard(m_mutex);

        assert(stream);
        assert(!bus.empty());
        assert(m_bus.find(bus) != m_bus.end());

        if (!stream) {
            WG_LOG_ERROR("passed null stream to create playback " << name);
            return ref_ptr<AudioPlayback>{};
        }
        if (m_bus.find(bus) == m_bus.end()) {
            WG_LOG_ERROR("no such bus to create playback " << bus);
            return ref_ptr<AudioPlayback>{};
        }

        return make_ref<ALAudioPlayback>(std::move(stream), bus, name, *this);
    }
    ref_ptr<AudioBus> ALAudioEngine::make_bus(const StringId& name) {
        std::lock_guard guard(m_mutex);

        assert(!name.empty());
        assert(m_bus.find(name) == m_bus.end());

        if (name.empty()) {
            WG_LOG_ERROR("empty bus name is not allowed");
            return ref_ptr<ALAudioBus>{};
        }
        if (m_bus.find(name) != m_bus.end()) {
            WG_LOG_ERROR("already have created bus with name " << name);
            return ref_ptr<ALAudioBus>{};
        }

        return (m_bus[name] = make_ref<ALAudioBus>(name, *this)).as<AudioBus>();
    }
    ref_ptr<AudioBus> ALAudioEngine::find_bus(const StringId& name) {
        std::lock_guard guard(m_mutex);
        auto            query = m_bus.find(name);
        return query != m_bus.end() ? query->second.as<AudioBus>() : ref_ptr<AudioBus>{};
    }
    bool ALAudioEngine::has_bus(const StringId& name) {
        std::lock_guard guard(m_mutex);
        return m_bus.find(name) != m_bus.end();
    }

    ALAudioBus* ALAudioEngine::get_bus(const StringId& name) {
        auto query = m_bus.find(name);
        assert(query != m_bus.end());
        return query != m_bus.end() ? query->second.get() : nullptr;
    }
    ALCdevice* ALAudioEngine::get_device() {
        return m_device;
    }
    ALCcontext* ALAudioEngine::get_context() {
        return m_context;
    }
    std::recursive_mutex& ALAudioEngine::get_mutex() {
        return m_mutex;
    }

    bool ALAudioEngine::init_device() {
        WG_AUTO_PROFILE_OPENAL();

        m_device = alcOpenDevice(nullptr);
        if (!m_device) {
            WG_LOG_ERROR("failed to initialize default device");
            return false;
        }

        ALDebug::device = m_device;

        return true;
    }
    bool ALAudioEngine::init_context() {
        WG_AUTO_PROFILE_OPENAL();

        WG_ALC_CHECK(m_context = alcCreateContext(m_device, nullptr));
        if (!m_context) {
            WG_LOG_ERROR("failed to make context");
            return false;
        }

        ALCboolean is_current = ALC_FALSE;

        WG_ALC_CHECK(is_current = alcMakeContextCurrent(m_context));
        if (!is_current) {
            WG_LOG_ERROR("failed to make context current");
            return false;
        }

        return true;
    }
    bool ALAudioEngine::init_caps() {
        WG_AUTO_PROFILE_OPENAL();

        auto str_or_null = [](const ALCchar* str) { return str ? str : "<none>"; };

        WG_ALC_CHECK(m_caps.default_device_specifier = str_or_null(alcGetString(m_device, ALC_DEFAULT_DEVICE_SPECIFIER)));
        WG_ALC_CHECK(m_caps.capture_default_device_specifier = str_or_null(alcGetString(m_device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER)));
        WG_ALC_CHECK(m_caps.device_specifier = str_or_null(alcGetString(m_device, ALC_DEVICE_SPECIFIER)));
        WG_ALC_CHECK(m_caps.extensions = str_or_null(alcGetString(m_device, ALC_EXTENSIONS)));
        WG_ALC_CHECK(alcGetIntegerv(m_device, ALC_MAJOR_VERSION, sizeof(ALCint), &m_caps.major_version));
        WG_ALC_CHECK(alcGetIntegerv(m_device, ALC_MINOR_VERSION, sizeof(ALCint), &m_caps.minor_version));

        return true;
    }

}// namespace wmoge