function libpng_build()
  if os.target() == "windows" then
    libpng_fetch()

    project "zlib"
      language "c"
      kind "sharedlib"
      systemversion "latest"
      defines { "ZLIB_DLL" }
      files { "pkg/zlib-1.2.11/*.c", "pkg/zlib-1.2.11/*.h" }

    project "libpng"
      language "c"
      kind "sharedlib"
      systemversion "latest"
      includedirs { "pkg/zlib-1.2.11" }
      files { "pkg/lpng1636/*.c", "pkg/lpng1636/*.h" }
      links { "zlib" }
  end
end

function libpng_config()
  if os.target() == "windows" then
    links { "zlib", "libpng" }
    includedirs { "pkg/zlib-1.2.11", "pkg/lpng1636" }
  else
    links { "png", "z" }
  end
end

function libpng_fetch()
  os.mkdir("pkg")
  
  if not os.isfile("pkg/libpng.zip") then
    print("Fetching libpng...")
    http.download("http://prdownloads.sourceforge.net/libpng/lpng1636.zip?download", "pkg/libpng.zip")
    print("Extracting libpng...")
    zip.extract("pkg/libpng.zip", "pkg")
    os.copyfile("pkg/lpng1636/scripts/pnglibconf.h.prebuilt", "pkg/lpng1636/pnglibconf.h")
  end
  if not os.isfile("pkg/zlib.zip") then
    print("Fetching zlib...")
    http.download("http://zlib.net/zlib1211.zip", "pkg/zlib.zip")
    print("Extracting zlib...")
    zip.extract("pkg/zlib.zip", "pkg")
  end
end
