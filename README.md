# StreamingPrototype
Streaming Prototype is a demo version of the Cloud-Based Rendering Engine that creates photorealistic images for seamless viewing and editing.

## System requirements and Dependencies

- [C++ Boost 1.63](http://www.boost.org/)
- [Google Protocol Buffer v3.2.0](https://github.com/google/protobuf)
- [x264](http://www.videolan.org/developers/x264.html)
- [Radeon Rays](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK)
- [Heterogeneous Queue](https://github.com/KaoCC/HeterogeneousQueue)
- [OpenImageIO](https://sites.google.com/site/openimageio/home)


The following compilers are supported:

- Visual Studio 2015 or 2017

- GCC 5.4

## Build

### Windows

- Create Visual Studio 2017 Solution

`./Tool/premake/win/premake5.exe vs2017`


- (Optional) Create C++ classes from Google Protocol Buffer to modify the message format

`./Tool/protoc/win32/bin/protoc.exe -I="./SP/src/" --cpp_out="./Protocol" ./Protocol/message.proto`


### Linux

- Create the Makefile

`./Tool/premake/linux64/premake gmake`

- Build it !

`make config=release_x64`


