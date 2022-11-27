/*
 * 	Filename: 	Chip8.cpp
 * 	Author: 	Emanuel Aracena Beriguete
 * 	Description:	Implementation for Chip8 class.
 */

#include "Chip8.h"

//      Constructor
Chip8::Chip8(const std::string& game_name) {
  display_.set_height(320);
  display_.set_width(640);
  display_.initialize();

  cpu_.load_game(game_name);
}

void Chip8::run(Debugger* debugger = nullptr) {
  while (display_.is_running()) {
    while (display_.poll_event(event_)) {
      if (debugger != nullptr) 
        ImGui_ImplSDL2_ProcessEvent(&event_);

      if (event_.type == SDL_QUIT ||
          (event_.type == SDL_WINDOWEVENT &&
           event_.window.event == SDL_WINDOWEVENT_CLOSE &&
           (event_.window.windowID == display_.get_window_id() ||
            event_.window.windowID == debugger->get_window_id()))) {
        cleanup();
        if (debugger != nullptr) debugger->cleanup();
      } else if (event_.type == SDL_KEYUP) {
        cpu_.update_key_states(event_);
      }
    }

    // Render and update debugger window
    if (debugger != nullptr)
      debugger->render(&cpu_);

    if (!debugger->paused || debugger->stepped) {
      cpu_.emulate_cycle();

      // Draw changes on screen
      display_.clear_screen();
      if (cpu_.draw_flag == 1) display_.draw_graphics(cpu_.screen);
      display_.update_screen();

      // Check for breakpoints
      if (debugger->breakpoints.contains(cpu_.program_counter)) {
        debugger->paused = true;
        debugger->breakpoints.erase(cpu_.program_counter);
      }

      // Sleep to slow down emulation
      // std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    if (debugger->stepped)
      debugger->stepped = false;
  }
}

void Chip8::cleanup() { display_.close(); }