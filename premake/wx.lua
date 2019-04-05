function wx_config()
   if os.host() ~= "windows" then
      buildoptions { "`wx-config --cxxflags`"}
      linkoptions { "`wx-config --libs`" }
   end
end
