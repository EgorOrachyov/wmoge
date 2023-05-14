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

#ifndef WMOGE_AL_ENGINE_HPP
#define WMOGE_AL_ENGINE_HPP

#include "audio/audio_engine.hpp"
#include "audio/openal/al_bus.hpp"
#include "audio/openal/al_defs.hpp"
#include "audio/openal/al_playback.hpp"
#include "core/fast_map.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class ALAudioEngine
     * @brief OpenAL implementation of audio engine
     */
    class ALAudioEngine final : public AudioEngine {
    public:
        ALAudioEngine();
        ~ALAudioEngine() override;

        void               update() override;
        void               shutdown() override;
        Ref<AudioPlayback> make_playback(Ref<AudioStream> stream, const StringId& bus, const StringId& name) override;
        Ref<AudioBus>      make_bus(const StringId& name) override;
        Ref<AudioBus>      find_bus(const StringId& name) override;
        bool               has_bus(const StringId& name) override;

        ALAudioBus*           get_bus(const StringId& name);
        ALCdevice*            get_device();
        ALCcontext*           get_context();
        std::recursive_mutex& get_mutex();

    protected:
        bool init_device();
        bool init_context();
        bool init_caps();

    private:
        fast_map<StringId, Ref<ALAudioBus>> m_bus;
        ALCdevice*                          m_device  = nullptr;
        ALCcontext*                         m_context = nullptr;
        std::recursive_mutex                m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_AL_ENGINE_HPP
