#include <catch2/catch_test_macros.hpp>
#include <algorithm>
#include "../src/CPU.h"

TEST_CASE("Opcodes correctly manipulate the CPU", "[cpu]") {
  CPU cpu;

  SECTION("op_00E0 clears cpu.screen") {    
    // Check that atleast one place in screen is set
    std::fill(cpu.screen.begin(), cpu.screen.end(), 1);
    CHECK(std::find(cpu.screen.begin(), cpu.screen.end(), 1) != cpu.screen.end());

    // Execute
    cpu.opcode = 0x00E0;
    cpu.execute();
    
    // Check that screen is now empty
    CHECK(std::find(cpu.screen.begin(), cpu.screen.end(), 1) == cpu.screen.end());
  }
};