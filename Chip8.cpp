/*
 * 	Filename: 	Chip8.cpp
 * 	Author: 	Emanuel Aracena <earacenadev@gmail.com>
 * 	Description:	Implementation for Chip8 class.
 */

#include "Chip8.h"


//      Constructor
Chip8::Chip8(const std::string & game_name)
{
        display_.set_height( 64 );
        display_.set_width( 32 );
        display_.initialize();

	cpu_.load_game(game_name);	
        cpu_.initialize();
}

void Chip8::run()
{
  //uint64_t cycle_number = 0;
        while ( display_.is_running() ) {
                while ( display_.poll_event( event_ ) ) {
                        if ( event_.type == sf::Event::Closed )
                                display_.close_window();
                }

		//std::cout << std::dec << "Cycle " << cycle_number << ":" << std::endl << "\t";
                cpu_.emulate_cycle();
                //++cycle_number;
		
                if ( cpu_.get_draw_status() == 1)
		  display_.draw_graphics(cpu_.get_screen());

                display_.clear_screen();
                display_.update_screen();
        }
}
