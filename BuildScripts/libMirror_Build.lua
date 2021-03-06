------------------------------------------------------------------------------------------------------------------------
--
--  ElflordPP/JSONBuild.lua
--
--  Running the build process for jsoncpp.
--
------------------------------------------------------------------------------------------------------------------------
--  Copyright (c) 2018 Miki Ryan
------------------------------------------------------------------------------------------------------------------------
configureEngineLib("libMirror")

includedirs({
    "ThirdParty",
    "ThirdParty/rttr/src",
})

addDependencies({
    "libCore",
    "libMath"
})

dependson({ "rttr" })
