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

#ifndef WMOGE_REF_HPP
#define WMOGE_REF_HPP

#include <atomic>
#include <cassert>
#include <functional>
#include <utility>

namespace wmoge {

    class RefCnt {
    public:
        virtual ~RefCnt();

        unsigned int ref();
        unsigned int unref();
        unsigned int refs_count();

    protected:
        virtual void destroy();

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

    template<typename T>
    class ref_ptr {
    public:
        ref_ptr() = default;

        ref_ptr(std::nullptr_t){};

        explicit ref_ptr(T* object) {
            m_ptr = ref(object);
        }

        ref_ptr(const ref_ptr& p) {
            m_ptr = ref(p.m_ptr);
        }

        template<typename G>
        ref_ptr(const ref_ptr<G>& p) {
            m_ptr = ref(p.get());
        }

        ref_ptr(ref_ptr&& p) noexcept {
            m_ptr = p.release();
        }

        template<typename G>
        ref_ptr(ref_ptr<G>&& p) noexcept {
            m_ptr = p.release();
        }

        ~ref_ptr() {
            unref(m_ptr);
        }

        ref_ptr& operator=(const ref_ptr& p) {
            if (this != &p)
                reset(ref(p.m_ptr));
            return *this;
        }

        ref_ptr& operator=(ref_ptr&& p) noexcept {
            if (this != &p)
                reset(p.release());
            return *this;
        }

        bool operator==(const ref_ptr& p) const {
            return m_ptr == p.m_ptr;
        }

        bool operator!=(const ref_ptr& p) const {
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

        template<class G>
        ref_ptr<G> as() const {
            return ref_ptr<G>(m_ptr);
        }

        template<typename G>
        ref_ptr<G> cast() const {
            return ref_ptr<G>(dynamic_cast<G*>(m_ptr));
        }

    private:
        T* m_ptr = nullptr;
    };

    template<typename T, typename... TArgs>
    ref_ptr<T> make_ref(TArgs&&... args) {
        return ref_ptr<T>(new T(std::forward<TArgs>(args)...));
    }

}// namespace wmoge

namespace std {

    template<typename T>
    struct hash<wmoge::ref_ptr<T>> {
    public:
        std::size_t operator()(const wmoge::ref_ptr<T>& ref) const {
            return std::hash<T*>()(ref.get());
        }
    };

}// namespace std

#endif//WMOGE_REF_HPP
