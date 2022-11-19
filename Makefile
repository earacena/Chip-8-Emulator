###########################################################
# 	Makefile for Chip-8 Emulator                          #
# 	Name: 		Emanuel Aracena Beriguete                   #
#	Date created:	May 25, 2019                              #
#	Description:	Makefile for Linux executable.            #
###########################################################

# Flags
CPP_FLAG = -g -std=c++20 -Wall -Wextra -pedantic
EXEC_DIR = ./build

# Rule for .cpp files
# .SUFFIXES: .cpp.o

.cpp.o:
	g++ $(CPP_FLAG) $(INCLUDE) -c $< -o $@

# Include libraries
INCLUDE = -I/usr/include/SDL2

# SFML libraries
# SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# SDL libraries
SDL_LIBS = -lSDL2

# Libraries
LIBS = -L/usr/lib -L/usr/local/lib $(SDL_LIBS)

# Optimal multiplications
OBJ_0 = src/main.o src/Chip8.o src/CPU.o src/Display.o
PROGRAM_0 = Chip8
$(PROGRAM_0): $(OBJ_0)
	g++ $(CPP_FLAG) -o $(EXEC_DIR)/$@ $(OBJ_0) $(INCLUDE) $(LIBS)


# Compile all
all:
	(make $(PROGRAM_0); mv src/*.o build;)

run:
	./$(PROGRAM_0)

clean:
	(rm -f src/*.o; rm -f $(PROGRAM_0);)
