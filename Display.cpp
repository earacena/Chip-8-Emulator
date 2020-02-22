/*      Filename:       Display.h
 *      Author:         Emanuel Aracena Beriguete
 *      Description:    Implementation of Display sclass.
 */

#include "Display.h"

void Display::initialize()
{
  window_.create( sf::VideoMode( screen_X_, screen_Y_ ), "CHIP-8" );
  window_.setFramerateLimit( 60 );
  window_.setVerticalSyncEnabled( true );
}

bool Display::is_running()
{
  return window_.isOpen();
}

bool Display::poll_event( sf::Event& event )
{
  return window_.pollEvent( event );
}


void Display::set_height( const uint16_t& height)
{
  screen_Y_ = height;
}

void Display::set_width( const uint16_t& width )
{
  screen_X_ = width;
}

uint8_t Display::get_height()
{
  return screen_Y_;
}

uint8_t Display::get_width()
{
  return screen_X_;
}

void Display::close_window()
{
  window_.close();
}

void Display::clear_screen()
{
  window_.clear(sf::Color::Black);
}

void Display::draw_graphics(const std::vector<uint8_t>& screen )
{
  double X_Pos = 0;
  double Y_Pos = 0;

  sf::Vector2f size( 10, 10 );
        
  std::vector< sf::RectangleShape > pixel(2048);
  for ( uint16_t counter = 0; counter < 2048; ++counter ) {
    if ( counter % 64 == 0 ) {
      X_Pos = 0.0;
      Y_Pos += 10.0;
    }

    pixel[ counter ].setPosition(X_Pos, Y_Pos);
    pixel[ counter ].setSize( size );
               
    if ( screen[ counter ] == 1) 
      pixel[ counter ].setFillColor(sf::Color::Red);
    else 
      pixel[ counter ].setFillColor(sf::Color::Black);
    

    X_Pos += 10;
  }
  
  for ( uint16_t counter = 0; counter < 2048; ++counter ) {
    window_.draw( pixel[ counter ] );

    // Text debug, to confirm correct screen array behavior
    // if(counter % 64 == 0)
    //   std::cout << std::endl;
    // (screen[counter] == 1 ? std::cout << "*" : std::cout << "_");
    
  }

}


void Display::update_screen()
{
  window_.display();
}


