
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

// fmt
#include <fmt/core.h>

#include "CPU.h"

class Debugger {
public:
  Debugger();
  void start_frame();
  void place_widgets();
  void render(CPU * cpu);
  void cleanup();

private:

  const char * glsl_version_ = "#version 130";
  SDL_Window * window_;
  SDL_GLContext gl_context_;
  ImGuiIO * io_;
};

#endif