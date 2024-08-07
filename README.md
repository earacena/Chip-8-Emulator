# CHIP-8 Emulator

An emulation of the CHIP-8 interpreted language with a debugger/disassembler written in C++.

## Features

* Emulator
  * Loads CHIP-8 ROMs
  * Emulates ROMs
  * Handles input

* Debugger/Disassembler
  * Shows registers and values during emulation
  * Shows stack values
  * Show memory addresses, stored values, and disassembled opcodes
  * Start, stop, and step buttons for emulation control
  * Adding/Removing breakpoints

## Controls (case-sensitive)

```text
    [1][2][3][4]
    [Q][W][E][R]
    [A][S][D][F]
    [Z][X][C][V]
```

## Supported Platforms

 Linux 32/64-bit distributions
  
## Dependencies

* g++
* cmake
* SDL2 (2.42.2)
* fmtlib
* Catch2 for tests (3.1.0 and up)

## Build

To build:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Run

To run with selected ROM, enter the following in the build folder:

```bash
./chip8 [ROM filepath]
```

### Clean up

To remove all .o and compiled executables, enter the following in ```build/```:

```bash
make clean
```

## Known Issues

* Out-of-bounds segmentation fault found while testing PONG ROM, detected whenever player paddle passes screen bounds.

* Some games are unresponsive

## References

 <https://web.archive.org/web/20130401182240/http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.0>
