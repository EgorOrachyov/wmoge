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

#include <atomic>
#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>

namespace wmoge {

    /** 
     * @class RefCnt
     * @brief Base class for any atomic reference counted class
     * 
     * @note Thread-safe
     */
    class RefCnt {
    public:
        virtual ~RefCnt();

        unsigned int ref();
        unsigned int unref();
        unsigned int refs_count();

    protected:
        virtual void destroy();
        virtual void reach_zero();

    private:
        std::atomic_uint m_refs_count{0};
    };

    template<typename T>
    inline T* ref(T* object) {
        if (object) object->ref();
        return object;
    }

    template<typename T>
    inline void unref(T* object) {
        if (object) object->unref();
    }

    /** 
     * @class Ref
     * @brief Smart shared reference-counted pointer to T class
     */
    template<typename T>
    class Ref {
    public:
        Ref() = default;

        Ref(std::nullptr_t) {}

        explicit Ref(T* object) {
            m_ptr = ref(object);
        }

        Ref(const Ref& p) {
            m_ptr = ref(p.m_ptr);
        }

        template<typename G>
        Ref(const Ref<G>& p) {
            m_ptr = ref(p.get());
        }

        Ref(Ref&& p) noexcept {
            m_ptr = p.release();
        }

        template<typename G>
        Ref(Ref<G>&& p) noexcept {
            m_ptr = p.release();
        }

        ~Ref() {
            unref(m_ptr);
        }

        Ref& operator=(const Ref& p) {
            if (this != &p)
                reset(ref(p.m_ptr));
            return *this;
        }

        Ref& operator=(Ref&& p) noexcept {
            if (this != &p)
                reset(p.release());
            return *this;
        }

        bool operator==(const Ref& p) const {
            return m_ptr == p.m_ptr;
        }

        bool operator!=(const Ref& p) const {
            return m_ptr != p.m_ptr;
        }

        operator bool() const {
            return m_ptr != nullptr;
        }

        T* operator->() const {
            return m_ptr;
        }

        T& operator*() const {
            assert(m_ptr);
            return *m_ptr;
        }

        void reset(T* object = nullptr) {
            T* old = m_ptr;
            m_ptr  = object;
            unref(old);
        }

        T* release() {
            T* old = m_ptr;
            m_ptr  = nullptr;
            return old;
        }

        T* get() const {
            return m_ptr;
        }

        T* get_checked() const {
            assert(m_ptr);
            return m_ptr;
        }

        operator T*() const { return m_ptr; }

        template<typename G, typename std::enable_if_t<std::is_convertible_v<T*, G*>> = true>
        operator Ref<G>() const {
            return Ref<G>(m_ptr);
        }

        template<class G>
        Ref<G> as() const {
            return Ref<G>(m_ptr);
        }

        template<typename G>
        Ref<G> cast() const {
            return Ref<G>(dynamic_cast<G*>(m_ptr));
        }

    protected:
        T* m_ptr = nullptr;
    };

    template<typename T, typename... TArgs>
    Ref<T> make_ref(TArgs&&... args) {
        return Ref<T>(new T(std::forward<TArgs>(args)...));
    }

}// namespace wmoge

namespace std {

    template<typename T>
    struct hash<wmoge::Ref<T>> {
    public:
        std::size_t operator()(const wmoge::Ref<T>& ref) const {
            return std::hash<T*>()(ref.get());
        }
    };

}// namespace std