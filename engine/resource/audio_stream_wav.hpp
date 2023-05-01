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

#ifndef WMOGE_AUDIO_STREAM_WAV_HPP
#define WMOGE_AUDIO_STREAM_WAV_HPP

#include "core/data.hpp"
#include "core/fast_vector.hpp"
#include "resource/audio_stream.hpp"
#include "resource/resource.hpp"

namespace wmoge {

    /**
     * @class AudioStreamWav
     * @brief Simple audio resource loaded from `.wav` file format
     */
    class AudioStreamWav final : public AudioStream {
    public:
        WG_OBJECT(AudioStreamWav, AudioStream)

        ref_ptr<Data> get_channel_data(int channel) override;

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

    private:
        fast_vector<ref_ptr<Data>, 2> m_data;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_STREAM_WAV_HPP
