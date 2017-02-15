
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
    libdirs {"./3rdparty/protobuf/include"}

    filter {"configurations:Release" }
        links {"libprotobuf"}
    filter {"configurations:Debug"}
        links {"libprotobufd"}
    filter {}

    
    project "SP"

        kind "ConsoleApp"

        -- tmp
        flags { "StaticRuntime" }

        includedirs {"SP/include", "Protocol/"}
        files {"SP/**.cpp", "SP/**.hpp", "Protocol/**.cc", "Protocol/**.h"}


    project "Test"

        kind "ConsoleApp"

        -- tmp
        flags { "StaticRuntime" }

        includedirs {"Test/include", "SP/include", "Protocol/"}
        files {"Test/**.cpp", "Protocol/**.cc", "Protocol/**.h", "SP/include/Packet.hpp", "SP/src/Packet.cpp"}

