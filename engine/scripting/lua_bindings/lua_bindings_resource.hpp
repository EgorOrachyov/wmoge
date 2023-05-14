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

#include "core/engine.hpp"
#include "resource/audio_stream.hpp"
#include "resource/config_file.hpp"
#include "resource/font.hpp"
#include "resource/image.hpp"
#include "resource/material.hpp"
#include "resource/material_shader.hpp"
#include "resource/pfx_effect.hpp"
#include "resource/resource_manager.hpp"
#include "resource/sprite.hpp"
#include "resource/texture.hpp"

namespace wmoge {

    struct LuaResource : public LuaObject {
        LuaResource duplicate() const {
            return LuaResource{cast_unsafe<Resource>()->duplicate()};
        }
        const StringId& get_name() const {
            return cast_unsafe<Resource>()->get_name();
        }
    };

    struct LuaAudioStream : public LuaResource {
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

    struct LuaConfigFile : public LuaResource {
        bool get_bool(const StringId& key, bool default_value) {
            return cast_unsafe<ConfigFile>()->get_bool(key, default_value);
        }
        int get_int(const StringId& key, int default_value) {
            return cast_unsafe<ConfigFile>()->get_int(key, default_value);
        }
        float get_float(const StringId& key, float default_value) {
            return cast_unsafe<ConfigFile>()->get_float(key, default_value);
        }
        std::string get_string(const StringId& key, std::string default_value) {
            return cast_unsafe<ConfigFile>()->get_string(key, default_value);
        }
    };

    struct LuaFont : public LuaResource {
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

    struct LuaTexture : public LuaResource {
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
        bool get_compression() const {
            return cast_unsafe<Texture>()->get_compression();
        }
    };

    struct LuaSprite : public LuaResource {
        int _c_get_animation_texture(lua_State* state) {
            auto animation = WG_LUA_ARG(state, 1).cast<int>();
            LuaTypeTraits::from_object(state, cast_unsafe<Sprite>()->get_animation_texture(animation).get()).push();
            return 1;
        }
        const Vec4f& get_animation_frame(int animation, int frame) const {
            return cast_unsafe<Sprite>()->get_animation_frame(animation, frame);
        }
        int get_animation_n_frames(int animation) const {
            return cast_unsafe<Sprite>()->get_animation_n_frames(animation);
        }
        float get_animation_speed(int animation) const {
            return cast_unsafe<Sprite>()->get_animation_speed(animation);
        }
        bool get_animation_loop(int animation) const {
            return cast_unsafe<Sprite>()->get_animation_loop(animation);
        }
        int get_animation_id(const StringId& animation) const {
            return cast_unsafe<Sprite>()->get_animation_id(animation);
        }
        int get_n_animations() const {
            return cast_unsafe<Sprite>()->get_n_animations();
        }
        const Vec2f& get_size() const {
            return cast_unsafe<Sprite>()->get_size();
        }
        const Vec2f& get_pivot() const {
            return cast_unsafe<Sprite>()->get_pivot();
        }
    };

    struct LuaPfxEffect : public LuaResource {
        int get_components_count() const {
            return cast_unsafe<PfxEffect>()->get_components_count();
        }
        std::string get_type() const {
            return std::string(magic_enum::enum_name(cast_unsafe<PfxEffect>()->get_type()));
        }
    };

    struct LuaMaterial : public LuaResource {
        void set_int(const StringId& name, int value) {
            cast_unsafe<Material>()->set_int(name, value);
        }
        void set_float(const StringId& name, float value) {
            cast_unsafe<Material>()->set_float(name, value);
        }
        void set_vec2(const StringId& name, const Vec2f& value) {
            cast_unsafe<Material>()->set_vec2(name, value);
        }
        void set_vec3(const StringId& name, const Vec3f& value) {
            cast_unsafe<Material>()->set_vec3(name, value);
        }
        void set_vec4(const StringId& name, const Vec4f& value) {
            cast_unsafe<Material>()->set_vec4(name, value);
        }
        void set_texture(const StringId& name, const Ref<Texture>& texture) {
            cast_unsafe<Material>()->set_texture(name, texture);
        }
    };

}// namespace wmoge

#endif//WMOGE_LUA_BINDINGS_RESOURCE_HPP
