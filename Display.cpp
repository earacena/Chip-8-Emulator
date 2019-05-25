/*      Filename:       Display.h
 *      Author:         Emanuel Aracena <earacenadev@gmail.com>
 *      Description:    Implementation of Display sclass.
 */

#include "Display.h"

void Display::initialize()
{
        window_.create( sf::VideoMode( screen_X_, screen_Y_ ), "CHIP-8" );
        window_.setFramerateLimit( 60 );
        window_.setVerticalSyncEnabled( true );
}

bool Display::is_running() const
{
        return window_.isOpen();
}

bool Display::poll_event( sf::Event& event ) const
{
        return window_.pollEvent( event );
}


void Display::set_height( const uint8_t& height)
{
        height_ = height;
}

void Display::set_width( const uint8_t& width )
{
        width_ = width;
}

uint8_t Display::get_height() const
{
        return screen_X_;
}

uint8_t Display::get_width() const
{
        return screen_Y_
}

void Display::close_window() const
{
        window_.close();
}

void Display::clear_screen() const
{
        window_.clear();
}

void Display::draw_graphics(const std::vector<uint8_t>& screen )
{
        uint8_t X_Pos = 0;
        uint8_t Y_Pos = 0;
        sf::Vector2i size( 8, 4 );
        
        std::vector< sf::IntRect > pixel( 2048);
        for ( uint16_t counter = 0; counter < 2048; ++counter ) {
                if ( counter % 64 == 0 ) {
                        X_Pos = 0;
                        Y_Pos += 4;
                }
                
                if ( screen[ counter ] == 1) {
                        pixel[ counter ].setOrigin( 0, 0 );
                        pixel[ counter ].setPosition(X_Pos, Y_pos);
                        pixel[ counter ].setSize( size );
                }
                
        }
        
        for ( uint16_t counter = 0; counter < 2048; ++counter )
                window_.draw( pixel[ counter ] );
}


void Display::update_screen() const
{
        window_.display();
}


