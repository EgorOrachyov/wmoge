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

#include "lua_bindings_scene.hpp"

namespace wmoge {

    static int lua_SceneManager_next_running(lua_State* state) {
        auto scene = WG_LUA_ARG(state, 0);

        WG_LUA_CHECK_MSG(scene.isInstance<LuaScene>(), "expected scene object");
        auto* scene_manager = Engine::instance()->scene_manager();
        scene_manager->next_running(scene.cast<LuaScene>().ptr.cast<Scene>());

        return 0;
    }

    static int lua_SceneManager_get_running_scene(lua_State* state) {
        auto* scene_manager = Engine::instance()->scene_manager();
        auto  scene         = scene_manager->get_running_scene();
        LuaTypeTraits::from_object(state, scene.get()).push();

        return 1;
    }

    static int lua_SceneManager_make_scene(lua_State* state) {
        auto name = WG_LUA_ARG(state, 0);

        auto* scene_manager = Engine::instance()->scene_manager();
        auto  scene         = scene_manager->make_scene(LuaTypeTraits::to_sid(name));
        LuaTypeTraits::from_object(state, scene.get()).push();

        return 1;
    }

    static int lua_SceneManager_make_object(lua_State* state) {
        auto name = WG_LUA_ARG(state, 0);

        auto* scene_manager = Engine::instance()->scene_manager();
        auto  object        = scene_manager->make_object(LuaTypeTraits::to_sid(name));
        LuaTypeTraits::from_object(state, object.get()).push();

        return 1;
    }

    void LuaBindings::bind_scene(luabridge::Namespace& ns, fast_map<const wmoge::Class*, wmoge::LuaConvCppToLua>& mapping) {
        WG_LUA_MAP_CLASS(mapping, SceneObject);
        ns = ns.deriveClass<LuaSceneObject, LuaObject>("SceneObject")
                     .addConstructor<void (*)(void)>()
                     .addFunction("add_child", &LuaSceneObject::add_child)
                     .addFunction("add_sibling", &LuaSceneObject::add_sibling)
                     .addFunction("remove_child", &LuaSceneObject::remove_child)
                     .addFunction("remove_children", &LuaSceneObject::remove_children)
                     .addFunction("get_scene", &LuaSceneObject::_c_get_scene)
                     .addFunction("get_parent", &LuaSceneObject::_c_get_parent)
                     .addFunction("get_child", &LuaSceneObject::_c_get_child)
                     .addFunction("find_child", &LuaSceneObject::_c_find_child)
                     .addFunction("get", &LuaSceneObject::_c_get)
                     .addFunction("get_or_create", &LuaSceneObject::_c_get_or_create)
                     .addFunction("get_name", &LuaSceneObject::get_name)
                     .addFunction("get_name_absolute", &LuaSceneObject::get_name_absolute)
                     .addFunction("is_in_scene", &LuaSceneObject::is_in_scene)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, SceneComponent);
        ns = ns.deriveClass<LuaSceneComponent, LuaObject>("SceneComponent")
                     .addFunction("get_scene", &LuaSceneComponent::_c_get_scene)
                     .addFunction("get_scene_object", &LuaSceneComponent::_c_get_scene_object)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Scene);
        ns = ns.deriveClass<LuaScene, LuaObject>("Scene")
                     .addFunction("add_child", &LuaScene::add_child)
                     .addFunction("get_child", &LuaScene::_c_get_child)
                     .addFunction("find_child", &LuaScene::_c_find_child)
                     .endClass();

        ns = ns.beginNamespace("SceneManager")
                     .addFunction("next_running", lua_SceneManager_next_running)
                     .addFunction("get_running_scene", lua_SceneManager_get_running_scene)
                     .addFunction("make_scene", lua_SceneManager_make_scene)
                     .addFunction("make_object", lua_SceneManager_make_object)
                     .endNamespace();
    }

}// namespace wmoge