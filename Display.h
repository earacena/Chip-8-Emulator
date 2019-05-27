/*      Filename:       Display.h
 *      Author:         Emanuel Aracena <earacenadev@gmail.com>
 *      Description:    Interface of Display sclass.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <iostream>
#include <cinttypes>

#include <SFML/Graphics.hpp>

class Display 
{
public:
        void initialize();
        bool is_running();
        bool poll_event( sf::Event& event );
        void set_height( const uint16_t& height );
        void set_width(  const uint16_t& width );
        uint8_t get_height();
        uint8_t get_width();
        void close_window();
        void clear_screen();
        void draw_graphics( const std::vector< uint8_t >& screen );
        void update_screen();
        
private:
        sf::RenderWindow window_;
        
        uint16_t screen_X_;
        uint16_t screen_Y_;
};

#endif
