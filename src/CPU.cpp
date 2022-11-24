/*
 *      Filename:    CPU.cpp
 *      Author:      Emanuel Aracena Beriguete
 *      Description: Implementation of CPU class.
 */

#include "CPU.h"

CPU::CPU() {
  // Initialize variables
  program_counter = static_cast<uint64_t>(0x200);
  opcode = 0;
  index_register = 0;
  stack_ptr = 0;
  delay_timer = 0;
  sound_timer = 0;

  // Initialize vectors
  memory.resize(4096, 0);
  V.resize(16, 0);
  screen.resize(2048, 0);
  key.resize(16, 0);
  stack.resize(16, 0);

  fontset = {
      0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
      0xF0, 0x80, 0xF0, 0x80, 0x80   // F
  };

  // Load fontset into memory, 80 is default size of 80
  for (uint8_t index = 0; index < 80; ++index) memory[index] = fontset[index];

  // Initialize keys
  keys_map_ = {
      {SDLK_KP_0, 0x00}, {SDLK_KP_1, 0x01}, {SDLK_KP_2, 0x02},
      {SDLK_KP_3, 0x03}, {SDLK_KP_4, 0x04}, {SDLK_KP_5, 0x05},
      {SDLK_KP_6, 0x06}, {SDLK_KP_7, 0x07}, {SDLK_KP_8, 0x08},
      {SDLK_KP_9, 0x09}, {SDLK_q, 0x0A},    {SDLK_w, 0x0B},
      {SDLK_e, 0x0C},    {SDLK_a, 0x0D},    {SDLK_s, 0x0E},
      {SDLK_d, 0x0F},
  };

  // Add opcode function calls to appropriate table
  // CPU table
  chip8_cputable_ = {
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_1NNN, this),
      std::bind(&CPU::op_2NNN, this),     std::bind(&CPU::op_3XNN, this),
      std::bind(&CPU::op_4XNN, this),     std::bind(&CPU::op_5XY0, this),
      std::bind(&CPU::op_6XNN, this),     std::bind(&CPU::op_7XNN, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_ANNN, this),     std::bind(&CPU::op_BNNN, this),
      std::bind(&CPU::op_CXNN, this),     std::bind(&CPU::op_DXYN, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this),
  };

  // CPU Math opcodes
  chip8_mathtable_ = {
      std::bind(&CPU::op_8XY0, this),     std::bind(&CPU::op_8XY1, this),
      std::bind(&CPU::op_8XY2, this),     std::bind(&CPU::op_8XY3, this),
      std::bind(&CPU::op_8XY4, this),     std::bind(&CPU::op_8XY5, this),
      std::bind(&CPU::op_8XY6, this),     std::bind(&CPU::op_8XY7, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_8XYE, this),     std::bind(&CPU::op_cpu_null, this),
  };

  // CPU call table opcodes
  chip8_calltable_ = {
      std::bind(&CPU::op_00E0, this),     std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_00EE, this),     std::bind(&CPU::op_cpu_null, this),
  };

  // CPU skip table
  chip8_skiptable_ = {
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_EXA1, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_9XY0, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_cpu_null, this), std::bind(&CPU::op_cpu_null, this),
      std::bind(&CPU::op_EX9E, this),     std::bind(&CPU::op_cpu_null, this),
  };
}

void CPU::load_game(const std::string& game_path) {
  std::ifstream file(game_path, std::ios::binary);

  if (file) {
    file.seekg(0, std::ios::end);
    // uint64_t length = file.tellg();
    char buffer[4096 - 512];
    file.seekg(0, std::ios::beg);
    file.read(&buffer[0], (4096 - 512));
    file.close();

    for (uint64_t counter = 0; counter < (4096 - 512); ++counter)
      memory[counter + 512] = buffer[counter];
  }
}

void CPU::fetch() {
  opcode = memory[program_counter] << 8 | memory[program_counter + 1];
  std::cout << static_cast<int>(program_counter) << ": 0x" << std::uppercase
            << std::hex << opcode << std::endl;
}

