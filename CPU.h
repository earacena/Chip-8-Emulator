/*      
 *      Filename:    CPU.cpp
 *      Author:      Emanuel Aracena <earacenadev@gmail.com>
 *      Description: Interface of CPU class.
 */
#ifndef CPU_H
#define CPU_H

#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>

#include "Opcode.h"

class CPU
{
        friend class Opcode;
public:
        CPU();
        void initialize();
        void load_game( const std::string& game_path );
        void fetch();
        void execute();
        void emulate_cycle();
        
private:
        // Memory of size 4096 initialized to 0
        std::vector< uint8_t > memory_;
        
        // V[x] registers
        std::vector< uint8_t > V_;
        
        // Display of 64x32 size
        std::vector< uint8_t > screen_;
        
        // Keypad (0x0 to 0xF)
        std::vector< uint8_t > key_;
        
        // Memory stack
        std::vector< uint16_t > stack_;
        
        // Fontset
        std::vector< uint8_t > fontset_;
        
        std::vector< void (*)() > chip8_cputable_;
        void cpu_null();
        
        std::vector< void (*)() > chip8_mathtable_;
        void cpu_math();
        
        std::vector< void (*)() > chip8_calltable_;
        void cpu_call();
        
        std::vector< void (*)() > chip8_skiptable_;
        void cpu_skip();
        
        std::vector< void (*)() > chip8_fxtable_; 
        void cpu_fx();
        
        uint8_t delay_timer_;
        uint8_t sound_timer_;
        uint8_t drawflag_;
        uint8_t index_register_;
        uint8_t program_counter_;
        uint16_t stack_ptr_;
        uint16_t opcode_;
};

#endif
