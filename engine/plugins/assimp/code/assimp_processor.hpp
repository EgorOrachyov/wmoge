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

#include "core/array_view.hpp"
#include "core/string_id.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_builder.hpp"
#include "mesh/mesh_import_settings.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace wmoge {

    /**
     * @class AssimpProcessorBase
     * @brief Assimp base processor for file importing
    */
    class AssimpProcessorBase {
    public:
        AssimpProcessorBase() = default;

        Status read(std::string file_name, array_view<const std::uint8_t> data, const MeshImportProcess& flags);
        Status process();

        [[nodiscard]] const MeshImportProcess& get_flags() const { return m_flags; }
        [[nodiscard]] const std::string&       get_file_name() const { return m_file_name; }
        [[nodiscard]] int                      get_next_mesh_id() const { return m_next_mesh_id; }

    protected:
        virtual Status process_node(aiNode* node, const Mat4x4f& parent_transform, const Mat4x4f& inv_parent_transform, std::optional<int> parent);
        virtual Status process_mesh(aiMesh* mesh, const Mat4x4f& transform, const Mat4x4f& inv_transform, std::optional<int> parent) { return WG_OK; }

    private:
        Assimp::Importer  m_importer;
        MeshImportProcess m_flags;
        std::string       m_file_name;
        unsigned int      m_options      = 0;
        const aiScene*    m_scene        = nullptr;
        int               m_next_mesh_id = 0;
    };

    /**
     * @class AssimpMeshImporter
     * @brief Assimp mesh processor
    */
    class AssimpProcessorMesh : public AssimpProcessorBase {
    public:
        AssimpProcessorMesh() = default;

        void         set_attribs(GfxVertAttribs attribs) { m_attribs = attribs; }
        MeshBuilder& get_builder() { return m_builder; }

    protected:
        Status process_mesh(aiMesh* mesh, const Mat4x4f& transform, const Mat4x4f& inv_transform, std::optional<int> parent) override;

    protected:
        GfxVertAttribs m_attribs;
        MeshBuilder    m_builder;
    };

}// namespace wmoge