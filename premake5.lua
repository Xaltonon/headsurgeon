dofile "premake/wx.lua"
dofile "premake/libpng.lua"
dofile "premake/libwebp.lua"

workspace "headsurgeon"
  configurations { "debug", "release" }
 
  architecture "x86_64"
  systemversion "latest"

  filter "toolset:gcc or toolset:clang"
    buildoptions {"-Wall"}
    links { "stdc++fs" }
    defines { "EXPORT" }

  filter "configurations:debug"
    defines { "DEBUG" }
    symbols "on"

  filter "configurations:release"
    optimize "full"
    flags { "linktimeoptimization" }

  filter "system:windows"
    defines { "EXPORT=__declspec(dllexport)" }

workspace "headsurgeon"
  libwebp_build()
  libpng_build()

project "headsurgeon"
  language "c++"
  cppdialect "c++17"
  kind "sharedlib"

  files { "src/*.cpp", "src/*.h", "src/codecs/*.cpp", "src/codecs/*.h" }

  includedirs { "src" }
  libpng_config()
  libwebp_config()

project "hsdmi"
  kind "consoleapp"
  language "c++"
  cppdialect "c++17"

  files { "cli/*.cpp", "cli/*.h" }

  links { "headsurgeon" }
  includedirs { "src", "cli/CLI11/include" }
  libpng_config()
  libwebp_config()

--[[
project "hsgui"
  kind "windowedapp"
  language "c++"
  cppdialect "c++17"

  files { "gui/*.cpp", "gui/*.h" }

  links { "headsurgeon" }
  includedirs { "src", "gui" }
  wx_config()
--]]