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

#include "lua_bindings_resource.hpp"

#include "scripting/lua/lua_type_traits.hpp"

namespace wmoge {

    static int lua_ResourceManager_load(lua_State* state) {
        auto resource_id = WG_LUA_ARG(state, 0);

        auto* resource_manager = Engine::instance()->resource_manager();
        auto  resource         = resource_manager->load(LuaTypeTraits::to_sid(resource_id));
        LuaTypeTraits::from_object(state, resource.get()).push();

        return 1;
    }

    static int lua_ResourceManager_find(lua_State* state) {
        auto resource_id = WG_LUA_ARG(state, 0);

        auto* resource_manager = Engine::instance()->resource_manager();
        auto  resource         = resource_manager->find(LuaTypeTraits::to_sid(resource_id));
        LuaTypeTraits::from_object(state, resource.get()).push();

        return 1;
    }

    void LuaBindings::bind_resource(luabridge::Namespace& ns, flat_map<const Class*, LuaConvCppToLua>& mapping) {
        WG_LUA_MAP_CLASS(mapping, Resource);
        ns = ns.deriveClass<LuaResource, LuaObject>("Resource")
                     .addFunction("duplicate", &LuaResource::duplicate)
                     .addFunction("get_name", &LuaResource::get_name)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, AudioStream);
        ns = ns.deriveClass<LuaAudioStream, LuaResource>("AudioStream")
                     .addFunction("get_length", &LuaAudioStream::get_length)
                     .addFunction("get_samples_rate", &LuaAudioStream::get_samples_rate)
                     .addFunction("get_bits_per_sample", &LuaAudioStream::get_bits_per_sample)
                     .addFunction("get_num_samples", &LuaAudioStream::get_num_samples)
                     .addFunction("get_num_channels", &LuaAudioStream::get_num_channels)
                     .addFunction("is_stereo", &LuaAudioStream::is_stereo)
                     .addFunction("is_mono", &LuaAudioStream::is_mono)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, ConfigFile);
        ns = ns.deriveClass<LuaConfigFile, LuaResource>("ConfigFile")
                     .addFunction("get_bool", &LuaConfigFile::get_bool)
                     .addFunction("get_int", &LuaConfigFile::get_int)
                     .addFunction("get_float", &LuaConfigFile::get_float)
                     .addFunction("get_string", &LuaConfigFile::get_string)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Texture);
        ns = ns.deriveClass<LuaTexture, LuaResource>("Texture")
                     .addFunction("get_width", &LuaTexture::get_width)
                     .addFunction("get_height", &LuaTexture::get_height)
                     .addFunction("get_depth", &LuaTexture::get_depth)
                     .addFunction("get_array_slices", &LuaTexture::get_array_slices)
                     .addFunction("get_mips", &LuaTexture::get_mips)
                     .addFunction("get_srgb", &LuaTexture::get_srgb)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, PfxEffect);
        ns = ns.deriveClass<LuaPfxEffect, LuaResource>("PfxEffect")
                     .addFunction("get_components_count", &LuaPfxEffect::get_components_count)
                     .addFunction("get_type", &LuaPfxEffect::get_type)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Material);
        ns = ns.deriveClass<LuaMaterial, LuaResource>("Material")
                     .addFunction("set_int", &LuaMaterial::set_int)
                     .addFunction("set_float", &LuaMaterial::set_float)
                     .addFunction("set_vec2", &LuaMaterial::set_vec2)
                     .addFunction("set_vec3", &LuaMaterial::set_vec3)
                     .addFunction("set_vec4", &LuaMaterial::set_vec4)
                     .addFunction("set_texture", &LuaMaterial::set_texture)
                     .endClass();

        ns = ns.beginNamespace("ResourceManager")
                     .addCFunction("load", lua_ResourceManager_load)
                     .addCFunction("find", lua_ResourceManager_find)
                     .endNamespace();
    }

}// namespace wmoge