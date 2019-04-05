function libwebp_build()
  if os.target() == "windows" then
    libwebp_fetch()

    project "libwebp"
      language "c"
      kind "sharedlib"
      systemversion "latest"
      includedirs { "pkg/libwebp-1.0.2" }
      defines { "WEBP_EXTERN=__declspec(dllexport)"}
      files { "pkg/libwebp-1.0.2/src/**.c", "pkg/libwebp-1.0.2/src/**.h" }
  end
end

function libwebp_config()
  if os.target() == "windows" then
    links { "libwebp" }
    includedirs { "pkg/libwebp-1.0.2/src" }
  else
    links { "webpmux", "webpdemux" }
  end
end

function libwebp_fetch()
  os.mkdir("pkg")
  
  if not os.isfile("pkg/libwebp.zip") then
    print("Fetching libwebp...")
    http.download("https://github.com/webmproject/libwebp/archive/v1.0.2.zip", "pkg/libwebp.zip")
    print("Extracting libwebp...")
    zip.extract("pkg/libwebp.zip", "pkg")
  end
end