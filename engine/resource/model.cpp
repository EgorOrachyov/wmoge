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

#include "model.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, ModelLod& data) {
        WG_YAML_READ_AS_OPT(node, "mesh", data.mesh);
        WG_YAML_READ_AS_OPT(node, "materials", data.materials);
        WG_YAML_READ_AS_OPT(node, "screen_size", data.screen_size);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ModelLod& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "mesh", data.mesh);
        WG_YAML_WRITE_AS(node, "materials", data.materials);
        WG_YAML_WRITE_AS(node, "screen_size", data.screen_size);
        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, ModelLodSettings& data) {
        WG_YAML_READ_AS_OPT(node, "minimum_lod", data.minimum_lod);
        WG_YAML_READ_AS_OPT(node, "num_of_lods", data.num_of_lods);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ModelLodSettings& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS_OPT(node, "minimum_lod", data.minimum_lod.has_value(), data.minimum_lod);
        WG_YAML_WRITE_AS_OPT(node, "num_of_lods", data.num_of_lods.has_value(), data.num_of_lods);
        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, ModelFile& data) {
        WG_YAML_READ_AS_OPT(node, "materials", data.materials);
        WG_YAML_READ_AS_OPT(node, "lods", data.lods);
        WG_YAML_READ_AS_OPT(node, "lod_settings", data.lod_settings);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ModelFile& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "materials", data.materials);
        WG_YAML_WRITE_AS(node, "lods", data.lods);
        WG_YAML_WRITE_AS(node, "lod_settings", data.lod_settings);
        return StatusCode::Ok;
    }

    Status Model::read_from_yaml(const YamlConstNodeRef& node) {
        ModelFile model_file;
        WG_YAML_READ(node, model_file);

        m_materials    = std::move(model_file.materials);
        m_lods         = std::move(model_file.lods);
        m_lod_settings = std::move(model_file.lod_settings);

        update_aabb();

        return StatusCode::Ok;
    }
    Status Model::copy_to(Object& other) const {
        Resource::copy_to(other);
        auto* ptr           = dynamic_cast<Model*>(&other);
        ptr->m_materials    = m_materials;
        ptr->m_lods         = m_lods;
        ptr->m_lod_settings = m_lod_settings;
        ptr->m_aabb         = m_aabb;
        return StatusCode::Ok;
    }

    void Model::update_aabb() {
        m_aabb = Aabbf();

        for (const auto& lod : m_lods) {
            m_aabb = m_aabb.join(lod.mesh->get_aabb());
        }
    }

    void Model::register_class() {
        auto* cls = Class::register_class<Model>();
    }

}// namespace wmoge