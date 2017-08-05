


# find_package(OpenCL REQUIRED)
find_package (Threads REQUIRED)
find_package(Protobuf REQUIRED)

# boost
find_package(Boost REQUIRED)



# x264 ?
#find_package(x264 REQUIRED)

# OpenImageIO ?
#find_package(OpenImageIO REQUIRED)

include_directories(${SP_DIR}/include)
include_directories(${SP_DIR}/src)

# include_directories(${OpenCL_INCLUDE_DIR})
include_directories(${BOOST_INCLUDE_DIRS})


add_executable(
    Radiance
    ./src/Common.cpp
    ./src/SyncBuffer.cpp
    ./src/ConfigManager.cpp 
    ./src/LightField.cpp    
    ./src/Packet.cpp        
    ./src/SyncBuffer.hpp
    ./src/ConfigManager.hpp 
    ./src/LightField.hpp         
    ./src/main.cpp

    )

# target_link_libraries(${OpenCL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
