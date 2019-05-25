/*      Filename:       Display.h
 *      Author:         Emanuel Aracena <earacenadev@gmail.com>
 *      Description:    Interface of Display sclass.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <cinttypes>

#include <SFML/Graphics.hpp>

class Display 
{
public:
        void initialize();
        bool is_running() const;
        bool poll_event( sf::Event& event );
        void set_height( const uint8_t& height );
        void set_width(  const uint8_t& width );
        uint8_t get_height() const;
        uint8_t get_width() const;
        void close_window() const;
        void clear_screen() const;
        void draw_graphics( const std::vector< uint8_t >& screen );
        void update_screen() const;
        
private:
        sf::RenderWindow window_;
        
        uint8_t screen_X_;
        uint8_t screen_Y_;
};

#endif
