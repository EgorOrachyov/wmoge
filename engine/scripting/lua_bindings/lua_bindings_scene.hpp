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

#ifndef WMOGE_LUA_BINDINGS_SCENE_HPP
#define WMOGE_LUA_BINDINGS_SCENE_HPP

#include "scripting/lua_bindings/lua_bindings.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"

#include "core/engine.hpp"
#include "scene/scene.hpp"
#include "scene/scene_component.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene_object.hpp"

namespace wmoge {

    struct LuaSceneObject : public LuaObject {
        void add_child(const LuaSceneObject& child) {
            cast_unsafe<SceneObject>()->add_child(child.ptr.cast<SceneObject>());
        }
        void add_sibling(const LuaSceneObject& sibling) {
            cast_unsafe<SceneObject>()->add_sibling(sibling.ptr.cast<SceneObject>());
        }
        void remove_child(const LuaSceneObject& child) {
            cast_unsafe<SceneObject>()->remove_child(child.ptr.cast<SceneObject>());
        }
        void remove_children() {
            cast_unsafe<SceneObject>()->remove_children();
        }
        int _c_get_scene(lua_State* state) {
            LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_scene()).push();
            return 1;
        }
        int _c_get_parent(lua_State* state) {
            LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_parent()).push();
            return 1;
        }
        int _c_get_child(lua_State* state) {
            auto selector = WG_LUA_ARG(state, 1);

            if (selector.isNumber()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_child(selector.cast<int>())).push();
                return 1;
            }
            if (selector.isString() || selector.isInstance<StringId>()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_child(LuaTypeTraits::to_sid(selector))).push();
                return 1;
            }

            WG_LOG_ERROR("invalid selector to get child " << selector.tostring());
            return 0;
        }
        int _c_find_child(lua_State* state) {
            auto selector = WG_LUA_ARG(state, 1);

            if (selector.isString() || selector.isInstance<StringId>()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->find_child(LuaTypeTraits::to_sid(selector))).push();
                return 1;
            }

            WG_LOG_ERROR("invalid selector to find child " << selector.tostring());
            return 0;
        }
        int _c_get(lua_State* state) {
            auto selector = WG_LUA_ARG(state, 1);

            if (selector.isNumber()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_component_base(selector.cast<int>())).push();
                return 1;
            }
            if (selector.isString()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_component_base(SID(selector.cast<std::string>()))).push();
                return 1;
            }
            if (selector.isInstance<StringId>()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_component_base(selector.cast<StringId>())).push();
                return 1;
            }

            WG_LOG_ERROR("invalid selector to get component " << selector.tostring());
            return 0;
        }
        int _c_get_or_create(lua_State* state) {
            auto selector = WG_LUA_ARG(state, 1);

            if (selector.isString()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_or_create_component_base(SID(selector.cast<std::string>()))).push();
                return 1;
            }
            if (selector.isInstance<StringId>()) {
                LuaTypeTraits::from_object(state, cast_unsafe<SceneObject>()->get_or_create_component_base(selector.cast<StringId>())).push();
                return 1;
            }

            WG_LOG_ERROR("invalid selector to get or create component " << selector.tostring());
            return 0;
        }
        StringId get_name() {
            return cast_unsafe<SceneObject>()->get_name();
        }
        StringId get_name_absolute() {
            return cast_unsafe<SceneObject>()->get_name_absolute();
        }
        bool is_in_scene() {
            return cast_unsafe<SceneObject>()->is_in_scene();
        }
    };

    struct LuaSceneComponent : public LuaObject {
        int _c_get_scene(lua_State* state) {
            LuaTypeTraits::from_object(state, cast_unsafe<SceneComponent>()->get_scene()).push();
            return 1;
        }
        int _c_get_scene_object(lua_State* state) {
            LuaTypeTraits::from_object(state, cast_unsafe<SceneComponent>()->get_scene_object()).push();
            return 1;
        }
    };

    struct LuaScene : public LuaObject {
        void add_child(const LuaSceneObject& object) {
            cast_unsafe<Scene>()->add_child(object.ptr.cast<SceneObject>());
        }
        int _c_get_child(lua_State* state) {
            auto selector = WG_LUA_ARG(state, 1);

            if (selector.isNumber()) {
                LuaTypeTraits::from_object(state, cast_unsafe<Scene>()->get_child(selector.cast<int>())).push();
                return 1;
            }

            WG_LOG_ERROR("invalid selector to get child " << selector.tostring());
            return 0;
        }
        int _c_find_child(lua_State* state) {
            auto selector = WG_LUA_ARG(state, 1);

            if (selector.isString() || selector.isInstance<StringId>()) {
                LuaTypeTraits::from_object(state, cast_unsafe<Scene>()->find_child(LuaTypeTraits::to_sid(selector))).push();
                return 1;
            }

            WG_LOG_ERROR("invalid selector to find child " << selector.tostring());
            return 0;
        }
    };

}// namespace wmoge

#endif//WMOGE_LUA_BINDINGS_SCENE_HPP
