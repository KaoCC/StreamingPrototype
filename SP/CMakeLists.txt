



include_directories(${SP_DIR}/include)
include_directories(${SP_DIR}/src)
include_directories(${SP_DIR}/src/Renderer)
include_directories(${SP_DIR}/src/Encoder)
include_directories(${SP_DIR}/src/Network)
include_directories(${Protocol_DIR}/)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENIMAGEIO_INCLUDE_DIRS})
include_directories(${X264_INCLUDE_DIRS})
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}) # for auto generated files
# include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/RadeonRays/include)

if (WIN32)
    message("Windows Boost: ${Boost_LIBRARY_DIRS}")
    link_directories(${Boost_LIBRARY_DIRS})
endif()

message("PROTOC: ${Protobuf_PROTOC_EXECUTABLE}")
protobuf_generate_cpp(PROTO_SRCS PROTO_HEADER ${Protocol_DIR}/message.proto)
set_source_files_properties(${PROTO_SRCS} ${PROTO_HEADER} PROPERTIES GENERATED TRUE)
message("PROTO Files: ${PROTO_SRCS}, ${PROTO_HEADER}")


if (WIN32)
    set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
    foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

add_executable(
    Radiance

    ./include/Common.hpp 
    ./include/Packet.hpp

    ./src/ConfigManager.hpp
    ./src/LightField.hpp
    ./src/SyncBuffer.hpp

    ./src/Network/Connection.hpp
    ./src/Network/Server.hpp

    ./src/Encoder/Encoder.hpp
    ./src/Encoder/EncoderImpl.hpp

    ./src/Renderer/ApiEngine.hpp
    ./src/Renderer/BxDFHelper.hpp
    ./src/Renderer/CorrelatedMultiJitterSampler.hpp
    ./src/Renderer/DifferentialGeometry.hpp
    ./src/Renderer/MathUtility.hpp
    ./src/Renderer/Output.hpp
    ./src/Renderer/Path.hpp
    ./src/Renderer/PtRenderer.hpp
    ./src/Renderer/RandomSampler.hpp
    ./src/Renderer/Renderer.hpp
    ./src/Renderer/RenderingManager.hpp
    ./src/Renderer/RenderOutput.hpp
    ./src/Renderer/Sampler.hpp
    ./src/Renderer/SceneTracker.hpp
    ./src/Renderer/SimpleRenderer.hpp
    ./src/Renderer/DefaultList.hpp
    ./src/Renderer/DefaultList.cpp

    ./src/Renderer/Scene/IO/tiny_obj_loader.h

    ./src/Renderer/Scene/Camera.hpp
    ./src/Renderer/Scene/Iterator.hpp
    ./src/Renderer/Scene/Light.hpp
    ./src/Renderer/Scene/Material.hpp
    ./src/Renderer/Scene/Scene.hpp
    ./src/Renderer/Scene/SceneObject.hpp
    ./src/Renderer/Scene/Shape.hpp
    ./src/Renderer/Scene/Texture.hpp

    ./src/Renderer/Scene/IO/ImageIO.hpp
    ./src/Renderer/Scene/IO/MaterialIO.hpp
    ./src/Renderer/Scene/IO/SceneIO.hpp

    ./src/Common.cpp
    ./src/SyncBuffer.cpp
    ./src/ConfigManager.cpp 
    ./src/LightField.cpp    
    ./src/Packet.cpp               
    ./src/main.cpp

    ./src/Network/Server.cpp
    ./src/Network/Connection.cpp

    ./src/Encoder/EncoderImpl.cpp

    ${PROTO_SRCS}
    ${PROTO_HEADER}

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
message("VARS :  ${Protobuf_INCLUDE_DIR}, ${Protobuf_VERSION}, ${Boost_LIBRARIES}")



target_link_libraries(Radiance ${Boost_LIBRARIES} ${RR_LIBRARIES} ${X264_LIBRARIES} ${OPENIMAGEIO_LIBRARIES} ${Protobuf_LIBRARIES})


add_custom_command(TARGET Radiance POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:Radiance>/Resources)

if (WIN32)
    set(WK_DIR "$<TARGET_FILE_DIR:Radiance>")
    set_target_properties(Radiance PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR})
endif()
