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

#include "scene_transform.hpp"

#include <cassert>

namespace wmoge {

    SceneTransform::SceneTransform(class SceneTransformManager* manager) {
        assert(manager);

        m_manager = manager;
    }

    SceneTransform::SceneTransform(const Ref<SceneTransform>& parent) {
        assert(parent);
        assert(parent->m_manager);

        m_manager = parent->m_manager;
        parent->add_child(Ref<SceneTransform>(this));
    }

    SceneTransform::~SceneTransform() {
        set_layer(-1);
    }

    void SceneTransform::set_layer(int layer) {
        assert(layer == -1 || layer >= 0);

        if (layer != m_layer) {
            if (m_layer != -1) {
                m_manager->remove(m_layer, this);
            }

            m_layer = layer;

            if (m_layer != -1) {
                m_manager->add(m_layer, this);
            }

            for (auto& child : m_children) {
                child->set_layer(is_linked() ? m_layer + 1 : -1);
            }
        }
    }

    void SceneTransform::set_lt(const Mat4x4f& matrix, const Mat4x4f& matrix_inverted) {
        m_lt          = matrix;
        m_lt_inverted = matrix_inverted;
        m_dirty       = true;
    }
    void SceneTransform::set_wt(const Mat4x4f& matrix, const Mat4x4f& matrix_inverted) {
        m_lt          = matrix;
        m_lt_inverted = matrix_inverted;
        m_dirty       = true;

        if (m_parent) {
            // matrix          ~ l2w =   parent->l2w_cached  * lt
            // matrix_inverted ~ w2l =   lt_inverted         * parent->w2l_cached

            m_lt          = m_parent->m_w2l_cached * matrix;
            m_lt_inverted = matrix * m_parent->m_l2w_cached;
        }
    }

    void SceneTransform::add_child(const Ref<SceneTransform>& child) {
        assert(child);
        assert(child->m_parent == nullptr);
        assert(child->m_manager == m_manager);
        assert(child.get() != this);

        m_children.push_back(child);
        child->m_parent = this;
        child->m_dirty  = true;
        child->update(true);
        child->set_layer(is_linked() ? m_layer + 1 : -1);
    }

    void SceneTransform::remove_child(const Ref<SceneTransform>& child) {
        assert(child);
        assert(child->m_parent == this);

        m_children.erase(std::find(m_children.begin(), m_children.end(), child));
        child->m_parent = nullptr;
        child->m_dirty  = true;
        child->update(true);
        child->set_layer(-1);
    }

    void SceneTransform::update(bool recursive) {
        if (m_dirty) {
            m_l2w_cached = m_lt;
            m_w2l_cached = m_lt_inverted;

            if (m_parent) {
                m_l2w_cached = m_parent->m_l2w_cached * m_l2w_cached;
                m_w2l_cached = m_w2l_cached * m_parent->m_w2l_cached;
            }

            if (!recursive) {
                for (auto& child : m_children) {
                    child->m_dirty = true;
                }
            }

            if (recursive) {
                for (auto& child : m_children) {
                    child->update(recursive);
                }
            }

            m_dirty = false;
        }
    }

    void SceneTransformManager::add(int layer, SceneTransform* transform) {
        std::lock_guard guard(m_mutex);

        assert(transform);

        if (layer >= m_layers.size()) {
            m_layers.resize(layer + 1);
        }

        m_layers[layer].push_back(transform);
    }

    void SceneTransformManager::remove(int layer, SceneTransform* transform) {
        std::lock_guard guard(m_mutex);

        assert(layer < m_layers.size());
        assert(transform);
        assert(std::find(m_layers[layer].begin(), m_layers[layer].end(), transform) != m_layers[layer].end());

        m_layers[layer].erase(std::find(m_layers[layer].begin(), m_layers[layer].end(), transform));
    }

    SceneTransformManager::~SceneTransformManager() {
        for (auto& layer : m_layers) {
            assert(layer.empty());
        }
    }

}// namespace wmoge