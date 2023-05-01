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

#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"

#include <AudioFile.h>

#include <cstring>

namespace wmoge {

    ref_ptr<Data> AudioStreamWav::get_channel_data(int channel) {
        assert(channel < m_num_channels);
        return m_data[channel];
    }

    bool AudioStreamWav::load_from_import_options(const YamlTree& tree) {
        WG_AUTO_PROFILE_RESOURCE();

        if (!AudioStream::load_from_import_options(tree)) {
            return false;
        }

        auto                      params      = tree["params"];
        auto                      file_path   = Yaml::read_str(params["file"]);
        auto*                     file_system = Engine::instance()->file_system();
        std::vector<std::uint8_t> file_data;

        if (!file_system->read_file(file_path, file_data)) {
            WG_LOG_ERROR("field to read wav file " << file_path);
            return false;
        }

        AudioFile<float> file;

        if (!file.loadFromMemory(file_data)) {
            WG_LOG_ERROR("failed to load from memory wav file " << file_path);
            return false;
        }

        if (file.getNumChannels() <= 0) {
            WG_LOG_ERROR("no channels in loaded wav file " << file_path);
            return false;
        }

        m_length          = float(file.getLengthInSeconds());
        m_samples_rate    = file.getSampleRate();
        m_bits_per_sample = file.getBitDepth();
        m_num_samples     = file.getNumSamplesPerChannel();
        m_num_channels    = file.getNumChannels();

        m_data.reserve(m_num_channels);

        for (int i = 0; i < m_num_channels; i++) {
            auto buffer = make_ref<Data>(sizeof(float) * m_num_samples);
            std::memcpy(buffer->buffer(), file.samples[i].data(), m_num_samples * sizeof(float));
            m_data.push_back(buffer);
        }

        return true;
    }
    void AudioStreamWav::copy_to(Resource& copy) {
        AudioStream::copy_to(copy);
        auto* audio_stream_wav   = dynamic_cast<AudioStreamWav*>(&copy);
        audio_stream_wav->m_data = m_data;
    }

    void AudioStreamWav::register_class() {
        auto* cls = Class::register_class<AudioStreamWav>();
    }

}// namespace wmoge