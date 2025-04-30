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

#include "ui_binder.hpp"

#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    UiBinder::UiBinder(const Ref<UiElement>& element, const Ref<RttiObject>& data_source)
        : m_element(element), m_data_source(data_source) {
    }

    Status UiBinder::bind() {
        WG_PROFILE_CPU_UI("UiBinder::bind");

        WG_CHECKED(bind_element(m_element));
        m_element->data_source = m_data_source;

        return WG_OK;
    }

    Status UiBinder::bind_element(const Ref<UiElement>& element) {
        assert(element);

        WG_CHECKED(bind_attributes(element));

        for (const Ref<UiElement>& child : element->children) {
            WG_CHECKED(bind_element(child));
        }

        RttiClass* cls = element->get_class();
        for (const RttiField& field : cls->get_fields()) {
            const RttiType* type = field.get_type();
            if (!type->archetype_is(RttiArchetype::Ref)) {
                continue;
            }
            const RttiType* type_elem = static_cast<const RttiTypeRef*>(type)->get_value_type();
            if (!type_elem->archetype_is(RttiArchetype::Class)) {
                continue;
            }
            const RttiClass* type_cls = static_cast<const RttiClass*>(type_elem);
            if (!type_cls->is_subtype_of(UiElement::get_class_static())) {
                continue;
            }

            const Ref<UiElement>& slot = *(reinterpret_cast<const Ref<UiElement>*>(
                    reinterpret_cast<const std::uint8_t*>(element.get()) + field.get_byte_offset()));

            if (slot) {
                WG_CHECKED(bind_element(slot));
            }
        }

        return WG_OK;
    }

    Status UiBinder::bind_attributes(const Ref<UiElement>& element) {
        if (element->bindings.empty()) {
            return WG_OK;
        }

        element->bindings_updater.reserve(element->bindings.size());

        for (std::size_t i = 0; i < element->bindings.size(); i++) {
            const UiBinding& binding = element->bindings[i];

            const RttiField* target = element->get_class()->find_field(binding.property).value_or(nullptr);
            if (!target) {
                WG_LOG_ERROR("no target property " << binding.property);
                return StatusCode::InvalidData;
            }

            const RttiField* source = m_data_source->get_class()->find_field(binding.data_path).value_or(nullptr);
            if (!source) {
                WG_LOG_ERROR("no source property " << binding.data_path);
                return StatusCode::InvalidData;
            }

            if (target->get_type() != source->get_type()) {
                WG_LOG_ERROR("types for target and source property must match");
                return StatusCode::InvalidData;
            }

            auto update_func = [type = binding.type, target, source, element, data_source = m_data_source]() {
                std::size_t   offset_target = target->get_byte_offset();
                std::uint8_t* ptr_target    = reinterpret_cast<std::uint8_t*>(element.get()) + offset_target;

                std::size_t   offset_source = source->get_byte_offset();
                std::uint8_t* ptr_source    = reinterpret_cast<std::uint8_t*>(data_source.get()) + offset_source;

                const RttiType* t = target->get_type();

                if (type == UiBindingType::ToTarget) {
                    t->copy(ptr_target, ptr_source);
                }
                if (type == UiBindingType::ToSource) {
                    t->copy(ptr_source, ptr_target);
                }
            };

            if (binding.type == UiBindingType::ToTargetOnce) {
                update_func();
                continue;
            }
            if (binding.type == UiBindingType::ToTarget) {
                update_func();
            }

            element->bindings_updater.emplace_back(update_func);
        }

        return WG_OK;
    }

}// namespace wmoge
