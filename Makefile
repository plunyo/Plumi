# ---- compiler & flags ----
CC = gcc
CFLAGS = -g -Wall -Wextra -Isrc

# ---- sources & objects ----
SRC = $(shell find src -name "*.c")
OBJ_DIR = build
OBJ = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC))

TARGET = plumi  # your bash clone executable

# ---- targets ----
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# pattern rule to compile .c -> .o
$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) compile_commands.json

run: $(TARGET)
	./$(TARGET)

# generate compile_commands.json using bear
compile_commands:
	bear -- make all

.PHONY: all clean run compile_commands
