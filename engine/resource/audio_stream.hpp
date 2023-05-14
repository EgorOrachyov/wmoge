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

#ifndef WMOGE_AUDIO_STREAM_HPP
#define WMOGE_AUDIO_STREAM_HPP

#include "core/data.hpp"
#include "resource/resource.hpp"

namespace wmoge {

    /**
     * @class AudioStream
     * @brief Base class for audio files which can be loaded and played
     */
    class AudioStream : public Resource {
    public:
        WG_OBJECT(AudioStream, Resource)

        virtual Ref<Data> get_channel_data(int channel) { return Ref<Data>{}; }

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

        float get_length() const;
        int   get_samples_rate() const;
        int   get_bits_per_sample() const;
        int   get_num_samples() const;
        int   get_num_channels() const;
        bool  is_stereo() const;
        bool  is_mono() const;

    protected:
        float m_length          = 0.0f;
        int   m_samples_rate    = 0;
        int   m_bits_per_sample = 0;
        int   m_num_samples     = 0;
        int   m_num_channels    = 0;
    };

}// namespace wmoge

#endif//WMOGE_AUDIO_STREAM_HPP
