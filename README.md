# spank/report: A SPANK plugin for Slurm using TRES Billing

**Warning:** The plugin was initially made for a specific slurm version. Please verify `versions.lock`.

## Configuring

Availables options are shown in the `CMakeLists.txt`.

## Compiling

Fetch the includes of slurm or install slurm. This library depends on the public includes of slurm.

Next, build the library. The outputs will be in the `build/lib` directory.

```sh
mkdir -p build
cd build && cmake ..
make -j$(nproc)
# Or with Ninja:
# cd build && cmake .. -G Ninja
# ninja
```

## Installing

```sh
sudo make install  # or sudo ninja install
```

This will install `spank_report.a` and `spank_report.so` in the `/usr/lib/slurm` directory.

To change the directory use:

```sh
mkdir -p build
cd build && cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/lib/slurm
make -j$(nproc)
sudo make install
```

## Using

Install on the worker nodes, the spank plugins and add the plugin to `plugstack.conf`.

```sh
required /usr/local/lib/slurm/spank_report.so
```

## Maintaining and updating the plugin for new Slurm Version

TBH, you will have to google a little to find some documentations.

Normally, you only need the headers of the [slurm directory in the slurm repository](https://github.com/SchedMD/slurm/tree/master/slurm) (e.g the public headers).

To install these headers, you will have to configure slurm (execute `./configure`) and copy the files `./slurm/*.h` to `/usr/local/include` or `$HOME/.local/include`.

```sh
git clone --depth=1 https://github.com/SchedMD/slurm
cd slurm && ./configure
mkdir -p $HOME/.local/include
cp -r ./slurm $HOME/.local/include

# If you need to build
mkdir -p build && cd build
cmake .. -DSLURM_INCLUDE_DIR=$HOME/.local/include
make -j$(nproc)
```

Then configure your IDE to include `$HOME/.local/include`.

Official documentation is here: https://slurm.schedmd.com/spank.html

A good example of SPANK plugin is: https://github.com/NVIDIA/pyxis

The CI will check each week if the plugin can compile against the new slurm version.

## LICENSE

```sh
MIT License

Copyright (c) 2021 Marc Nguyen, Cohesive Computing SA and DeepSquare Association

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
