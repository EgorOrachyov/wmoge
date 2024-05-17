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

#include "al_defs.hpp"

#include "audio/openal/al_engine.hpp"

namespace wmoge {

    ALCdevice* ALDebug::device = nullptr;

    bool ALDebug::has_al_error() {
        return alGetError() != AL_NO_ERROR;
    }
    std::string ALDebug::get_al_error_message() {
        ALenum error = alGetError();

        switch (error) {
            case AL_INVALID_NAME:
                return "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
            case AL_INVALID_ENUM:
                return "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
            case AL_INVALID_VALUE:
                return "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
            case AL_INVALID_OPERATION:
                return "AL_INVALID_OPERATION: the requested operation is not valid";
            case AL_OUT_OF_MEMORY:
                return "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
            default:
                return "";
        }
    }
    bool ALDebug::has_alc_error() {
        return alcGetError(device) != ALC_NO_ERROR;
    }
    std::string ALDebug::get_alc_error_message() {
        ALenum error = alcGetError(device);

        switch (error) {
            case ALC_INVALID_DEVICE:
                return "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
            case ALC_INVALID_CONTEXT:
                return "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
            case ALC_INVALID_ENUM:
                return "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
            case ALC_INVALID_VALUE:
                return "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
            case ALC_OUT_OF_MEMORY:
                return "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
            default:
                return "";
        }
    }

}// namespace wmoge