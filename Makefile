CC := gcc
CFLAG := -g -std=c17 

SRC := $(wildcard *.c) $(wildcard base/*.c)
OBJ := $(SRC:.c=.o)
EXECUTABLE := a.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAG) -c $< -o $@

