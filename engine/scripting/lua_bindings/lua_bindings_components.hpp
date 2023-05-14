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

#ifndef WMOGE_LUA_BINDINGS_COMPONENTS_HPP
#define WMOGE_LUA_BINDINGS_COMPONENTS_HPP

#include "scripting/lua/lua_script_instance.hpp"
#include "scripting/lua_bindings/lua_bindings.hpp"
#include "scripting/lua_bindings/lua_bindings_resource.hpp"
#include "scripting/lua_bindings/lua_bindings_scene.hpp"

#include "components/audio_listener_2d.hpp"
#include "components/audio_source_2d.hpp"
#include "components/camera_2d.hpp"
#include "components/canvas_item.hpp"
#include "components/canvas_layer.hpp"
#include "components/canvas_text.hpp"
#include "components/particles_2d.hpp"
#include "components/script_component.hpp"
#include "components/spatial_2d.hpp"
#include "components/sprite_instance.hpp"
#include "components/tag.hpp"

namespace wmoge {

    struct LuaSpatial2d : public LuaSceneComponent {
        void update_transform(const Transform2d& transform) {
            cast_unsafe<Spatial2d>()->update_transform(transform);
        }
        void translate(const Vec2f& translation) {
            cast_unsafe<Spatial2d>()->translate(translation);
        }
        void rotate(float angle_rad) {
            cast_unsafe<Spatial2d>()->rotate(angle_rad);
        }
        void scale(const Vec2f& scale) {
            cast_unsafe<Spatial2d>()->scale(scale);
        }
        void flush() {
            cast_unsafe<Spatial2d>()->flush();
        }
        Mat3x3f get_matr_local() const {
            return cast_unsafe<Spatial2d>()->get_matr_local();
        }
        Mat3x3f get_matr_local_inv() const {
            return cast_unsafe<Spatial2d>()->get_matr_local_inv();
        }
        Mat3x3f get_matr_global() const {
            return cast_unsafe<Spatial2d>()->get_matr_global();
        }
        Mat3x3f get_matr_global_inv() const {
            return cast_unsafe<Spatial2d>()->get_matr_global_inv();
        }
        Transform2d get_transform() const {
            return cast_unsafe<Spatial2d>()->get_transform();
        }
    };

    struct LuaCanvasLayer : public LuaSceneComponent {
        void hide() {
            cast_unsafe<CanvasLayer>()->hide();
        }
        void show() {
            cast_unsafe<CanvasLayer>()->show();
        }
        void set_layer_id(int id) {
            cast_unsafe<CanvasLayer>()->set_layer_id(id);
        }
        Vec2f get_offset() {
            return cast_unsafe<CanvasLayer>()->get_offset();
        }
        float get_rotation() {
            return cast_unsafe<CanvasLayer>()->get_rotation();
        }
        int get_id() {
            return cast_unsafe<CanvasLayer>()->get_id();
        }
        bool get_is_visible() {
            return cast_unsafe<CanvasLayer>()->get_is_visible();
        }
    };

    struct LuaCanvasItem : public LuaSceneComponent {
        void hide() {
            cast_unsafe<CanvasItem>()->hide();
        }
        void show() {
            cast_unsafe<CanvasItem>()->show();
        }
        void set_layer_id(int id) {
            cast_unsafe<CanvasItem>()->set_layer_id(id);
        }
        void set_tint(const Color4f& color) {
            cast_unsafe<CanvasItem>()->set_tint(color);
        }
        LuaMaterial get_material() {
            return LuaMaterial{cast_unsafe<CanvasItem>()->get_material()};
        }
        Vec2f get_pos_global() const {
            return cast_unsafe<CanvasItem>()->get_pos_global();
        }
        const Mat3x3f& get_matr_global() {
            return cast_unsafe<CanvasItem>()->get_matr_global();
        }
        const Mat3x3f& get_matr_global_inv() {
            return cast_unsafe<CanvasItem>()->get_matr_global_inv();
        }
        const Color4f& get_tint() {
            return cast_unsafe<CanvasItem>()->get_tint();
        }
        int get_layer_id() {
            return cast_unsafe<CanvasItem>()->get_layer_id();
        }
        bool is_visible() {
            return cast_unsafe<CanvasItem>()->is_visible();
        }
    };

    struct LuaCamera2d : public LuaCanvasItem {
        void set_viewport_rect(const Vec4f& viewport_rect) {
            cast_unsafe<Camera2d>()->set_viewport_rect(viewport_rect);
        }
        void set_screen_space(const Vec2f& screen_size) {
            cast_unsafe<Camera2d>()->set_screen_space(screen_size);
        }
        const Vec4f& get_viewport_rect() {
            return cast_unsafe<Camera2d>()->get_viewport_rect();
        }
        const Vec2f& get_screen_space() {
            return cast_unsafe<Camera2d>()->get_screen_space();
        }
    };

