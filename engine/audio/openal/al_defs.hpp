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

#ifndef WMOGE_AL_DEFS_HPP
#define WMOGE_AL_DEFS_HPP

#include "core/log.hpp"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#include <string>

namespace wmoge {

    class ALDebug {
    public:
        static ALCdevice*  device;
        static bool        has_al_error();
        static std::string get_al_error_message();
        static bool        has_alc_error();
        static std::string get_alc_error_message();
    };

}// namespace wmoge

#define WG_AL_CHECK(function)                                                                    \
    do {                                                                                         \
        function;                                                                                \
        if (ALDebug::has_al_error()) {                                                           \
            WG_LOG_ERROR("al error " << ALDebug::get_al_error_message() << " in " << #function); \
        }                                                                                        \
    } while (false)

#define WG_ALC_CHECK(function)                                                                     \
    do {                                                                                           \
        function;                                                                                  \
        if (ALDebug::has_alc_error()) {                                                            \
            WG_LOG_ERROR("alc error " << ALDebug::get_alc_error_message() << " in " << #function); \
        }                                                                                          \
    } while (false)

#endif//WMOGE_AL_DEFS_HPP
