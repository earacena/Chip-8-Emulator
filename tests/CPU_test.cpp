#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include "../src/CPU.h"

TEST_CASE("Opcodes correctly manipulate the CPU", "[cpu]") {
  CPU cpu;

  SECTION("op_00E0 clears cpu.screen") {    
    // Check that atleast one place in screen is set
    std::fill(cpu.screen.begin(), cpu.screen.end(), 1);
    REQUIRE(std::find(cpu.screen.begin(), cpu.screen.end(), 1) != cpu.screen.end());

    // Execute
    cpu.opcode = 0x00E0;
    cpu.execute();
    
    // Check that screen is now empty
    CHECK(std::find(cpu.screen.begin(), cpu.screen.end(), 1) == cpu.screen.end());
  }

  SECTION("op_00EE returns from subroutine") {
    // Activate subroutine
    cpu.stack_ptr = 0;
    cpu.program_counter = 0x300;
    cpu.stack[cpu.stack_ptr] = cpu.program_counter;
    ++cpu.stack_ptr;
    cpu.program_counter = 0x400;

    REQUIRE(cpu.stack_ptr == 1);
    REQUIRE(cpu.program_counter == 0x400);
    REQUIRE(cpu.stack[0] == 0x300);

    // Execute
    cpu.opcode = 0x00EE;
    cpu.execute();

    // Check that cpu returned from subroutine
    CHECK(cpu.stack_ptr == 0);
    CHECK(cpu.program_counter == 0x302);
  }

  SECTION("op_1NNN jumps to address NNN") {
    // Prepare
    cpu.program_counter = 0x200;
    REQUIRE(cpu.program_counter == 0x200);

    // Execute
    cpu.opcode = 0x1300;
    cpu.execute();

    // Check if the address jumped to is correct
    CHECK(cpu.program_counter == 0x300);
  }

  SECTION("op_2NNN calls subroutine at NNN") {
    // Prepare
    cpu.stack_ptr = 0;
    cpu.program_counter = 0x300;

    REQUIRE(cpu.stack_ptr == 0);
    REQUIRE(cpu.program_counter == 0x300);

    // Execute
    cpu.opcode = 0x2400;
    cpu.execute();

    // Check if subroutine was called
    CHECK(cpu.program_counter == 0x400);
    CHECK(cpu.stack_ptr == 1);
    CHECK(cpu.stack[0] == 0x300);
  }

  SECTION("op_3XNN skips next instruction if VX = NN") {
    // Prepare
    cpu.program_counter = 0x200;
    cpu.V.at(0xA) = 0x44;

    REQUIRE(cpu.program_counter == 0x200);
    REQUIRE(cpu.V.at(0xA) == 0x44);

    // Execute
    cpu.opcode = 0x3A44;
    cpu.execute();

    // Check if program counter in incremented two instructions
    REQUIRE(cpu.program_counter == 0x200 + 4);
  }

  SECTION("op_3XNN doesn't skips next instruction if VX != NN") {
    // Prepare
    cpu.program_counter = 0x200;
    cpu.V.at(0xA) = 0x43;

    REQUIRE(cpu.program_counter == 0x200);
    REQUIRE(cpu.V.at(0xA) == 0x43);

    // Execute
    cpu.opcode = 0x3A44;
    cpu.execute();

    // Check if program counter in incremented two instructions
    REQUIRE(cpu.program_counter == 0x200 + 2);
  }
};