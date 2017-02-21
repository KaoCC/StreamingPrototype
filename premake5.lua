
if os.is("windows") then
    defines{ "WIN32" }
end

workspace "StreamingPrototype"

    location "Generated"

    language "C++"

    configurations {"Debug", "Release"}
    platforms {"x64", "x32"}

    filter { "platforms:x64" }
        architecture "x86_64"
    filter { "platforms:x32" }
        architecture "x86"
    
    filter { }


    filter { "configurations:Debug" }
        flags { "Symbols" }

    filter { "configurations:Release" }
        -- Release should be optimized
        optimize "On"


    -- Reset the filter for other settings
    filter { }


    targetdir ("Build/Bin/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")
    objdir ("Build/Obj/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")



    -- find include dirs and link with Boost
    dofile ("./FindBoost.lua")

    -- find include dirs and link with protobuf
    -- dofile ("./FindProtobuf.lua")

    -- Google Protocol Buffer
    includedirs {"./3rdparty/protobuf/include"}
    libdirs {"./3rdparty/protobuf/lib"}

    filter {"configurations:Release" }
        links {"libprotobuf"}
    filter {"configurations:Debug"}
        links {"libprotobufd"}
    filter {}


    -- Radeon Rays
    includedirs {"./3rdparty/RadeonRays/include"}
    libdirs {"./3rdparty/RadeonRays/lib"}

    filter {"platforms:x64"}
        links {"RadeonRays64"}
    filter {"platforms:x86"}
        links {"RadeonRays"}
    filter {}

    --OIIO
    includedirs {"./3rdparty/oiio/include"}
    libdirs {"./3rdparty/oiio/lib/%{cfg.platform}"}

    filter {"configurations:Release" }
        links {"OpenImageIO"}
    filter {"configurations:Debug"}
        links {"OpenImageIOD"}
    filter {}

    
    project "SP"

        kind "ConsoleApp"

        -- tmp
        flags { "StaticRuntime" }

        includedirs {"SP/include", "Protocol/"}
        files {"SP/**.cpp", "SP/**.hpp", "SP/**.cc", "SP/**.h", "Protocol/**.cc", "Protocol/**.h"}


    project "Test"

        kind "ConsoleApp"

        -- tmp
        flags { "StaticRuntime" }

        includedirs {"Test/include", "SP/include", "Protocol/"}
        files {"Test/**.cpp", "Protocol/**.cc", "Protocol/**.h", "SP/include/Packet.hpp", "SP/src/Packet.cpp"}

