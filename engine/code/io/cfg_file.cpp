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

#include "cfg_file.hpp"

#include "core/data.hpp"
#include "core/string_utils.hpp"
#include "io/ini.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

#include <fstream>

namespace wmoge {

    Status CfgFile::load_from_file(FileSystem* file_system, const std::string& path) {
        WG_PROFILE_CPU_ASSET("CfgFile::load_from_file");

        std::string content;
        if (!file_system->read_file(path, content)) {
            WG_LOG_ERROR("failed to read config file from " << path);
            return StatusCode::FailedRead;
        }

        return load_from_content(content);
    }

    Status CfgFile::load_from_content(const std::string& content) {
        WG_PROFILE_CPU_ASSET("CfgFile::load_from_content");

        IniFile file;
        WG_CHECKED(file.parse(content));

        for (auto& entry : file.get_sections()) {
            IniSection& section = entry.second;

            for (auto& value : section.values) {
                Strid key(section.name + "." + value.first);
                m_entries[key] = std::move(value.second);
            }
        }

        return WG_OK;
    }

    Status CfgFile::stack(const CfgFile& other, ConfigStackMode mode) {
        WG_PROFILE_CPU_ASSET("CfgFile::stack");

        for (const auto& other_entry : other.m_entries) {
            const Strid& key = other_entry.first;

            if (m_entries.find(key) == m_entries.end() ||
                mode == ConfigStackMode::Overwrite) {
                m_entries[key] = other_entry.second;
            }
        }

        return WG_OK;
    }

    void CfgFile::clear() {
        m_entries.clear();
    }

    bool CfgFile::is_empty() {
        return m_entries.empty();
    }

    Status CfgFile::set_bool(const Strid& key, const bool& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return WG_OK;
        }
        return StatusCode::NoValue;
    }
    Status CfgFile::set_int(const Strid& key, const int& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return WG_OK;
        }
        return StatusCode::NoValue;
    }
    Status CfgFile::set_float(const Strid& key, const float& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return WG_OK;
        }
        return StatusCode::NoValue;
    }
    Status CfgFile::set_string(const Strid& key, const std::string& value, bool overwrite) {
        if (m_entries.find(key) == m_entries.end() || overwrite) {
            m_entries[key] = value;
            return WG_OK;
        }
        return StatusCode::NoValue;
    }

    Status CfgFile::get_bool(const Strid& key, bool& value) const {
        const Var* p_var;
        if (!get_value(key, p_var)) return StatusCode::NoValue;
        value = (*p_var).operator int();
        return WG_OK;
    }
    Status CfgFile::get_int(const Strid& key, int& value) const {
        const Var* p_var;
        if (!get_value(key, p_var)) return StatusCode::NoValue;
        value = *p_var;
        return WG_OK;
    }
    Status CfgFile::get_float(const Strid& key, float& value) const {
        const Var* p_var;
        if (!get_value(key, p_var)) return StatusCode::NoValue;
        value = *p_var;
        return WG_OK;
    }
    Status CfgFile::get_string(const Strid& key, std::string& value) const {
        const Var* p_var;
        if (!get_value(key, p_var)) return StatusCode::NoValue;
        value = p_var->operator String();
        return WG_OK;
    }
    Status CfgFile::get_color4f(const Strid& key, Color4f& value) const {
        const Var* p_var;
        if (!get_value(key, p_var)) return StatusCode::NoValue;
        value = Color::from_hex4(static_cast<unsigned int>(StringUtils::to_ulong(p_var->operator String(), 16)));
        return WG_OK;
    }

    bool CfgFile::get_bool_or_default(const Strid& key, bool value) const {
        get_bool(key, value);
        return value;
    }
    int CfgFile::get_int_or_default(const Strid& key, int value) const {
        get_int(key, value);
        return value;
    }
    float CfgFile::get_float_or_default(const Strid& key, float value) const {
        get_float(key, value);
        return value;
    }
    std::string CfgFile::get_string_or_default(const Strid& key, std::string value) const {
        get_string(key, value);
        return value;
    }
    Color4f CfgFile::get_color4f_or_default(const Strid& key, Color4f value) const {
        get_color4f(key, value);
        return value;
    }

    bool CfgFile::get_value(const Strid& key, Var*& element) {
        auto it_element = m_entries.find(key);
        if (it_element == m_entries.end()) return false;

        element = &it_element->second;
        return true;
    }
    bool CfgFile::get_value(const Strid& key, const Var*& element) const {
        auto it_element = m_entries.find(key);
        if (it_element == m_entries.end()) return false;

        element = &it_element->second;
        return true;
    }

}// namespace wmoge
