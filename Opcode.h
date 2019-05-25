/*      
 *      Filename:    Opcode.h
 *      Author:      Emanuel Aracena <earacenadev@gmail.com>
 *      Description: Prototypes for Opcode functions.
 */

#ifndef OPCODE_H
#define OPCODE_H

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>


class Opcode 
{
  friend class CPU;
private:        
  //void 0NNN();          Not necessary for most ROMS
  void 00E0();  // Clears the screen
  void 00EE();  // Returns from a subroutine
  void 1NNN();  // Jumps to address NNN
  void 2NNN();  // Calls subroutine at NNN
  void 3XNN();  // Skips the next instruction if VX == NN
  void 4XNN();  // Skips the next instruction if VX != NN
  void 5XY0();  // Skips the next instruction if VX == VY
  void 6XNN();  // Sets VX to NN
  void 7XNN();  // Adds NN to VX
  void 8XY0();  // Sets VX to the value of VY
  void 8XY1();  // Sets VX to VX or VY (bitwise OR)
  void 8XY2();  // Sets VX to VX and VY (bitwise AND)
  void 8XY3();  // Sets VX to VX xor VY (bitwise XOR)
  void 8XY4();  // Adds VY to VX. VF is set to 1 when there carry, 0 otherwise
  void 8XY5();  // VY is subtracted from VX. VF is set to 0 when theres a borrow, 1 otherwise
  void 8XY6();  // Stores the LSB of VX in VF and then shifts VX to the right by 1
  void 8XY7();  // Sets VX to VY minus VX. VF set to 0 when theres borrow, 1 otherwise
  void 8XYE();  // Stores the MSB of VX in VF and then shifts VX to the left by 1
  void 9XY0();  // Skips the next instr if VX doesnt equal VY
  void ANNN();  // Sets I to the address NNN
  void BNNN();  // Jumps to the address NNN plus V0
  void CXNN();  // Sets VX to the result of a bitwise and operation on a random number (0 to 255)
  void DXYN();  // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and N pixels high
  void EX9E();  // Skips the next instr if the key is stored in VX is pressed
  void EXA1();  // Skips the next instr if the the key stored in VX isn't pressed
  void FX07();  // Sets VX to the value of the delay timer
  void FX0A();  // A key press is awaited, and then stored in VX
  void FX15();  // Sets the delay timer to VX
  void FX18();  // Sets the sound timer to VX
  void FX1E();  // Adds VX to I
  void FX29();  // Sets I to the location of the sprite for the character in VX
  void FX33();  // Stores the binary-coded decimal representation of VX.
  void FX55();  // Stores V0 to VX in memory starting at address I
  void FX65();  // Fills V0 to VX from values from memory starting at address I
};
#endif
