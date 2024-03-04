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

#include "config_file.hpp"

#include "core/data.hpp"
#include "core/string_utils.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"
#include "system/engine.hpp"

#include <fstream>

namespace wmoge {

    Status ConfigFile::load(const std::string& path) {
        WG_AUTO_PROFILE_RESOURCE("ConfigFile::load");

        std::fstream file;
        FileSystem*  file_system = Engine::instance()->file_system();

        if (!file_system->open_file(path, file, std::ios_base::in | std::ios_base::binary)) {
            WG_LOG_ERROR("failed to read config file from " << path);
            return StatusCode::FailedRead;
        }

        Strid       section;
        std::string line;

        while (!file.eof()) {
            std::getline(file, line);

            if (!line.empty() && line[line.length() - 1] == '\r') {
                line = line.substr(0, line.length() - 1);
            }

            if (line[0] == '[') {
                section = SID(line.substr(1, line.find_last_of(']') - 1));
                continue;
            }
            if (line[0] == ';') {
                continue;
            }

            auto pos = line.find(" = ");
            if (pos != std::string::npos) {
                std::string key   = line.substr(0, pos);
                std::string value = line.substr(pos + 3);
                Var         var;

                if (value == "true") {
                    var = Var(1);
                } else if (value == "false") {
                    var = Var(0);
                } else if (value[0] == '\"') {
                    var = Var(value.substr(1, value.find_last_of('\"') - 1));
                } else {
                    var = Var(value);
                }

                m_entries.emplace(SID(section.str() + "." + key), var);
            }
        }

        return StatusCode::Ok;
    }

    Status ConfigFile::stack(const ConfigFile& other, ConfigStackMode mode) {
        WG_AUTO_PROFILE_RESOURCE("ConfigFile::stack");

        for (const auto& other_entry : other.m_entries) {
            const Strid& key = other_entry.first;

            if (m_entries.find(key) == m_entries.end() ||
                mode == ConfigStackMode::Overwrite) {
                m_entries[key] = other_entry.second;
            }
        }

        return StatusCode::Ok;
    }

    Status ConfigFile::load_and_stack(const std::string& path, ConfigStackMode mode) {
        ConfigFile config_file;

        if (!config_file.load(path)) {
            return StatusCode::FailedRead;
        }

        stack(config_file, mode);
        return StatusCode::Ok;
    }

    void ConfigFile::clear() {
        m_entries.clear();
    }

    bool ConfigFile::is_empty() {
        return m_entries.empty();
    }

    Status ConfigFile::set(const Strid& key, const bool& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return StatusCode::Ok;
        }
        return StatusCode::NoValue;
    }
    Status ConfigFile::set(const Strid& key, const int& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return StatusCode::Ok;
        }
        return StatusCode::NoValue;
    }
    Status ConfigFile::set(const Strid& key, const float& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return StatusCode::Ok;
        }
        return StatusCode::NoValue;
    }
    Status ConfigFile::set(const Strid& key, const std::string& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return StatusCode::Ok;
        }
        return StatusCode::NoValue;
    }

    Status ConfigFile::get(const Strid& key, bool& value) {
        Var* p_var;
        if (!get_element(key, p_var)) return StatusCode::NoValue;
        value = (*p_var).operator int();
        return StatusCode::Ok;
    }
    Status ConfigFile::get(const Strid& key, int& value) {
        Var* p_var;
        if (!get_element(key, p_var)) return StatusCode::NoValue;
        value = *p_var;
        return StatusCode::Ok;
    }
    Status ConfigFile::get(const Strid& key, float& value) {
        Var* p_var;
        if (!get_element(key, p_var)) return StatusCode::NoValue;
        value = *p_var;
        return StatusCode::Ok;
    }
    Status ConfigFile::get(const Strid& key, std::string& value) {
        Var* p_var;
        if (!get_element(key, p_var)) return StatusCode::NoValue;
        value = p_var->operator String();
        return StatusCode::Ok;
    }
    Status ConfigFile::get(const Strid& key, Color4f& value) {
        Var* p_var;
        if (!get_element(key, p_var)) return StatusCode::NoValue;
        value = Color::from_hex4(static_cast<unsigned int>(StringUtils::to_ulong(p_var->operator String(), 16)));
        return StatusCode::Ok;
    }

    bool ConfigFile::get_bool(const Strid& key, bool value) {
        get(key, value);
        return value;
    }
    int ConfigFile::get_int(const Strid& key, int value) {
        get(key, value);
        return value;
    }
    float ConfigFile::get_float(const Strid& key, float value) {
        get(key, value);
        return value;
    }
    std::string ConfigFile::get_string(const Strid& key, std::string value) {
        get(key, value);
        return value;
    }
    Color4f ConfigFile::get_color4f(const Strid& key, Color4f value) {
        get(key, value);
        return value;
    }

    Status ConfigFile::copy_to(Object& copy) const {
        Resource::copy_to(copy);
        auto config       = dynamic_cast<ConfigFile*>(&copy);
        config->m_entries = m_entries;
        return StatusCode::Ok;
    }

    bool ConfigFile::get_element(const Strid& key, Var*& element) {
        auto it_element = m_entries.find(key);
        if (it_element == m_entries.end()) return false;

        element = &it_element->second;
        return true;
    }

    void ConfigFile::register_class() {
        auto cls = Class::register_class<ConfigFile>();
        cls->add_method(ClassMethod(VarType::Int, SID("get_bool"), {SID("key")}), &ConfigFile::get_bool, {});
        cls->add_method(ClassMethod(VarType::Int, SID("get_int"), {SID("key")}), &ConfigFile::get_int, {});
        cls->add_method(ClassMethod(VarType::Float, SID("get_float"), {SID("key")}), &ConfigFile::get_float, {});
        cls->add_method(ClassMethod(VarType::String, SID("get_string"), {SID("key")}), &ConfigFile::get_string, {});
    }

}// namespace wmoge
