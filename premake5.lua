
if os.is("windows") then
    defines{ "WIN32" }
end

if os.is("linux") then
	flags {"C++14"}
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
        symbols "On"

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

	 if (os.is("windows")) then

    includedirs {"./3rdparty/protobuf/include"}
    libdirs {"./3rdparty/protobuf/lib"}

   	    filter {"configurations:Release" }
	    	links {"libprotobuf"}
	    filter {"configurations:Debug"}
	    	links {"libprotobufd"}
	    filter {}
    elseif (os.is("linux")) then
	    -- filter {"configurations:Release"}
	    	







		buildoptions{"`pkg-config --cflags protobuf`"} --  "-D_GLIBCXX_USE_CXX11_ABI=0"}

		linkoptions{"`pkg-config --libs protobuf`"}

		links{"protobuf"}


	--filter{"configurations:Debug"}
		--links{"protobufd"}
	--filter{}
    end


    -- x264

    if (os.is("windows")) then
    includedirs {"./3rdparty/x264/include"}

    filter {"platforms:x64"}
        libdirs {"./3rdparty/x264/lib/x64"}
    filter {"platforms:x86"}
        libdirs {"./3rdparty/x264/lib/x86"}
    filter {}
    links {"x264"}
    elseif(os.is("linux")) then
	    links("x264")
    end


    -- Radeon Rays
    includedirs {"./3rdparty/RadeonRays/include"}
    libdirs {"./3rdparty/RadeonRays/lib"}

    filter {"platforms:x64"}
        links {"RadeonRays64"}
    filter {"platforms:x86"}
        links {"RadeonRays"}
    filter {}

    --OIIO
    --
    

    if (os.is("windows")) then

    includedirs {"./3rdparty/oiio/include"}
    libdirs {"./3rdparty/oiio/lib/%{cfg.platform}"}

    filter {"configurations:Release" }
        links {"OpenImageIO"}
    filter {"configurations:Debug"}
        links {"OpenImageIOD"}
    filter {}
    elseif (os.is("linux")) then

	    includedirs{"./3rdparty/oiio/dist/linux64/include"}
	    libdirs {"./3rdparty/oiio/dist/linux64/lib"}

	    links{"OpenImageIO", "pthread", "boost_system"}
    end

    
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

