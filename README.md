# StreamingPrototype
Streaming Prototype

## System requirements and Dependencies

- [C++ Boost 1.63](http://www.boost.org/)
- [Google Protocol Buffer v3.2.0](https://github.com/google/protobuf)
- [x264](http://www.videolan.org/developers/x264.html)
- [Radeon Rays](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK)
- [Heterogeneous Queue](https://github.com/KaoCC/HeterogeneousQueue)

## Build

### Windows

- Create Visual Studio 2015 Solution

`./Tool/premake/win/premake5.exe vs2015`


- (Optional) Create C++ classes from Google Protocol Buffer to modify the message format

`./Tool/protoc/win32/bin/protoc.exe -I="./SP/src/" --cpp_out="./Protocol" ./Protocol/message.proto`


