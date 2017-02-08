

if os.is("windows") then

    print("Boost : please input path (enter for the default):")
    io.flush()
    local boostUserInput = io.read()
    print("Boost : user input path:", boostUserInput)

    -- tmp: include boost here
    local boostPath = "H:\\boost_1_63_0"

    if (boostUserInput ~= "") then 
        boostPath = boostUserInput
    end

    if (os.isdir(boostPath) and boostPath ~= "") then
        boostPath = string.gsub(boostPath, "\\", "/");
        local boostInclude = string.format("%s",boostPath);
        local boostLibs = string.format("%s/stage/lib",boostPath);

        print ("C++ Boost path: ", boostPath)

        if (os.isdir(boostInclude) and os.isdir(boostLibs)) then

            print ("Boost include path: ", boostInclude)
            print ("C++ Boost Library path: ", boostLibs)

            includedirs {boostInclude}
            libdirs {boostLibs}

        else
            print("Boost : include path and libraries NOT FOUND")
        end

    else
        print("C++ Boost Path NOT FOUND")
    end

end