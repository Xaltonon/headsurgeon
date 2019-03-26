workspace "headsurgeon"
  configurations { "debug", "release" }

  filter "configurations:debug"
    defines { "DEBUG" }
    symbols "on"

  filter "configurations:release"
    optimize "full"
    flags { "linktimeoptimization" }

project "headsurgeon"
  kind "sharedlib"
  language "c++"
  cppdialect "c++17"

  files { "src/*.cpp", "src/*.h", "src/codecs/*.cpp", "src/codecs/*.h" }

  links { "png", "webp", "webpmux", "stdc++fs" }
  includedirs { "src" }

project "headsurgeon-cli"
  kind "consoleapp"
  language "c++"
  cppdialect "c++17"

  files { "cli/*.cpp", "cli/*.h"}

  links { "headsurgeon" }
  includedirs { "src" }
