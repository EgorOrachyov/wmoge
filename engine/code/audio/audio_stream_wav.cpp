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

#include "audio_stream_wav.hpp"

#include "profiler/profiler_cpu.hpp"

#include <AudioFile.h>

#include <cstring>

namespace wmoge {

    Status AudioStreamWav::load(array_view<const std::uint8_t> file_data) {
        WG_PROFILE_CPU_ASSET("AudioStreamWav::load");

        std::vector<std::uint8_t> data;
        data.resize(file_data.size());
        std::memcpy(data.data(), file_data.data(), file_data.size());

        AudioFile<float> file;

        if (!file.loadFromMemory(data)) {
            WG_LOG_ERROR("failed to load from memory wav file " << get_name());
            return StatusCode::Error;
        }
        if (file.getNumChannels() <= 0) {
            WG_LOG_ERROR("no channels in loaded wav file " << get_name());
            return StatusCode::InvalidData;
        }

        m_length          = float(file.getLengthInSeconds());
        m_samples_rate    = int(file.getSampleRate());
        m_bits_per_sample = file.getBitDepth();
        m_num_samples     = file.getNumSamplesPerChannel();
        m_num_channels    = file.getNumChannels();

        m_data.reserve(m_num_channels);

        for (int i = 0; i < m_num_channels; i++) {
            auto buffer = make_ref<Data>(sizeof(float) * m_num_samples);
            std::memcpy(buffer->buffer(), file.samples[i].data(), m_num_samples * sizeof(float));
            m_data.push_back(buffer);
        }

        return WG_OK;
    }

    Ref<Data> AudioStreamWav::get_channel_data(int channel) {
        assert(channel < m_num_channels);
        return m_data[channel];
    }

}// namespace wmoge