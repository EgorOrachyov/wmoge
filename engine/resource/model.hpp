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

#ifndef WMOGE_MODEL_HPP
#define WMOGE_MODEL_HPP

#include "core/fast_vector.hpp"
#include "io/yaml.hpp"
#include "math/aabb.hpp"
#include "resource/material.hpp"
#include "resource/mesh.hpp"
#include "resource/model.hpp"
#include "resource/resource.hpp"
#include "resource/resource_ref.hpp"

namespace wmoge {

    /**
     * @class ModelLod
     * @brief Serializable struct to hold single lod params
     */
    struct ModelLod {
        ResRef<Mesh>        mesh;
        fast_vector<int, 1> materials;
        float               screen_size = 1.0f;

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
        fast_vector<ResRef<Material>, 1> materials;
        fast_vector<ModelLod, 1>         lods;
        ModelLodSettings                 lod_settings;

        friend Status yaml_read(const YamlConstNodeRef& node, ModelFile& data);
        friend Status yaml_write(YamlNodeRef node, const ModelFile& data);
    };

    /**
     * @class Model
     * @brief Resource which stores a complete textured mesh model with level of details and other settings
     * 
     * Model incapsulates complete setup of a geometry, required for a runtime high-quality rendering.
     * It tores level of details, each of them has own mesh and materials setup.
     * Shared list of materials is also stored. This list can be used accross all lods' meshes' chunks.
     * Model can be used to setup static or skinned mesh renderer in a scene.
     * 
     * @see Material
     * @see Shader
     * @see Mesh
     */
    class Model final : public Resource {
    public:
        WG_OBJECT(Model, Resource)

        [[nodiscard]] ArrayView<const ResRef<Material>> get_materials() const { return m_materials; }
        [[nodiscard]] ArrayView<const ModelLod>         get_lods() const { return m_lods; }
        [[nodiscard]] const ModelLodSettings&           get_lod_settings() const { return m_lod_settings; }
        [[nodiscard]] const Aabbf&                      get_aabb() const { return m_aabb; }

        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status copy_to(Object& other) const override;

    private:
        void update_aabb();

    private:
        fast_vector<ResRef<Material>, 1> m_materials;
        fast_vector<ModelLod, 1>         m_lods;
        ModelLodSettings                 m_lod_settings;
        Aabbf                            m_aabb;
    };

}// namespace wmoge

#endif//WMOGE_MODEL_HPP