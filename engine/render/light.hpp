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

#include "asset/texture.hpp"
#include "core/synchronization.hpp"
#include "math/aabb.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/vec.hpp"
#include "render/camera.hpp"

#include <vector>

namespace wmoge {

    /**
     * @brief Type of supported light sources
    */
    enum class LightType {
        Dir   = 0,
        Spot  = 1,
        Point = 2,
        Area  = 3,
        Total = 4
    };

    /**
     * @class Light
     * @brief Light sources representation for the rendering engine
    */
    class Light final {
    public:
        Light(LightType type = LightType::Dir) : m_type(type) {}

        void set_type(LightType type) { m_type = type; }
        void set_intensity(float intensity) { m_intensity = intensity; }
        void set_radius(float radius) { m_radius = radius; }
        void set_attenuation(float attenuation) { m_attenuation = attenuation; }
        void set_color(const Vec3f& color) { m_color = color; }
        void set_spot_angle_inner(float rad) { m_angle_rad_inner = rad; }
        void set_spot_angle_outer(float rad) { m_angle_rad_outer = rad; }
        void set_mask(const Ref<Texture>& mask) { m_mask = mask; }
        void set_transform(const Mat4x4f);

        [[nodiscard]] const Vec3f&        get_world_pos() const { return m_world_pos; };
        [[nodiscard]] const Vec3f&        get_world_dir() const { return m_world_dir; };
        [[nodiscard]] const Vec3f&        get_world_up() const { return m_world_up; };
        [[nodiscard]] const Vec3f&        get_color() const { return m_color; };
        [[nodiscard]] float               get_intensity() const { return m_intensity; };
        [[nodiscard]] float               get_attenuation() const { return m_attenuation; };
        [[nodiscard]] float               get_radius() const { return m_radius; };
        [[nodiscard]] float               get_angle_rad_inner() const { return m_angle_rad_inner; };
        [[nodiscard]] float               get_angle_rad_outer() const { return m_angle_rad_outer; };
        [[nodiscard]] const Ref<Texture>& get_mask() const { return m_mask; };
        [[nodiscard]] LightType           get_type() const { return m_type; };

    private:
        Mat4x4f      m_local_to_world  = Math3d::identity();
        Mat4x4f      m_world_to_local  = Math3d::identity();
        Vec3f        m_world_pos       = Vec3f(0, 0, 0);
        Vec3f        m_world_dir       = Vec3f(0, 0, 1);
        Vec3f        m_world_up        = Vec3f(0, 1, 0);
        Vec3f        m_color           = Vec3f(1, 1, 1);
        float        m_intensity       = 1.0f;
        float        m_attenuation     = 1.0f;
        float        m_radius          = 0.0f;
        float        m_angle_rad_inner = 0.0f;
        float        m_angle_rad_outer = 0.0f;
        Ref<Texture> m_mask;
        LightType    m_type = LightType::Dir;
    };

}// namespace wmoge