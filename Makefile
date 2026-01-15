CC = gcc
LINK = -lpanel -lncurses
CFLAGS = -Wall -g

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN = $(BUILD_DIR)/yahtzee

OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/game.o $(OBJ_DIR)/yahtzee.o $(OBJ_DIR)/ui.o $(OBJ_DIR)/input.o

all: build $(BIN)

# Link finale
$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LINK) -o $(BIN) $(OBJ)

# Compilazione dei file .c in build/obj
$(OBJ_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/yahtzee.o: yahtzee.c yahtzee.h
	$(CC) $(CFLAGS) -c yahtzee.c -o $(OBJ_DIR)/yahtzee.o

$(OBJ_DIR)/ui.o: ui.c ui.h
	$(CC) $(CFLAGS) -c ui.c -o $(OBJ_DIR)/ui.o

$(OBJ_DIR)/game.o: game.c game.h
	$(CC) $(CFLAGS) -c game.c -o $(OBJ_DIR)/game.o

$(OBJ_DIR)/input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c -o $(OBJ_DIR)/input.o

# Creazione directory
build:
	mkdir -p $(OBJ_DIR)

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build clean run

