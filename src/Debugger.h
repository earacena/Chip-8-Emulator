
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>

// SDL2
#include "SDL.h"
#include "SDL_opengl.h"

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "Chip8.h"

class Debugger {
public:
  Debugger(Chip8 * chip8) : emulator(chip8) {}

  void initialize_debugger();
  void run_emulator();

private:
  Chip8 * emulator;
};

#endif