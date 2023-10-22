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

#ifndef WMOGE_RENDER_OBJECT_HPP
#define WMOGE_RENDER_OBJECT_HPP

#include "core/array_view.hpp"
#include "core/mask.hpp"
#include "core/string_id.hpp"
#include "core/synchronization.hpp"
#include "math/aabb.hpp"
#include "math/mat.hpp"
#include "mesh/mesh_batch.hpp"
#include "render/render_camera.hpp"
#include "render/render_scene.hpp"
#include "resource/material.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class RenderObject 
     * @brief Base class for any scene object which can be rendered
     */
    class RenderObject {
    public:
        virtual ~RenderObject() = default;

        virtual void                         collect(const RenderCameras& cameras, RenderCameraMask mask, MeshBatchCollector& collector) = 0;
        virtual void                         procces_visibility(RenderCameraMask mask, float distance)                                   = 0;
        virtual bool                         has_materials() const                                                                       = 0;
        virtual std::optional<Ref<Material>> get_material() const                                                                        = 0;
        virtual std::vector<Ref<Material>>   get_materials() const                                                                       = 0;
        virtual Aabbf                        get_aabb() const                                                                            = 0;
        virtual StringId                     get_friendly_name() const;
        virtual int                          get_primitive_id() const;
        virtual void                         set_friendly_name(StringId name);
        virtual void                         set_primitive_id(int id);

    protected:
        StringId m_name;
        int      m_primitive_id = -1;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_OBJECT_HPP