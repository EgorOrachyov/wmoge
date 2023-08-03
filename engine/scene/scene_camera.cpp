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

    Camera::Camera(CameraManager* manager) : m_manager(manager) {
    }

    void Camera::set_name(const StringId& name) {
        m_name = name;
    }
    void Camera::set_fov(float fov) {
        m_fov = fov;
    }
    void Camera::set_color(const Color4f& color) {
        m_color = color;
    }

    bool Camera::is_active() const {
        return m_manager->is_active(this);
    }
    bool Camera::is_default() const {
        return m_manager->is_default(this);
    }
    bool Camera::is_debug() const {
        return m_manager->is_default(this);
    }

    CameraDebug::CameraDebug(CameraManager* manager) : Camera(manager) {
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
                if (!is_active()) {
                    m_to_restore = m_manager->get_active_camera();
                    m_manager->make_active(Ref<Camera>(this));
                } else {
                    m_manager->make_active(m_to_restore);
                    m_to_restore.reset();
                }

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
        m_camera_default = make_ref<Camera>(this);
        m_camera_default->set_name(SID("default"));

        m_camera_debug = make_ref<CameraDebug>(this);
        m_camera_debug->set_name(SID("debug"));

        m_camera_active = m_camera_default;

        m_cameras.push_back(m_camera_default);
        m_cameras.push_back(m_camera_debug);
    }

    Ref<Camera> CameraManager::make_camera(const StringId& name) {
        auto camera = make_ref<Camera>(this);
        camera->set_name(name);

        m_cameras.push_back(camera);

        return camera;
    }
    Ref<Camera> CameraManager::find_camera(const StringId& name) {
        for (auto& camera : m_cameras) {
            if (camera->get_name() == name) {
                return camera;
            }
        }
        return {};
    }

    void CameraManager::make_active(const Ref<Camera>& camera) {
        m_camera_active = camera;
    }

    bool CameraManager::is_active(const Camera* camera) const {
        return camera == m_camera_active.get();
    }
    bool CameraManager::is_default(const Camera* camera) const {
        return camera == m_camera_default.get();
    }
    bool CameraManager::is_debug(const Camera* camera) const {
        return camera == m_camera_debug.get();
    }

}// namespace wmoge
