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

#include "core/synchronization.hpp"
#include "gfx/gfx_vector.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "grc/font.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"

#include <deque>
#include <memory>
#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class AuxDrawManager
     * @brief Utility-class for rendering debug geometry and text
     *
     * Aux draw manager provides a low-level possibility to draw basic primitives, shapes,
     * screen and world text for debug purposes onto final rendered image. Also supports
     * persistent primitives with desired life-time.
     *
     * @note thread-safe
     */
    class AuxDrawManager final {
    public:
        static constexpr float LIFETIME_ONE_FRAME = 0.0f;
        static constexpr float LIFETIME_SMALL     = 10.0f;
        static constexpr float LIFETIME_INFINITY  = 2000000.0f;

        AuxDrawManager();
        ~AuxDrawManager();

        void draw_line(const Vec3f& from, const Vec3f& to, const Color4f& color, float lifetime = LIFETIME_ONE_FRAME);
        void draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color, bool solid = true, float lifetime = LIFETIME_ONE_FRAME);
        void draw_sphere(const Vec3f& pos, float radius, const Color4f& color, bool solid = true, float lifetime = LIFETIME_ONE_FRAME);
        void draw_cylinder(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid = true, float lifetime = LIFETIME_ONE_FRAME);
        void draw_cone(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid = true, float lifetime = LIFETIME_ONE_FRAME);
        void draw_box(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot, bool solid = true, float lifetime = LIFETIME_ONE_FRAME);
        void draw_text_3d(std::string text, const Vec3f& pos, float size, const Color4f& color, float lifetime = LIFETIME_ONE_FRAME);
        void draw_text_2d(std::string text, const Vec2f& pos, float size, const Color4f& color, float lifetime = LIFETIME_ONE_FRAME);

        void render(const Ref<Window>& window, const Rect2i& viewport, const Mat4x4f& mat_proj_view);
        void flush(float delta_time);

        [[nodiscard]] bool is_empty() const;
        [[nodiscard]] int  get_size() const;

    private:
        std::vector<std::unique_ptr<struct AuxDrawPrimitive>> m_added;
        std::deque<std::unique_ptr<struct AuxDrawPrimitive>>  m_storage;
        GfxVector<GfxVF_Pos3Col4, GfxVertBuffer>              m_lines;
        GfxVector<GfxVF_Pos3Col4, GfxVertBuffer>              m_tria_solid;
        GfxVector<GfxVF_Pos3Col4, GfxVertBuffer>              m_tria_wired;
        GfxVector<GfxVF_Pos3Col4Uv2, GfxVertBuffer>           m_text;
        Ref<Font>                                             m_font;
        Vec2f                                                 m_screen_size;

        mutable SpinMutex m_mutex;
    };

}// namespace wmoge