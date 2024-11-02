# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRC = memory_manager.c bitmap.c
TEST_SRC = test_main.c

# Object files
OBJ = $(SRC:.c=.o)

# Static library
LIB = libmemory_manager.a

# Executable name
EXEC = test_memory_manager

# Default rule
all: $(LIB) $(EXEC)

# Create static library from object files
$(LIB): $(OBJ)
	ar rcs $@ $^

# Link the static library with the test program
$(EXEC): $(TEST_SRC:.c=.o) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(LIB) $(EXEC) *.o

# Run the tests
run: all
	./$(EXEC)

.PHONY: all clean run
