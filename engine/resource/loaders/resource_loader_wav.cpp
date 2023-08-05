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

#include "resource_loader_wav.hpp"

#include "debug/profiler.hpp"
#include "resource/audio_stream_wav.hpp"

namespace wmoge {

    bool ResourceLoaderWav::load(const StringId& name, const ResourceMeta& meta, Ref<Resource>& res) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderWav::load");

        Ref<AudioStreamWav> audio = meta.cls->instantiate().cast<AudioStreamWav>();

        if (!audio) {
            WG_LOG_ERROR("failed to instantiate audio " << name);
            return false;
        }

        res = audio;
        res->set_name(name);

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("No import options to load audio " << name);
            return false;
        }

        AudioImportOptions options;
        WG_YAML_READ_AS(meta.import_options->crootref(), "params", options);

        return audio->load(options.source_file);
    }
    StringId ResourceLoaderWav::get_name() {
        return SID("wav");
    }

}// namespace wmoge