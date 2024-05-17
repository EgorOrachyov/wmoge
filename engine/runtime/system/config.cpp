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

#include "config.hpp"

namespace wmoge {

    Status Config::load(const std::string& path, ConfigStackMode mode) {
        return m_file->load_and_stack(path, mode);
    }

    Status Config::set_bool(const Strid& key, const bool& value, bool overwrite) {
        return m_file->set_bool(key, value, overwrite);
    }

    Status Config::set_int(const Strid& key, const int& value, bool overwrite) {
        return m_file->set_int(key, value, overwrite);
    }

    Status Config::set_float(const Strid& key, const float& value, bool overwrite) {
        return m_file->set_float(key, value, overwrite);
    }

    Status Config::set_string(const Strid& key, const std::string& value, bool overwrite) {
        return m_file->set_string(key, value, overwrite);
    }

    Status Config::get_bool(const Strid& key, bool& value) const {
        return m_file->get_bool(key, value);
    }

    Status Config::get_int(const Strid& key, int& value) const {
        return m_file->get_int(key, value);
    }

    Status Config::get_float(const Strid& key, float& value) const {
        return m_file->get_float(key, value);
    }

    Status Config::get_string(const Strid& key, std::string& value) const {
        return m_file->get_string(key, value);
    }

    Status Config::get_color4f(const Strid& key, Color4f& value) const {
        return m_file->get_color4f(key, value);
    }

    bool Config::get_bool_or_default(const Strid& key, bool def_value) const {
        return m_file->get_bool_or_default(key, def_value);
    }

    int Config::get_int_or_default(const Strid& key, int def_value) const {
        return m_file->get_int_or_default(key, def_value);
    }

    float Config::get_float_or_default(const Strid& key, float def_value) const {
        return m_file->get_float_or_default(key, def_value);
    }

    std::string Config::get_string_or_default(const Strid& key, std::string def_value) const {
        return m_file->get_string_or_default(key, def_value);
    }

    Color4f Config::get_color4f_or_default(const Strid& key, Color4f def_value) const {
        return m_file->get_color4f_or_default(key, def_value);
    }

}// namespace wmoge