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

#ifndef WMOGE_CANVAS_LAYER_HPP
#define WMOGE_CANVAS_LAYER_HPP

#include "math/vec.hpp"
#include "scene/scene_component.hpp"

namespace wmoge {

    /**
     * @class CanvasLayer
     * @brief Canvas drawing layer
     *
     * Canvas items that are children of this layer will be draw inside
     * this layer. The layer itself is a numeric index that defines the order
     * of drawing. The engine provides total 16 induced for layers. The layer
     * with the lowes index is shown on the top. The layer with the larges index
     * is shown on the bottom.
     *
     * @note There must be a unique relation ship between canvas layer and used
     *       index. A unique index can be used only for single layer.
     *
     * @see Camera2d
     * @see CanvasItem
     */
    class CanvasLayer : public SceneComponent {
    public:
        WG_OBJECT(CanvasLayer, SceneComponent);

        void  hide();
        void  show();
        void  set_layer_id(int id);
        Vec2f get_offset() const;
        float get_rotation() const;
        int   get_id() const;
        bool  get_is_visible() const;

        bool on_load_from_yaml(const YamlConstNodeRef& node) override;

    private:
        Vec2f m_offset;
        float m_rotation   = 0.0f;
        int   m_id         = 0;
        bool  m_is_visible = true;
    };

}// namespace wmoge

#endif//WMOGE_CANVAS_LAYER_HPP
