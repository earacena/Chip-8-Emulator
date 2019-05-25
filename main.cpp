/*
 * 	Filename: 	main.cpp
 * 	Author: 	Emanuel Aracena <earacenadev@gmail.com>
 * 	Description:	CHIP-8 Emulator
 */
#include <iostream>

#include "Chip8.h"

int main()
{
	// Initialize
	Chip8 chip8;
	
	chip8.run();
	
	chip8.clean_up();
	
	return EXIT_SUCCESS;
}
