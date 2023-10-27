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

#ifndef WMOGE_SCENE_CAMERA_HPP
#define WMOGE_SCENE_CAMERA_HPP

#include "core/array_view.hpp"
#include "core/ref.hpp"
#include "event/event_listener.hpp"
#include "math/color.hpp"
#include "math/math_utils.hpp"
#include "math/math_utils3d.hpp"
#include "render/render_camera.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class Camera
     * @brief In-game camera for rendering the scene
     */
    class Camera : public RefCnt {
    public:
        ~Camera() override = default;

        void set_name(const StringId& name);
        void set_fov(float fov);
        void set_color(const Color4f& color);
        void set_near_far(float near, float far);
        void set_projection(CameraProjection projection);
        void make_active(bool active);
        void update_render_camera(Size2i target);

        [[nodiscard]] const RenderCamera&     get_render_camera() const { return m_render_camera; }
        [[nodiscard]] const Vec3f&            get_position() const { return m_position; }
        [[nodiscard]] const Vec3f&            get_direction() const { return m_direction; }
        [[nodiscard]] const Vec3f&            get_up() const { return m_up; }
        [[nodiscard]] const Color4f&          get_color() const { return m_color; }
        [[nodiscard]] const Vec4f&            get_viewport() const { return m_viewport; }
        [[nodiscard]] float                   get_fov() const { return m_fov; }
        [[nodiscard]] float                   get_near() const { return m_near; }
        [[nodiscard]] float                   get_far() const { return m_far; }
        [[nodiscard]] const StringId&         get_name() const { return m_name; }
        [[nodiscard]] const CameraProjection& get_projection() const { return m_projection; }
        [[nodiscard]] bool                    is_active() const { return m_active; }

    protected:
        RenderCamera     m_render_camera;
        Vec3f            m_position;
        Vec3f            m_direction  = Vec3f ::axis_z();
        Vec3f            m_up         = Vec3f ::axis_y();
        Color4f          m_color      = Color::BLACK4f;
        Vec4f            m_viewport   = Vec4f(0, 0, 1, 1);
        float            m_fov        = Math::deg_to_rad(45.0f);
        float            m_near       = 0.1f;
        float            m_far        = 10000.0f;
        StringId         m_name       = SID("default");
        CameraProjection m_projection = CameraProjection::Perspective;
        bool             m_active     = false;
    };

    /**
     * @class CameraDebug
     * @brief Special debug camera for free-fly navigation in-game
     */
    class CameraDebug : public Camera {
    public:
        CameraDebug();
        ~CameraDebug() override;

        [[nodiscard]] float get_speed_move() const { return m_speed_move; }
        [[nodiscard]] float get_speed_rotate() const { return m_speed_rotate; }

    private:
        EventListenerHnd m_action_listener;
        float            m_speed_move   = 10.0f;
        float            m_speed_rotate = 2.0f;
    };

    /**
     * @class CameraManager
     * @brief Manages all cameras on the scene
     */
    class CameraManager final {
    public:
        CameraManager();

        Ref<Camera>                make_camera(const StringId& name = StringId());
        std::optional<Ref<Camera>> find_camera(const StringId& name);
        std::optional<Ref<Camera>> find_active();
        std::optional<Ref<Camera>> find_first(const std::function<bool(const Ref<Camera>&)>& pred);
        std::vector<Ref<Camera>>   filter(const std::function<bool(const Ref<Camera>&)>& pred);
        void                       fill_render_cameras(RenderCameras& cameras);

        [[nodiscard]] ArrayView<const Ref<Camera>> get_cameras() const { return m_cameras; }
        [[nodiscard]] Ref<Camera>                  get_debug_camera() const { return m_camera_debug; }
        [[nodiscard]] Ref<Camera>                  get_default_camera() const { return m_camera_default; }
        [[nodiscard]] bool                         is_default(const Camera* camera) const;
        [[nodiscard]] bool                         is_debug(const Camera* camera) const;

    private:
        std::vector<Ref<Camera>> m_cameras;
        Ref<Camera>              m_camera_debug;
        Ref<Camera>              m_camera_default;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_CAMERA_HPP
