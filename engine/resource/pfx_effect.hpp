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

#include "core/fast_vector.hpp"
#include "pfx/pfx_component.hpp"
#include "pfx/pfx_emitter.hpp"
#include "pfx/pfx_feature.hpp"
#include "pfx/pfx_storage.hpp"
#include "resource/resource.hpp"

namespace wmoge {

    /**
     * @class PfxEffectType
     * @brief Type of pfx effect (for 2d and 3d support)
     */
    enum class PfxEffectType {
        None,
        Effect2d,
    };

    /**
     * @class PfxEffect
     * @brief Complete pfx effect which can be instantiated for emitting
     *
     * Effects is a hierarchical collection of different pfx components.
     * Components are updated in a order of parend-child dependency.
     * Events in a parent component can trigger changes in child components,
     * what can be used to make complex pfx effects.
     *
     * Effect can be instantiated into a particular running instance using
     * pfx emitter class.
     */
    class PfxEffect final : public Resource {
    public:
        WG_OBJECT(PfxEffect, Resource)

        Status copy_to(Object& other) const override;

        void                     add_component(Ref<PfxComponent> component);
        const Ref<PfxComponent>& get_component(int id) const;
        int                      get_components_count() const;
        PfxEffectType            get_type() const;

    private:
        fast_vector<Ref<PfxComponent>> m_components;
        PfxEffectType                  m_type = PfxEffectType::None;
    };

}// namespace wmoge