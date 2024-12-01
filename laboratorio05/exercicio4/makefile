# Define the compiler
CC = g++

# Define the compiler flags
CFLAGS = -Wall -Wextra -Werror -std=c++11

# Define the target executable
TARGET = ex4

# Define the source files
SRCS = $(wildcard *.cpp)

# Define the object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile the source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)