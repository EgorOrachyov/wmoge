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

#include "lua_bindings_components.hpp"

namespace wmoge {

    void LuaBindings::bind_components(luabridge::Namespace& ns, fast_map<const wmoge::Class*, wmoge::LuaConvCppToLua>& mapping) {
        WG_LUA_MAP_CLASS(mapping, Spatial2d);
        ns = ns.deriveClass<LuaSpatial2d, LuaSceneComponent>("Spatial2d")
                     .addFunction("update_transform", &LuaSpatial2d::update_transform)
                     .addFunction("translate", &LuaSpatial2d::translate)
                     .addFunction("rotate", &LuaSpatial2d::rotate)
                     .addFunction("scale", &LuaSpatial2d::scale)
                     .addFunction("flush", &LuaSpatial2d::flush)
                     .addFunction("get_matr_local", &LuaSpatial2d::get_matr_local)
                     .addFunction("get_matr_local_inv", &LuaSpatial2d::get_matr_local_inv)
                     .addFunction("get_matr_global", &LuaSpatial2d::get_matr_global)
                     .addFunction("get_matr_global_inv", &LuaSpatial2d::get_matr_global_inv)
                     .addFunction("get_transform", &LuaSpatial2d::get_transform)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, CanvasLayer);
        ns = ns.deriveClass<LuaCanvasLayer, LuaSceneComponent>("CanvasLayer")
                     .addFunction("hide", &LuaCanvasLayer::hide)
                     .addFunction("show", &LuaCanvasLayer::show)
                     .addFunction("set_layer_id", &LuaCanvasLayer::set_layer_id)
                     .addFunction("get_offset", &LuaCanvasLayer::get_offset)
                     .addFunction("get_rotation", &LuaCanvasLayer::get_rotation)
                     .addFunction("get_id", &LuaCanvasLayer::get_id)
                     .addFunction("is_visible", &LuaCanvasLayer::get_is_visible)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, CanvasItem);
        ns = ns.deriveClass<LuaCanvasItem, LuaSceneComponent>("CanvasItem")
                     .addFunction("hide", &LuaCanvasItem::hide)
                     .addFunction("show", &LuaCanvasItem::show)
                     .addFunction("set_layer_id", &LuaCanvasItem::set_layer_id)
                     .addFunction("set_tint", &LuaCanvasItem::set_tint)
                     .addFunction("get_material", &LuaCanvasItem::get_material)
                     .addFunction("get_pos_global", &LuaCanvasItem::get_pos_global)
                     .addFunction("get_matr_global", &LuaCanvasItem::get_matr_global)
                     .addFunction("get_matr_global_inv", &LuaCanvasItem::get_matr_global_inv)
                     .addFunction("get_tint", &LuaCanvasItem::get_tint)
                     .addFunction("get_layer_id", &LuaCanvasItem::get_layer_id)
                     .addFunction("is_visible", &LuaCanvasItem::is_visible)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Camera2d);
        ns = ns.deriveClass<LuaCamera2d, LuaCanvasItem>("Camera2d")
                     .addFunction("set_viewport_rect", &LuaCamera2d::set_viewport_rect)
                     .addFunction("set_screen_space", &LuaCamera2d::set_screen_space)
                     .addFunction("get_viewport_rect", &LuaCamera2d::get_viewport_rect)
                     .addFunction("get_screen_space", &LuaCamera2d::get_screen_space)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, SpriteInstance);
        ns = ns.deriveClass<LuaSpriteInstance, LuaCanvasItem>("SpriteInstance")
                     .addFunction("play_animation", &LuaSpriteInstance::_c_play_animation)
                     .addFunction("stop_animation", &LuaSpriteInstance::stop_animation)
                     .addFunction("get_sprite", &LuaSpriteInstance::_c_get_sprite)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, CanvasText);
        ns = ns.deriveClass<LuaCanvasText, LuaCanvasItem>("CanvasLayer")
                     .addFunction("set_text", &LuaCanvasText::set_text)
                     .addFunction("set_font", &LuaCanvasText::set_font)
                     .addFunction("set_font_size", &LuaCanvasText::set_font_size)
                     .addFunction("set_centered", &LuaCanvasText::set_centered)
                     .addFunction("get_text", &LuaCanvasText::get_text)
                     .addFunction("get_font", &LuaCanvasText::_c_get_font)
                     .addFunction("get_font_size", &LuaCanvasText::get_font_size)
                     .endClass();

        ns = ns.beginClass<PfxSpawnParams>("SpawnParams")
                     .addConstructor<void (*)(void)>()
                     .addProperty("amount", &PfxSpawnParams::amount)
                     .addProperty("pos2d", &PfxSpawnParams::pos2d)
                     .addProperty("vel2d", &PfxSpawnParams::vel2d)
                     .addProperty("angle", &PfxSpawnParams::angle)
                     .addProperty("size", &PfxSpawnParams::size)
                     .addProperty("lifetime", &PfxSpawnParams::lifetime)
                     .addProperty("color", &PfxSpawnParams::color)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Particles2d);
        ns = ns.deriveClass<LuaParticles2d, LuaCanvasItem>("Particles2d")
                     .addFunction("emit", &LuaParticles2d::emit)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, AudioListener2d);
        ns = ns.deriveClass<LuaAudioListener2d, LuaCanvasItem>("AudioListener2d")
                     .addFunction("set_position", &LuaAudioListener2d::set_position)
                     .addFunction("set_velocity", &LuaAudioListener2d::set_velocity)
                     .addFunction("set_direction", &LuaAudioListener2d::set_direction)
                     .addFunction("get_position", &LuaAudioListener2d::get_position)
                     .addFunction("get_velocity", &LuaAudioListener2d::get_velocity)
                     .addFunction("get_direction", &LuaAudioListener2d::get_direction)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, AudioSource2d);
        ns = ns.deriveClass<LuaAudioSource2d, LuaCanvasItem>("AudioSource2d")
                     .addFunction("create", &LuaAudioSource2d::create)
                     .addFunction("play", &LuaAudioSource2d::play)
                     .addFunction("stop", &LuaAudioSource2d::stop)
                     .addFunction("has_playback", &LuaAudioSource2d::has_playback)
                     .addFunction("set_position", &LuaAudioSource2d::set_position)
                     .addFunction("set_velocity", &LuaAudioSource2d::set_velocity)
                     .addFunction("set_direction", &LuaAudioSource2d::set_direction)
                     .addFunction("set_pitch_scale", &LuaAudioSource2d::set_pitch_scale)
                     .addFunction("set_gain", &LuaAudioSource2d::set_gain)
                     .addFunction("set_min_gain", &LuaAudioSource2d::set_min_gain)
                     .addFunction("set_max_gain", &LuaAudioSource2d::set_max_gain)
                     .addFunction("set_max_distance", &LuaAudioSource2d::set_max_distance)
                     .addFunction("get_position", &LuaAudioSource2d::get_position)
                     .addFunction("get_velocity", &LuaAudioSource2d::get_velocity)
                     .addFunction("get_direction", &LuaAudioSource2d::get_direction)
                     .addFunction("get_pitch_scale", &LuaAudioSource2d::get_pitch_scale)
                     .addFunction("get_gain", &LuaAudioSource2d::get_gain)
                     .addFunction("get_min_gain", &LuaAudioSource2d::get_min_gain)
                     .addFunction("get_max_gain", &LuaAudioSource2d::get_max_gain)
                     .addFunction("get_max_distance", &LuaAudioSource2d::get_max_distance)
                     .endClass();

        WG_LUA_MAP_CLASS_CUSTOM(mapping, ScriptComponent, LuaScriptComponent::mapping_rule);
        ns = ns.deriveClass<LuaScriptComponent, LuaSceneComponent>("ScriptComponent")
                     .endClass();

        //        WG_LUA_MAP_CLASS(mapping, Tag);
        //        ns = ns.deriveClass<LuaTag, LuaSceneComponent>("Tag")
        //                     .endClass();
    }

}// namespace wmoge