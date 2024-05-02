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

#include "asset.hpp"

#include "core/class.hpp"
#include "system/engine.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace wmoge {

    Status yaml_read(IoContext& context, YamlConstNodeRef node, AssetId& id) {
        WG_YAML_READ(context, node, id.m_name);
        return StatusCode::Ok;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const AssetId& id) {
        WG_YAML_WRITE(context, node, id.m_name);
        return StatusCode::Ok;
    }
    Status archive_read(IoContext& context, Archive& archive, AssetId& id) {
        WG_ARCHIVE_READ(context, archive, id.m_name);
        return StatusCode::Ok;
    }
    Status archive_write(IoContext& context, Archive& archive, const AssetId& id) {
        WG_ARCHIVE_WRITE(context, archive, id.m_name);
        return StatusCode::Ok;
    }

    AssetId::AssetId(const std::string& id) {
        m_name = SID(id);
    }
    AssetId::AssetId(const Strid& id) {
        m_name = id;
    }

    void AssetDependencies::set_mode(CollectionMode mode, std::optional<int> num_levels) {
        assert(m_cur_depth == 0);

        if (mode == CollectionMode::OneLevel) {
            m_max_depth = 1;
        }
        if (mode == CollectionMode::MultipleLevels) {
            m_max_depth = num_levels.value_or(1);
        }
        if (mode == CollectionMode::FullDepth) {
            m_max_depth = std::numeric_limits<int>::max();
        }

        m_mode = mode;
    }

    void AssetDependencies::add(const Ref<Asset>& asset) {
        if (m_cur_depth >= m_max_depth) {
            return;
        }
        if (!asset) {
            return;
        }

        m_cur_depth += 1;

        m_assets.emplace(asset);
        asset->collect_deps(*this);

        m_cur_depth -= 1;
    }

    buffered_vector<Ref<Asset>> AssetDependencies::to_vector() const {
        buffered_vector<Ref<Asset>> vec(m_assets.size());
        std::copy(m_assets.begin(), m_assets.end(), vec.begin());
        return vec;
    }

}// namespace wmoge