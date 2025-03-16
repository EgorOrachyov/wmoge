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

#include "core/array_view.hpp"
#include "core/ref.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    template<typename T>
    class UiAttribute {
    public:
        UiAttribute() = default;
        UiAttribute(T value) : m_value(std::move(value)) {}

        UiAttribute& operator=(const T& value) {
            m_value = value;
            return *this;
        }

        UiAttribute& operator=(T&& value) {
            m_value = std::move(value);
            return *this;
        }

        void set(T value) { m_value = std::move(value); }

        operator T() const { return m_value; }

        [[nodiscard]] const T& get() const { return m_value; }
        [[nodiscard]] T&       get() { return m_value; }
        [[nodiscard]] const T* get_ptr() const { return &m_value; }
        [[nodiscard]] T*       get_ptr() { return &m_value; }

    protected:
        T m_value;
    };

    template<typename T>
    class UiAttributeList : public UiAttribute<std::vector<T>> {
    public:
        using UiAttribute<std::vector<T>>::m_value;

        void add_element(T element) { m_value.push_back(std::move(element)); }
    };

    template<typename T>
    class UiAttributeOpt : public UiAttribute<std::optional<T>> {
    public:
        using UiAttribute<std::optional<T>>::m_value;
        using UiAttribute<std::optional<T>>::operator=;

        bool     has_value() const { return m_value.has_value(); }
        const T& value() const { return m_value.value(); }
        void     reset() { m_value.reset(); }
    };

    template<typename T>
    class UiEvent : public UiAttribute<T> {
    public:
        using UiAttribute<T>::m_value;
        using UiAttribute<T>::operator=;

        bool has_callback() const { return (bool) m_value; }
    };

    template<typename T>
    class UiSlot : public UiAttribute<Ref<T>> {
    public:
        using UiAttribute<Ref<T>>::m_value;
        using UiAttribute<Ref<T>>::operator=;

        bool has_value() const { return m_value; }
    };

    template<typename T>
    class UiSlots : public UiAttribute<std::vector<T>> {
    public:
        using UiAttribute<std::vector<T>>::m_value;

        T&                  add_slot() { return m_value.emplace_back(); }
        array_view<const T> get_slots() const { return m_value; }
    };

}// namespace wmoge