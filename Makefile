###########################################################
# 	Makefile for Chip-8 Emulator			  #
# 	Name: 		Emanuel Aracena			  #
#	Date created:	May 25, 2019			  #
#	Description:	Makefile for Linux executable.	  #
###########################################################

# Flags
CPP_FLAG = -g -std=c++11 -Wall -Wextra -pedantic
EXEC_DIR = .

# Rule for .cpp files
# .SUFFIXES: .cpp.o

.cpp.o:
	g++ $(CPP_FLAG) $(INCLUDE) -c $< -o $@

# Include libraries
INCLUDE = -I.

# SFML libraries
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Libraries
LIBS = -L/usr/lib -L/usr/local/lib $(SFML_LIBS)

# Optimal multiplications
OBJ_0 = main.o Chip8.o CPU.o Display.o
PROGRAM_0 = Chip8
$(PROGRAM_0): $(OBJ_0)
	g++ $(CPP_FLAG) -o $(EXEC_DIR)/$@ $(OBJ_0) $(INCLUDE) $(LIBS)


# Compile all
all:
	make $(PROGRAM_0)

run:
	./$(PROGRAM_0)

clean:
	(rm -f *.o; rm -f $(PROGRAM_0);)
