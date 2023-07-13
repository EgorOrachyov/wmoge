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

#ifndef WMOGE_PFX_COMPONENT_HPP
#define WMOGE_PFX_COMPONENT_HPP

#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "pfx/pfx_feature.hpp"

namespace wmoge {

    /**
     * @class PfxComponent
     * @brief Single component of a pfx effect with particles
     *
     * Component represent a single distinct collection of particles with
     * the same attributes configure controlled by fixed features set, stored
     * inside this component.
     *
     * Each component inside running pfx effect gets its own pfx storage.
     * Particles inside storage are updates and evaluated using features
     * and features settings of this component.
     *
     * In time of processing, each feature gets notification on some process,
     * such as spawn, update, movement, rendering, physics, audio, etc.
     * Features are called in the order they are added into this component.
     *
     * Each component updated using separate async task in the engine task system.
     */
    class PfxComponent final : public RefCnt {
    public:
        ~PfxComponent() override = default;

        bool load_from_options(const YamlConstNodeRef& node);
        void add_feature(Ref<PfxFeature> feature);
        void set_amount(int amount);
        void set_active(bool active);

        const Ref<PfxFeature>& get_feature(int id) const;
        PfxAttributes          get_attributes() const;
        const StringId&        get_name() const;
        int                    get_features_count() const;
        int                    get_amount() const;
        bool                   is_active() const;

    private:
        fast_vector<Ref<PfxFeature>> m_features;
        PfxAttributes                m_attributes;
        StringId                     m_name;
        int                          m_amount = 0;
        bool                         m_active = false;
    };

}// namespace wmoge

#endif//WMOGE_PFX_COMPONENT_HPP
