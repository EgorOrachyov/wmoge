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

#include "asset_resolver.hpp"

namespace wmoge {

    void AssetResolver::add(std::string path, UUID uuid) {
        std::lock_guard guard(m_mutex);
        m_uuid_to_path[uuid]            = path;
        m_path_to_uuid[std::move(path)] = uuid;
    }

    void AssetResolver::remove(const std::string& path) {
        std::lock_guard guard(m_mutex);
        m_path_to_uuid.erase(path);
    }

    std::optional<UUID> AssetResolver::resolve(const std::string& path) {
        std::lock_guard guard(m_mutex);
        auto            query = m_path_to_uuid.find(path);
        if (query != m_path_to_uuid.end()) {
            return query->second;
        }
        return std::nullopt;
    }

    std::optional<std::string> AssetResolver::resolve(UUID uuid) {
        std::lock_guard guard(m_mutex);
        auto            query = m_uuid_to_path.find(uuid);
        if (query != m_uuid_to_path.end()) {
            return query->second;
        }
        return std::nullopt;
    }

}// namespace wmoge