void CPU::execute() {
  uint8_t MSB_id = (opcode & 0xF000) >> 12;

  if (MSB_id == 0x0) {
    chip8_calltable_[opcode & 0x000F]();
  } else if (((MSB_id >= 1) && (MSB_id <= 7)) ||
             ((MSB_id >= 0xA) && (MSB_id <= 0xD))) {
    chip8_cputable_[MSB_id]();
  } else if (MSB_id == 0x8) {
    chip8_mathtable_[opcode & 0x000F]();
  } else if (MSB_id == 0x0) {
    chip8_calltable_[opcode & 0x000F]();
  } else if ((MSB_id == 0xE) || (MSB_id == 0x9)) {
    if (MSB_id == 0xE)
      chip8_skiptable_[opcode & 0x000F]();
    else
      chip8_skiptable_[9]();  // 9XY0
  } else if (MSB_id == 0xF) {
    if ((opcode & 0x00FF) == 0x07)
      op_FX07();
    else if ((opcode & 0x00FF) == 0x0A)
      op_FX0A();
    else if ((opcode & 0x00FF) == 0x15)
      op_FX15();
    else if ((opcode & 0x00FF) == 0x18)
      op_FX18();
    else if ((opcode & 0x00FF) == 0x1E)
      op_FX1E();
    else if ((opcode & 0x00FF) == 0x29)
      op_FX29();
    else if ((opcode & 0x00FF) == 0x33)
      op_FX33();
    else if ((opcode & 0x00FF) == 0x55)
      op_FX55();
    else if ((opcode & 0x00FF) == 0x65)
      op_FX65();
  } else {
    std::cout << "\tUnrecognized opcode.";
  }
}

void CPU::emulate_cycle() {
  fetch();
  execute();

  if (delay_timer > 0) --delay_timer;

  if (sound_timer > 0) {
    if (sound_timer == 1) std::cout << "BEEP!";
  }

  // Reset input flags
  is_key_pressed = false;
  key_pressed = 0xFF;
}

// INPUT, 0x0 -> 0xF
// Handle the actual keys pressed using SFML
// KEYPAD(array mapped locations):
// [1][2][3][C]
// [4][5][6][D]
// [7][8][9][E]
// [A][0][B][F]
//
// ACTUAL KEYS USED:
// [1][2][3][4]
// [Q][W][E][R]
// [A][S][D][F]
// [Z][X][C][V]

// bool CPU::is_key_event()
// {
//   is_key_pressed = (event_.type == SDL_KEYDOWN);
//   std::cout << "is_key_pressed: " << is_key_pressed << std::endl;
//   return event_.type == SDL_KEYDOWN;
// }

uint8_t CPU::get_key_pressed(const SDL_Event& event) {
  // Check if key pressed is in stored in key mapping
  return keys_map_.contains(event.key.keysym.sym)
             ? keys_map_.at(event.key.keysym.sym)
             : 0xFF;
}

void CPU::update_key_states(const SDL_Event& event) {
  // Set all key states to 0 in order to get proper states
  // Using this design, only one key can be pressed and updated every cycle
  for (uint8_t i = 0; i < 16; ++i) key[i] = 0;

  uint8_t k = get_key_pressed(event);
  if (k != 0xFF) {
    is_key_pressed = true;
    key_pressed = k;
    key[k] = 1;
  }
}

// Opcodes
void CPU::op_cpu_null() { std::cout << "\tCPU_NULL"; }

void CPU::op_00E0() {
  // 0x00E0: Clears the screen
  std::fill(screen.begin(), screen.end(), 0);
  program_counter += 2;
}

void CPU::op_00EE() {
  // 0x00EE: Returns from subroutine
  --stack_ptr;
  program_counter = stack[stack_ptr];
  program_counter += 2;
}

void CPU::op_1NNN() {
  // 0x1NNN: Jumps to address NNN
  program_counter = (opcode & 0x0FFF);
}

void CPU::op_2NNN() {
  // 0x2NNN: Calls subroutine at NNN

  stack[stack_ptr] = program_counter;
  ++stack_ptr;
  program_counter = opcode & 0x0FFF;
}

void CPU::op_3XNN() {
  uint8_t VX = (opcode & 0x0F00) >> 8;

  // 0x3XKK: Skip next instruction if VX = NN
  if (V[VX] == (opcode & 0x00FF))
    program_counter += 4;
  else
    program_counter += 2;
}

void CPU::op_4XNN() {
  // 0x4XNN: SKip next instruction if VX != NN
  uint8_t X = (opcode & 0x0F00) >> 8;
  if (V[X] != (opcode & 0x00FF))
    program_counter += 4;
  else
    program_counter += 2;
}

void CPU::op_5XY0() {
  // 0x5XY0: Skip next instruction if VX = VY
  uint8_t VX = (opcode & 0x0F00) >> 8;
  uint8_t VY = (opcode & 0x00F0) >> 4;

  if (V[VX] == V[VY])
    program_counter += 4;
  else
    program_counter += 2;
}

