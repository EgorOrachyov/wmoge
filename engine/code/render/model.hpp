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
#include "asset/asset_ref.hpp"
#include "core/buffered_vector.hpp"
#include "io/serialization.hpp"
#include "material/material.hpp"
#include "math/aabb.hpp"
#include "mesh/mesh.hpp"

#include <cinttypes>
#include <optional>

namespace wmoge {

    /** @brief Model obj flag */
    enum class ModelObjFlag {
    };

    /** @brief Flags to configure model obj */
    using ModelObjFlags = Mask<ModelObjFlag>;

    /**
     * @class ModelObj
     * @brief Serializable struct to hold single renderable model obj
     */
    struct ModelObj {
        std::int16_t  mesh_idx     = 0;
        std::int16_t  material_idx = 0;
        std::int16_t  chunk_idx    = 0;
        ModelObjFlags flags;
        Strid         name;
    };

    /**
     * @class ModelLod
     * @brief Serializable struct to hold single lod params
     */
    struct ModelLod {
        std::vector<Size2i> ranges;
    };

    /**
     * @class ModelLodSettings
     * @brief Serializable struct to hold model lods settings
     */
    struct ModelLodSettings {
        std::vector<float> area;
        std::optional<int> minimum_lod;
        std::optional<int> num_of_lods;
    };

    /**
     * @class ModelDesc
     * @brief Serializable struct to hold model info
     */
    struct ModelDesc {
        WG_RTTI_STRUCT(ModelDesc)

        std::vector<ModelObj>           objs;
        std::vector<AssetRef<Mesh>>     meshes;
        std::vector<AssetRef<Material>> materials;
        ModelLod                        lod;
        ModelLodSettings                lod_settings;
        Aabbf                           aabb;
    };

    WG_RTTI_STRUCT_BEGIN(ModelDesc) {
    }
    WG_RTTI_END;

    /**
     * @class Model
     * @brief Asset which stores a complete textured mesh model with level of details and other settings
     * 
     * Model incapsulates complete setup of a geometry, required for a runtime high-quality rendering.
     * It stores level of details, each of them has own mesh and materials setup.
     * Model can be used to setup static or skinned mesh renderer in a scene.
     * 
     * @see Material
     * @see Shader
     * @see Mesh
     */
    class Model final : public Asset {
    public:
        WG_RTTI_CLASS(Model, Asset);

        Model()           = default;
        ~Model() override = default;

        void update_aabb();

        [[nodiscard]] array_view<ModelObj>    get_objs() { return m_objs; }
        [[nodiscard]] const ModelLodSettings& get_lod_settings() const { return m_lod_settings; }
        [[nodiscard]] const Aabbf&            get_aabb() const { return m_aabb; }

    private:
        buffered_vector<ModelObj>       m_objs;
        buffered_vector<AssetRef<Mesh>> m_meshes;
        ModelLod                        m_lod;
        ModelLodSettings                m_lod_settings;
        Aabbf                           m_aabb;
    };

    WG_RTTI_CLASS_BEGIN(Model) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge