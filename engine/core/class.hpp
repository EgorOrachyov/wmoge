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

#ifndef WMOGE_CLASS_HPP
#define WMOGE_CLASS_HPP

#include "core/log.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @class Property
     * @brief Accessible object class property
     */
    class Property final {
    public:
        Property(VarType type, StringId name, StringId getter = StringId(), StringId setter = StringId());

        const StringId& name() const { return m_name; };
        const StringId& getter() const { return m_getter; };
        const StringId& setter() const { return m_setter; };
        VarType         type() const { return m_type; };

        bool has_setter() const { return !m_setter.empty(); }
        bool has_getter() const { return !m_getter.empty(); }

    private:
        StringId m_name;
        StringId m_getter;
        StringId m_setter;
        VarType  m_type;
    };

    /**
     * @class Method
     * @brief Callable object class public member method
     */
    class Method final {
    public:
        using Call = std::function<int(const Method&, Object*, int, const Var*, Var&)>;

        Method(VarType ret, StringId name, std::vector<StringId> args);

        int call(Object* object, int argc, const Var* argv, Var& ret) const;

        const std::vector<StringId>& args_names() const { return m_args_names; }
        const std::vector<Var>&      args_values() const { return m_args_values; }
        const std::size_t            args_count() const { return m_args_names.size(); }
        const StringId&              name() const { return m_name; }
        VarType                      ret() const { return m_ret; }

        bool has_ret() const { return m_ret != VarType::Nil; }

    private:
        friend class Class;

        Call                  m_callable;
        std::vector<StringId> m_args_names;
        std::vector<Var>      m_args_values;
        StringId              m_name;
        VarType               m_ret;
    };

    /**
     * @class ClassDB
     * @brief Stores information about all registered engine object classes
     */
    using ClassDB = std::unordered_map<StringId, std::unique_ptr<class Class>>;

    /**
     * @class Class
     * @brief Storage for reflection information of particular object-hierarchy class
     */
    class Class final {
    public:
        bool has_super() const { return !m_super_name.empty(); }

        const StringId& name() const { return m_name; }
        const StringId& super_name() const { return m_super_name; }
        std::size_t     size() const { return m_size; }
        const Class*    super() const;
        const Property* property(const StringId& name) const;
        const Method*   method(const StringId& name) const;
        ref_ptr<Object> instantiate() const;
        bool            is_inherited_from(const StringId& name) const;

        static Class*   class_ptr(StringId name);
        static Class*   register_class(const StringId& name, const StringId& super, std::size_t size, std::function<Object*()> instantiate);
        static ClassDB* class_db();

        template<class T>
        static Class* register_class();

        template<typename Callable>
        Class* set_instantiate(Callable&& instantiate);

        template<typename T, typename... Args>
        Class* add_method(Method method, void (T::*p_method)(Args...), const std::vector<Var>& defaults);

        template<typename T, typename R, typename... Args>
        Class* add_method(Method method, R (T::*p_method)(Args...), const std::vector<Var>& defaults);

        Class* add_property(Property property);

    private:
        friend class Main;
        static void register_types();

    private:
        StringId                               m_name;
        StringId                               m_super_name;
        std::size_t                            m_size;
        std::function<Object*()>               m_instantiate;
        std::unordered_map<StringId, Property> m_properties;
        std::unordered_map<StringId, Method>   m_methods;
        std::unordered_set<StringId>           m_supers;
    };

    template<class T>
    Class* Class::register_class() {
        return register_class(T::class_name_static(), T::super_class_name_static(), sizeof(T), []() { return new T(); });
    }

    template<typename Callable>
    Class* Class::set_instantiate(Callable&& instantiate) {
        m_instantiate = std::move(std::function(instantiate));
        return this;
    }

    template<typename T, typename... Args>
    Class* Class::add_method(wmoge::Method method, void (T::*p_method)(Args...), const std::vector<Var>& defaults) {
        std::vector<Var> args_values(method.args_count());
        std::copy(defaults.begin(), defaults.end(), args_values.begin() + (method.args_count() - defaults.size()));
        method.m_args_values = std::move(args_values);
        method.m_callable    = [=](const Method& m, Object* obj, int argc, const Var* argv, Var&) -> int {
            assert(obj);
            static_assert(sizeof...(Args) <= 4, "methods args count support is limited");

            const std::size_t       to_fill = sizeof...(Args);
            const Var*              argvp[to_fill + 1];
            const std::vector<Var>& defaults = method.args_values();
            std::size_t             it       = 0;

            while (it < to_fill && argc > 0) {
                argvp[it] = &argv[it];
                it += 1;
                argc -= 1;
            }
            while (it < to_fill) {
                argvp[it] = &defaults[it];
                it += 1;
            }

            T* target = (T*) obj;

            if constexpr (sizeof...(Args) == 0)
                (*target.*p_method)();
            else if constexpr (sizeof...(Args) == 1)
                (*target.*p_method)(*argvp[0]);
            else if constexpr (sizeof...(Args) == 2)
                (*target.*p_method)(*argvp[0], *argvp[1]);
            else if constexpr (sizeof...(Args) == 3)
                (*target.*p_method)(*argvp[0], *argvp[1], *argvp[2]);
            else if constexpr (sizeof...(Args) == 4)
                (*target.*p_method)(*argvp[0], *argvp[1], *argvp[2], *argvp[3]);

            return 0;
        };
        m_methods.emplace(method.name(), std::move(method));
        return this;
    }

    template<typename T, typename R, typename... Args>
    Class* Class::add_method(Method method, R (T::*p_method)(Args...), const std::vector<Var>& defaults) {
        std::vector<Var> args_values(method.args_count());
        std::copy(defaults.begin(), defaults.end(), args_values.begin() + (method.args_count() - defaults.size()));
        method.m_args_values = std::move(args_values);
        method.m_callable    = [=](const Method& m, Object* obj, int argc, const Var* argv, Var& res) -> int {
            assert(obj);
            static_assert(sizeof...(Args) <= 4, "methods args count support is limited");

            const std::size_t       to_fill = sizeof...(Args);
            const Var*              argvp[to_fill + 1];
            const std::vector<Var>& defaults = method.args_values();
            std::size_t             it       = 0;

            while (it < to_fill && argc > 0) {
                argvp[it] = &argv[it];
                it += 1;
                argc -= 1;
            }
            while (it < to_fill) {
                argvp[it] = &defaults[it];
                it += 1;
            }

            T* target = (T*) obj;

            if constexpr (sizeof...(Args) == 0)
                res = (*target.*p_method)();
            else if constexpr (sizeof...(Args) == 1)
                res = (*target.*p_method)(*argvp[0]);
            else if constexpr (sizeof...(Args) == 2)
                res = (*target.*p_method)(*argvp[0], *argvp[1]);
            else if constexpr (sizeof...(Args) == 3)
                res = (*target.*p_method)(*argvp[0], *argvp[1], *argvp[2]);
            else if constexpr (sizeof...(Args) == 4)
                res = (*target.*p_method)(*argvp[0], *argvp[1], *argvp[2], *argvp[3]);

            return 0;
        };
        m_methods.emplace(method.name(), std::move(method));
        return this;
    }

}// namespace wmoge

#endif//WMOGE_CLASS_HPP
