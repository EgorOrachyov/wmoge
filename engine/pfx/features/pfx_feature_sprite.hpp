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

#ifndef WMOGE_PFX_FEATURE_SPRITE_HPP
#define WMOGE_PFX_FEATURE_SPRITE_HPP

#include "pfx/pfx_feature.hpp"
#include "resource/shader_2d.hpp"
#include "resource/sprite.hpp"

namespace wmoge {

    /**
     * @class PfxFeatureSprite
     * @brief Add sprite so particles can be animated and rendered in 2d space
     */
    class PfxFeatureSprite : public PfxFeature {
    public:
        static constexpr const int N_VERTICES_PER_PARTICLE = 4;
        static constexpr const int N_INDICES_PER_PARTICLE  = 6;

        WG_OBJECT(PfxFeatureSprite, PfxFeature)

        Ref<PfxFeature> create() const override;
        StringId        get_feature_name() const override;
        StringId        get_feature_family() const override;

        bool load_from_options(const YamlConstNodeRef& node) override;

        void on_added(PfxAttributes& attributes) override;
        void on_spawn(class PfxComponentRuntime& runtime, const struct PfxSpawnParams& params) override {}
        void on_update(class PfxComponentRuntime& runtime, float dt) override {}
        void on_provided_render(class PfxComponentRuntime& runtime, std::unique_ptr<class PfxRenderer>& renderer) override;
        void on_prepare_render(class PfxComponentRuntime& runtime) override;

    private:
        Ref<Shader2d> m_shader;
        Ref<Sprite>   m_sprite;
        int           m_animation = -1;
    };

}// namespace wmoge

#endif//WMOGE_PFX_FEATURE_SPRITE_HPP
