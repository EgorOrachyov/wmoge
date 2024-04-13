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

#include "debug/profiler.hpp"

namespace wmoge {

    WG_IO_BEGIN(ModelObj)
    WG_IO_FIELD(material)
    WG_IO_FIELD(mesh_idx)
    WG_IO_FIELD(chunk_idx)
    WG_IO_FIELD_OPT(flags)
    WG_IO_FIELD_OPT(name)
    WG_IO_END(ModelObj)

    WG_IO_BEGIN(ModelLod)
    WG_IO_FIELD_OPT(ranges)
    WG_IO_END(ModelLod)

    WG_IO_BEGIN(ModelLodSettings)
    WG_IO_FIELD_OPT(area)
    WG_IO_FIELD(minimum_lod)
    WG_IO_FIELD(num_of_lods)
    WG_IO_END(ModelLodSettings)

    WG_IO_BEGIN(ModelFile)
    WG_IO_FIELD(objs)
    WG_IO_FIELD(meshes)
    WG_IO_FIELD_OPT(lod)
    WG_IO_FIELD_OPT(lod_settings)
    WG_IO_FIELD_OPT(aabb)
    WG_IO_END(ModelFile)

    void Model::update_aabb() {
        m_aabb = Aabbf();

        for (const auto& mesh : m_meshes) {
            m_aabb = m_aabb.join(mesh->get_aabb());
        }
    }

    Status Model::read_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_ASSET("Model::read_from_yaml");

        ModelFile model_file;
        WG_YAML_READ(node, model_file);

        m_objs         = std::move(model_file.objs);
        m_meshes       = std::move(model_file.meshes);
        m_lod          = std::move(model_file.lod);
        m_lod_settings = std::move(model_file.lod_settings);
        m_aabb         = std::move(model_file.aabb);

        update_aabb();

        return StatusCode::Ok;
    }
    Status Model::copy_to(Object& other) const {
        Asset::copy_to(other);
        auto* ptr           = dynamic_cast<Model*>(&other);
        ptr->m_objs         = m_objs;
        ptr->m_lod          = m_lod;
        ptr->m_lod_settings = m_lod_settings;
        ptr->m_aabb         = m_aabb;
        return StatusCode::Ok;
    }

    void Model::register_class() {
        auto* cls = Class::register_class<Model>();
    }

}// namespace wmoge