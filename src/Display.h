/*      Filename:       Display.h
 *      Author:         Emanuel Aracena Beriguete
 *      Description:    Interface of Display sclass.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <iostream>
#include <cinttypes>
#include <vector>
#include <memory>

// #include <SFML/Graphics.hpp>
#include <SDL.h>

class Display 
{
public:
  int initialize();
  bool is_running();
  void set_height(const uint16_t & height );
  void set_width(const uint16_t & width );
  int poll_event(SDL_Event & event);
  uint8_t get_height();
  uint8_t get_width();
  void close();
  void clear_screen();
  void draw_graphics(const std::vector<uint8_t>& screen);
  void update_screen();
        
private:
  // sf::RenderWindow window_;
  SDL_Window * window_;
  SDL_Surface * screen_surface_;
  SDL_Renderer * renderer_;
  
  // Screen width
  uint16_t screen_X_;
  
  // Screen height
  uint16_t screen_Y_;

  // Emulator execution status
  bool running_;
};

#endif