    struct LuaSpriteInstance : public LuaCanvasItem {
        int _c_play_animation(lua_State* state) {
            auto animation = WG_LUA_ARG(state, 1);

            if (animation.isInstance<StringId>()) {
                cast_unsafe<SpriteInstance>()->play_animation(animation.cast<StringId>());
                return 0;
            }
            if (animation.isNumber()) {
                cast_unsafe<SpriteInstance>()->play_animation(animation.cast<int>());
                return 0;
            }

            WG_LOG_ERROR("invalid animation name " << animation.tostring());
            return 0;
        }
        void stop_animation() {
            cast_unsafe<SpriteInstance>()->stop_animation();
        }
        int _c_get_sprite(lua_State* state) {
            LuaTypeTraits::from_object(state, cast_unsafe<SpriteInstance>()->get_sprite().get()).push();
            return 1;
        }
    };

    struct LuaCanvasText : public LuaCanvasItem {
        void set_text(std::string text) {
            cast_unsafe<CanvasText>()->set_text(std::move(text));
        }
        void set_font(const LuaFont& font) {
            cast_unsafe<CanvasText>()->set_font(font.ptr.cast<Font>());
        }
        void set_font_size(float font_size) {
            cast_unsafe<CanvasText>()->set_font_size(font_size);
        }
        void set_centered() {
            cast_unsafe<CanvasText>()->set_centered();
        }
        const std::string& get_text() {
            return cast_unsafe<CanvasText>()->get_text();
        }
        int _c_get_font(lua_State* state) {
            LuaTypeTraits::from_object(state, cast_unsafe<CanvasText>()->get_font().get()).push();
            return 1;
        }
        float get_font_size() {
            return cast_unsafe<CanvasText>()->get_font_size();
        }
    };

    struct LuaScriptComponent : public LuaSceneComponent {
        static luabridge::LuaRef mapping_rule(lua_State* state, Object* object) {
            ScriptComponent* component = dynamic_cast<ScriptComponent*>(object);
            if (auto instance = component->get_script().cast<LuaScriptInstance>()) {
                return instance->get_script_object();
            }
            return luabridge::LuaRef(state, LuaScriptComponent{Ref<RefCnt>(object)});
        }
    };

    struct LuaParticles2d : public LuaCanvasItem {
        void emit(const PfxSpawnParams& params) {
            cast_unsafe<Particles2d>()->emit(params);
        }
    };

    struct LuaAudioListener2d : public LuaCanvasItem {
        void set_position(Vec2f value) {
            cast_unsafe<AudioListener2d>()->set_position(value);
        }
        void set_velocity(Vec2f value) {
            cast_unsafe<AudioListener2d>()->set_velocity(value);
        }
        void set_direction(Vec2f value) {
            cast_unsafe<AudioListener2d>()->set_direction(value);
        }

        Vec2f get_position() const {
            return cast_unsafe<AudioListener2d>()->get_position();
        }
        Vec2f get_velocity() const {
            return cast_unsafe<AudioListener2d>()->get_velocity();
        }
        Vec2f get_direction() const {
            return cast_unsafe<AudioListener2d>()->get_direction();
        }
    };

    struct LuaAudioSource2d : public LuaCanvasItem {
        void create(const LuaAudioStream& stream, const StringId& bus) {
            cast_unsafe<AudioSource2d>()->create(stream.ptr.cast<AudioStream>(), bus);
        }
        void play() {
            cast_unsafe<AudioSource2d>()->play();
        }
        void stop() {
            cast_unsafe<AudioSource2d>()->stop();
        }
        bool has_playback() {
            return cast_unsafe<AudioSource2d>()->has_playback();
        }

        void set_position(Vec2f value) {
            cast_unsafe<AudioSource2d>()->set_position(value);
        }
        void set_velocity(Vec2f value) {
            cast_unsafe<AudioSource2d>()->set_velocity(value);
        }
        void set_direction(Vec2f value) {
            cast_unsafe<AudioSource2d>()->set_direction(value);
        }
        void set_pitch_scale(float value) {
            cast_unsafe<AudioSource2d>()->set_pitch_scale(value);
        }
        void set_gain(float value) {
            cast_unsafe<AudioSource2d>()->set_gain(value);
        }
        void set_min_gain(float value) {
            cast_unsafe<AudioSource2d>()->set_min_gain(value);
        }
        void set_max_gain(float value) {
            cast_unsafe<AudioSource2d>()->set_max_gain(value);
        }
        void set_max_distance(float value) {
            cast_unsafe<AudioSource2d>()->set_max_distance(value);
        }

        Vec2f get_position() const {
            return cast_unsafe<AudioSource2d>()->get_position();
        }
        Vec2f get_velocity() const {
            return cast_unsafe<AudioSource2d>()->get_velocity();
        }
        Vec2f get_direction() const {
            return cast_unsafe<AudioSource2d>()->get_direction();
        }
        float get_pitch_scale() const {
            return cast_unsafe<AudioSource2d>()->get_pitch_scale();
        }
        float get_gain() const {
            return cast_unsafe<AudioSource2d>()->get_gain();
        }
        float get_min_gain() const {
            return cast_unsafe<AudioSource2d>()->get_min_gain();
        }
        float get_max_gain() const {
            return cast_unsafe<AudioSource2d>()->get_max_gain();
        }
        float get_max_distance() const {
            return cast_unsafe<AudioSource2d>()->get_max_distance();
        }
    };

    struct LuaTag : public LuaSceneComponent {
    };

}// namespace wmoge

#endif//WMOGE_LUA_BINDINGS_COMPONENTS_HPP
