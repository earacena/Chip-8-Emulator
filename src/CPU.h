/*      
 *      Filename:    CPU.cpp
 *      Author:      Emanuel Aracena Beriguete
 *      Description: Interface of CPU class.
 */
#ifndef CPU_H
#define CPU_H

#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <SDL.h>

class CPU
{
public:
  CPU();
  void load_game( const std::string& game_path );
  void fetch();
  void execute();
  void emulate_cycle();
  void update_key_states(const SDL_Event & event);

  // CPU Components
  // Memory of size 4096 initialized to 0
  std::vector< uint8_t > memory;
        
  // V[x] registers
  std::vector< uint8_t > V;
        
  // Display of 64x32 size
  std::vector< uint8_t > screen;
        
  // Keypad (0x0 to 0xF)
  std::vector< uint8_t > key;
        
  // Memory stack
  std::vector< uint16_t > stack;
        
  // Fontset
  std::vector< uint8_t > fontset;

  // Special components/registers
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t draw_flag;
  uint16_t index_register;
  uint64_t program_counter;
  uint16_t stack_ptr;
  uint16_t opcode;

  // Input/Keypad related
  bool is_key_pressed;
  uint8_t key_pressed;
  uint8_t get_key_pressed(const SDL_Event & event);
  //void update_key_states(SDL_Event & e);

private:
  // Opcode tables
  std::vector< std::function<void()> > chip8_cputable_;
  std::vector< std::function<void()> > chip8_mathtable_;
  std::vector< std::function<void()> > chip8_calltable_;
  std::vector< std::function<void()> > chip8_skiptable_;

  std::unordered_map<SDL_Keycode, uint8_t> keys_map_;
  
  // Opcodes
  // Null, function table padding
  void op_cpu_null();

  //void 0NNN();          Not necessary for most ROMS
  void op_00E0();  // Clears the screen
  void op_00EE();  // Returns from a subroutine
  void op_1NNN();  // Jumps to address NNN
  void op_2NNN();  // Calls subroutine at NNN
  void op_3XNN();  // Skips the next instruction if VX == NN
  void op_4XNN();  // Skips the next instruction if VX != NN
  void op_5XY0();  // Skips the next instruction if VX == VY
  void op_6XNN();  // Sets VX to NN
  void op_7XNN();  // Adds NN to VX
  void op_8XY0();  // Sets VX to the value of VY
  void op_8XY1();  // Sets VX to VX or VY (bitwise OR)
  void op_8XY2();  // Sets VX to VX and VY (bitwise AND)
  void op_8XY3();  // Sets VX to VX xor VY (bitwise XOR)
  void op_8XY4();  // Adds VY to VX. VF is set to 1 when there carry, 0 otherwise
  void op_8XY5();  // VY is subtracted from VX. VF is set to 0 when theres a borrow, 1 otherwise
  void op_8XY6();  // Stores the LSB of VX in VF and then shifts VX to the right by 1
  void op_8XY7();  // Sets VX to VY minus VX. VF set to 0 when theres borrow, 1 otherwise
  void op_8XYE();  // Stores the MSB of VX in VF and then shifts VX to the left by 1
  void op_9XY0();  // Skips the next instr if VX doesnt equal VY
  void op_ANNN();  // Sets I to the address NNN
  void op_BNNN();  // Jumps to the address NNN plus V0
  void op_CXNN();  // Sets VX to the result of a bitwise and operation on a random number (0 to 255)
  void op_DXYN();  // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and N pixels high
  void op_EX9E();  // Skips the next instr if the key is stored in VX is pressed
  void op_EXA1();  // Skips the next instr if the the key stored in VX isn't pressed
  void op_FX07();  // Sets VX to the value of the delay timer
  void op_FX0A();  // A key press is awaited, and then stored in VX
  void op_FX15();  // Sets the delay timer to VX
  void op_FX18();  // Sets the sound timer to VX
  void op_FX1E();  // Adds VX to I
  void op_FX29();  // Sets I to the location of the sprite for the character in VX
  void op_FX33();  // Stores the binary-coded decimal representation of VX.
  void op_FX55();  // Stores V0 to VX in memory starting at address I
  void op_FX65();  // Fills V0 to VX from values from memory starting at address I
};

#endif
