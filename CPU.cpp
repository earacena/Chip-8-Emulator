/*      
 *      Filename:    CPU.cpp
 *      Author:      Emanuel Aracena <earacenadev@gmail.com>
 *      Description: Implementation of CPU class.
 */

#include "CPU.h"


CPU::CPU()
{
  initialize();
}

void CPU::initialize()
{
        // Initialize variables
        program_counter_ = static_cast< uint64_t >( 0x200 );
        opcode_          = 0;
        index_register_  = 0;
        stack_ptr_       = 0;
        delay_timer_     = 0;
        sound_timer_     = 0;
        
        // Initialize vectors
        memory_.resize( 4096, 0);
        V_.resize( 16, 0);
        screen_.resize( 2048, 0);
        key_.resize( 16, 0 );
        stack_.resize( 16, 0 );
        
        fontset_ = {
                0xF0, 0x90, 0x90, 0x90, 0xF0,	// 0
                0x20, 0x60, 0x20, 0x20, 0x70, 	// 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, 	// 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, 	// 3
                0x90, 0x90, 0xF0, 0x10, 0x10, 	// 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, 	// 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, 	// 6
                0xF0, 0x10, 0x20, 0x40, 0x40, 	// 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, 	// 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, 	// 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, 	// A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, 	// B
                0xF0, 0x80, 0x80, 0x80, 0xF0, 	// C
                0xE0, 0x90, 0x90, 0x90, 0xE0, 	// D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, 	// E
                0xF0, 0x80, 0xF0, 0x80, 0x80  	// F
        };
        
        // Load fontset into memeory, 80 is default size of 80
        for ( uint8_t index = 0; index < 80; ++index )
                memory_[ index ] = fontset_[ index ];

	// Add opcode function calls to appropriate table

	// CPU table
	chip8_cputable_ = {
			   [this]{ op_cpu_null(); },
			   [this]{ op_1NNN(); },
			   [this]{ op_2NNN(); },
			   [this]{ op_3XNN(); },
			   [this]{ op_4XNN(); },
			   [this]{ op_5XY0(); },
			   [this]{ op_6XNN(); },
			   [this]{ op_7XNN(); },
			   [this]{ op_cpu_null(); },
			   [this]{ op_cpu_null(); },
			   [this]{ op_ANNN(); },
			   [this]{ op_BNNN(); },
			   [this]{ op_CXNN(); },
			   [this]{ op_DXYN(); },
			   [this]{ op_cpu_null(); },
			   [this]{ op_cpu_null(); },
			   [this]{ op_cpu_null(); }
	};
	

	// CPU Math opcodes
	chip8_mathtable_ = {
			    [this]{ op_8XY0(); },
			    [this]{ op_8XY1(); },
			    [this]{ op_8XY2(); },
			    [this]{ op_8XY3(); },
			    [this]{ op_8XY4(); },
			    [this]{ op_8XY5(); },
			    [this]{ op_8XY6(); },
			    [this]{ op_8XY7(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },			     
			    [this]{ op_cpu_null(); },			     
			    [this]{ op_8XYE(); },	
			    [this]{ op_cpu_null(); }
	};
	
	// CPU call table opcodes

	chip8_calltable_ = {
			    [this]{ op_00E0(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_00EE(); },
			    [this]{ op_cpu_null(); }
	};

	// CPU skip table
	chip8_skiptable_ = {
			    [this]{ op_cpu_null(); },
			    [this]{ op_EXA1(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_9XY0(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_cpu_null(); },
			    [this]{ op_EX9E(); },
			    [this]{ op_cpu_null(); },

	};
}

void CPU::load_game( const std::string& game_path )
{
        std::ifstream file( game_path, std::ios::binary );
                
        if ( file ) {
                file.seekg( 0, std::ios::end );
                //uint64_t length = file.tellg();
                char buffer[ 4096 - 512 ];
                file.seekg( 0, std::ios::beg );
                file.read( &buffer[0], (4096 - 512) );
                file.close();
        
        
		for ( uint64_t counter = 0; counter < (4096 - 512); ++counter )
                memory_[ counter + 512 ] = buffer[ counter ];
        }
}


void CPU::fetch()
{
        opcode_ = memory_[ program_counter_ ] << 8 | memory_[ program_counter_ + 1 ];
	std::cout << static_cast<int>(program_counter_) << ": 0x"
		  << std::uppercase << std::hex << opcode_ << std::endl;
}

void CPU::execute()
{
  uint8_t MSB_id = (opcode_ & 0xF000) >> 12;
  if ( MSB_id == 0x0) {
    chip8_calltable_[opcode_ & 0x000F ]();
  } else if ( ((MSB_id >= 1) && (MSB_id <= 7)) || ((MSB_id >= 0xA) && (MSB_id <= 0xD))) {
    chip8_cputable_[ MSB_id ]();
  } else if ( MSB_id == 0x8 ) {
    chip8_mathtable_[ opcode_ & 0x000F ]();
  } else if ( MSB_id == 0x0 ) {
    chip8_calltable_[ opcode_ & 0x000F ]();
  } else if ( (MSB_id == 0xE) || (MSB_id == 0x9) ){
    if (MSB_id == 0xE)
      chip8_skiptable_[ opcode_ & 0x000F ]();
    else
      chip8_skiptable_[ 9 ](); // 9XY0
  } else if ( MSB_id == 0xF ) {
    if ( (opcode_ & 0x00FF) == 0x07 )
      op_FX07();
    else if ( (opcode_ & 0x00FF) == 0x0A )
      op_FX0A();
    else if ( (opcode_ & 0x00FF) == 0x15 )
      op_FX15();
    else if ( (opcode_ & 0x00FF) == 0x18 )
      op_FX18();
    else if ( (opcode_ & 0x00FF) == 0x1E )
      op_FX1E();
    else if ( (opcode_ & 0x00FF) == 0x29 )
      op_FX29();
    else if ( (opcode_ & 0x00FF) == 0x33 )
      op_FX33();
    else if ( (opcode_ & 0x00FF) == 0x55 )
      op_FX55();
    else if ( (opcode_ & 0x00FF) == 0x65 )
      op_FX65();
  } else {
    std::cout << "\tUnrecognized opcode.";
  }
}

void CPU::emulate_cycle()
{
        fetch();
        execute();
        
        if ( delay_timer_ > 0 )
                --delay_timer_;
        
        if ( sound_timer_ > 0 ) {
                if ( sound_timer_ == 1)
                        std::cout << "BEEP!";
        }
}

uint8_t CPU::get_draw_status()
{
  return drawflag_;
}

std::vector<uint8_t> CPU::get_screen()
{
  return screen_;
}


// Opcodes

void CPU::op_cpu_null() {
  std::cout << "\tCPU_NULL";
}

void CPU::op_00E0()
{
  // 0x00E0: Clears the screen
  std::fill( screen_.begin(), screen_.end(), 0);
}

void CPU::op_00EE()
{
  // 0x00EE: Returns from subroutine
  --stack_ptr_;
  program_counter_ = stack_[ stack_ptr_ ];
  program_counter_ += 2;
}


void CPU::op_1NNN()
{
  // 0x1NNN: Jumps to address NNN
  program_counter_ = ( opcode_ & 0x0FFF );
}


void CPU::op_2NNN()
{
	
  // 0x2NNN: Calls subroutine at NNN
        
  stack_[ stack_ptr_ ] = program_counter_;
  ++stack_ptr_;
  program_counter_ = opcode_ & 0x0FFF;
}

void CPU::op_3XNN()
{
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
        
  // 0x3XKK: Skip next instruction if VX = NN
  if ( V_[ VX ] == ( opcode_ & 0x00FF ) )
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void CPU::op_4XNN()
{
  // 0x4XNN: SKip next instruction if VX != NN
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  if ( V_[ X ] != ( opcode_ & 0x00FF ) )
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void CPU::op_5XY0()
{
  // 0x5XY0: Skip next instruction if VX = VY
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;
        
  if ( V_[ VX ] == V_[ VY ] )
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void CPU::op_6XNN()
{
  // 0x6XNN: Set VX = NN
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
        
  V_[ X ] = ( opcode_ & 0x00FF );
  program_counter_ += 2;
}

void CPU::op_7XNN()
{
  // 0x7XNN: Set VX = VX + NN
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
        
  V_[ X ] += ( opcode_ & 0x00FF );
  program_counter_ += 2;
}

void CPU::op_8XY0()
{
  // 8XY0: Set VX = VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] = V_[ Y ];
  program_counter_ += 2;
}

void CPU::op_8XY1()
{
  // 0x8XY1: Set VX = VX OR VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] |= V_[ Y ];
  program_counter_ += 2;
}

void CPU::op_8XY2()
{
  // 0x8XY2: Set VX = VX AND VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  V_[ X ] &= V_[ Y ];
  program_counter_ += 2;
}

void CPU::op_8XY3()
{
  // 0x8XY3: Set VX = VX XOR VY
  uint8_t X = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t Y = ( opcode_ & 0x00F0 ) >> 4;
  
  V_[ X ] ^= V_[ Y ];
  
  program_counter_ += 2;
}

void CPU::op_8XY4()
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
 
void CPU::op_8XY5()
{
  // 0x8XY5: Set VX = VX - VY, set VF to carry
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;

  uint8_t value_Y = V_[VY];
  uint8_t value_X = V_[VX];

  V_[VX] = value_X - value_Y;
  
  if (value_Y > value_X)
    V_[0xF] = 1;
  else
    V_[0xF] = 0;

  program_counter_ += 2;
}

void CPU::op_8XY6()
{
  // Stores the LSB of VX in VF and shifts VX to the right by 1
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;

  uint8_t value_X = V_[VX];

  V_[0xF] = value_X & 0x01;
  V_[VX] = V_[VX] >> 1;

  program_counter_ += 2;
}

void CPU::op_8XY7()
{
  // Sets VX to VY minus VX. VF set to 0 when theres borrow, 1 otherwise
  uint8_t VX = (opcode_ & 0x0F00) >> 8;
  uint8_t VY = (opcode_ & 0x00F0) >> 4;
  
  uint8_t value_Y = V_[VY];
  uint8_t value_X = V_[VX];

  V_[VX] = value_Y - value_X;
  
  if (value_X > value_Y)
    V_[0xF] = 1;
  else
    V_[0xF] = 0;

  program_counter_ += 2;
}

void CPU::op_8XYE()
{
  // Stores the MSB of VX in VF and then shifts VX to the left by 1
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;

  uint8_t value_X = V_[VX];

  V_[0xF] = value_X & 0x80;
  V_[VX] = V_[VX] << 1;

  program_counter_ += 2;

}

void CPU::op_9XY0()
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

void CPU::op_ANNN()
{
  // 0xANNN: Sets index register to the address NNN
  index_register_ = opcode_ & 0x0FFF;
  program_counter_ += 2;
}

void CPU::op_BNNN()
{
  // Jumps to address NNN
  program_counter_ = opcode_ & 0x0FFF;
}

void CPU::op_CXNN()
{
  // Sets VX to the result of a bitwise and operation on a random number (0 to 255)
  std::srand(std::time(nullptr));
  uint8_t VX = opcode_ & 0x0F00;
  uint8_t value_X = V_[VX];

  value_X &= std::rand() % 256;

  V_[VX] = value_X;

  program_counter_ += 2;
}

void CPU::op_DXYN()
{
  // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
  // Each row of 8 pixels is read as bit-coded starting from memory location I;
  //   I value doesn’t change after the execution of this instruction.
  // As described above, VF is set to 1 if any screen pixels are flipped from set to
  // unset when the sprite is drawn, and to 0 if that doesn’t happen 
  // 
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
  uint8_t VY = ( opcode_ & 0x00F0 ) >> 4;
  uint16_t value_X = V_[ VX ];
  uint16_t value_Y = V_[ VY ];
  uint16_t height = opcode_ & 0x000F;
  uint16_t pixel;
  uint16_t pixel_position;
  uint8_t scan;
        
  V_[ 0xF ] = 0;
        
  //IF RECEIVEING GRAPHICAL ERROR CHANGE ++yline to yline++
  for ( uint8_t yline = 0; yline < height; yline++ ) {
    pixel = memory_[ index_register_ + yline ];
                
    for ( uint8_t xline = 0; xline < 8; xline++ ) {
      scan = ( 0x80 >> xline );
      if ( ( pixel & scan ) != 0 ) {
  	pixel_position = value_X + xline + ( ( value_Y + yline ) * 64 );
  	if ( screen_[ pixel_position ]  == 1 )
  	  V_[ 0xF ] = 1;
  	screen_[ pixel_position ] ^= 1;
      }
    }
  }

  drawflag_ = 1;
  program_counter_ += 2;
}

void CPU::op_EX9E()
{
  // 0xEX9E: Skips the next instruction if the key is stored in VX is pressed
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;
        
  if ( key_[ V_[ VX ] ] != 0 )
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void CPU::op_EXA1()
{
  // Skips the next instruction if the key is stored in VX isnt pressed
  uint8_t VX = ( opcode_ & 0x0F00 ) >> 8;

  if( key_[ V_[ VX ] ] == 0 )
    program_counter_ += 4;
  else
    program_counter_ += 2;
}

void CPU::op_FX07()
{
  // Sets VX to the value of the delay timer
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  V_[VX] = delay_timer_;
  
  program_counter_ += 2;
}

void CPU::op_FX0A()
{
  // A key press is awaited, and then stored in VX
  program_counter_ += 2;
}

void CPU::op_FX15()
{
  // Sets the delay timer to VX
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  delay_timer_ = V_[VX];

  program_counter_ += 2;
}

void CPU::op_FX18()
{
  // Sets the sound timer to VX
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  sound_timer_ = V_[VX];

  program_counter_ += 2;
}

void CPU::op_FX1E()
{
  // Adds VX to I
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  index_register_ += V_[VX];

  program_counter_ += 2;
}

void CPU::op_FX29()
{
  // Sets I to the location of the sprite for the character in VX
  uint8_t VX = (opcode_ & 0x0F00) >> 8;
  index_register_ = memory_[ V_[VX] * 5 ];

  program_counter_ += 2;
}

// Used implementation of FX33 by TJA
// (http://www.multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml)
void CPU::op_FX33()
{
  // Stores the binary-coded decimal representation of VX
  uint8_t VX = (opcode_ & 0x0F00) >> 8;
  
  memory_[ index_register_ ] = V_[VX] / 100;
  memory_[ index_register_+1 ] = (V_[VX] / 10) % 100;
  memory_[ index_register_+2 ] = (V_[VX] % 100) % 10;

  program_counter_ += 2;
}

void CPU::op_FX55()
{
  // Stores V0 to VX in memory starting at address I.
  uint8_t VX = (opcode_ & 0x0F00) >> 8;
  uint8_t offset = 0;
  
  for(uint8_t reg = 0x0; reg <= VX; ++reg) {
    memory_[index_register_+offset] = V_[reg];
    ++offset;
  }

  program_counter_ += 2;
}

void CPU::op_FX65()
{
  // Fills V0 to VX from values from memory starting at address I
  uint8_t VX = (opcode_ & 0x0F00) >> 8;

  for(uint8_t reg = 0x0; reg <= VX; ++reg)
    V_[reg] = memory_[index_register_+reg];

  program_counter_ += 2;
}
