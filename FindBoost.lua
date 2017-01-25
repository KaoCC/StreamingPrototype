

if os.is("windows") then

    -- tmp: include boost here
    local boostPath = "E:\\boost"
    if (os.isdir(boostPath)) then
        boostPath = string.gsub(boostPath, "\\", "/");
        local boostInclude = string.format("%s/include",boostPath);
        local boostLibs = string.format("%s/lib",boostPath);
        print ("C++ Boost path: ", boostPath)
        print ("Boost include path: ", boostInclude)
        print ("C++ Library path: ", boostLibs)

        includedirs {boostInclude}
        libdirs {boostLibs}

    else
        print("C++ Boost Path NOT FOUND")
    end

end