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

#ifndef WMOGE_SCRIPT_HPP
#define WMOGE_SCRIPT_HPP

#include "resource/resource.hpp"
#include "scripting/script_instance.hpp"

namespace wmoge {

    /**
     * @class Script
     * @brief Base class for any script which can be used to extend an object logic
     *
     * Script allows to extend a functionality of any engine object which inherits
     * engine core `Object` class. Script provides mechanism for the user to
     * declare a custom class with custom methods, properties and event handles
     * which can be trigger on the object. Script itself is a class, which must
     * be instanced and attached to an object.
     *
     * Script instance is an actual instance of the script, which can be attached
     * to an object. In order to extend object call `script->attach_to(object)`.
     * It creates new instance and attaches it to object.
     *
     * Script can be internally implemented as an native C++ class or as a text
     * file with script code using scriptable language, such as Lua.
     *
     * @see ScriptInstance
     * @see Object
     */
    class Script : public Resource {
    public:
        WG_OBJECT(Script, Resource)

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

        virtual Ref<ScriptInstance> attach_to(Object* object);
        virtual bool                has_property(const StringId& property);
        virtual bool                has_method(const StringId& method);

        ScriptFunctionsMask get_mask();
        const StringId&     get_language();
        const std::string&  get_code();

    protected:
        StringId            m_language;
        std::string         m_code;
        ScriptFunctionsMask m_mask;
    };

}// namespace wmoge

#endif//WMOGE_SCRIPT_HPP
