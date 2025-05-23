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

#pragma once

#include "asset/asset.hpp"
#include "audio/audio_stream.hpp"
#include "core/array_view.hpp"
#include "core/buffered_vector.hpp"
#include "core/data.hpp"

namespace wmoge {

    /**
     * @class AudioStreamWav
     * @brief Simple audio asset loaded from `.wav` file format
     */
    class AudioStreamWav final : public AudioStream {
    public:
        WG_RTTI_CLASS(AudioStreamWav, AudioStream);

        AudioStreamWav()           = default;
        ~AudioStreamWav() override = default;

        /**
         * @brief Loads an audio from a wav file using audio file library
         *
         * @param file_data Audio file data in wav format
         *
         * @return True if loaded
         */
        Status load(array_view<const std::uint8_t> file_data);

        Ref<Data> get_channel_data(int channel) override;

    private:
        buffered_vector<Ref<Data>, 2> m_data;
    };

    WG_RTTI_CLASS_BEGIN(AudioStreamWav) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge