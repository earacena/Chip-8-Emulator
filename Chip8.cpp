/*
 * 	Filename: 	Chip8.cpp
 * 	Author: 	Emanuel Aracena <earacenadev@gmail.com>
 * 	Description:	Implementation for Chip8 class.
 */

#include "Chip8.h"


//      Constructor
Chip8::Chip8()
{
        display_.set_height( 64 );
        display_.set_width( 32 );
        display_.initialize();
        
        cpu_.initialize();
}

void Chip8::run()
{
        while ( display_.is_running() ) {
                while ( display_.poll_event( event_ ) ) {
                        if ( event_.type == sf::Event::Closed )
                                display_.close_window();
                }
                
                cpu_.emulate_cycle();
                
                if ( cpu_.drawflag_ == 1)
                        display_.draw_graphics();

                display_.clear_screen();
                display_.update_screen();
        }
}
