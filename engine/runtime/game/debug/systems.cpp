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

#include "systems.hpp"

namespace wmoge {

    static bool gm_debug_cull_item(const Vec3f& cam_pos, const Vec3f& pos, int entity_idx, EcsQueryContext& query) {
        if (!query.has_component<GmDebugDistMinMaxComponent>()) {
            return true;
        }
        const float dist   = Vec3f::distance(cam_pos, pos);
        auto&       bounds = query.get_component<GmDebugDistMinMaxComponent>(entity_idx);

        return bounds.dist_min <= dist && dist <= bounds.dist_max;
    }

    void gm_draw_debug_label_system(AuxDrawManager* draw_manager, Vec3f cam_pos, EcsQueryContext& query) {
        query.for_each([&](int entity_idx) {
            auto& label   = query.get_component<GmDebugLabelComponent>(entity_idx);
            auto& mat_l2w = query.get_component<GmMatLocalToWorldComponent>(entity_idx);

            const Vec3f pos = Math3d::extract_translation(mat_l2w.m);

            if (!gm_debug_cull_item(cam_pos, pos, entity_idx, query)) {
                return;
            }

            draw_manager->draw_text_3d(label.text, pos, label.size, label.color);
        });
    }

    void gm_draw_debug_primitive_system(AuxDrawManager* draw_manager, Vec3f cam_pos, bool solid, EcsQueryContext& query) {
        query.for_each([&](int entity_idx) {
            auto& prim    = query.get_component<GmDebugPrimitiveComponent>(entity_idx);
            auto& mat_l2w = query.get_component<GmMatLocalToWorldComponent>(entity_idx);

            Vec3f pos;
            Vec3f scale;
            Quatf rot;
            Math3d::decompose(mat_l2w.m, pos, scale, rot);

            if (gm_debug_cull_item(cam_pos, pos, entity_idx, query)) {
                return;
            }

            if (prim.type == GmDebugPrimitiveType::Box) {
                draw_manager->draw_box(pos, scale, prim.color, rot, solid);
            }
            if (prim.type == GmDebugPrimitiveType::Sphere) {
                draw_manager->draw_sphere(pos, scale.x(), prim.color, solid);
            }
            if (prim.type == GmDebugPrimitiveType::Cone) {
                draw_manager->draw_cone(pos, scale.x(), scale.y(), prim.color, rot, solid);
            }
            if (prim.type == GmDebugPrimitiveType::Cylinder) {
                draw_manager->draw_cylinder(pos, scale.x(), scale.y(), prim.color, rot, solid);
            }
        });
    }

}// namespace wmoge