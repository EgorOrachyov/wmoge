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

#include "register_classes_asset.hpp"

#include "asset/array_mesh.hpp"
#include "asset/asset.hpp"
#include "asset/audio_stream.hpp"
#include "asset/audio_stream_wav.hpp"
#include "asset/config_file.hpp"
#include "asset/font.hpp"
#include "asset/image.hpp"
#include "asset/material.hpp"
#include "asset/mesh.hpp"
#include "asset/model.hpp"
#include "asset/pfx_effect.hpp"
#include "asset/prefab.hpp"
#include "asset/scene_packed.hpp"
#include "asset/scene_tree_packed.hpp"
#include "asset/script.hpp"
#include "asset/shader.hpp"
#include "asset/texture.hpp"

namespace wmoge {

    void register_classes_asset() {
        Asset::register_class();
        ArrayMesh::register_class();
        AudioStream::register_class();
        AudioStreamWav::register_class();
        Image::register_class();
        Font::register_class();
        ConfigFile::register_class();
        Mesh::register_class();
        Shader::register_class();
        Texture::register_class();
        Texture2d::register_class();
        TextureCube::register_class();
        Material::register_class();
        Model::register_class();
        Script::register_class();
        PfxEffect::register_class();
        Prefab::register_class();
        ScenePacked::register_class();
        SceneTreePacked::register_class();
    }

}// namespace wmoge