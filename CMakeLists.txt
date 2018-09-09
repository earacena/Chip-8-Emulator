cmake_minimum_required(VERSION 2.6)
project(chip_8_emulator_cpp)

add_executable(chip_8_emulator_cpp main.cpp)

install(TARGETS chip_8_emulator_cpp RUNTIME DESTINATION bin)
