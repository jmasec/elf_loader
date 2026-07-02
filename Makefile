BIN_DIR = bin
INC_DIR = include

TARGET = main
EXECUTABLE = $(BIN_DIR)/$(TARGET)

CC = gcc
CFLAGS = -I$(INC_DIR) -Ielf_parser/include -g

C_FILES = \
	src/main.c \
	src/elf_loader.c \
	elf_parser/src/elf_parser.c \

O_FILES = $(patsubst %.c,$(BIN_DIR)/%.o,$(C_FILES))

all: $(BIN_DIR) $(EXECUTABLE)

$(EXECUTABLE): $(O_FILES)
	$(CC) $^ -o $@

$(BIN_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)