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

#ifndef WMOGE_SCRIPT_NATIVE_HPP
#define WMOGE_SCRIPT_NATIVE_HPP

#include "core/class.hpp"
#include "core/object.hpp"
#include "resource/script.hpp"
#include "scripting/script_instance.hpp"

namespace wmoge {

    /**
     * @class ScriptNativeInstance
     * @brief An extension to simplify C++ native scripts creation
     */
    class ScriptNativeInstance : public ScriptInstance {
    public:
        ScriptNativeInstance(Ref<Script> script, Object* object, ScriptFunctionsMask mask);

        ScriptFunctionsMask get_mask() override;
        Script*             get_script() override;
        Object*             get_owner() override;

    private:
        Ref<Script>         m_script;
        Object*             m_object;
        ScriptFunctionsMask m_mask;
    };

}// namespace wmoge

#define WG_BEGIN_NATIVE_SCRIPT(name, ...)                                                    \
    class name final : public Script {                                                       \
    public:                                                                                  \
        WG_OBJECT(name, Script)                                                              \
                                                                                             \
        ref_ptr<ScriptInstance> attach_to(Object* object) override {                         \
            return make_ref<name##Instance>(ref_ptr<Script>(this), object, get_mask());      \
        }                                                                                    \
        ScriptFunctionsMask get_mask() override {                                            \
            return __VA_ARGS__;                                                              \
        }                                                                                    \
                                                                                             \
        class name##Instance final : public ScriptNativeInstance {                           \
        public:                                                                              \
            name##Instance(ref_ptr<Script> script, Object* object, ScriptFunctionsMask mask) \
                : ScriptNativeInstance(std::move(script), object, mask) {}

#define WG_END_NATIVE_SCRIPT(name)                 \
    }                                              \
    ;                                              \
    }                                              \
    ;                                              \
    void name::register_class() {                  \
        auto* cls = Class::register_class<name>(); \
    }

#endif//WMOGE_SCRIPT_NATIVE_HPP
