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

#pragma once

#include "io/context.hpp"

#include <mutex>
#include <string>
#include <vector>

namespace wmoge {
    class FileSystem;
    class AssetDb;
    class AssetImportManager;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetMonitor
     * @brief Monitors for a source directory and brings to the engine new or updated assets from sources
     */
    class AssetMonitor {
    public:
        AssetMonitor(IoContext           io_context,
                     FileSystem*         file_system,
                     AssetDb*            asset_db,
                     AssetImportManager* asset_import_manager);

    private:
        std::vector<std::string> m_directories;
        IoContext                m_io_context;
        FileSystem*              m_file_system          = nullptr;
        AssetDb*                 m_asset_db             = nullptr;
        AssetImportManager*      m_asset_import_manager = nullptr;
        mutable std::mutex       m_mutex;
    };

}// namespace wmoge