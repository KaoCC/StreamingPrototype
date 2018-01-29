# StreamingPrototype
Streaming Prototype is a demo version of the Cloud-Based Rendering Engine that creates photo-realistic images for seamless viewing and online editing.

## System requirements and Dependencies

- [C++17 Compiler](http://en.cppreference.com/w/cpp/compiler_support)
- [CMake 3.10](https://cmake.org/)
- [C++ Boost 1.63](http://www.boost.org/)
- [Google Protocol Buffer v3.2.0](https://github.com/google/protobuf)
- [x264](http://www.videolan.org/developers/x264.html)
- [Radeon Rays](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK)
- [Heterogeneous Queue](https://github.com/KaoCC/HeterogeneousQueue)
- [OpenImageIO](https://sites.google.com/site/openimageio/home)

Note: The RadeonRays framework may require [Embree](https://embree.github.io/) or [OpenCL](https://software.intel.com/en-us/intel-opencl) in order to support the corresponding backends.

Currently the following compilers are tested:

- Visual Studio 2017 Version 15.5.5

## Build

### Windows & Mac OS X & Linux

We have recently changed from premake to cmake.

1. Create your build directory `mkdir SP_build`
2. Run CMake `cmake ..` or `cmake -DCMAKE_BUILD_TYPE=Debug` for debugging
3. Compile by running `make`
4. The binary is named Radiance.


## Client Code

[StreamingPrototypeClient](https://github.com/lctseng/StreamingPrototypeClient)

## Demo Video

- [Basic Scene](https://youtu.be/lMdvIzpLWpQ)
- [Arbitrary viewpoint in Conference](https://youtu.be/RelNG831QU0)
- [Wide angle viewing in Conference ](https://youtu.be/JWcialjxyy4)
- [Projection to ST plane in Sponza](https://youtu.be/JtGV25Jj2x8)
- [Full screen rotation in Sponza](https://youtu.be/XTZMCKUFCxA)

