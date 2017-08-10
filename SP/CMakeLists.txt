



include_directories(${SP_DIR}/include)
include_directories(${SP_DIR}/src)
include_directories(${SP_DIR}/src/Renderer)
include_directories(${SP_DIR}/src/Encoder)
include_directories(${SP_DIR}/src/Network)
include_directories(${Protocol_DIR}/)

# include_directories(${OpenCL_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/RadeonRays/include)

add_executable(
    Radiance
    ./src/Common.cpp
    ./src/SyncBuffer.cpp
    ./src/ConfigManager.cpp 
    ./src/LightField.cpp    
    ./src/Packet.cpp               
    ./src/main.cpp

    ./src/Network/Server.cpp
    ./src/Network/Connection.cpp

    ./src/Encoder/EncoderImpl.cpp

    ${Protocol_DIR}/message.pb.cc

    ./src/Renderer/ApiEngine.cpp
    ./src/Renderer/BxDFHelper.cpp
    ./src/Renderer/CorrelatedMultiJitterSampler.cpp
    ./src/Renderer/DifferentialGeometry.cpp
    ./src/Renderer/MathUtility.cpp
    ./src/Renderer/Output.cpp
    ./src/Renderer/Path.cpp
    ./src/Renderer/PtRenderer.cpp
    ./src/Renderer/RandomSampler.cpp
    ./src/Renderer/RenderOutput.cpp
    ./src/Renderer/Renderer.cpp
    ./src/Renderer/RenderingManager.cpp
    ./src/Renderer/Sampler.cpp
    ./src/Renderer/SceneTracker.cpp
    ./src/Renderer/SimpleRenderer.cpp

    ./src/Renderer/Scene/Camera.cpp
    ./src/Renderer/Scene/Light.cpp
    ./src/Renderer/Scene/Material.cpp
    ./src/Renderer/Scene/Scene.cpp
    ./src/Renderer/Scene/SceneObject.cpp
    ./src/Renderer/Scene/Shape.cpp
    ./src/Renderer/Scene/Texture.cpp

    ./src/Renderer/Scene/IO/ImageIO.cpp
    ./src/Renderer/Scene/IO/ImageIOImpl.cpp
    ./src/Renderer/Scene/IO/MaterialIOImpl.cpp
    ./src/Renderer/Scene/IO/SceneIO.cpp
    ./src/Renderer/Scene/IO/SceneIOImpl.cpp

    ./src/Renderer/Scene/IO/tiny_obj_loader.cc

    )

# message("Protobuf_LIBRARIES= ${Protobuf_LIBRARIES} , ${Protobuf_LITE_LIBRARIES}, ${Protobuf_LIBRARY}, ${Protobuf_LIBRARY_DEBUG}")
message("VARS : ${Protobuf_FOUND}, ${Protobuf_INCLUDE_DIR}, ${Protobuf_VERSION}, ${Boost_LIBRARIES}")

target_link_libraries(Radiance ${X264_LIBRARIES} ${OPENIMAGEIO_LIBRARIES} ${Protobuf_LIBRARIES} Boost::system Boost::thread)

# target_link_libraries(Radiance ${OpenCL_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
