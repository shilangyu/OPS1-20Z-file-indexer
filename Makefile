CC = gcc
CFLAGS = -lpthread -lm -Wall

SOURCE_FILES = $(shell find src/ -type f -name '*.c')

mole: $(SOURCE_FILES)
	$(CC) -o $@ $? $(CFLAGS) -fanalyzer

test: $(SOURCE_FILES)
	$(CC) -o $@ $? $(CFLAGS) -D TEST
	@echo 'run tests with "./test < assets/test-inputs.txt"'

archive:
	tar -czf wojnarowskim.projectD.tar.gz src/ assets/ .clang-format Makefile
