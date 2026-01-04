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

#include "asset_deps_collector.hpp"

#include "asset/asset_ref.hpp"
#include "core/string_utils.hpp"
#include "rtti/class.hpp"
#include "rtti/visitor.hpp"

#include <algorithm>

namespace wmoge {

    /** @brief Visitor implementation to collect deps asset refs from a root asset */
    class AssetDepsCollectorVisitor : public RttiVisitor {
    public:
        AssetDepsCollectorVisitor(flat_set<UUID>& asset_refs) : m_asset_refs(asset_refs) {}
        ~AssetDepsCollectorVisitor() override = default;

    protected:
        Status visit_asset_ref(const RttiTypeAssetRef* rtti, std::uint8_t* src) override {
            const Ref<RttiObject>& as_ptr = *((Ref<RttiObject>*) src);
            if (!as_ptr) {
                return WG_OK;
            }
            Ref<Asset> asset = as_ptr.cast<Asset>();
            if (asset->get_id()) {
                m_asset_refs.insert(asset->get_id());
            }
            return WG_OK;
        }

    private:
        flat_set<UUID>& m_asset_refs;
    };

    AssetDepsCollector::AssetDepsCollector(Ref<Asset> asset, std::string name)
        : m_asset(std::move(asset)),
          m_name(std::move(name)) {
    }

    Status AssetDepsCollector::collect() {
        AssetDepsCollectorVisitor visitor{m_asset_refs};
        return visitor.visit(m_asset->get_class(), (std::uint8_t*) m_asset.get());
    }

    std::vector<UUID> AssetDepsCollector::to_vector() const {
        std::vector<UUID> uuids(m_asset_refs.size());
        std::copy(m_asset_refs.begin(), m_asset_refs.end(), uuids.begin());
        return std::move(uuids);
    }

}// namespace wmoge