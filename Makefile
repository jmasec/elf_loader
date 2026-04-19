BIN_DIR = bin
INC_DIR = include

# Define the executable name
TARGET = main
EXECUTABLE = $(BIN_DIR)/$(TARGET)

# List all source files
C_FILES = src/main.c src/elf_parser.c src/elf_loader.c

# Automatically generate object file names from source files
O_FILES = $(addprefix $(BIN_DIR)/, $(notdir $(C_FILES:.c=.o)))

# Define the include directory flag
CC = gcc
CFLAGS = -I$(INC_DIR) -g

all: $(BIN_DIR) $(EXECUTABLE)

# Rule to link object files into the executable
$(EXECUTABLE): $(O_FILES)
	$(CC) $^ -o $@

# Rule to build object files from source files (using the source path as prerequisite)
# The output .o file path is in the BIN_DIR, but the input .c is in the src/ dir.
$(BIN_DIR)/%.o: src/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)


# Optional: Clean up generated files
clean:
	rm -rf $(BIN_DIR)