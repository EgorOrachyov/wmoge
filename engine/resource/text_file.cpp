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

#include "text_file.hpp"

#include "core/engine.hpp"
#include "platform/file_system.hpp"

#include <sstream>
#include <tinyxml2.hpp>

namespace wmoge {

    bool TextFile::load(const std::string& path, bool new_line, bool last_line) {
        std::vector<std::uint8_t> file;
        if (!Engine::instance()->file_system()->read_file(path, file)) {
            WG_LOG_ERROR("failed to read file " << path);
            return false;
        }

        tinyxml2::XMLDocument document;
        if (document.Parse(reinterpret_cast<const char*>(file.data()), file.size())) {
            WG_LOG_ERROR("failed to parse text file: " << path);
            return false;
        }

        auto root    = document.FirstChildElement("text_file");
        auto section = root->FirstChildElement("section");

        while (section) {
            std::stringstream lines;
            auto              line        = section->FirstChildElement("line");
            bool              loaded_line = false;

            while (line) {
                if (loaded_line && new_line) lines << "\n";
                lines << line->GetText();
                line        = line->NextSiblingElement("line");
                loaded_line = true;
            }
            if (loaded_line && last_line) lines << "\n";

            m_text_sections.emplace_back(lines.str());
            m_new_line  = new_line;
            m_last_line = last_line;
            section     = section->NextSiblingElement("section");
        }

        return true;
    }

    const std::vector<std::string>& TextFile::get_text_sections() {
        return m_text_sections;
    }
    const std::string& TextFile::get_text_section(int num) {
        return m_text_sections[num];
    }
    int TextFile::get_text_sections_count() {
        return static_cast<int>(m_text_sections.size());
    }

    bool TextFile::load_from_import_options(const YamlTree& tree) {
        std::string source_file;
        bool        new_line;
        bool        last_line;

        auto params = tree["params"];
        params["source_file"] >> source_file;
        params["new_line"] >> new_line;
        params["last_line"] >> last_line;

        return load(source_file, new_line, last_line);
    }
    void TextFile::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto text_file             = dynamic_cast<TextFile*>(&copy);
        text_file->m_text_sections = m_text_sections;
        text_file->m_new_line      = m_new_line;
        text_file->m_last_line     = m_last_line;
    }
    std::string TextFile::to_string() {
        std::stringstream ss;
        ss << "{Text file: " << get_name() << "}";
        return ss.str();
    }

    void TextFile::register_class() {
        auto cls = Class::register_class<TextFile>();
        cls->add_property(Property(VarType::Int, SID("text_sections_count"), SID("get_text_sections_count")));
        cls->add_method(Method(VarType::String, SID("get_text_section"), {SID("num")}), &TextFile::get_text_section, {});
        cls->add_method(Method(VarType::Int, SID("get_text_sections_count"), {}), &TextFile::get_text_sections_count, {});
    }

}// namespace wmoge
