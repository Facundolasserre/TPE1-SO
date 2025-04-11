CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c11
LDFLAGS = -lrt -lpthread
BIN_DIR = bin

all: $(BIN_DIR) jugador vista

jugador: $(BIN_DIR)/jugador.o $(BIN_DIR)/game_logic.o $(BIN_DIR)/shmemory.o
	$(CC) $(CFLAGS) -o $(BIN_DIR)/jugador $^ $(LDFLAGS)

vista: $(BIN_DIR)/vista.o $(BIN_DIR)/shmemory.o
	$(CC) $(CFLAGS) -o $(BIN_DIR)/vista $^ $(LDFLAGS)

$(BIN_DIR)/%.o: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)