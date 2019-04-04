workspace "headsurgeon"
  configurations { "debug", "release" }

  filter "toolset:gcc or toolset:clang"
    buildoptions {"-Wall"}

  filter "configurations:debug"
    defines { "DEBUG" }
    symbols "on"

  filter "configurations:release"
    optimize "full"
    flags { "linktimeoptimization" }

project "headsurgeon"
  language "c++"
  cppdialect "c++17"
  kind "sharedlib"

  files { "src/*.cpp", "src/*.h", "src/codecs/*.cpp", "src/codecs/*.h" }

  links { "png", "webp", "webpmux", "webpdemux", "stdc++fs" }
  includedirs { "src" }

project "hsdmi"
  kind "consoleapp"
  language "c++"
  cppdialect "c++17"

  files { "cli/*.cpp", "cli/*.h"}

  links { "headsurgeon", "stdc++fs" }
  includedirs { "src", "cli/CLI11/include" }
