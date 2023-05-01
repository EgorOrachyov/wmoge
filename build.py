##################################################################################
# Wmoge game engine                                                              #
# Available at github https://github.com/EgorOrachyov/wmoge                      #
##################################################################################
# MIT License                                                                    #
#                                                                                #
# Copyright (c) 2023 Egor Orachyov                                               #
#                                                                                #
# Permission is hereby granted, free of charge, to any person obtaining a copy   #
# of this software and associated documentation files (the "Software"), to deal  #
# in the Software without restriction, including without limitation the rights   #
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      #
# copies of the Software, and to permit persons to whom the Software is          #
# furnished to do so, subject to the following conditions:                       #
#                                                                                #
# The above copyright notice and this permission notice shall be included in all #
# copies or substantial portions of the Software.                                #
#                                                                                #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    #
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  #
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  #
# SOFTWARE.                                                                      #
##################################################################################

import subprocess
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--build-dir", default="build", help="folder name to locate build files")
    parser.add_argument("--build-type", default="Release", help="type of build: `Debug`, `Release` or `RelWithDebInfo`")
    parser.add_argument("--target", default="all", help="which target to build")
    parser.add_argument("--nt", default="4", help="number of os threads for build")
    parser.add_argument("--arch", help="target architecture on MacOS `x64` or `arm64`")
    parser.add_argument("--verbose", help="allow verbose compiler output")
    args = parser.parse_args()

    build_config_args = ["cmake", ".", "-B", args.build_dir, "-G", "Ninja", f"-DCMAKE_BUILD_TYPE={args.build_type}"]

    if args.arch:
        build_config_args += [f"-DCMAKE_OSX_ARCHITECTURES={args.arch}"]

    build_run_args = ["cmake", "--build", args.build_dir, "--target", args.target, "-j", args.nt]

    if args.verbose:
        build_run_args += ["--verbose"]

    subprocess.check_call(build_config_args)
    subprocess.check_call(build_run_args)

    print(f"\nsuccessfully build target `{args.target}`")


if __name__ == '__main__':
    main()
