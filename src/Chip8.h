/*
 * 	Filename: 	Chip8.h
 * 	Author: 	Emanuel Aracena Beriguete 
 * 	Description:	Chip8 class interface.
 */

#ifndef CHIP8_H
#define CHIP8_H

#include <string>
#include <cinttypes>
#include <SDL.h>
#include <chrono>
#include <thread>

#include "Debugger.h"
#include "CPU.h"
#include "Display.h"

class Chip8 
{
public:
  Chip8(const std::string & game_name);
  void run(Debugger * debugger);
  void cleanup();

private:
  SDL_Event event_;
  CPU cpu_;
  Display display_;
};

#endif
