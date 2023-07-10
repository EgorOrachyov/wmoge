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

#include "pfx_feature_sprite.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "math/color.hpp"
#include "math/math_utils2d.hpp"
#include "math/transform.hpp"
#include "pfx/pfx_component_runtime.hpp"
#include "pfx/pfx_emitter.hpp"
#include "render/aux_draw_canvas.hpp"
#include "resource/resource_manager.hpp"
#include "resource/sprite.hpp"

namespace wmoge {

    Ref<PfxFeature> PfxFeatureSprite::create() const {
        return make_ref<PfxFeatureSprite>();
    }
    StringId PfxFeatureSprite::get_feature_name() const {
        static StringId name("Sprite");
        return name;
    }
    StringId PfxFeatureSprite::get_feature_family() const {
        static StringId family("Visual");
        return family;
    }
    bool PfxFeatureSprite::load_from_options(const YamlConstNodeRef& node) {
        StringId sprite;
        StringId shader;

        Yaml::read(node["sprite"], sprite);
        if (sprite.empty()) {
            WG_LOG_ERROR("empty sprite name for feature");
            return false;
        }

        Yaml::read(node["shader"], shader);
        if (shader.empty()) {
            WG_LOG_ERROR("empty shader name for feature");
            return false;
        }

        auto* resource_manager = Engine::instance()->resource_manager();

        m_sprite = resource_manager->load(sprite).cast<Sprite>();
        if (!m_sprite) {
            WG_LOG_ERROR("failed to load sprite " << sprite);
            return false;
        }

        m_shader = resource_manager->load(shader).cast<Shader>();
        if (!m_shader) {
            WG_LOG_ERROR("failed to load shader " << shader);
            return false;
        }

        node["animation"] >> m_animation;

        return true;
    }
    void PfxFeatureSprite::on_added(PfxAttributes& attributes) {
        attributes.set(PfxAttribute::Time);
        attributes.set(PfxAttribute::Pos2d);
        attributes.set(PfxAttribute::Size);
        attributes.set(PfxAttribute::Angle);
        attributes.set(PfxAttribute::Color);
    }
    void PfxFeatureSprite::on_provided_render(class PfxComponentRuntime& runtime, std::unique_ptr<class PfxRenderer>& renderer) {
        WG_AUTO_PROFILE_PFX("PfxFeatureSprite::on_provided_render");
    }
    void PfxFeatureSprite::on_prepare_render(PfxComponentRuntime& runtime) {
        WG_AUTO_PROFILE_PFX("PfxFeatureSprite::on_prepare_render");
    }

    void PfxFeatureSprite::register_class() {
        auto* cls = Class::register_class<PfxFeatureSprite>();
    }

}// namespace wmoge