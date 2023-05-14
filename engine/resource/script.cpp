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

#include "script.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "platform/file_system.hpp"
#include "scripting/script_system.hpp"

namespace wmoge {

    bool Script::load_from_import_options(const YamlTree& tree) {
        if (!Resource::load_from_import_options(tree)) {
            return false;
        }

        auto        params = tree["params"];
        std::string file;
        std::string language;

        params["file"] >> file;
        if (file.empty()) {
            WG_LOG_ERROR("no file provided in " << get_name());
            return false;
        }

        params["language"] >> language;
        if (language.empty()) {
            WG_LOG_ERROR("no language provided in " << get_name());
            return false;
        }

        m_language = SID(language);

        if (!Engine::instance()->file_system()->read_file(file, m_code)) {
            WG_LOG_ERROR("failed to read script code from file " << file);
            return false;
        }

        return true;
    }
    void Script::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto* script       = dynamic_cast<Script*>(&copy);
        script->m_mask     = m_mask;
        script->m_code     = m_code;
        script->m_language = m_language;
    }

    Ref<ScriptInstance> Script::attach_to(Object* object) {
        return Ref<ScriptInstance>{};
    }
    bool Script::has_property(const StringId& property) {
        return false;
    }
    bool Script::has_method(const StringId& method) {
        return false;
    }
    ScriptFunctionsMask Script::get_mask() {
        return m_mask;
    }
    const StringId& Script::get_language() {
        return m_language;
    }
    const std::string& Script::get_code() {
        return m_code;
    }

    void Script::register_class() {
        auto* cls = Class::register_class<Script>();
    }

}// namespace wmoge
