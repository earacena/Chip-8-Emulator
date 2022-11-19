# CHIP-8 Emulator

A simple emulation of the CHIP-8 interpreted language written in C++. Used Cowgod's technical reference [1] for opcode implementations. Tested with "PONG" game from David Winter's www.pong-story.com/chip8/

### Controls (case-sensitive)

```
    [1][2][3][4]
    [Q][W][E][R]
    [A][S][D][F]
    [Z][X][C][V]
```

## Suppoted Platforms

 Linux 32/64-bit distributions
  
## Dependencies

* g++ (compiler)

* make (for the Makefile)
* SFML 2.5.0 (and up) for window and 2D rendering functions.

## Build

To build, type:

```bash
make clean
make
```

## Run

To run with selected ROM, type:

```bash
./Chip8 [ROM filepath]
```

### Clean up

To remove all .o and compiled executables, type:

```bash
make clean
```

## Known Issues

* Out-of-bounds segmentation fault found while testing PONG ROM, detected whenever player paddle passes screen bounds.

* Some games are unresponsive

## References

 [1] <https://web.archive.org/web/20130401182240/http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.0>
