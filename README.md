# CHIP-8 Emulator

A simple emulation of the CHIP-8 interpreted language written in C++. Used Cowgod's technical reference [1] for opcode implementations.

## Controls (case-sensitive)

```text
    [1][2][3][4]
    [Q][W][E][R]
    [A][S][D][F]
    [Z][X][C][V]
```

## Suppoted Platforms

 Linux 32/64-bit distributions
  
## Dependencies

* g++
* cmake
* SDL2 (2.42.2)
* fmtlib

## Build

To build:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Run

To run with selected ROM, type:

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

 [1] <https://web.archive.org/web/20130401182240/http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.0>