void CPU::op_6XNN() {
  // 0x6XNN: Set VX = NN
  uint8_t X = (opcode & 0x0F00) >> 8;

  V[X] = (opcode & 0x00FF);
  program_counter += 2;
}

void CPU::op_7XNN() {
  // 0x7XNN: Set VX = VX + NN
  uint8_t X = (opcode & 0x0F00) >> 8;

  V[X] += (opcode & 0x00FF);
  program_counter += 2;
}

void CPU::op_8XY0() {
  // 8XY0: Set VX = VY
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;
  V[X] = V[Y];
  program_counter += 2;
}

void CPU::op_8XY1() {
  // 0x8XY1: Set VX = VX OR VY
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;
  V[X] |= V[Y];
  program_counter += 2;
}

void CPU::op_8XY2() {
  // 0x8XY2: Set VX = VX AND VY
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;
  V[X] &= V[Y];
  program_counter += 2;
}

void CPU::op_8XY3() {
  // 0x8XY3: Set VX = VX XOR VY
  uint8_t X = (opcode & 0x0F00) >> 8;
  uint8_t Y = (opcode & 0x00F0) >> 4;

  V[X] ^= V[Y];

  program_counter += 2;
}

void CPU::op_8XY4() {
  // 0x8XY4: Add the value of register VY to VX
  // Register VF (register 16) set to 1 if carry, otherwise zero.

  uint8_t VY = (opcode & 0x00F0) >> 4;
  uint8_t VX = (opcode & 0x0F00) >> 8;
  // set value_Y to the value located in the specified register V[ VY ]
  uint8_t value_Y = V[VY];
  uint8_t value_X = 0xFF - V[VX];

  // Add
  V[VX] += value_Y;

  // carry flag
  if (value_Y > value_X)
    V[0xF] = 1;
  else
    V[0xF] = 0;

  program_counter += 2;
}

void CPU::op_8XY5() {
  // 0x8XY5: Set VX = VX - VY, set VF to carry
  uint8_t VX = (opcode & 0x0F00) >> 8;
  uint8_t VY = (opcode & 0x00F0) >> 4;

  uint8_t value_Y = V[VY];
  uint8_t value_X = V[VX];

  V[VX] = value_X - value_Y;

  if (value_Y > value_X)
    V[0xF] = 1;
  else
    V[0xF] = 0;

  program_counter += 2;
}

void CPU::op_8XY6() {
  // Stores the LSB of VX in VF and shifts VX to the right by 1
  uint8_t VX = (opcode & 0x0F00) >> 8;

  uint8_t value_X = V[VX];

  V[0xF] = value_X & 0x01;
  V[VX] = V[VX] >> 1;

  program_counter += 2;
}

void CPU::op_8XY7() {
  // Sets VX to VY minus VX. VF set to 0 when theres borrow, 1 otherwise
  uint8_t VX = (opcode & 0x0F00) >> 8;
  uint8_t VY = (opcode & 0x00F0) >> 4;

  uint8_t value_Y = V[VY];
  uint8_t value_X = V[VX];

  V[VX] = value_Y - value_X;

  if (value_X > value_Y)
    V[0xF] = 1;
  else
    V[0xF] = 0;

  program_counter += 2;
}

void CPU::op_8XYE() {
  // Stores the MSB of VX in VF and then shifts VX to the left by 1
  uint8_t VX = (opcode & 0x0F00) >> 8;

  uint8_t value_X = V[VX];

  V[0xF] = value_X & 0x80;
  V[VX] = V[VX] << 1;

  program_counter += 2;
}

void CPU::op_9XY0() {
  // Skips the next instr if VX doesnt equal VY
  uint8_t VX = (opcode & 0x0F00) >> 8;
  uint8_t VY = (opcode & 0x00F0) >> 4;

  uint8_t value_X = V[VX];
  uint8_t value_Y = V[VY];

  if (value_X == value_Y)
    program_counter += 4;
  else
    program_counter += 2;
}

void CPU::op_ANNN() {
  // 0xANNN: Sets index register to the address NNN
  index_register = opcode & 0x0FFF;
  program_counter += 2;
}

void CPU::op_BNNN() {
  // Jumps to address NNN
  program_counter = opcode & 0x0FFF;
}

void CPU::op_CXNN() {
  // Sets VX to the result of a bitwise and operation on a random number (0 to
  // 255)
  std::srand(std::time(nullptr));
  uint8_t VX = opcode & 0x0F00;
  uint8_t value_X = V[VX];

  value_X &= std::rand() % 256;

  V[VX] = value_X;

  program_counter += 2;
}

