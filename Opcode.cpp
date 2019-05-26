/*      
 *      Filename:    Opcode.h
 *      Author:      Emanuel Aracena <earacenadev@gmail.com>
 *      Description: Implementations for Opcode functions.
 */

#include "Opcode.h"

void op_cpu_null() {}

void op_00E0()
{
  // 0x00E0: Clears the screen
  std::fill( screen_.begin(), screen_.end(), 0);
}

void op_00EE()
{
  // 0x00EE: Returns from subroutine
  --stack_pointer_;
  program_counter_ = stack_[ stack_pointer_ ];
  program_counter += 2;
}


void op_1NNN()
{
  // 0x1NNN: Jumps to address NNN
  program_counter_ = ( opcode_ & 0x0FFF );
}


void op_2NNN()
{
	
  // 0x2NNN: Calls subroutine at NNN
        
  stack_[ stack_ptr_ ] = program_counter_;
  ++stack_ptr_;
  program_counter_ = opcode_ & 0x0FFF;
}

void Opcode::op_3XNN()
{
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
        
  // 0x3XKK: Skip next instruction if VX = NN
  if ( V_[ opcode_ & 0x0F00 ] == ( opcode_ & 0x00FF ) )
    program += 4;
  else
    program += 2;
}

void op_4XNN()
{
  // 0x4XNN: SKip next instruction if VX != NN
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  if ( V_[ X ] != ( opcode_ & 0x00FF ) )
    program += 4;
  else
    program += 2;
}

void op_5XY0()
{
  // 0x5XY0: Skip next instruction if VX = VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
        
  if ( V_[ X ] == V_[ Y ] )
    program += 4;
  else
    program += 2;
}

void Opcode::op_6XNN()
{
  // 0x6XNN: Set VX = NN
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
        
  V_[ X ] = ( opcode_ & 0x00FF );
  program_counter_ += 2;
}

void op_7XNN()
{
  // 0x7XNN: Set VX = VX + NN
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
        
  V_[ X ] += ( opcode_ & 0x00FF );
  program_counter_ += 2;
}

void op_8XY0()
{
  // 8XY0: Set VX = VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] = V_[ Y ];
  program_counter_ += 2;
}

void op_8XY1()
{
  // 0x8XY1: Set VX = VX OR VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] |= V_[ Y ];
  program_counter_ += 2;
}

void op_8XY2()
{
  // 0x8XY2: Set VX = VX AND VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] &= V_[ Y ];
  program_counter_ += 2;
}

void op_8XY3()
{
  // 0x8XY3: Set VX = VX XOR VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] ^= V_[ Y ];
  program_counter_ += 2;
}

void op_8XY4()
{
  // 0x8XY4: Add the value of register VY to VX
  // Register VF (register 16) set to 1 if carry, otherwise zero.
        
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  // set value_Y to the value located in the specified register V[ VY ]
  uint8_t value_Y = V_[ VY ];
  uint8_t value_X = 0xFF - V_[ VX ];

  // Add
  V_[ VX ] += value_Y;

  // carry flag
  if (value_Y > value_X)
    V_[0xF] = 1;
  else
    V_[0xF] = 0;
  
  program_counter_ += 2;
}
 
void op_8XY5()
{
  // 0x8XY5: Set VX = VX - VY, set VF to carry
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;

  uint8_t value_Y = V_[VY];
  uint8_t value_X = V_[VX];

  if (value_Y > value_X)
    V_[0xF] = 1;
  else
    V_[0xF] = 0;

  V_[VX] -= value_Y;
  
  program_counter_ += 2;
}

void op_8XY6()
{
  // Stores the LSB of VX in VF and shifts VX to the right by 1
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;

  uint8_t value_X = V_[VX];

  V_[0xF] = value_X & 0x01;
  V_[VX] = V_[VX] >> 1;

  program_counter_ += 2;
}

