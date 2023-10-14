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

#ifndef WMOGE_RENDER_MESH_STATIC_HPP
#define WMOGE_RENDER_MESH_STATIC_HPP

#include "core/fast_vector.hpp"
#include "math/math_utils3d.hpp"
#include "render/render_object.hpp"
#include "resource/material.hpp"
#include "resource/mesh.hpp"
#include "resource/model.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class RenderMeshStatic 
     * @brief Static renderable mesh
     */
    class RenderMeshStatic : public RenderObject {
    public:
        RenderMeshStatic(Ref<Model> model);

        void                         collect(const RenderCameras& cameras, RenderCameraMask mask, MeshBatchCollector& collector) override;
        void                         update_transform(const Mat4x4f& l2w) override;
        bool                         has_materials() const override;
        std::optional<Ref<Material>> get_material() const override;
        std::vector<Ref<Material>>   get_materials() const override;

    private:
        Ref<Model> m_model;
        Mat4x4f    m_transform_l2w = Math3d::identity();
    };

}// namespace wmoge

#endif//WMOGE_RENDER_MESH_STATIC_HPP