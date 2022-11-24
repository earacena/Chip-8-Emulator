/*      Filename:       Display.h
 *      Author:         Emanuel Aracena Beriguete
 *      Description:    Implementation of Display sclass.
 */

#include "Display.h"

void Display::initialize()
{
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    throw std::string(fmt::format("Failed to initialize SDL: {}", SDL_GetError()));

  // Create window
  window_ = SDL_CreateWindow(
    "Chip-8 Emulator",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    screen_X_,
    screen_Y_,
    SDL_WINDOW_SHOWN
  );

  if (window_ == nullptr) 
     throw std::string(fmt::format("Failed to create window: {}", SDL_GetError()));

  // Create renderer
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (renderer_ == nullptr) 
    throw std::string(fmt::format("Failed to create renderer: {}", SDL_GetError()));
}

bool Display::is_running()
{
  return running_;
}

int Display::poll_event( SDL_Event & event )
{
  return SDL_PollEvent( &event );
}

void Display::set_height( const uint16_t & height)
{
  screen_Y_ = height;
}

void Display::set_width( const uint16_t & width )
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

void Display::close()
{
  // Stop emulator loop
  running_ = false;

  // Destroy and release SDL components
  SDL_DestroyWindow(window_);
  SDL_DestroyRenderer(renderer_);
  window_ = nullptr;
  renderer_ = nullptr;

  SDL_Quit();
}

void Display::clear_screen()
{
  SDL_RenderClear(renderer_);
}

void Display::draw_graphics(const std::vector<uint8_t> & screen)
{
  int x_pos = 0;
  int y_pos = 0;

  int cell_width = 20;
  int cell_height = 20;
   
  std::vector< SDL_Rect > pixel(2048);
  for ( uint16_t counter = 0; counter < 2048; ++counter ) {
    if ( counter % 64 == 0 ) {
      x_pos = 0.0;
      y_pos += 10.0;
    }

    pixel.at(counter).x = x_pos;
    pixel.at(counter).y = y_pos;
    pixel.at(counter).w = cell_width;
    pixel.at(counter).h = cell_height;

    x_pos += 10;
  }
  
  for ( uint16_t counter = 0; counter < 2048; ++counter ) {
    // Change color depending if screen is marked 
    if ( screen[ counter ] == 1)
      SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 0); // Red
    else 
      SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0); // Black

    // Fill in rectangle
    SDL_RenderFillRect(renderer_, &pixel.at(counter));

    //window_.draw( pixel[ counter ] );

    // Text debug, to confirm correct screen array behavior
    // if(counter % 64 == 0)
    //   std::cout << std::endl;
    // (screen[counter] == 1 ? std::cout << "*" : std::cout << "_");
    
  }

}

void Display::update_screen()
{
  SDL_RenderPresent(renderer_);
}


