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

#ifndef WMOGE_LUA_BINDINGS_RESOURCE_HPP
#define WMOGE_LUA_BINDINGS_RESOURCE_HPP

#include "scripting/lua_bindings/lua_bindings.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"

#include "asset/asset_manager.hpp"
#include "audio/audio_stream.hpp"
#include "grc/font.hpp"
#include "grc/image.hpp"
#include "grc/texture.hpp"
#include "material/material.hpp"
#include "pfx/pfx_effect.hpp"
#include "system/config.hpp"
#include "system/engine.hpp"

namespace wmoge {

    struct LuaAsset : public LuaObject {
        LuaAsset duplicate() const {
            return LuaAsset{cast_unsafe<Asset>()->duplicate()};
        }
        const Strid& get_name() const {
            return cast_unsafe<Asset>()->get_name();
        }
    };

    struct LuaAudioStream : public LuaAsset {
        float get_length() const {
            return cast_unsafe<AudioStream>()->get_length();
        }
        int get_samples_rate() const {
            return cast_unsafe<AudioStream>()->get_samples_rate();
        }
        int get_bits_per_sample() const {
            return cast_unsafe<AudioStream>()->get_bits_per_sample();
        }
        int get_num_samples() const {
            return cast_unsafe<AudioStream>()->get_num_samples();
        }
        int get_num_channels() const {
            return cast_unsafe<AudioStream>()->get_num_channels();
        }
        bool is_stereo() const {
            return cast_unsafe<AudioStream>()->is_stereo();
        }
        bool is_mono() const {
            return cast_unsafe<AudioStream>()->is_mono();
        }
    };

    struct LuaFont : public LuaAsset {
        const std::string& get_family_name() {
            return cast_unsafe<Font>()->get_family_name();
        }
        const std::string& get_style_name() {
            return cast_unsafe<Font>()->get_style_name();
        }
        int get_height() {
            return cast_unsafe<Font>()->get_height();
        }
        int get_glyphs_in_row() {
            return cast_unsafe<Font>()->get_glyphs_in_row();
        }
        int get_max_width() {
            return cast_unsafe<Font>()->get_max_width();
        }
        int get_max_height() {
            return cast_unsafe<Font>()->get_max_height();
        }
    };

    struct LuaTexture : public LuaAsset {
        int get_width() const {
            return cast_unsafe<Texture>()->get_width();
        }
        int get_height() const {
            return cast_unsafe<Texture>()->get_height();
        }
        int get_depth() const {
            return cast_unsafe<Texture>()->get_depth();
        }
        int get_array_slices() const {
            return cast_unsafe<Texture>()->get_array_slices();
        }
        int get_mips() const {
            return cast_unsafe<Texture>()->get_mips();
        }
        bool get_srgb() const {
            return cast_unsafe<Texture>()->get_srgb();
        }
    };

    struct LuaPfxEffect : public LuaAsset {
        int get_components_count() const {
            return cast_unsafe<PfxEffect>()->get_components_count();
        }
        std::string get_type() const {
            return std::string(magic_enum::enum_name(cast_unsafe<PfxEffect>()->get_type()));
        }
    };

    struct LuaMaterial : public LuaAsset {
    };

}// namespace wmoge

#endif//WMOGE_LUA_BINDINGS_RESOURCE_HPP
