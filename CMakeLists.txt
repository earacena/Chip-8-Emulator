# Set cmake versions
cmake_minimum_required(VERSION 3.7...3.24)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# Set project name and details
project(
  chip8 
  VERSION 1.0
  DESCRIPTION "A CHIP-8 emulator written in C++."
  LANGUAGES CXX
)

# Find/Define libraries

# fmtlib
find_package(fmt REQUIRED)

# SDL
find_package(SDL2 REQUIRED)

# ImGUI
add_library(
  imgui
  external/imgui/imgui.cpp
  external/imgui/imgui_widgets.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_impl_sdl.cpp
  external/imgui/imgui_impl_opengl3.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_demo.cpp
)

# Define executable target
add_executable(
  chip8
  src/main.cpp
  src/Display.cpp
  src/Debugger.cpp
  src/Chip8.cpp
  src/CPU.cpp
)

# Include directories
target_include_directories(chip8 PUBLIC "${SDL2_INCLUDE_DIR}")
target_include_directories(imgui PUBLIC "${SDL2_INCLUDE_DIR}")
target_include_directories(imgui PUBLIC "external/imgui")

# Link libraries
#target_link_libraries(imgui)
target_link_libraries(chip8 ${SDL2_LIBRARIES})
target_link_libraries(chip8 imgui -lGL)
target_link_libraries(chip8 fmt::fmt)

# Set compiler flags
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_options(chip8 PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_compile_options(chip8 PRIVATE -Wall -Wextra -Wpedantic)
endif() 

# Set the C++ standard
target_compile_features(chip8 PRIVATE cxx_std_20)