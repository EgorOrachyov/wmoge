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

#ifndef WMOGE_SPRITE_HPP
#define WMOGE_SPRITE_HPP

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"
#include "resource/resource.hpp"
#include "resource/texture.hpp"

namespace wmoge {

    /**
     * @class Sprite
     * @brief Sprite is a sequence of images in a texture which can be animated
     *
     * Sprite is a single of a number of images which can be used to draw and
     * animate 2d object.
     *
     * Sprite consists of a set of animations. Each animation can have unique
     * texture atlas with frames. Frames in the animation describe, which part
     * of the atlas must rendered.
     *
     * An animation can have a speed (number of frames shown per second) and loop
     * (true if animation must be played in cycled fashion). Sprite have a size
     * in canvas units. An animation can be played by its name.
     */
    class Sprite : public Resource {
    public:
        WG_OBJECT(Sprite, Resource);

        bool load_from_import_options(const YamlTree& tree) override;
        void copy_to(Resource& copy) override;

        void add_animation(StringId animation, ref_ptr<Texture2d> texture);
        void add_animation_frame(int animation, const Vec4f& frame);
        void set_animation_speed(int animation, float speed);
        void set_animation_loop(int animation, bool loop);

        const fast_vector<Vec4f>& get_animation_frames(int animation);
        const ref_ptr<Texture2d>& get_animation_texture(int animation);
        const Vec4f&              get_animation_frame(int animation, int frame);
        int                       get_animation_n_frames(int animation);
        float                     get_animation_speed(int animation);
        bool                      get_animation_loop(int animation);
        int                       get_animation_id(const StringId& animation);
        int                       get_n_animations();
        const Vec2f&              get_size();
        const Vec2f&              get_pivot();

    private:
        struct Animation {
            StringId           name;
            fast_vector<Vec4f> frames;
            ref_ptr<Texture2d> texture;
            float              speed = 0.0f;
            bool               loop  = true;
        };

        fast_map<StringId, int> m_animations_id;
        fast_vector<Animation>  m_animations;
        Vec2f                   m_size;
        Vec2f                   m_pivot;
    };

}// namespace wmoge

#endif//WMOGE_SPRITE_HPP
