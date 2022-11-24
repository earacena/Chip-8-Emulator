/*
 * 	Filename: 	main.cpp
 * 	Author: 	Emanuel Aracena Beriguete
 * 	Description:	CHIP-8 Emulator
 */
#include <iostream>

#include "Chip8.h"

int main(int argc, char** argv)
{
  if(argc < 2) {
    std::cout << "Usage: ./Chip8 <Path to ROM>" << std::endl;
    return 0;
  }

  const std::string game_name(argv[1]);

  std::cout << "Using ROM directory/file: " << game_name << std::endl;
  
  // Initialize
  try {
    Chip8 chip8(game_name);
    Debugger debugger;
    chip8.run(&debugger);
  } catch (const std::string & e) {
    std::cerr << e << std::endl;
  }
}
