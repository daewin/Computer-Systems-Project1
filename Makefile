# 
# Sample Makefile for project 1 (COMP30017)
#
# The program used for illustration purposes is
# a simple program using prime numbers
#


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall  


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		simulation.c getops.c process.c linked_list.c memory.c multi_scheduler.c
OBJ =		simulation.o getops.o process.o linked_list.o memory.o multi_scheduler.o
EXE = 		simulation

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE) 

## Dependencies

##driver.o:	prime.h
##prime.o:	prime.h

simulation.o: memory.h simulation.h
process.o: process.h
linked_list.o: linked_list.h
getops.o: memory.h
memory.o: memory.h linked_list.h process.h
multi_scheduler.o: simulation.h