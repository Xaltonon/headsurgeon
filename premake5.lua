dofile "conanbuildinfo.lua"

workspace "headsurgeon"
  configurations { "debug", "release" }

  architecture "x86_64"
  systemversion "latest"
  
  includedirs { conan_includedirs }
  bindirs { conan_bindirs }
  libdirs { conan_libdirs }

  filter "toolset:gcc or toolset:clang"
    buildoptions {"-Wall"}
    links { "stdc++fs" }
    defines { "EXPORT=" }

  filter "configurations:debug"
    defines { "DEBUG" }
    symbols "on"

  filter "configurations:release"
    optimize "full"
    flags { "linktimeoptimization" }

  filter "system:windows"
    defines { "EXPORT=__declspec(dllexport)" }

project "headsurgeon"
  language "c++"
  cppdialect "c++17"
  kind "sharedlib"

  files { "src/*.cpp", "src/*.h", "src/codecs/*.cpp", "src/codecs/*.h" }

  includedirs { "src" }
  links { conan_libs_libpng, conan_libs_libwebp }
  
project "hsdmi"
  kind "consoleapp"
  language "c++"
  cppdialect "c++17"

  files { "cli/*.cpp", "cli/*.h" }

  links { "headsurgeon" }
  includedirs { "src", "cli/CLI11/include" }
  links { conan_libs_libpng, conan_libs_libwebp }

project "hsgui"
  kind "windowedapp"
  language "c++"
  cppdialect "c++17"
  
  files { "gui/*.cpp", "gui/*.h" }
  
  links { "headsurgeon" }
  includedirs { "src" }
  links { conan_libs_libui }
  
  filter "system:windows"
    linkoptions { "/ENTRY:\"mainCRTStartup\"" }
  
newaction {
  trigger = "package",
  description = "Generate an installable package",
  execute = function()
    if os.target() == "windows" then
      os.execute("candle -o obj/installer.wixobj pkg/windows/installer.wxs")
      os.execute("light -ext WIXUiExtension obj/installer.wixobj -out bin/headsurgeon.msi")
    end
  end
}