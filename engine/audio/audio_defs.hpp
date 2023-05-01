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

#ifndef WMOGE_AUDIO_DEFS_HPP
#define WMOGE_AUDIO_DEFS_HPP

#include <string>

namespace wmoge {

    /**
     * @class AudioPlaybackState
     * @brief State of audio stream playback
     */
    enum class AudioPlaybackState : int {
        None,
        Playing,
        Paused,
        Stopped
    };

    /**
     * @class AudioBusState
     * @brief State of bus for playing sounds
     */
    enum class AudioBusState : int {
        None,
        Active,
        Inactive
    };

    /**
     * @class AudioDriverCaps
     * @brief Optional capabilities and specs info about audio engine driver
     */
    struct AudioDriverCaps {
        std::string default_device_specifier;
        std::string capture_default_device_specifier;
        std::string device_specifier;
        std::string capture_device_specifier;
        std::string extensions;
        int         major_version = -1;
        int         minor_version = -1;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_DEFS_HPP
