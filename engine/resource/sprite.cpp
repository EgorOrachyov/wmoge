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

#include "sprite.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "core/log.hpp"
#include "resource/resource_manager.hpp"

namespace wmoge {

    void Sprite::add_animation(StringId animation, ref_ptr<Texture2d> texture) {
        assert(get_animation_id(animation) == -1);
        assert(!animation.empty());
        assert(texture);
        m_animations_id[animation] = int(m_animations.size());
        m_animations.emplace_back();
    }
    void Sprite::add_animation_frame(int animation, const Vec4f& frame) {
        m_animations[animation].frames.push_back(frame);
    }
    void Sprite::set_animation_speed(int animation, float speed) {
        m_animations[animation].speed = speed;
    }
    void Sprite::set_animation_loop(int animation, bool loop) {
        m_animations[animation].loop = loop;
    }
    const fast_vector<Vec4f>& Sprite::get_animation_frames(int animation) {
        return m_animations[animation].frames;
    }
    const ref_ptr<Texture2d>& Sprite::get_animation_texture(int animation) {
        return m_animations[animation].texture;
    }
    const Vec4f& Sprite::get_animation_frame(int animation, int frame) {
        assert(frame < get_animation_n_frames(animation));
        return m_animations[animation].frames[frame];
    }
    int Sprite::get_animation_n_frames(int animation) {
        return int(m_animations[animation].frames.size());
    }
    float Sprite::get_animation_speed(int animation) {
        return m_animations[animation].speed;
    }
    bool Sprite::get_animation_loop(int animation) {
        return m_animations[animation].loop;
    }
    int Sprite::get_animation_id(const StringId& animation) {
        auto it = m_animations_id.find(animation);
        return it != m_animations_id.end() ? it->second : -1;
    }
    int Sprite::get_n_animations() {
        return int(m_animations.size());
    }
    const Vec2f& Sprite::get_size() {
        return m_size;
    }
    const Vec2f& Sprite::get_pivot() {
        return m_pivot;
    }

    bool Sprite::load_from_import_options(const YamlTree& tree) {
        if (!Resource::load_from_import_options(tree)) return false;

        auto params = tree["params"];
        params["size_x"] >> m_size[0];
        params["size_y"] >> m_size[1];
        params["pivot_x"] >> m_pivot[0];
        params["pivot_y"] >> m_pivot[1];

        for (auto anim = params["animations"].first_child(); anim.valid(); anim = anim.next_sibling()) {
            Animation animation;

            std::string name;
            std::string texture;
            anim["name"] >> name;
            anim["texture"] >> texture;

            animation.texture = Engine::instance()->resource_manager()->find(SID(texture)).cast<Texture2d>();
            if (!animation.texture) {
                WG_LOG_ERROR("failed to load sprite texture " << texture);
                return false;
            }

            animation.name = SID(name);

            anim["speed"] >> animation.speed;
            anim["loop"] >> animation.loop;

            for (auto frame = anim["frames"].first_child(); frame.valid(); frame = frame.next_sibling()) {
                Vec4f frame_uv;

                std::string frame_uv_str;
                frame >> frame_uv_str;

                std::stringstream frame_uv_stream(frame_uv_str);
                frame_uv_stream >> frame_uv[0] >> frame_uv[1] >> frame_uv[2] >> frame_uv[3];

                animation.frames.push_back(frame_uv);
            }

            m_animations_id[animation.name] = int(m_animations.size());
            m_animations.push_back(std::move(animation));
        }

        return true;
    }
    void Sprite::copy_to(wmoge::Resource& copy) {
        auto* sprite            = dynamic_cast<Sprite*>(&copy);
        sprite->m_animations_id = m_animations_id;
        sprite->m_animations    = m_animations;
        sprite->m_size          = m_size;
    }

    void Sprite::register_class() {
        auto* cls = Class::register_class<Sprite>();
    }

}// namespace wmoge
