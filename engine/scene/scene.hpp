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

#ifndef WMOGE_SCENE_HPP
#define WMOGE_SCENE_HPP

#include "core/class.hpp"
#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "ecs/ecs_world.hpp"
#include "platform/window.hpp"
#include "scene/scene_camera.hpp"
#include "scene/scene_transform.hpp"

#include <memory>

namespace wmoge {

    /**
     * @class Scene
     * @brief Scene objects container representing running game state
     */
    class Scene final : public RefCnt {
    public:
        Scene(StringId name = StringId());
        ~Scene() override = default;

        [[nodiscard]] const StringId&        get_name();
        [[nodiscard]] EcsWorld*              get_ecs_world();
        [[nodiscard]] SceneTransformManager* get_transforms();
        [[nodiscard]] CameraManager*         get_cameras();

    private:
        std::unique_ptr<EcsWorld>              m_ecs_world;
        std::unique_ptr<SceneTransformManager> m_transforms;
        std::unique_ptr<CameraManager>         m_cameras;

        StringId m_name;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_HPP
