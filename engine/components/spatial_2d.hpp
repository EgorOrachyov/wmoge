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

#ifndef WMOGE_SPATIAL_2D_HPP
#define WMOGE_SPATIAL_2D_HPP

#include "math/mat.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "scene/scene_component.hpp"

namespace wmoge {

    /**
     * @class Spatial2d
     * @brief 2d-space hierarchical transformation attached to the object
     */
    class Spatial2d : public SceneComponent {
    public:
        WG_OBJECT(Spatial2d, SceneComponent)

        void update_transform(const Transform2d& transform);
        void translate(const Vec2f& translation);
        void rotate(float angle_rad);
        void scale(const Vec2f& scale);
        void flush();

        Mat3x3f            get_matr_local();
        Mat3x3f            get_matr_local_inv();
        const Mat3x3f&     get_matr_global();
        const Mat3x3f&     get_matr_global_inv();
        const Transform2d& get_transform();

        void on_scene_enter() override;
        void on_transform_updated() override;
        bool on_load_from_yaml(const YamlConstNodeRef& node) override;

    private:
        Transform2d m_transform;
        Mat3x3f     m_matr_global     = Math2d::identity3x3();
        Mat3x3f     m_matr_global_inv = Math2d::identity3x3();
        Spatial2d*  m_parent          = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_SPATIAL_2D_HPP
