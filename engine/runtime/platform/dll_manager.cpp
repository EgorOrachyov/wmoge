#include "dll_manager.hpp"
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

#include "dll_manager.hpp"

#include "platform/file_system.hpp"
#include "system/ioc_container.hpp"

#include <dynalo/dynalo.hpp>

namespace wmoge {

    struct DllLibrary {
        Strid                  name;
        std::string            name_native;
        std::filesystem::path  path;
        dynalo::native::handle handle{};
    };

    DllManager::DllManager() = default;

    DllManager::~DllManager() {
        for (auto& lib : m_libraries) {
            dynalo::close(lib->handle);
        }
    };

    Status wmoge::DllManager::load(const Strid& library, const std::string& path) {
        if (is_loaded(library)) {
            return StatusCode::InvalidState;
        }

        FileSystem* fs = IocContainer::iresolve_v<FileSystem>();

        DllLibrary dll_library{};
        dll_library.name        = library;
        dll_library.name_native = dynalo::to_native_name(library.str());
        dll_library.path        = std::filesystem::path(fs->resolve_physical(path)) / dll_library.name_native;

        try {
            dll_library.handle = dynalo::open(dll_library.path.string());
        } catch (const std::runtime_error&) {
            return StatusCode::FailedLoadLibrary;
        }

        m_libraries.push_back(std::make_unique<DllLibrary>(std::move(dll_library)));

        return WG_OK;
    }

    Status DllManager::load_symbol(const Strid& library, const std::string& symbol_name, void*& addr) {
        for (auto& lib : m_libraries) {
            if (lib->name == library) {
                try {
                    addr = (void*) dynalo::get_function<void()>(lib->handle, symbol_name);
                } catch (const std::runtime_error&) {
                    return StatusCode::FailedLoadSymbol;
                }
            }
        }

        return StatusCode::InvalidState;
    }

    bool DllManager::is_loaded(const Strid& library) {
        for (auto& lib : m_libraries) {
            if (lib->name == library) {
                return true;
            }
        }

        return false;
    }

}// namespace wmoge
