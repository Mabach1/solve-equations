CC := gcc
CFLAG := -g -std=c17 

SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)
EXECUTABLE := a.exe

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAG) -c $< -o $@

clean:
	-@del $(OBJ)
