/*
 * 	Filename: 	Chip8.h
 * 	Author: 	Emanuel Aracena <earacenadev@gmail.com>
 * 	Description:	Chip8 class interface.
 */

#ifndef CHIP8_H
#define CHIP8_H

#include <string>
#include <cinttypes>

#include "CPU.h"
#include "Display.h"

class Chip8 
{
public:
        Chip8();
        
        void run();
        
        void clean_up();
        
private:
        sf::Event event_;
        CPU cpu_;
        Display display_;
        
};

#endif
