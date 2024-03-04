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

#ifndef WMOGE_AUDIO_BUS_HPP
#define WMOGE_AUDIO_BUS_HPP

#include "audio/audio_playback.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class AudioBus
     * @brief An abstract sound bus used to control and modify played audio
     */
    class AudioBus : public RefCnt {
    public:
        ~AudioBus() override = default;

        virtual void make_active()                                             = 0;
        virtual void make_inactive()                                           = 0;
        virtual void set_gain_scale(float value)                               = 0;
        virtual void set_pitch_scale(float value)                              = 0;
        virtual void get_playbacks(std::vector<Ref<AudioPlayback>>& playbacks) = 0;

        const Strid& get_name() const { return m_name; }

    protected:
        Strid m_name;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_BUS_HPP
