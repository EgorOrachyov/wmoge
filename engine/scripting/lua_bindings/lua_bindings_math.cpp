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

#include "lua_bindings_math.hpp"

namespace wmoge {

#define WG_ADD_VEC_PROPS(Vec, p) \
    addProperty(#p, &Vec::p, &Vec::set_##p)

#define WG_ADD_VEC_FUNCS_F(Vec)                                                                     \
    addFunction("__eq", std::function([](const Vec* v, const Vec& m) { return *v == m; }))          \
            .addFunction("__mul", std::function([](const Vec* v, float a) { return *v * a; }))      \
            .addFunction("__div", std::function([](const Vec* v, float a) { return *v / a; }))      \
            .addFunction("__add", std::function([](const Vec* v, const Vec& m) { return *v + m; })) \
            .addFunction("__sub", std::function([](const Vec* v, const Vec& m) { return *v - m; })) \
            .addFunction("__tostring", &Vec::to_string)                                             \
            .addFunction("to_string", &Vec::to_string)                                              \
            .addFunction("abs", &Vec::abs)                                                          \
            .addFunction("pow", &Vec::pow)                                                          \
            .addProperty("length2", &Vec::length2)                                                  \
            .addProperty("length", &Vec::length)                                                    \
            .addProperty("normalized", &Vec::normalized)                                            \
            .addStaticFunction("distance", &Vec::distance)                                          \
            .addStaticFunction("distance2", &Vec::distance2)                                        \
            .addStaticFunction("dot", &Vec::dot)                                                    \
            .addStaticFunction("angle", &Vec::angle)                                                \
            .addStaticFunction("cross", &Vec::cross)                                                \
            .addStaticFunction("triple", &Vec::triple)                                              \
            .addStaticFunction("lerp", &Vec::lerp)                                                  \
            .addStaticFunction("slerp", &Vec::slerp)                                                \
            .addStaticFunction("min", &Vec::min)                                                    \
            .addStaticFunction("max", &Vec::max)                                                    \
            .addStaticFunction("clamp", &Vec::clamp)

#define WG_ADD_VEC_FUNCS_I(Vec)                                                                     \
    addFunction("__eq", std::function([](const Vec* v, const Vec& m) { return *v == m; }))          \
            .addFunction("__mul", std::function([](const Vec* v, float a) { return *v * a; }))      \
            .addFunction("__div", std::function([](const Vec* v, float a) { return *v / a; }))      \
            .addFunction("__add", std::function([](const Vec* v, const Vec& m) { return *v + m; })) \
            .addFunction("__sub", std::function([](const Vec* v, const Vec& m) { return *v - m; })) \
            .addFunction("__tostring", &Vec::to_string)                                             \
            .addFunction("to_string", &Vec::to_string)                                              \
            .addFunction("abs", &Vec::abs)                                                          \
            .addFunction("pow", &Vec::pow)                                                          \
            .addProperty("length2", &Vec::length2)                                                  \
            .addProperty("length", &Vec::length)                                                    \
            .addProperty("normalized", &Vec::normalized)                                            \
            .addStaticFunction("distance", &Vec::distance)                                          \
            .addStaticFunction("distance2", &Vec::distance2)                                        \
            .addStaticFunction("dot", &Vec::dot)                                                    \
            .addStaticFunction("angle", &Vec::angle)                                                \
            .addStaticFunction("cross", &Vec::cross)                                                \
            .addStaticFunction("triple", &Vec::triple)                                              \
            .addStaticFunction("min", &Vec::min)                                                    \
            .addStaticFunction("max", &Vec::max)                                                    \
            .addStaticFunction("clamp", &Vec::clamp)

    void LuaBindings::bind_math(luabridge::Namespace& ns, flat_map<const Class*, LuaConvCppToLua>&) {
        ns = ns.beginClass<Vec2f>("Vec2f")
                     .addStaticFunction("new", std::function([](float x, float y) { return Vec2f(x, y); }))
                     .WG_ADD_VEC_PROPS(Vec2f, x)
                     .WG_ADD_VEC_PROPS(Vec2f, y)
                     .WG_ADD_VEC_FUNCS_F(Vec2f)
                     .endClass();

        ns = ns.beginClass<Vec3f>("Vec3f")
                     .addStaticFunction("new", std::function([](float x, float y, float z) { return Vec3f(x, y, z); }))
                     .WG_ADD_VEC_PROPS(Vec3f, x)
                     .WG_ADD_VEC_PROPS(Vec3f, y)
                     .WG_ADD_VEC_PROPS(Vec3f, z)
                     .WG_ADD_VEC_FUNCS_F(Vec3f)
                     .endClass();

        ns = ns.beginClass<Vec4f>("Vec4f")
                     .addStaticFunction("new", std::function([](float x, float y, float z, float w) { return Vec4f(x, y, z, w); }))
                     .WG_ADD_VEC_PROPS(Vec4f, x)
                     .WG_ADD_VEC_PROPS(Vec4f, y)
                     .WG_ADD_VEC_PROPS(Vec4f, z)
                     .WG_ADD_VEC_PROPS(Vec4f, w)
                     .WG_ADD_VEC_FUNCS_F(Vec4f)
                     .endClass();

        ns = ns.beginClass<Vec2i>("Vec2i")
                     .addStaticFunction("new", std::function([](int x, int y) { return Vec2i(x, y); }))
                     .WG_ADD_VEC_PROPS(Vec2i, x)
                     .WG_ADD_VEC_PROPS(Vec2i, y)
                     .WG_ADD_VEC_FUNCS_I(Vec2i)
                     .endClass();

        ns = ns.beginClass<Vec3i>("Vec3i")
                     .addStaticFunction("new", std::function([](int x, int y, int z) { return Vec3i(x, y, z); }))
                     .WG_ADD_VEC_PROPS(Vec3i, x)
                     .WG_ADD_VEC_PROPS(Vec3i, y)
                     .WG_ADD_VEC_PROPS(Vec3i, z)
                     .WG_ADD_VEC_FUNCS_I(Vec3i)
                     .endClass();

        ns = ns.beginClass<Vec4i>("Vec4i")
                     .addStaticFunction("new", std::function([](int x, int y, int z, int w) { return Vec4i(x, y, z, w); }))
                     .WG_ADD_VEC_PROPS(Vec4i, x)
                     .WG_ADD_VEC_PROPS(Vec4i, y)
                     .WG_ADD_VEC_PROPS(Vec4i, z)
                     .WG_ADD_VEC_PROPS(Vec4i, w)
                     .WG_ADD_VEC_FUNCS_I(Vec4i)
                     .endClass();

        ns = ns.beginClass<Mat2x2f>("Mat2x2f")
                     .addFunction("to_string", &Mat2x2f::to_string)
                     .endClass();

        ns = ns.beginClass<Mat3x3f>("Mat3x3f")
                     .addFunction("to_string", &Mat3x3f::to_string)
                     .endClass();

        ns = ns.beginClass<Mat4x4f>("Mat4x4f")
                     .addFunction("to_string", &Mat4x4f::to_string)
                     .endClass();

        ns = ns.beginClass<Quatf>("Quatf")
                     .addProperty("scalar", std::function([](const Quatf* q) { return q->scalar; }), std::function([](Quatf* q, float v) { q->scalar = v; }))
                     .addProperty("vec", std::function([](const Quatf* q) { return q->vec; }), std::function([](Quatf* q, Vec3f v) { q->vec = v; }))
                     .endClass();

        ns = ns.beginClass<Transform2d>("Transform2d")
                     .addConstructor<void (*)(void)>()
                     .addProperty("translation", &Transform2d::get_translation, &Transform2d::set_translation)
                     .addProperty("rotation", &Transform2d::get_rotation, &Transform2d::set_rotation)
                     .addProperty("scale", &Transform2d::get_scale, &Transform2d::set_scale)
                     .addFunction("translate", &Transform2d::translate)
                     .addFunction("rotate", &Transform2d::rotate)
                     .addFunction("scale", &Transform2d::scale)
                     .endClass();

        ns = ns.beginNamespace("Math")
                     .addFunction("clampf", &Math::clamp<float>)
                     .addConstant("PIf", Math::PIf)
                     .addConstant("HALF_PIf", Math::HALF_PIf)
                     .addConstant("QUARTER_PIf", Math::QUARTER_PIf)
                     .addConstant("SQRT2f", Math::SQRT2f)
                     .addConstant("Ef", Math::Ef)
                     .endNamespace();

        ns = ns.beginNamespace("Math2d")
                     .addFunction("intersects", &Math2d::intersects)
                     .endNamespace();

        ns = ns.beginNamespace("Color")
                     .addConstant("WHITE4f", &Color::WHITE4f)
                     .addConstant("BLACK4f", &Color::BLACK4f)
                     .addConstant("RED4f", &Color::RED4f)
                     .addConstant("GREEN4f", &Color::GREEN4f)
                     .addConstant("BLUE4f", &Color::BLUE4f)
                     .addConstant("YELLOW4f", &Color::YELLOW4f)
                     .addConstant("WHITE3f", &Color::WHITE3f)
                     .addConstant("BLACK3f", &Color::BLACK3f)
                     .addConstant("RED3f", &Color::RED3f)
                     .addConstant("GREEN3f", &Color::GREEN3f)
                     .addConstant("BLUE3f", &Color::BLUE3f)
                     .addConstant("YELLOW3f", &Color::YELLOW3f)
                     .endNamespace();
    }

}// namespace wmoge
