CC = gcc
CFLAGS = -lpthread -lm -Wall -std=c18

mole: $(shell find src/ -type f -name '*.c')
	$(CC) -o $@ $? $(CFLAGS) 
