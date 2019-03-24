project(headsurgeon CXX)

cmake_minimum_required(VERSION 3.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY bin)

find_package(PNG REQUIRED)
find_package(ImageMagick 7 REQUIRED COMPONENTS Magick++)

add_definitions(-DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=0)
file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)
include_directories(src)
include_directories(${PNG_INCLUDE_DIR})
include_directories(${ImageMagick_INCLUDE_DIRS})

set(CMAKE_BUILD_TYPE RelWithDebInfo)
add_compile_options("-Wall" "-Wextra" "-std=c++17")
add_library(headsurgeon SHARED ${SOURCES})
set_target_properties(headsurgeon PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF)
target_link_libraries(headsurgeon ${PNG_LIBRARY} ${ImageMagick_LIBRARIES})

if(MSVC)
  add_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
else(MSVC)
  add_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
endif(MSVC)
