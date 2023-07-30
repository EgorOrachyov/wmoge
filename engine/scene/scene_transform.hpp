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

#ifndef WMOGE_SCENE_TRANSFORM_HPP
#define WMOGE_SCENE_TRANSFORM_HPP

#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/transform.hpp"

namespace wmoge {

    /**
     * @class SceneTransform
     * @brief Single node in a hierarchy of a transformation
     */
    class SceneTransform : public RefCnt {
    public:
    private:
        fast_vector<Ref<SceneTransform>> m_children;
        SceneTransform*                  m_parent = nullptr;
        Transform3d                      m_transform;
        Mat4x4f                          m_l2w_cached = Math3d::identity();
        Mat4x4f                          m_w2l_cached = Math3d::identity();
        int                              m_layer      = -1;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TRANSFORM_HPP
