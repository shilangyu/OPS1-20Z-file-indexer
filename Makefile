CC = gcc
CFLAGS = -lpthread -lm -Wall

SOURCE_FILES = $(shell find src/ -type f -name '*.c')

mole: $(SOURCE_FILES)
	$(CC) -o $@ $? $(CFLAGS) 

test: $(SOURCE_FILES)
	$(CC) -o $@ $? $(CFLAGS) -D TEST
