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

#include "core/fast_vector.hpp"
#include "io/yaml.hpp"
#include "math/aabb.hpp"
#include "resource/material.hpp"
#include "resource/mesh.hpp"
#include "resource/model.hpp"
#include "resource/resource.hpp"
#include "resource/resource_ref.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class ModelLod
     * @brief Serializable struct to hold single drawable model mesh chunk info
     */
    struct ModelChunk {
        ResRef<Material> material;

        friend Status yaml_read(const YamlConstNodeRef& node, ModelChunk& data);
        friend Status yaml_write(YamlNodeRef node, const ModelChunk& data);
    };

    /**
     * @class ModelLod
     * @brief Serializable struct to hold single lod params
     */
    struct ModelLod {
        fast_vector<ModelChunk>     chunks;
        ResRef<Mesh>                mesh;
        std::optional<ResRef<Mesh>> shadow_mesh;

        friend Status yaml_read(const YamlConstNodeRef& node, ModelLod& data);
        friend Status yaml_write(YamlNodeRef node, const ModelLod& data);
    };

    /**
     * @class ModelLodSettings
     * @brief Serializable struct to hold model lods settings
     */
    struct ModelLodSettings {
        std::optional<int> minimum_lod;
        std::optional<int> num_of_lods;

        friend Status yaml_read(const YamlConstNodeRef& node, ModelLodSettings& data);
        friend Status yaml_write(YamlNodeRef node, const ModelLodSettings& data);
    };

    /**
     * @class ModelFile
     * @brief Serializable struct to hold model info
     */
    struct ModelFile {
        fast_vector<ModelLod, 1>    lods;
        ModelLodSettings            lod_settings;
        std::optional<ResRef<Mesh>> collision_mesh;

        friend Status yaml_read(const YamlConstNodeRef& node, ModelFile& data);
        friend Status yaml_write(YamlNodeRef node, const ModelFile& data);
    };

    /**
     * @class Model
     * @brief Resource which stores a complete textured mesh model with level of details and other settings
     * 
     * Model incapsulates complete setup of a geometry, required for a runtime high-quality rendering.
     * It stores level of details, each of them has own mesh and materials setup.
     * Model can be used to setup static or skinned mesh renderer in a scene.
     * 
     * @see Material
     * @see Shader
     * @see Mesh
     */
    class Model final : public Resource {
    public:
        WG_OBJECT(Model, Resource)

        void update_aabb();

        [[nodiscard]] ArrayView<const ModelLod> get_lods() const { return m_lods; }
        [[nodiscard]] const ModelLodSettings&   get_lod_settings() const { return m_lod_settings; }
        [[nodiscard]] const Aabbf&              get_aabb() const { return m_aabb; }

        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status copy_to(Object& other) const override;

    private:
        fast_vector<ModelLod, 1> m_lods;
        ModelLodSettings         m_lod_settings;
        Aabbf                    m_aabb;
    };

}// namespace wmoge