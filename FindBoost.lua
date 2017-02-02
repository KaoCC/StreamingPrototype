

if os.is("windows") then

    -- tmp: include boost here
    local boostPath = "E:\\boost_1_63_0"

    if (os.isdir(boostPath) and boostPath ~= "") then
        boostPath = string.gsub(boostPath, "\\", "/");
        local boostInclude = string.format("%s",boostPath);
        local boostLibs = string.format("%s/stage/lib",boostPath);

        print ("C++ Boost path: ", boostPath)

        if (os.isdir(boostInclude) and os.isdir(boostLibs)) then

            print ("Boost include path: ", boostInclude)
            print ("C++ Library path: ", boostLibs)

            includedirs {boostInclude}
            libdirs {boostLibs}

        else
            print("include path and libraries NOT FOUND")
        end

    else
        print("C++ Boost Path NOT FOUND")
    end

end