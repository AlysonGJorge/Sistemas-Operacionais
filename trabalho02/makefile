CC = gcc
CFLAGS = -I./headers
SOURCES = $(shell find . -name "*.c")
OUTPUT = gordao32

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
