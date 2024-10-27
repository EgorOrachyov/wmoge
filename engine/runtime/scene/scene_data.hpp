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

#include "asset/asset.hpp"
#include "core/string_id.hpp"
#include "rtti/traits.hpp"
#include "scene/scene_feature.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class EntityDesc
     * @brief Struct describing scene entity
     */
    struct EntityDesc {
        WG_RTTI_STRUCT(EntityDesc)

        UUID                            uuid;
        std::string                     name;
        std::vector<Ref<EntityFeature>> features;
    };

    WG_RTTI_STRUCT_BEGIN(EntityDesc) {
        WG_RTTI_FIELD(uuid, {});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(features, {});
    }
    WG_RTTI_END;

    /**
     * @class SceneData
     * @brief Sctuct storing scene raw data
     */
    struct SceneData {
        WG_RTTI_STRUCT(SceneData)

        Strid                   name;
        std::vector<EntityDesc> entities;
    };

    WG_RTTI_STRUCT_BEGIN(SceneData) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(entities, {});
    }
    WG_RTTI_END;

    /**
     * @class SceneDataAsset
     * @brief Represents scene data as asset which can be saved and loaded from disc
     */
    class SceneDataAsset : public Asset {
    public:
        WG_RTTI_CLASS(SceneDataAsset, Asset)

        [[nodiscard]] const SceneData& get_data() const { return m_data; }

    private:
        SceneData m_data;
    };

    WG_RTTI_CLASS_BEGIN(SceneDataAsset) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_data, {});
    }
    WG_RTTI_END;

}// namespace wmoge