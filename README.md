![logo](https://github.com/EgorOrachyov/wmoge/raw/main/docs/media/wmoge-banner-trsp-x2.png?raw=true&sanitize=true)

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

Current progress can be tracked at GitHub [tasks page](https://github.com/EgorOrachyov/wmoge/issues).

![gif](https://github.com/EgorOrachyov/wmoge/raw/main/docs/media/background.gif?raw=true&sanitize=true)

> Work in progress, new features coming soon!

## About the engine

### Features

* 🔨 Moder C++17 based engine with clear architecture.
* 🔨 Platform-agnostic, with support for common operating systems such as Windows, Linux and macOS.
* 🔨 Runtime reflection system with class, method and property information.
* ⚙ Simple stack-based config system.
* 🎥 Abstract graphics (gfx) device interface for GPU driver communication.
* 🎥 Vulkan-based gfx device backend.
* 🎥 (in progress) OpenGL-based gfx device backend.
* 🎨 Lightweight runtime and offline shader cache system to speed-up shaders compilation.
* 🎨 High-level shaders and render-passes with generated static reflection from a python script.
* 📁 Async resource-loading system with runtime cache.
* 📁 Unified resources handling, meta-information, support for external formats through custom loaders.
* 🅰️ Custom math library with 2d and 3d-space primitives.
* 🧱 Ecs-based scene model with fast, parallel and memory-friendly update.
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

In order to run a template project demo, you need to manually copy `template` folder content into the
`build/template` directory and run template.exe. The template project will start up and enter main loop. Alternatively,
the executable can be stated from the IDE by running `template/main.cpp` entry point.

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