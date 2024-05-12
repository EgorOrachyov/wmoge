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

#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

#include <AudioFile.h>

#include <cstring>

namespace wmoge {

    Status AudioStreamWav::load(const std::string& file_path) {
        WG_AUTO_PROFILE_ASSET("AudioStreamWav::load");

        std::vector<std::uint8_t> file_data;

        if (!Engine::instance()->file_system()->read_file(file_path, file_data)) {
            WG_LOG_ERROR("field to read wav file " << file_path);
            return StatusCode::FailedRead;
        }

        AudioFile<float> file;

        if (!file.loadFromMemory(file_data)) {
            WG_LOG_ERROR("failed to load from memory wav file " << file_path);
            return StatusCode::Error;
        }

        if (file.getNumChannels() <= 0) {
            WG_LOG_ERROR("no channels in loaded wav file " << file_path);
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