void op_8XY7()
{
  // Sets VX to VY minus VX. VF set to 0 when theres borrow, 1 otherwise
  uint8_t VX = (opcode_ & 0x0F00) >> 8;
  uint8_t VY = (opcode_ & 0x00F0) >> 4;
  
  uint8_t value_Y = V_[VY];
  uint8_t value_X = V_[VX];

  if (value_X > value_Y)
    V_[0xF] = 1;
  else
    V_[0xF] = 0;

  V_[VY] -= value_X;

  program_counter_ += 2;
}

void op_8XYE()
{
  // Stores the MSB of VX in VF and then shifts VX to the left by 1
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;

  uint8_t value_X = V_[VX];

  V_[0xF] = value_X & 0x80;
  V_[VX] = V_[VX] << 1;

  program_counter_ += 2;

}

void op_9XY0()
{
  // Skips the next instr if VX doesnt equal VY
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;

  uint8_t value_X = V_[VX];
  uint8_t value_Y = V_[VY];

  if ( value_X == value_Y)
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void op_ANNN()
{
  // 0xANNN: Sets index register to the address NNN
  index_register_ = opcode_ & 0x0FFF;
  program_counter_ += 2;
}

void op_BNNN()
{
  // Jumps to address NNN
  program_counter_ = opcode_ & 0x0FFF;
}

void op_CXNN()
{
  // Sets VX to the result of a bitwise and operation on a random number (0 to 255)
  std::srand(std::time(nullptr));
  uint8_t VX = opcode_ & 0x0F00;
  uint8_t value_X = V_[VX];

  value_X &= std::rand() % 256;

  V_[VX] = value_X;

  program_counter_ += 2;
}

void op_DXYN()
{
  // 0xDXYN: Draws sprite at coordinate VX, VY
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;
  uint16_t value_X = v_[ VX ];
  uint16_t value_Y = v_[ VY ];
  uint16_t height = opcode_ & 0x000F;
  uint16_t pixel;
  uint16_t pixel_position;
  uint8_t scan;
        
  V_[ 0xF ] = 0;
        
  // IF RECEIVEING GRAPHICAL ERROR CHANGE ++yline to yline++
  for ( uint8_t yline = 0; yline < height; ++yline ) {
    pixel = memory_[ index_register_ + yline ];
                
    for ( uint8_t xline = 0; xline < 0; ++xline ) {
      scan = pixel & ( 0x80 >> xline );
      if ( ( pixel & scan ) != 0 ) {
	pixel_position = x + xline + ( ( y + yline ) * 64 );
	if ( screen_[ pixel_position ]  == 1 )
	  V_[ 0xF ] = 1;
	screen_[ pixel_position ] ^= 1;
      }
    }
  }
        
  drawflag_ = 1;
  program_counter_ += 2;
}

void op_EX9E()
{
  // 0xEX9E: Skips the next instruction if the key is stored in VX is pressed
  uint8_t VX = ( opcode_ & 0x0F00 );
  VX = VX >> 8;
        
  if ( key_[ v_[ VX ] ] != 0 )
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void op_EXA1()
{
  // Skips the next instructin if the key is stored in VX isnt pressed
  // ....
}

void op_FX07()
{
  // Sets VX to the value of the delay timer
  uint8_t VX = (opcode & 0x0F00) >> 8;

  V_[VX] = delay_timer_;
  
  program_counter_ += 2;
}

void op_FX0A()
{
  // A key press is awaited, and then stored in VX
}

void op_FX15()
{
  // Sets the delay timer to VX
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  delay_timer_ = V_[VX];

  program_counter_ += 2;
}

void op_FX18()
{
  // Sets the sound timer to VX
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  sound_timer_ = V_[VX];

  program_counter_ += 2;
}

void op_FX1E()
{
  // Adds VX to I
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  index_register_ += V_[VX];

  program_counter_ += 2;
}

void op_FX29()
{
  // Sets I to the location of the sprite for the character in VX
}

void op_FX33()
{
  // Stores the binary-coded decimal representation of VX
}

void op_FX55()
{
  // Stores V0 to VX in memory starting at address I.
}

void op_FX65()
{
  // Fills V0 to VX from values from memory starting at address I
}
