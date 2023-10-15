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

#include "core/array_view.hpp"
#include "core/ref.hpp"
#include "core/synchronization.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class SceneTransform
     * @brief Single node in a hierarchy of a transformation
     */
    class SceneTransform final : public RefCnt {
    public:
        SceneTransform(class SceneTransformManager* manager);
        SceneTransform(const Ref<SceneTransform>& parent);
        ~SceneTransform() override;

        void set_layer(int layer);
        void set_lt(const Mat4x4f& matrix, const Mat4x4f& matrix_inverted);
        void set_wt(const Mat4x4f& matrix, const Mat4x4f& matrix_inverted);
        void add_child(const Ref<SceneTransform>& child);
        void remove_child(const Ref<SceneTransform>& child);

        void update(bool recursive = false);

        [[nodiscard]] ArrayView<const Ref<SceneTransform>> get_children() const { return m_children; }
        [[nodiscard]] SceneTransform*                      get_parent() const { return m_parent; }
        [[nodiscard]] const Mat4x4f&                       get_lt() const { return m_lt; }
        [[nodiscard]] const Mat4x4f&                       get_lt_inverted() const { return m_lt_inverted; }
        [[nodiscard]] const Mat4x4f&                       get_l2w_cached() const { return m_l2w_cached; }
        [[nodiscard]] const Mat4x4f&                       get_w2l_cached() const { return m_w2l_cached; }
        [[nodiscard]] std::optional<int>                   get_layer() const { return is_linked() ? std::optional{m_layer} : std::nullopt; }
        [[nodiscard]] bool                                 is_dirty() const { return m_dirty; }
        [[nodiscard]] bool                                 is_linked() const { return m_layer != -1; }

    private:
        std::vector<Ref<SceneTransform>> m_children;
        class SceneTransform*            m_parent      = nullptr;
        class SceneTransformManager*     m_manager     = nullptr;
        Mat4x4f                          m_lt          = Math3d::identity();
        Mat4x4f                          m_lt_inverted = Math3d::identity();
        Mat4x4f                          m_l2w_cached  = Math3d::identity();
        Mat4x4f                          m_w2l_cached  = Math3d::identity();
        int                              m_layer       = -1;
        bool                             m_dirty       = false;
    };

    /**
     * @class SceneTransformManager
     * @brief Manages layers of scene transforms for fast update
     */
    class SceneTransformManager final {
    public:
        ~SceneTransformManager();

        /** @brief Single horizonatl layer in a hierarchy of scene transforms */
        using Layer = std::vector<SceneTransform*>;

        void add(int layer, SceneTransform* transform);
        void remove(int layer, SceneTransform* transform);

        [[nodiscard]] ArrayView<Layer>           get_layers() { return m_layers; }
        [[nodiscard]] ArrayView<SceneTransform*> get_layer(int i) { return m_layers[i]; }
        [[nodiscard]] int                        get_num_layers() const { return int(m_layers.size()); }

    private:
        std::vector<Layer> m_layers;
        SpinMutex          m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TRANSFORM_HPP
