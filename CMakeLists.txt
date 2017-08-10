cmake_minimum_required(VERSION 3.3)

set(CMAKE_BUILD_TYPE Release)

set (CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)

set(3RDPARTY_DIR ${CMAKE_SOURCE_DIR}/3rdparty)
set(SP_DIR ${CMAKE_SOURCE_DIR}/SP)
set(Protocol_DIR ${CMAKE_SOURCE_DIR}/Protocol)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(StreamingPrototype)

# select the one with the highest version number
set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)

# find_package(OpenCL REQUIRED)

find_package(Threads REQUIRED)

if (WIN32)
    list(APPEND CMAKE_PREFIX_PATH ${3RDPARTY_DIR}/protobuf)
    list(APPEND CMAKE_PREFIX_PATH ${3RDPARTY_DIR}/x264)
    list(APPEND CMAKE_PREFIX_PATH ${3RDPARTY_DIR}/oiio)
endif()

list(APPEND CMAKE_PREFIX_PATH ${3RDPARTY_DIR}/RadeonRays)

# Google Protobuf
# find_package(Protobuf REQUIRED HINTS ${CMAKE_SOURCE_DIR}/3rdparty/protobuf/cmake)
find_package(Protobuf REQUIRED)



# boost
if (WIN32)
    set (Boost_DEBUG ON)
    set (Boost_USE_STATIC_LIBS ON)
    set (Boost_USE_STATIC_RUNTIME ON)
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
    message(STATUS "Boost_LIB_DIAGNOSTIC_DEFINITIONS ${Boost_LIB_DIAGNOSTIC_DEFINITIONS} ")
endif()

find_package(Boost REQUIRED COMPONENTS system thread)

# x264
find_package(x264 REQUIRED)

# OpenImageIO ?
find_package(OpenImageIO REQUIRED)


# RadeonRays
find_library(RR_LIBRARIES NAMES RadeonRays64)
message("RR: ${RR_LIBRARIES}")

message("CMAKE_BUILD_TYPE= ${CMAKE_BUILD_TYPE}")
message("CMAKE_SOURCE_DIR= ${CMAKE_SOURCE_DIR}")
message("CMAKE_BINARY_DIR= ${CMAKE_BINARY_DIR}")
message("CMAKE_INSTALL_PREFIX= ${CMAKE_INSTALL_PREFIX}")
message("CMAKE_CXX_STANDARD= ${CMAKE_CXX_STANDARD}")
message("CMAKE_SYSTEM_PREFIX_PATH= ${CMAKE_SYSTEM_PREFIX_PATH}")
message("CMAKE_MODULE_PATH= ${CMAKE_MODULE_PATH}")



add_subdirectory(SP)
# add_subdirectory(Protocol)

