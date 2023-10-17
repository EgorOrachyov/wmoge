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

#include "scene_camera.hpp"

#include "core/engine.hpp"
#include "event/event_action.hpp"
#include "event/event_manager.hpp"
#include "math/quat.hpp"

namespace wmoge {

    void Camera::set_name(const StringId& name) {
        m_name = name;
    }
    void Camera::set_fov(float fov) {
        m_fov = fov;
    }
    void Camera::set_color(const Color4f& color) {
        m_color = color;
    }
    void Camera::set_near_far(float near, float far) {
        m_near = near;
        m_far  = far;
    }
    void Camera::set_projection(CameraProjection projection) {
        m_projection = projection;
    }
    void Camera::make_active(bool active) {
        m_active = active;
    }

    void Camera::update_render_camera(Size2i target) {
        const float width     = float(target.x());
        const float height    = float(target.y());
        const float aspect    = height > 0 ? width / height : 1.0f;
        const Vec4f viewportf = Vec4f{width, height, width, height} * m_viewport;

        RenderCamera camera;
        camera.set_proj_params(m_fov, aspect, m_near, m_far);
        camera.set_viewport(Rect2i{int(viewportf.x()), int(viewportf.y()), int(viewportf.z()), int(viewportf.w())});
        camera.look(m_direction, m_up);
        camera.move_to(m_position);
        camera.validate();

        m_render_camera = camera;
    }

    CameraDebug::CameraDebug() {
        m_action_listener = Engine::instance()->event_manager()->subscribe<EventAction>([this](const EventAction& action) {
            static const StringId CD_TOGGLE       = SID("cd_toggle");
            static const StringId CD_MOVE_UP      = SID("cd_move_up");
            static const StringId CD_MOVE_DOWN    = SID("cd_move_down");
            static const StringId CD_MOVE_LEFT    = SID("cd_move_left");
            static const StringId CD_MOVE_RIGHT   = SID("cd_move_right");
            static const StringId CD_MOVE_FWD     = SID("cd_move_fwd");
            static const StringId CD_MOVE_BWD     = SID("cd_move_bwd");
            static const StringId CD_ROTATE_LEFT  = SID("cd_rotate_left");
            static const StringId CD_ROTATE_RIGHT = SID("cd_rotate_right");
            static const StringId CD_ROTATE_DOWN  = SID("cd_rotate_down");
            static const StringId CD_ROTATE_UP    = SID("cd_rotate_up");

            const auto& id = action.name;
            const float dt = Engine::instance()->get_delta_time_game();

            const auto right = Vec3f ::cross(m_direction, m_up);

            const float move_strength     = action.strength * dt * m_speed_move;
            const float rotation_strength = action.strength * dt * m_speed_rotate;

            float angle_vertical   = 0.0f;
            float angle_horizontal = 0.0f;

            if (id == CD_TOGGLE) {
                make_active(!is_active());
                return true;
            }

            if (!is_active()) {
                return false;
            }

            bool processed = false;

            if (id == CD_MOVE_UP) {
                m_position += Vec3f::axis_y() * dt * m_speed_move;
            } else if (id == CD_MOVE_DOWN) {
                m_position -= Vec3f::axis_y() * dt * m_speed_move;
            } else if (id == CD_MOVE_LEFT) {
                m_position -= right * move_strength;
            } else if (id == CD_MOVE_RIGHT) {
                m_position += right * move_strength;
            } else if (id == CD_MOVE_FWD) {
                m_position += m_direction * move_strength;
            } else if (id == CD_MOVE_BWD) {
                m_position -= m_direction * move_strength;
            } else if (id == CD_ROTATE_LEFT) {
                angle_horizontal += rotation_strength;
            } else if (id == CD_ROTATE_RIGHT) {
                angle_horizontal -= rotation_strength;
            } else if (id == CD_ROTATE_UP) {
                angle_vertical += rotation_strength;
            } else if (id == CD_ROTATE_DOWN) {
                angle_vertical -= rotation_strength;
            } else {
                processed = false;
            }

            auto rotation_1 = Quatf(Vec3f::axis_y(), angle_horizontal);
            m_direction     = rotation_1.rotate(m_direction);
            m_up            = rotation_1.rotate(m_up);

            auto rotation_2 = Quatf(Vec3f::cross(m_direction, m_up), angle_vertical);
            m_direction     = rotation_2.rotate(m_direction);
            m_up            = rotation_2.rotate(m_up);

            return processed;
        });
    }
    CameraDebug::~CameraDebug() {
        Engine::instance()->event_manager()->unsubscribe(m_action_listener);
    }

    CameraManager::CameraManager() {
        m_camera_default = make_ref<Camera>();
        m_camera_default->set_name(SID("default"));

        m_camera_debug = make_ref<CameraDebug>();
        m_camera_debug->set_name(SID("debug"));

        m_cameras.push_back(m_camera_default);
        m_cameras.push_back(m_camera_debug);
    }

    Ref<Camera> CameraManager::make_camera(const StringId& name) {
        auto camera = make_ref<Camera>();
        camera->set_name(name);
        m_cameras.push_back(camera);
        return camera;
    }
    std::optional<Ref<Camera>> CameraManager::find_camera(const StringId& name) {
        for (auto& camera : m_cameras) {
            if (camera->get_name() == name) {
                return camera;
            }
        }
        return std::nullopt;
    }
    std::optional<Ref<Camera>> CameraManager::find_active() {
        for (auto& camera : m_cameras) {
            if (camera->is_active()) {
                return camera;
            }
        }
        return std::nullopt;
    }
    std::optional<Ref<Camera>> CameraManager::find_first(const std::function<bool(const Ref<Camera>&)>& pred) {
        for (auto& camera : m_cameras) {
            if (pred(camera)) {
                return camera;
            }
        }
        return std::nullopt;
    }
    std::vector<Ref<Camera>> CameraManager::filter(const std::function<bool(const Ref<Camera>&)>& pred) {
        std::vector<Ref<Camera>> filtered;
        for (auto& camera : m_cameras) {
            if (pred(camera)) {
                filtered.push_back(camera);
            }
        }
        return filtered;
    }
    bool CameraManager::is_default(const Camera* camera) const {
        return camera == m_camera_default.get();
    }
    bool CameraManager::is_debug(const Camera* camera) const {
        return camera == m_camera_debug.get();
    }

}// namespace wmoge
