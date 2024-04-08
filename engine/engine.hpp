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

#include "audio/audio_bus.hpp"
#include "audio/audio_defs.hpp"
#include "audio/audio_engine.hpp"
#include "audio/audio_playback.hpp"

#include "core/array_view.hpp"
#include "core/async.hpp"
#include "core/buffered_vector.hpp"
#include "core/callback_stream.hpp"
#include "core/class.hpp"
#include "core/cmd_line.hpp"
#include "core/crc32.hpp"
#include "core/data.hpp"
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/log.hpp"
#include "core/mask.hpp"
#include "core/object.hpp"
#include "core/pool_vector.hpp"
#include "core/random.hpp"
#include "core/ref.hpp"
#include "core/signal.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/string_utf.hpp"
#include "core/string_utils.hpp"
#include "core/synchronization.hpp"
#include "core/task.hpp"
#include "core/task_hnd.hpp"
#include "core/task_manager.hpp"
#include "core/task_parallel_for.hpp"
#include "core/timer.hpp"
#include "core/typed_array.hpp"
#include "core/typed_map.hpp"
#include "core/uuid.hpp"
#include "core/var.hpp"
#include "core/weak_ref.hpp"

#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_memory.hpp"
#include "ecs/ecs_registry.hpp"
#include "ecs/ecs_system.hpp"
#include "ecs/ecs_world.hpp"

#include "event/event.hpp"
#include "event/event_action.hpp"
#include "event/event_filesystem.hpp"
#include "event/event_input.hpp"
#include "event/event_listener.hpp"
#include "event/event_manager.hpp"
#include "event/event_resource.hpp"
#include "event/event_scene.hpp"
#include "event/event_script.hpp"
#include "event/event_token.hpp"
#include "event/event_window.hpp"

#include "gameplay/action_manager.hpp"
#include "gameplay/action_map.hpp"
#include "gameplay/game_token_manager.hpp"

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_dynamic_buffers.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_pipeline_cache.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_texture.hpp"
#include "gfx/gfx_vector.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "gfx/gfx_vert_format_cache.hpp"

#include "grc/grc_shader.hpp"
#include "grc/grc_shader_manager.hpp"
#include "grc/grc_shader_param.hpp"
#include "grc/grc_shader_param_block.hpp"
#include "grc/grc_shader_pass.hpp"
#include "grc/grc_shader_reflection.hpp"
#include "grc/grc_shader_script.hpp"
#include "grc/grc_shader_script_builder.hpp"
#include "grc/grc_texture_compression.hpp"
#include "grc/grc_texture_manager.hpp"
#include "grc/grc_texture_resize.hpp"

#include "hgfx/hgfx_pass.hpp"
#include "hgfx/hgfx_pass_base.hpp"
#include "hgfx/hgfx_pass_text.hpp"

#include "io/archive.hpp"
#include "io/archive_file.hpp"
#include "io/archive_memory.hpp"
#include "io/base64.hpp"
#include "io/compression.hpp"
#include "io/enum.hpp"
#include "io/serialization.hpp"
#include "io/yaml.hpp"

#include "math/color.hpp"
#include "math/frustum.hpp"
#include "math/mat.hpp"
#include "math/math_utils.hpp"
#include "math/math_utils2d.hpp"
#include "math/math_utils3d.hpp"
#include "math/plane.hpp"
#include "math/quat.hpp"
#include "math/range.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"

#include "memory/mem_linear.hpp"
#include "memory/mem_pool.hpp"

#include "mesh/mesh_batch.hpp"
#include "mesh/mesh_bucket.hpp"
#include "mesh/mesh_builder.hpp"
#include "mesh/mesh_pass.hpp"

#include "pfx/pfx_component.hpp"
#include "pfx/pfx_emitter.hpp"
#include "pfx/pfx_feature.hpp"

#include "platform/application.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file.hpp"
#include "platform/file_system.hpp"
#include "platform/input.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"
#include "platform/mount_volume.hpp"
#include "platform/time.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"

#include "rtti/callable.hpp"
#include "rtti/class.hpp"
#include "rtti/enum.hpp"
#include "rtti/meta_data.hpp"
#include "rtti/struct.hpp"
#include "rtti/traits.hpp"
#include "rtti/type.hpp"
#include "rtti/type_storage.hpp"

#include "render/aux_draw_manager.hpp"
#include "render/camera.hpp"
#include "render/canvas.hpp"
#include "render/culling.hpp"
#include "render/graphics_pipeline.hpp"
#include "render/light.hpp"
#include "render/model_instance.hpp"
#include "render/render_engine.hpp"
#include "render/render_queue.hpp"
#include "render/render_scene.hpp"
#include "render/shader_manager.hpp"
#include "render/shader_pass.hpp"
#include "render/view.hpp"
#include "render/view_manager.hpp"

#include "resource/array_mesh.hpp"
#include "resource/audio_stream.hpp"
#include "resource/config_file.hpp"
#include "resource/font.hpp"
#include "resource/image.hpp"
#include "resource/material.hpp"
#include "resource/mesh.hpp"
#include "resource/model.hpp"
#include "resource/pfx_effect.hpp"
#include "resource/prefab.hpp"
#include "resource/resource.hpp"
#include "resource/resource_loader.hpp"
#include "resource/resource_manager.hpp"
#include "resource/resource_meta.hpp"
#include "resource/resource_pak.hpp"
#include "resource/resource_ref.hpp"
#include "resource/scene_packed.hpp"
#include "resource/scene_tree_packed.hpp"
#include "resource/script.hpp"
#include "resource/shader.hpp"
#include "resource/texture.hpp"

#include "scene/scene.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_data.hpp"
#include "scene/scene_entity.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene_node.hpp"
#include "scene/scene_node_props.hpp"
#include "scene/scene_tree.hpp"

#include "scripting/script_instance.hpp"
#include "scripting/script_native.hpp"
#include "scripting/script_system.hpp"

#include "system/engine.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"
#include "system/layer.hpp"
#include "system/plugin.hpp"
#include "system/plugin_manager.hpp"

#include "debug/console.hpp"
#include "debug/profiler.hpp"