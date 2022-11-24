/*
 * 	Filename: 	Chip8.cpp
 * 	Author: 	Emanuel Aracena Beriguete
 * 	Description:	Implementation for Chip8 class.
 */

#include "Chip8.h"


//      Constructor
Chip8::Chip8(const std::string & game_name)
{
  display_.set_height( 320 );
  display_.set_width( 640 );
  display_.initialize();

  cpu_.load_game(game_name);
  cpu_.initialize();
}

void Chip8::run(Debugger * debugger = nullptr)
{
  while ( display_.is_running() ) {
    while ( display_.poll_event( event_ ) != 0) {
      cpu_.sync_event(event_);
      if ( event_.type == SDL_QUIT ) {
        cleanup();
        if (debugger != nullptr) 
          debugger->cleanup();
      } else if (event_.type == SDL_KEYUP) {
        cpu_.update_key_states();
      }
    }

    cpu_.emulate_cycle();

    // Draw changes on screen
    display_.clear_screen();
    if ( cpu_.get_draw_status() == 1)
      display_.draw_graphics(cpu_.get_screen());
    display_.update_screen();

    // Render and update debugger window
    if (debugger != nullptr)
      debugger->render();

    // Sleep to slow down emulation
    // std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
  
}

void Chip8::cleanup() {
  display_.close();
}