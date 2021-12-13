cmake_minimum_required(VERSION 3.15)
project(term-video)

# Needed for Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_executable(main 
  src/main.cpp
  src/termutils.cpp
  src/termshow_blocks.cpp
)
add_executable(tester
  src/video-reader.cpp
)
target_include_directories(main
  PUBLIC include
)
target_link_libraries(main CONAN_PKG::opencv CONAN_PKG::fmt)
target_link_libraries(tester CONAN_PKG::opencv CONAN_PKG::fmt)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries(main pthread)
endif()