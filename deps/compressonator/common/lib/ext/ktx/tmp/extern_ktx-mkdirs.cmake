# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx"
  "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx/build"
  "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx"
  "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx/tmp"
  "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx/src/extern_ktx-stamp"
  "C:/Users/egoro/Documents/GitHub/wmoge/cmake-build-relwithdebinfo/deps/compressonator"
  "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx/src/extern_ktx-stamp"
)

set(configSubDirs Debug;Release)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/egoro/Documents/GitHub/wmoge/deps/compressonator/../common/lib/ext/ktx/src/extern_ktx-stamp/${subDir}")
endforeach()
