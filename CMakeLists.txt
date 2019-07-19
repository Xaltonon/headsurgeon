cmake_minimum_required(VERSION 3.15)
project(headsurgeon CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

file(GLOB_RECURSE HS_SOURCES libheadsurgeon/*.cpp)
add_library(headsurgeon SHARED ${HS_SOURCES})
target_include_directories(headsurgeon PUBLIC .)
target_compile_definitions(headsurgeon PUBLIC LIBHEADSURGEON_SHARED_BUILD)
target_link_libraries(headsurgeon CONAN_PKG::libpng CONAN_PKG::libwebp)
set_target_properties(headsurgeon PROPERTIES CXX_STANDARD 17)

add_executable(hsdmi hsdmi/main.cpp)
target_link_libraries(hsdmi CONAN_PKG::cli11 headsurgeon)
set_target_properties(hsdmi PROPERTIES CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
file(GLOB_RECURSE HSGUI_SOURCES hsgui/*.cpp)
add_executable(hsgui WIN32 ${HSGUI_SOURCES})
target_link_libraries(hsgui Qt5::Widgets headsurgeon)
set_target_properties(hsgui PROPERTIES CXX_STANDARD 17)

install(DIRECTORY build/bin/ DESTINATION .)

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_DESCRIPTION "A tool for manipulating BYOND DMIs.")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
include(CPack)