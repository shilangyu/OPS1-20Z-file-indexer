CC = gcc
CFLAGS = -lpthread -lm

mole: $(shell find src/ -type f -name '*.c')
	$(CC) -o $@ $? $(CFLAGS) 
