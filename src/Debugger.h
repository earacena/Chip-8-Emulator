
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <set>

// SDL2
#include "SDL.h"
#include "SDL_opengl.h"

// Dear ImGui
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

// fmt
#include <fmt/core.h>

#include "CPU.h"

class Debugger {
public:
  Debugger();
  void start_frame();
  void place_widgets(CPU * cpu);
  void render(CPU * cpu);
  std::string disassemble(uint16_t opcode);
  void cleanup();
  Uint32 get_window_id();

  // Breakpoint addresses
  std::set<uint8_t> breakpoints;

  bool paused = false;

private:
  const char * glsl_version_ = "#version 130";
  SDL_Window * window_;
  SDL_GLContext gl_context_;
  ImGuiIO * io_;
  std::unordered_map<uint16_t, std::string> disassembly_table_;
};

#endif