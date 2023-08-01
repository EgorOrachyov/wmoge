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

#ifndef WMOGE_SCENE_NODE_HPP
#define WMOGE_SCENE_NODE_HPP

#include "core/array_view.hpp"
#include "core/class.hpp"
#include "core/object.hpp"
#include "core/uuid.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "io/yaml.hpp"
#include "math/transform.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class SceneNode
     * @brief Represents single node (object) in an editable tree hierarchy of scene objects
     */
    class SceneNode : public Object {
    public:
        WG_OBJECT(SceneNode, Object)

        SceneNode(class SceneTree* tree);

        virtual void on_parented() {}
        virtual void on_unparented() {}
        virtual void on_transformed() {}
        virtual void on_renamed() {}
        virtual bool on_yaml_read(const YamlConstNodeRef& node);
        virtual bool on_yaml_write(YamlNodeRef node);

        void set_transform(const TransformEdt& transform);
        void set_name(const StringId& name);

        void add_child(const Ref<SceneNode>& child);
        void remove_child(const Ref<SceneNode>& child);

        [[nodiscard]] Mat4x4f get_lt() const;
        [[nodiscard]] Mat4x4f get_lt_inverse() const;
        [[nodiscard]] Mat4x4f get_l2w() const;
        [[nodiscard]] Mat4x4f get_w2l() const;

        [[nodiscard]] ArrayView<const Ref<SceneNode>> get_children() const { return m_children; }
        [[nodiscard]] class SceneNode*                get_parent() const { return m_parent; }
        [[nodiscard]] class SceneTree*                get_tree() const { return m_tree; }
        [[nodiscard]] const TransformEdt&             get_transform() const { return m_transform; }
        [[nodiscard]] const StringId&                 get_name() const { return m_name; }
        [[nodiscard]] const UUID&                     get_uuid() const { return m_uuid; };

    private:
        std::vector<Ref<SceneNode>> m_children;
        class SceneNode*            m_parent = nullptr;
        class SceneTree*            m_tree   = nullptr;
        TransformEdt                m_transform;
        StringId                    m_name;
        UUID                        m_uuid;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_NODE_HPP
