cmake_minimum_required(VERSION 3.15)
project(badapple-term)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(OpenCV REQUIRED)

add_executable(main 
  src/main.cpp
  src/termutils.cpp
  src/termshow.cpp
)
target_include_directories(main
  PUBLIC include
)
target_link_libraries(main ${OpenCV_LIBS})
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_link_libraries(main pthread)
endif()