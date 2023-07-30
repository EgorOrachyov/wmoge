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

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @class ClassMember
     * @brief Base class for any class member (field, property, method, etc.)
     */
    class ClassMember {
    public:
        ClassMember(StringId name);

        [[nodiscard]] const StringId& name() const { return m_name; };

    private:
        friend class Class;

        StringId m_name;
    };

    /**
     * @class ClassProperty
     * @brief Accessible object class property
     */
    class ClassProperty : public ClassMember {
    public:
        ClassProperty(VarType type, StringId name, StringId getter = StringId(), StringId setter = StringId());

        [[nodiscard]] const StringId& getter() const { return m_getter; };
        [[nodiscard]] const StringId& setter() const { return m_setter; };
        [[nodiscard]] VarType         type() const { return m_type; };

        [[nodiscard]] bool has_setter() const { return !m_setter.empty(); }
        [[nodiscard]] bool has_getter() const { return !m_getter.empty(); }

    private:
        friend class Class;

        StringId m_getter;
        StringId m_setter;
        VarType  m_type;
    };

    /**
     * @class ClassField
     * @brief Native object class filed type and access info
     */
    class ClassField final : public ClassProperty {
    public:
        ClassField(VarType type, StringId name);

        [[nodiscard]] int native_size() const { return m_native_size; }
        [[nodiscard]] int native_offset() const { return m_native_offset; }

    private:
        friend class Class;

        int m_native_size   = -1;
        int m_native_offset = -1;
    };

    /**
     * @class Method
     * @brief Callable object class public member method
     */
    class ClassMethod final : public ClassMember {
    public:
        /**
         * @class Call
         * @brief Function type used to execute method on an class instance
         */
        using Call = std::function<int(const ClassMethod&, Object*, int, const Var*, Var&)>;

        ClassMethod(VarType ret, StringId name, std::vector<StringId> args);

        /**
         * @brief Call this method on object instance
         *
         * @param object Object instance to call method on
         * @param argc Number of arguments passed
         * @param argv Array of arguments
         * @param ret Reference to value to return result if has
         *
         * @return Zero-status on successful call
         */
        int call(Object* object, int argc, const Var* argv, Var& ret) const;

        [[nodiscard]] const std::vector<StringId>& args_names() const { return m_args_names; }
        [[nodiscard]] const std::vector<Var>&      args_values() const { return m_args_values; }
        [[nodiscard]] std::size_t                  args_count() const { return m_args_names.size(); }
        [[nodiscard]] VarType                      ret() const { return m_ret; }

        [[nodiscard]] bool has_ret() const { return m_ret != VarType::Nil; }

    private:
        friend class Class;

        Call                  m_callable;
        std::vector<StringId> m_args_names;
        std::vector<Var>      m_args_values;
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
        [[nodiscard]] const StringId&           name() const { return m_name; }
        [[nodiscard]] const StringId&           super_name() const { return m_super_name; }
        [[nodiscard]] std::size_t               size() const { return m_size; }
        [[nodiscard]] const Class*              super() const;
        [[nodiscard]] const ClassProperty*      property(const StringId& name) const;
        [[nodiscard]] const ClassField*         field(const StringId& name) const;
        [[nodiscard]] const ClassMethod*        method(const StringId& name) const;
        [[nodiscard]] std::vector<ClassMember*> members() const;

        [[nodiscard]] bool has_super() const { return !m_super_name.empty(); }
        [[nodiscard]] bool is_inherited_from(const StringId& name) const;

        [[nodiscard]] Ref<Object> instantiate() const;

        static Class*   class_ptr(StringId name);
        static Class*   register_class(const StringId& name, const StringId& super, std::size_t size, std::function<Object*()> instantiate);
        static ClassDB* class_db();

        template<class T>
        static Class* register_class();

        template<typename Callable>
        Class* set_instantiate(Callable&& instantiate);

        template<typename T, typename... Args>
        Class* add_method(ClassMethod method, void (T::*p_method)(Args...), const std::vector<Var>& defaults);

        template<typename T, typename R, typename... Args>
        Class* add_method(ClassMethod method, R (T::*p_method)(Args...), const std::vector<Var>& defaults);

        template<typename T, typename F>
        Class* add_field(ClassField field, F T::*p_field, Var default_value = Var());

        Class* add_property(ClassProperty property);

    private:
        friend class Main;
        static void register_types();

    private:
        std::function<Object*()>                     m_instantiate;
        std::unordered_map<StringId, ClassProperty*> m_properties;
        std::unordered_map<StringId, ClassField*>    m_fields;
        std::unordered_map<StringId, ClassMethod*>   m_methods;
        std::unordered_set<StringId>                 m_supers;
        std::vector<std::shared_ptr<ClassMember>>    m_members;
        std::size_t                                  m_size;
        StringId                                     m_name;
        StringId                                     m_super_name;
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
    Class* Class::add_method(wmoge::ClassMethod method, void (T::*p_method)(Args...), const std::vector<Var>& defaults) {
        std::vector<Var> args_values(method.args_count());
        std::copy(defaults.begin(), defaults.end(), args_values.begin() + int(method.args_count() - defaults.size()));

        method.m_args_values = std::move(args_values);
        method.m_callable    = [=](const ClassMethod& m, Object* obj, int argc, const Var* argv, Var&) -> int {
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

        auto ptr_method = std::make_shared<ClassMethod>(std::move(method));
        m_members.push_back(ptr_method);
        m_methods.emplace(ptr_method->name(), ptr_method.get());

        return this;
    }

    template<typename T, typename R, typename... Args>
    Class* Class::add_method(ClassMethod method, R (T::*p_method)(Args...), const std::vector<Var>& defaults) {
        std::vector<Var> args_values(method.args_count());
        std::copy(defaults.begin(), defaults.end(), args_values.begin() + int(method.args_count() - defaults.size()));

        method.m_args_values = std::move(args_values);
        method.m_callable    = [=](const ClassMethod& m, Object* obj, int argc, const Var* argv, Var& res) -> int {
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

        auto ptr_method = std::make_shared<ClassMethod>(std::move(method));
        m_members.push_back(ptr_method);
        m_methods.emplace(ptr_method->name(), ptr_method.get());

        return this;
    }

    template<typename T, typename F>
    Class* Class::add_field(ClassField field, F T::*p_field, Var default_value) {
        union {
            F T::*        val_ptr;
            std::uint64_t val_uint;
        } field_offset;

        field_offset.val_ptr = p_field;

        field.m_native_size   = sizeof(F);
        field.m_native_offset = int(field_offset.val_uint);
        field.m_getter        = SID("__" + field.name().str() + "_getter");
        field.m_setter        = SID("__" + field.name().str() + "_setter");

        ClassMethod getter(field.type(), field.m_getter, {});
        ClassMethod setter(field.type(), field.m_setter, {SID("value")});

        getter.m_callable = [=](const ClassMethod&, Object* obj, int, const Var*, Var& res) -> int {
            assert(obj);

            T* target = (T*) obj;
            res       = (*target.*p_field);

            return 0;
        };

        setter.m_args_values = {std::move(default_value)};
        setter.m_callable    = [=](const ClassMethod& m, Object* obj, int argc, const Var* argv, Var& res) -> int {
            assert(obj);

            const std::vector<Var>& defaults = m.args_values();
            const Var*              argvp[1] = {argc > 0 ? &argv[0] : &defaults[0]};

            T* target          = (T*) obj;
            (*target.*p_field) = *argvp[0];

            return 0;
        };

        auto ptr_getter = std::make_shared<ClassMethod>(std::move(getter));
        auto ptr_setter = std::make_shared<ClassMethod>(std::move(setter));
        auto ptr_field  = std::make_shared<ClassField>(field);

        m_members.push_back(ptr_getter);
        m_members.push_back(ptr_setter);
        m_members.push_back(ptr_field);

        m_methods.emplace(ptr_getter->name(), ptr_getter.get());
        m_methods.emplace(ptr_setter->name(), ptr_setter.get());
        m_properties.emplace(ptr_field->name(), ptr_field.get());
        m_fields.emplace(ptr_field->name(), ptr_field.get());

        return this;
    }

}// namespace wmoge

#endif//WMOGE_CLASS_HPP
