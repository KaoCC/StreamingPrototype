cmake_minimum_required(VERSION 3.3)

set(CMAKE_BUILD_TYPE debug)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)


set(SP_DIR ${CMAKE_SOURCE_DIR}/SP)
set(Protocol_DIR ${CMAKE_SOURCE_DIR}/Protocol)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(StreamingPrototype)


# find_package(OpenCL REQUIRED)

find_package(Threads REQUIRED)

# Google Protobuf
find_package(Protobuf REQUIRED HINTS ${CMAKE_SOURCE_DIR}/3rdparty/protobuf/cmake)


# boost
find_package(Boost REQUIRED COMPONENTS system thread)

# x264
find_package(x264 REQUIRED)

# OpenImageIO ?
find_package(OpenImageIO REQUIRED)


# RadeonRays



message("CMAKE_BUILD_TYPE= ${CMAKE_BUILD_TYPE}")
message("CMAKE_SOURCE_DIR= ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR= ${CMAKE_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX= ${CMAKE_INSTALL_PREFIX}")
message("CMAKE_CXX_STANDARD= ${CMAKE_CXX_STANDARD}")
message("CMAKE_SYSTEM_PREFIX_PATH= ${CMAKE_SYSTEM_PREFIX_PATH}")
message("CMAKE_MODULE_PATH= ${CMAKE_MODULE_PATH}")



add_subdirectory(SP)
# add_subdirectory(Protocol)

