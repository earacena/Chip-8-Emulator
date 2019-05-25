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
        program_counter_ = static_cast< uint8_t >( 0x200 );
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
	chip8_cputable_.push_back(cpu_null);
	chip8_cputable_.push_back(1NNN);
	chip8_cputable_.push_back(2NNN);
	chip8_cputable_.push_back(3XNN);
	chip8_cputable_.push_back(4XNN);
	chip8_cputable_.push_back(5XY0);
	chip8_cputable_.push_back(6XNN);
	chip8_cputable_.push_back(7XNN);
	chip8_cputable_.push_back(cpu_null);
	chip8_cputable_.push_back(cpu_null);
	chip8_cputable_.push_back(ANNN);
	chip8_cputable_.push_back(BNNN);
	chip8_cputable_.push_back(CXNN);
	chip8_cputable_.push_back(DXYN);
	chip8_cputable_.push_back(cpu_null);
	chip8_cputable_.push_back(cpu_null);
	chip8_cputable_.push_back(cpu_null);


	// CPU Math opcodes
        chip8_mathtable_.push_back(8XY0);
	chip8_mathtable_.push_back(8XY1);
	chip8_mathtable_.push_back(8XY2);
	chip8_mathtable_.push_back(8XY3);
	chip8_mathtable_.push_back(8XY4);
	chip8_mathtable_.push_back(8XY5);
	chip8_mathtable_.push_back(8XY6);
	chip8_mathtable_.push_back(8XY7);
	chip8_mathtable_.push_back(cpu_null);
	chip8_mathtable_.push_back(cpu_null);
	chip8_mathtable_.push_back(cpu_null);
	chip8_mathtable_.push_back(cpu_null);
	chip8_mathtable_.push_back(cpu_null);
	chip8_mathtable_.push_back(cpu_null);
	chip8_mathtable_.push_back(8XYE);
	chip8_mathtable_.push_back(cpu_null);

	// CPU call table opcodes
        chip8_calltable_.push_back(00E0);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(cpu_null);
	chip8_calltable_.push_back(00EE);
	chip8_calltable_.push_back(cpu_null);


	// CPU skip table
        chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(EXA1);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(9XY0);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(cpu_null);
	chip8_skiptable_.push_back(EX9E);
	chip8_skiptable_.push_back(cpu_null);
	

}

void CPU::load_game( const std::string& game_path )
{
        std::ifstream file( game_path, std::ios::binary );
                
        if ( file ) {
                file.seekg( 0, std::ios::end );
                uint64_t length = file.tellg();
                char buffer[ length ];
                file.seekg( 0, std::ios::beg );
                file.read( &buffer[0], length );
                file.close();
        }
        
        for ( uint64_t counter = 0; counter < length; ++counter )
                memory_[ counter + 512 ] = buffer[ i ];
        
}

void CPU::cpu_null()
{
        // Do nothing
}

void CPU::fetch()
{
        opcode_ = memory[ program_counter_ ] << 8 | memory[ program_counter_ + 1 ];
        program_counter_ += 2;
}

void CPU::execute()
{
  uint8_t MSB_id = (opcode_ & 0xF000) >> 12;
  if ( MSB_id == 0x0) {
    chip8_calltable_[opcode_ & 0x000F ];
  } else if ( ((MSB_id >= 1) && (MSB_id <= 7)) || ((MSB_id >= 0xA) && (MSB_id <= 0xD))) {
    chip8_cputable_[ MSB_id ];
  } else if ( MSB_id == 0x8 ) {
    chip8_mathtable_[ opcode_ & 0x000F ];
  } else if ( MSB_id == 0x0 ) {
    chip8_calltable_[ opcode_ & 0x000F ];
  } else if ( (MSB_id == 0xE) || (MSB_id == 0x9) ){
    if (MSB_id == 0xE)
      chip8_skiptable_[ opcode_ & 0x000F ];
    else
      chip8_skiptable_[ 9 ]; // 9XY0
  } else if ( MSB_id == 0xF ) {
    if ( (opcode_ & 0x00FF) == 0x07 )
      FX07();
    else if ( (opcode_ & 0x00FF) == 0x0A )
      FX0A();
    else if ( (opcode_ & 0x00FF) == 0x15 )
      FX15();
    else if ( (opcode_ & 0x00FF) == 0x18 )
      FX18();
    else if ( (opcode_ & 0x00FF) == 0x1E )
      FX1E();
    else if ( (opcode_ & 0x00FF) == 0x29 )
      FX29();
    else if ( (opcode_ & 0x00FF) == 0x33 )
      FX33();
    else if ( (opcode_ & 0x00FF) == 0x55 )
      FX55();
    else if ( (opcode_ & 0x00FF) == 0x65 )
      FX65();
    
  }
}

void CPU::emulate_cycle()
{
        fetch();
        execute();
        
        if ( delay_timer_ > 0 )
                --delay_timer;
        
        if ( sound_timer_ > 0 ) {
                if ( sound_timer_ == 1)
                        std::cout << "BEEP!";
        }
}
