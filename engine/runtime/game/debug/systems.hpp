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

#include "ecs/ecs_query.hpp"
#include "game/debug/components.hpp"
#include "game/transform/components.hpp"
#include "render/aux_draw_manager.hpp"

namespace wmoge {

    struct GmDebugLabelAccess : EcsAccess {
        GmDebugLabelAccess() {
            add<GmDebugLabelComponent>();
            add<GmDebugDistMinMaxComponent>(EcsComponentPresence::Optional);
            add<GmMatLocalToWorldComponent>();
        }
    };

    struct GmDebugPrimitiveAccess : EcsAccess {
        GmDebugPrimitiveAccess() {
            add<GmDebugPrimitiveComponent>();
            add<GmDebugDistMinMaxComponent>(EcsComponentPresence::Optional);
            add<GmMatLocalToWorldComponent>();
        }
    };

    /**
     * @brief Renders entities with debug label in 3d space
     * 
     * @param draw_manager Aux draw manager for debug rendering
     * @param cam_pos Main camera pos for culling
     * @param query Ecs entities to draw
     */
    void gm_draw_debug_label_system(AuxDrawManager* draw_manager, Vec3f cam_pos, EcsQuery<GmDebugLabelAccess>& query);

    /**
     * @brief Renders entities with debug primitive shape in 3d space
     * 
     * @param draw_manager Aux draw manager for debug rendering
     * @param cam_pos Main camera pos for culling
     * @param query Ecs entities to draw
     */
    void gm_draw_debug_primitive_system(AuxDrawManager* draw_manager, Vec3f cam_pos, EcsQuery<GmDebugPrimitiveAccess>& query);

}// namespace wmoge