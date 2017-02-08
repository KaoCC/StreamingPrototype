

if os.is("windows") then

    print("protobuf : please input path (enter for the default):")
    io.flush()
    --local protobufUserInput = ""
    local protobufUserInput = io.read()
    print("protobuf : user input path:", protobufUserInput)

    -- tmp: include protobuf here
    local protobufPath = "H:\\protobuf_320_install"

    if (protobufUserInput ~= "") then 
        protobufPath = protobufUserInput
    end

    if (os.isdir(protobufPath) and protobufPath ~= "") then
        protobufPath = string.gsub(protobufPath, "\\", "/")
        local protobufInclude = string.format("%s/include",protobufPath)
        local protobufLibs = string.format("%s/lib",protobufPath)

        print ("C++ protobuf path: ", protobufPath)

        if (os.isdir(protobufInclude) and os.isdir(protobufLibs)) then

            print ("C++ protobuf include path: ", protobufInclude)
            print ("C++ protobuf library path: ", protobufLibs)

            includedirs {protobufInclude}
            libdirs {protobufLibs}

        else
            print("protobuf : include path and libraries NOT FOUND")
        end

    else
        print("C++ protobuf Path NOT FOUND")
    end

end