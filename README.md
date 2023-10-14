![logo](https://github.com/EgorOrachyov/wmoge/raw/main/docs/media/wmoge-banner-trsp.png?raw=true&sanitize=true)

# Wmoge Engine

[![build](https://github.com/EgorOrachyov/wmoge/actions/workflows/build.yml/badge.svg)](https://github.com/EgorOrachyov/wmoge/actions/workflows/build.yml)
[![clang format](https://github.com/EgorOrachyov/wmoge/actions/workflows/clang-format.yml/badge.svg)](https://github.com/EgorOrachyov/wmoge/actions/workflows/clang-format.yml)
[![docs c/c++](https://github.com/EgorOrachyov/wmoge/actions/workflows/docs-cxx.yml/badge.svg)](https://github.com/EgorOrachyov/wmoge/actions/workflows/docs-cxx.yml)
![language](https://img.shields.io/badge/language-C++17-_)
![license](https://img.shields.io/badge/license-MIT-_)

**"Write my own game engine (wmoge)"** is a modern high-performance 2d and 3d graphics game engine with Vulkan and
OpenGL backends. The idea of the project is to implement a self-sufficient, small, but featured engine and a number of
2d and 3d games using it. Primary focus is the core architecture of the engine, reliability, clean and performant code.
Currently, the work is focused on a minor refactoring, new runtime scene representation, building a high-level rendering
system, refactoring audio, scripting systems, and bringing physics support. The engine is a personal learning/hobby
project for educational purposes only.

* **Project C/C++ and Python documentation:** [egororachyov.github.io/wmoge](https://egororachyov.github.io/wmoge)
* **Current progress and task
  page:** [github.com/EgorOrachyov/wmoge/issues](https://github.com/EgorOrachyov/wmoge/issues).
* **Contributing
  guide:** [github.com/EgorOrachyov/wmoge/blob/main/CONTRIBUTING.md](https://github.com/EgorOrachyov/wmoge/blob/main/CONTRIBUTING.md)
* **Source code:** [github.com/EgorOrachyov/wmoge](https://github.com/EgorOrachyov/wmoge)

![gif](https://github.com/EgorOrachyov/wmoge/raw/main/docs/media/background.gif?raw=true&sanitize=true)
> Work in progress, new features coming soon! This 2d demo is an old build!

## About the engine

### Features

* 🖥️ Platform-agnostic, with support for common operating systems such as Windows, Linux and macOS.
* 🔨 Moder C++17 based engine with clear architecture.
* 🔨 Runtime reflection system with class, method and property information.
* 🏎️ Task manager and task graph support for concurrent execution.
* 🏎️ Custom async (future & promise) implementation with join, notify and continuation.
* ⚙ Simple stack-based config system.
* 🎥 Abstract graphics (gfx) device interface for GPU driver communication.
* 🎥 Vulkan-based gfx device backend.
* 🎥 (in progress) OpenGL-based gfx device backend.
* 🎨 Automated generation of shader passes with automated layout creation, compilation and reloading.
* 🎨 Gpu pipelines runtime and offline cache to speed-up rendering.
* 🎨 Lightweight runtime and offline shader cache system to speed-up shaders compilation.
* 🎨 High-level shaders and render-passes with generated static reflection from a python script.
* 📦 Async resource-loading system with runtime cache.
* 📦 Unified resources handling, meta-information, support for external formats through custom loaders.
* 📦 Yaml and binary serialization support.
* 📦 Gpu texture compression into BCn (1-7) format for faster loading, rendering and lower memory usage.
* 🅰️ Custom math library with 2d and 3d-space primitives.
* 🧱 Ecs-based scene model with fast, parallel and memory-friendly update.
* 🧱 Tree-based object model for editor (offline) scene description.
* 🎧 (in progress) OpenAL audio renderer for playing game sounds.
* 📜 (in progress) Modern Lua scripting backend for game logic programming.
* 🐞 Built-in CPU performance and tasking profiling with support to google trace exporting.
* 🐞 In-game dev console with support for console commands and console variables.
* 🐞 Debug labels objects and events for RenderDoc captures.

### Platforms

* Windows 10 (tested on Windows 10).
* Linux-based OS (tested on Ubuntu 20.04).
* macOS (tested on 10.14 Mojave).

### Dependencies

* [assimp](https://github.com/assimp/assimp) for geometry, material and scene assets importing into engine.
* [audio file](https://github.com/adamstark/AudioFile) for simple audio loading from wav files.
* [base64](https://github.com/ReneNyffenegger/cpp-base64) for base64 encoding/decoding in text serialization.
* [compressonator](https://github.com/GPUOpen-Tools/compressonator) texture compression library.
* [cxxopts](https://github.com/jarro2783/cxxopts) for command-line options processing.
* [filewatch](https://github.com/ThomasMonkman/filewatch) for OS file system events tracking.
* [freetype](https://github.com/freetype/freetype) for true-type fonts loading and rendering.
* [glfw](https://github.com/glfw/glfw) for cross-platform window and input management.
* [glslang](https://github.com/KhronosGroup/glslang) for runtime glsl to spir-v shaders compilation.
* [lua](https://github.com/walterschell/Lua) project C source with cmake script for scripting.
* [lua bridge](https://github.com/vinniefalco/LuaBridge) to simplify lua and C++ bindings.
* [lz4](https://github.com/lz4/lz4) for fast compression and decompression at runtime.
* [magic enum](https://github.com/Neargye/magic_enum) for static enum reflection.
* [openal soft](https://github.com/kcat/openal-soft) as a software audio renderer.
* [rapidyaml](https://github.com/biojppm/rapidyaml) for fast yaml files parsing and serialization.
* [robin hood hashing](https://github.com/martinus/robin-hood-hashing) for fast & memory efficient hashtable.
* [stbimage](https://github.com/nothings/stb) image utilities for images loading, writing and resizing.
* [svector](https://github.com/martinus/svector) compact SVO optimized vector for C++17 or higher.
* [tinyxml2](https://github.com/leethomason/tinyxml2) for utf-8 xml files processing.
* [vma](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) vulkan library for memory allocation.
* [volk](https://github.com/zeux/volk) meta loader for vulkan API.
* [whereami](https://github.com/gpakosz/whereami) for executable location extracting.
* [zlib](https://github.com/madler/zlib) for files compression (required for assimp and freetype).

### References

* [Refactoring the Mesh Drawing Pipeline for Unreal Engine 4.22](https://www.youtube.com/watch?v=qx1c190aGhs)
* [Mesh Drawing Pipeline | Unreal Engine 4.27 Documentation](https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Rendering/MeshDrawingPipeline/)
* [Creating a Custom Mesh Component in UE4](https://medium.com/realities-io/creating-a-custom-mesh-component-in-ue4-part-0-intro-2c762c5f0cd6)
* [Master Class: The CRYENGINE Rendering Pipeline](https://www.youtube.com/watch?v=34S3onEr3r8)
* [Master Class: In-Depth Guide to Working with Particle Effects in CRYENGINE](https://www.youtube.com/watch?v=npPlJ0npfVQ)
* [CRYENGINE Tutorial - Art Asset Pipeline: PBR Texture Mapping](https://www.youtube.com/watch?v=hGFP_4TYL2o)
* [Master Class: How to assign Game Inputs through Actionmapping | CRYENGINE](https://www.youtube.com/watch?v=uTmHUIypR-4)
* [HypeHype Mobile Rendering Architecture](https://advances.realtimerendering.com/s2023/index.html)
* [Creating a Tools Pipeline for Horizon: Zero Dawn](https://www.youtube.com/watch?v=KRJkBxKv1VM)
* [The Future of Scene Description on 'God of War'](https://www.gdcvault.com/play/1026345/The-Future-of-Scene-Description)
* [The Indirect Lighting Pipeline of 'God of War'](https://www.gdcvault.com/play/1026323/The-Indirect-Lighting-Pipeline-of)
* [A Context-Aware Character Dialog System in The Last of Us](https://www.gdcvault.com/play/1020386/A-Context-Aware-Character-Dialog)
* [Parallelizing the Naughty Dog Engine Using Fibers](https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
* [Deferred Lighting in Uncharted 4](https://advances.realtimerendering.com/s2016/)
* [GPU Driven Effects of The Last of Us Part II by Artem Kovalovs || SIGGRAPH 2020](https://www.youtube.com/watch?v=_bbPeCwNxAU)
* [Volumetric Fog of The Last of Us Part II by Artem Kovalovs || SIGGRAPH 2020](https://www.youtube.com/watch?v=lo5VN2nOL98)
* [Game Engine Architecture, Third Edition](https://www.gameenginebook.com/)
* [Learn OpenGL](https://learnopengl.com/)
* [Vulkan Tutorial](https://vulkan-tutorial.com/)

## Building from sources

### Prerequisites

- **Common**:
    - Git (to get source code)
    - CMake (the latest version)
    - Ninja (as build files generator)
    - Python 3.7+
- **Windows 10**:
    - Microsoft Visual C++ Compiler (MSVC) with C++ 17 support
    - x64 Native Tools Command Prompt for VS
- **Ubuntu 20.04**:
    - GNU C++ Compiler with C++ 17 support
- **MaсOS Catalina 10.15**:
    - Clang Compiler with C++ 17 support

### Get source code

The following code snippet downloads project source code repository, and enters project root folder. Must be executed
from the folder where you want to locate project.

```shell
$ git clone https://github.com/EgorOrachyov/wmoge.git
$ cd wmoge
```

### Configure and run build

> **Attention!** On Windows platform building commands must be executed in `x64 Native Tools Command Prompt for VS`.

The following code snippet runs `build.py` script, which allows configuring cmake and running of actual build with
selected options. You can specify build directory, build type, number of system threads for build.
Must be executed from project root folder.

```shell
$ python ./build.py --build-dir=build --build-type=Release --nt=4
```

On macOS, you can optionally specify target binaries architecture to build. Pass option `--arch`
with `x86_64` or `arm64` respectively. By default, build falls back to `CMAKE_SYSTEM_PROCESSOR` specified architecture.
See example bellow, replace `<arch>` with desired architecture for your build. Must be executed from project root
folder.

```shell
$ python ./build.py --build-dir=build --build-type=Release --nt=4 --arch=<arch>
```

In order to run a template project demo, execute following command to start demo game executable. The
optional `root_remap` argument tells engine where its root directory with all config files and resources is located. In
our case the template directory will be used as a root.

```shell
$ build/template/template.exe --root_remap="template/"
```

## Contributing

If you want to fix code or tutorial text issue, add new functionality, or even new tutorial step, feel free to
contribute to the project following [contributing guide](./CONTRIBUTING.md) and [code of conduct](./CODE_OF_CONDUCT.md).

## Disclaimer

This is a personal learning project. It does not claim to be the ultimate guide to building real AAA game engines.
However, it can be useful for those who, like me at the time, want to learn something more than the basics and do
something on their own. This project accumulates ideas and knowledge from several books and can be a good starting point
for learning!

## Also

If you have any questions, feel free to contact me at `egororachyov@gmail.com`. If you have any ideas, questions, how to
make or improve something, please, do not hesitate to open new issue or pull-request inside this project. I will try to
respond as soon as possible to them.

## Contributors

- Egor Orachyov (Github: [@EgorOrachyov](https://github.com/EgorOrachyov))

## Citation

```ignorelang
@online{wmoge,
  author = {Orachyov, Egor},
  title = {Wmoge: High-performance 2d and 3d game engine with Vulkan and OpenGL backends},
  year = 2023,
  url = {https://github.com/EgorOrachyov/wmoge},
  note = {Version 1.0.0}
}
```

## Project structure

| Entry               | Description                                               |
|:--------------------|:----------------------------------------------------------|
| `📁 .github`        | CI/CD scripts and GitHub related files                    |
| `📁 docs`           | Additional project documents and resources                |
| `📁 deps`           | Third-party project dependencies, stored as is            |
| `📁 engine`         | Engine sources compiled into library                      |
| `📁 template`       | Template project which can be used as a base for a game   |
| `📁 games`          | Example games created with engine                         |
| `📄 CMakeLists.txt` | CMake configuration, add as sub directory to your project |
| `📄 build.py`       | Script to build project sources and examples              |

## License

```
MIT License

Copyright (c) 2023 Egor Orachyov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```