void CPU::op_DXYN() {
  // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a
  // height of N pixels. Each row of 8 pixels is read as bit-coded starting from
  // memory location I;
  //   I value doesn’t change after the execution of this instruction.
  // As described above, VF is set to 1 if any screen pixels are flipped from
  // set to unset when the sprite is drawn, and to 0 if that doesn’t happen
  //
  uint8_t VX = (opcode & 0x0F00) >> 8;
  uint8_t VY = (opcode & 0x00F0) >> 4;
  uint16_t value_X = V[VX];
  uint16_t value_Y = V[VY];
  uint16_t height = opcode & 0x000F;
  uint16_t pixel;
  uint16_t pixel_position;
  uint8_t scan;

  V[0xF] = 0;

  // IF RECEIVEING GRAPHICAL ERROR CHANGE ++yline to yline++
  for (uint8_t yline = 0; yline < height; yline++) {
    pixel = memory[index_register + yline];

    for (uint8_t xline = 0; xline < 8; xline++) {
      scan = (0x80 >> xline);
      if ((pixel & scan) != 0) {
        pixel_position = value_X + xline + ((value_Y + yline) * 64);

        if (screen[pixel_position] == 1) {
          V[0xF] = 1;
        }

        screen[pixel_position] ^= 1;
      }
    }
  }

  draw_flag = 1;
  program_counter += 2;
}

void CPU::op_EX9E() {
  // 0xEX9E: Skips the next instruction if the key is stored in VX is pressed
  uint8_t VX = (opcode & 0x0F00) >> 8;

  if (key[V[VX]] != 0)
    program_counter += 4;
  else
    program_counter += 2;
}

void CPU::op_EXA1() {
  // Skips the next instruction if the key is stored in VX isnt pressed
  uint8_t VX = (opcode & 0x0F00) >> 8;

  if (key[V[VX]] == 0)
    program_counter += 4;
  else
    program_counter += 2;
}

void CPU::op_FX07() {
  // Sets VX to the value of the delay timer
  uint8_t VX = (opcode & 0x0F00) >> 8;

  V[VX] = delay_timer;

  program_counter += 2;
}

void CPU::op_FX0A() {
  // A key press is awaited, and then stored in VX
  uint8_t VX = (opcode & 0x0F00) >> 8;

  // Wait for key press, return to event polling loop
  if (!is_key_pressed) {
    std::cout << "WAITING FOR INPUT" << std::endl;
    return;
  }

  // Store key press if valid
  if (key_pressed != 0xFF) {
    key[key_pressed] = 1;
    V[VX] = key_pressed;
    program_counter += 2;
  }
}

void CPU::op_FX15() {
  // Sets the delay timer to VX
  uint8_t VX = (opcode & 0x0F00) >> 8;

  delay_timer = V[VX];

  program_counter += 2;
}

void CPU::op_FX18() {
  // Sets the sound timer to VX
  uint8_t VX = (opcode & 0x0F00) >> 8;

  sound_timer = V[VX];

  program_counter += 2;
}

void CPU::op_FX1E() {
  // Adds VX to I
  uint8_t VX = (opcode & 0x0F00) >> 8;

  index_register += V[VX];

  program_counter += 2;
}

void CPU::op_FX29() {
  // Sets I to the location of the sprite for the character in VX
  uint8_t VX = (opcode & 0x0F00) >> 8;
  index_register = memory[V[VX] * 5];

  program_counter += 2;
}

void CPU::op_FX33() {
  // Stores the binary-coded decimal representation of VX
  uint8_t VX = (opcode & 0x0F00) >> 8;

  memory[index_register] = V[VX] / 100;
  memory[index_register + 1] = ((V[VX] % 100) - (V[VX] % 10)) / 10;
  memory[index_register + 2] = (V[VX] % 10);

  program_counter += 2;
}

void CPU::op_FX55() {
  // Stores V0 to VX in memory starting at address I.
  uint8_t VX = (opcode & 0x0F00) >> 8;
  uint8_t offset = 0;

  for (uint8_t reg = 0x0; reg <= VX; ++reg) {
    memory[index_register + offset] = V[reg];
    ++offset;
  }

  program_counter += 2;
}

void CPU::op_FX65() {
  // Fills V0 to VX from values from memory starting at address I
  uint8_t VX = (opcode & 0x0F00) >> 8;

  for (uint8_t reg = 0x0; reg <= VX; ++reg)
    V[reg] = memory[index_register + reg];

  program_counter += 2;
}
