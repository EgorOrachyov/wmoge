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

#ifndef WMOGE_AUDIO_ENGINE_HPP
#define WMOGE_AUDIO_ENGINE_HPP

#include "audio/audio_bus.hpp"
#include "audio/audio_defs.hpp"
#include "audio/audio_playback.hpp"
#include "core/string_id.hpp"

namespace wmoge {

    /**
     * @class AudioEngine
     * @brief Audio engine interface
     *
     * Audio engine is responsible for a playing sounds in 2d and 3d space.
     * It provides API for playing sounds, looping, suspending, and mixing.
     *
     * Audio engine has a number of busses for playing sounds. Default is
     * `master` bus always provided. Instances of audio streams are created
     * and played withing busses. Bus can be used to control all sounds. It
     * gives an ability to suspend, resume all sounds, scale gain and pitch.
     *
     * @note thread-safe
     */
    class AudioEngine {
    public:
        virtual ~AudioEngine() = default;

        virtual void               update()                                                                          = 0;
        virtual void               shutdown()                                                                        = 0;
        virtual Ref<AudioPlayback> make_playback(Ref<AudioStream> stream, const StringId& bus, const StringId& name) = 0;
        virtual Ref<AudioBus>      make_bus(const StringId& name)                                                    = 0;
        virtual Ref<AudioBus>      find_bus(const StringId& name)                                                    = 0;
        virtual bool               has_bus(const StringId& name)                                                     = 0;

        const AudioDriverCaps& get_caps() const { return m_caps; }
        const StringId&        get_engine_name() const { return m_engine_name; }
        const StringId&        get_driver_name() const { return m_driver_name; }
        const StringId&        get_device_name() const { return m_device_name; }
        const StringId&        get_default_bus() const { return m_default_bus; }

    protected:
        AudioDriverCaps m_caps;
        StringId        m_engine_name;
        StringId        m_driver_name;
        StringId        m_device_name;
        StringId        m_default_bus;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_ENGINE_HPP
