CC = gcc
CFLAGS = -Wall -lncursesw 

FILE = $(wildcard *.c)
OBJ = $(patsubst %.c,%, $(wildcard *.c))

all:
	$(CC) $(CFLAGS) -o $(OBJ) $(FILE)

run: $(OBJ)
	./$(OBJ)
