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

    UiBinder::UiBinder(Ref<UiElement>& element, const Ref<UiMarkup>& markup, const Ref<UiBindable>& bindalbe)
        : m_element(element), m_markup(markup), m_bindalbe(bindalbe) {
    }

    Status UiBinder::bind() {
        WG_PROFILE_CPU_UI("UiBinder::bind");

        const UiMarkupDecs& desc = m_markup->get_desc();

        m_mediator = make_ref<UiBindMediator>();

        WG_CHECKED(bind_element(m_element, desc.root_element));

        m_mediator->bindable     = m_bindalbe;
        m_mediator->root_element = m_element.get();
        m_element->user_data     = m_mediator;

        UiBindInfo bind_info;
        bind_info.find_element = [m = m_mediator.get()](Strid tag) -> UiElement* {
            auto query = m->tagged_elements.find(tag);
            if (query != m->tagged_elements.end()) {
                return query->second;
            }
            return nullptr;
        };
        bind_info.notify_changed = [m = m_mediator.get()](Strid property) -> void {
            auto query = m->binded_properties.find(property);
            if (query != m->binded_properties.end()) {
                query->second();
            }
        };

        m_bindalbe->set_bind_info(std::move(bind_info));

        return m_bindalbe->on_bind();
    }

    Status UiBinder::bind_element(Ref<UiElement>& element, int element_id) {
        const UiMarkupDecs&    desc         = m_markup->get_desc();
        const UiMarkupElement& element_info = desc.elements[element_id];
        element                             = element_info.cls->instantiate().cast<UiElement>();

        for (int attribute_id : element_info.attributes) {
            WG_CHECKED(bind_element_attribute(element, attribute_id));
        }

        for (int slot_id : element_info.slots) {
            WG_CHECKED(bind_element_slot(element, slot_id));
        }

        if (!element->tag.empty()) {
            m_mediator->tagged_elements[element->tag] = element.get();
        }

        return WG_OK;
    }

    Status UiBinder::bind_element_slot(const Ref<UiElement>& element, int slot_id) {
        const UiMarkupDecs& desc      = m_markup->get_desc();
        const UiMarkupSlot& slot_info = desc.slots[slot_id];

        const RttiField* field = slot_info.field;
        const RttiType*  type  = field->get_type();

        const bool is_ref    = type->archetype_is(RttiArchetype::Ref);
        const bool is_vector = type->archetype_is(RttiArchetype::Vector);

        assert(is_ref || is_vector);

        std::uint8_t* object       = reinterpret_cast<std::uint8_t*>(element.get());
        std::size_t   field_offset = field->get_byte_offset();

        Ref<UiElement> child;
        WG_CHECKED(bind_element(child, slot_info.child_element));

        if (is_vector) {
            const RttiTypeVector* type_vector = static_cast<const RttiTypeVector*>(type);
            WG_CHECKED(type_vector->push_back(object + field_offset, &child));
        }
        if (is_ref) {
            WG_CHECKED(type->copy(object + field_offset, &child));
        }

        return WG_OK;
    }

    Status UiBinder::bind_element_attribute(const Ref<UiElement>& element, int attribute_id) {
        const UiMarkupDecs&      desc           = m_markup->get_desc();
        const UiMarkupAttribute& attribute_info = desc.attributes[attribute_id];

        const RttiField* field = attribute_info.field;
        const RttiType*  type  = field->get_type();

        std::uint8_t* object = reinterpret_cast<std::uint8_t*>(element.get());
        std::size_t   offset = field->get_byte_offset();

        const bool is_bind = attribute_info.bind_method;

        if (!is_bind) {
            const RttiTypeOptional* optional_type = nullptr;
            const RttiType*         value_type    = type;

            if (type->archetype_is(RttiArchetype::Optional)) {
                optional_type = static_cast<const RttiTypeOptional*>(type);
                value_type    = optional_type->get_value_type();
            }

            auto set_field = [&](const void* value, const void* value_inside_opt) {
                if (optional_type) {
                    WG_CHECKED(type->copy(object + offset, value_inside_opt));
                } else {
                    WG_CHECKED(type->copy(object + offset, value));
                }

                return WG_OK;
            };

            const RttiType* type_bool   = rtti_type<bool>();
            const RttiType* type_int    = rtti_type<int>();
            const RttiType* type_float  = rtti_type<float>();
            const RttiType* type_strid  = rtti_type<Strid>();
            const RttiType* type_string = rtti_type<std::string>();

            if (value_type == type_bool) {
                bool                v_val = attribute_info.value;
                std::optional<bool> v     = v_val;
                WG_CHECKED(set_field(&(v.value()), &v));
            } else if (value_type == type_int) {
                int                v_val = attribute_info.value;
                std::optional<int> v     = v_val;
                WG_CHECKED(set_field(&(v.value()), &v));
            } else if (value_type == type_float) {
                float                v_val = attribute_info.value;
                std::optional<float> v     = v_val;
                WG_CHECKED(set_field(&(v.value()), &v));
            } else if (value_type == type_strid) {
                Strid                v_val = attribute_info.value;
                std::optional<Strid> v     = v_val;
                WG_CHECKED(set_field(&(v.value()), &v));
            } else if (value_type == type_string) {
                std::string                v_val = attribute_info.value;
                std::optional<std::string> v     = std::move(v_val);
                WG_CHECKED(set_field(&(v.value()), &v));
            }
        } else {
            const RttiMethod*       method   = attribute_info.bind_method;
            const RttiTypeFunction* function = method->get_function();

            const bool is_function = type->archetype_is(RttiArchetype::Function);

            std::function<void()> callback = [e = element.get(),
                                              b = m_bindalbe.get(),
                                              f = function]() {
                RttiFrame frame;

                if (!f->call(frame, b, array_view<std::uint8_t>((std::uint8_t*) &e, sizeof(e)))) {
                    WG_LOG_ERROR("failed to call function " << f->get_name());
                }
            };

            if (is_function) {
                type->copy(object + offset, &callback);
            } else {
                m_mediator->binded_properties[method->get_name()] = std::move(callback);
            }
        }

        return WG_OK;
    }

}// namespace wmoge
