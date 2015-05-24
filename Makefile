CC=cc
CFLAGS=-g
BIN=bin

all: rshell

rshell: makebin src/rshell.c
	$(CC) $(CFLAGS) src/rshell.c -o $(BIN)/rshell

makebin: 
	mkdir $(BIN)

clean:
	rm -rf $(BIN)
