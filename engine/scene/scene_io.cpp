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

#include "scene_io.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, SceneDataCamera& data) {
        WG_YAML_READ_AS_OPT(node, "color", data.color);
        WG_YAML_READ_AS_OPT(node, "viewport", data.viewport);
        WG_YAML_READ_AS_OPT(node, "fov", data.fov);
        WG_YAML_READ_AS_OPT(node, "near", data.near);
        WG_YAML_READ_AS_OPT(node, "far", data.far);
        WG_YAML_READ_AS_OPT(node, "target", data.target);
        WG_YAML_READ_AS_OPT(node, "projection", data.projection);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const SceneDataCamera& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "color", data.color);
        WG_YAML_WRITE_AS(node, "viewport", data.viewport);
        WG_YAML_WRITE_AS(node, "fov", data.fov);
        WG_YAML_WRITE_AS(node, "near", data.near);
        WG_YAML_WRITE_AS(node, "far", data.far);
        WG_YAML_WRITE_AS(node, "target", data.target);
        WG_YAML_WRITE_AS(node, "projection", data.projection);

        return StatusCode::Ok;
    }

}// namespace wmoge