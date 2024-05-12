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

#include "core/ref.hpp"
#include "core/synchronization.hpp"

#include <optional>
#include <type_traits>

namespace wmoge {

    /**
     * @class WeakRefAccess
     * @brief Manages access to a shared object to access in a weak pointer
     * 
     * This base struct is referenced by weak ptr and alive until last weak ref is released.
     * Used to track life-time of a shared object. If object destructed, it is no
     * more reachable by any weak ref.
     * 
     * @note Thread-safe 
    */
    class WeakRefAccess : public RefCnt {
    public:
        WeakRefAccess() = default;
        WeakRefAccess(class RefCnt* object) : m_object(object) {}

        bool                             try_release_object();
        std::optional<Ref<class RefCnt>> try_acquire_object();

    private:
        mutable SpinMutex     m_mutex;
        mutable class RefCnt* m_object = nullptr;
    };

    /**
     * @class WeakRefCnt
     * @brief Base class for any shared object which wants weak referencing support
     * 
     * @tparam BaseRefCntClass Base class which provides shared ref cnt mechanism
    */
    template<typename BaseRefCntClass>
    class WeakRefCnt : public BaseRefCntClass {
    public:
        ~WeakRefCnt() override = default;

        [[nodiscard]] const Ref<WeakRefAccess>& get_weak_access() const { return m_weak_access; }

    protected:
        void reach_zero() override {
            if (m_weak_access->try_release_object()) {
                BaseRefCntClass::destroy();
            }
        }

    private:
        Ref<WeakRefAccess> m_weak_access = make_ref<WeakRefAccess>(this);
    };

    template<typename T>
    inline Ref<WeakRefAccess> weak_ref_access(T* object) {
        return object ? object->get_weak_access() : Ref<WeakRefAccess>();
    }

    /**
     * @class WeakRef
     * @brief Smart weak reference-counted pointer to type T 
    */
    template<typename T>
    class WeakRef {
    public:
        WeakRef() = default;

        WeakRef(std::nullptr_t){};

        explicit WeakRef(T* object) {
            m_ptr = std::move(weak_ref_access(object));
        }

        WeakRef(const WeakRef& p) {
            m_ptr = p.m_ptr;
        }

        WeakRef(const Ref<T>& p) {
            m_ptr = std::move(weak_ref_access(p.get()));
        }

        template<typename G, typename std::enable_if_t<std::is_convertible_v<G*, T*>> = true>
        WeakRef(const WeakRef<G>& p) {
            m_ptr = p.m_ptr;
        }

        template<typename G, typename std::enable_if_t<std::is_convertible_v<G*, T*>> = true>
        WeakRef(const Ref<G>& p) {
            m_ptr = weak_ref_access(p.get());
        }

        WeakRef(WeakRef&& p) noexcept {
            m_ptr = std::move(p.m_ptr);
        }

        template<typename G, typename std::enable_if_t<std::is_convertible_v<G*, T*>> = true>
        WeakRef(WeakRef<G>&& p) noexcept {
            m_ptr = std::move(p.m_ptr);
        }

        ~WeakRef() = default;

        WeakRef& operator=(const WeakRef& p) {
            m_ptr = p.ptr;
            return *this;
        }

        WeakRef& operator=(WeakRef&& p) noexcept {
            m_ptr = std::move(p.m_ptr);
            return *this;
        }

        bool operator==(const WeakRef& p) const {
            return m_ptr == p.m_ptr;
        }

        bool operator!=(const WeakRef& p) const {
            return m_ptr != p.m_ptr;
        }

        operator bool() const {
            return m_ptr != nullptr;
        }

        template<typename G, typename std::enable_if_t<std::is_convertible_v<T*, G*>> = true>
        operator WeakRef<G>() const {
            return WeakRef<G>(*this);
        }

        Ref<T> acquire() const {
            if (!m_ptr) {
                return Ref<T>();
            }
            std::optional<Ref<RefCnt>> access = m_ptr->try_acquire_object();
            if (!access) {
                return Ref<T>();
            }
            return std::move(access.value().template cast<T>());
        }

        template<typename G>
        Ref<G> acquire_cast() const {
            Ref<T> ref = std::move(acquire());
            return std::move(ref.template cast<G>());
        }

        void reset(T* object = nullptr) {
            m_ptr = std::move(weak_ref_access(object));
        }

        template<class G, typename std::enable_if_t<std::is_convertible_v<T*, G*>> = true>
        WeakRef<G> as() const {
            return WeakRef<G>(m_ptr);
        }

    private:
        Ref<WeakRefAccess> m_ptr;
    };

}// namespace wmoge

namespace std {

    template<typename T>
    struct hash<wmoge::WeakRef<T>> {
    public:
        std::size_t operator()(const wmoge::WeakRef<T>& ref) const {
            return std::hash<T*>()(ref.get());
        }
    };

}// namespace std