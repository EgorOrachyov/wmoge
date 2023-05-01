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

#ifndef WMOGE_TEXT_FILE_HPP
#define WMOGE_TEXT_FILE_HPP

#include "core/string_id.hpp"
#include "resource/resource.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class TextFile
     * @brief Simple text file resource
     */
    class TextFile final : public Resource {
    public:
        WG_OBJECT(TextFile, Resource);

        /**
         * @brief Load text file resource using provided path
         *
         * @param path Path to file in file system
         * @param new_line Add new line on section line loadings
         * @param last_line Add new line after last section line
         *
         * @return True if successfully loaded
         */
        bool load(const std::string& path, bool new_line = false, bool last_line = false);

        const std::vector<std::string>& get_text_sections();
        const std::string&              get_text_section(int num);
        int                             get_text_sections_count();

        bool        load_from_import_options(const YamlTree& tree) override;
        void        copy_to(Resource& copy) override;
        std::string to_string() override;

    private:
        std::vector<std::string> m_text_sections;
        bool                     m_new_line  = false;
        bool                     m_last_line = false;
    };

}// namespace wmoge

#endif//WMOGE_TEXT_FILE_HPP
