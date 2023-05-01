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

#ifndef WMOGE_RENDER_SPRITE_INSTANCE_HPP
#define WMOGE_RENDER_SPRITE_INSTANCE_HPP

#include "render/objects/render_canvas_item.hpp"
#include "resource/sprite.hpp"

namespace wmoge {

    /**
     * @class RenderSpriteInstance
     * @brief Render-system implementation of 2d animated sprite object
     */
    class RenderSpriteInstance final : public RenderCanvasItem {
    public:
        static constexpr const int N_VERTICES = 4;
        static constexpr const int N_INDICES  = 6;

        ~RenderSpriteInstance() override = default;

        void initialize(ref_ptr<Sprite> sprite, int animation, float speed_scale, bool playing);
        void set_animation(int animation);
        void set_playing(bool playing);

        void on_scene_enter() override;
        void on_update(float dt) override;
        void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) override;
        bool need_update() const override;
        bool need_render_dynamic() const override;

    protected:
        void rebuild_geom();
        void mark_dirty_geom();

    private:
        ref_ptr<Sprite> m_sprite;
        int             m_animation       = -1;
        int             m_frame           = -1;
        float           m_speed_scale     = 1.0f;
        float           m_frame_accum     = 0.0f;
        bool            m_playing         = false;
        bool            m_dirty_geom_data = true;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_SPRITE_INSTANCE_HPP
