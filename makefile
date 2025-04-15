# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Werror -pthread -lrt -g

# Directorios
BIN_DIR = bin
SRC_DIR = .

# Ejecutables
VISTA = $(BIN_DIR)/vista
JUGADOR = $(BIN_DIR)/jugador
MASTER = $(BIN_DIR)/master

# Archivos fuente
VISTA_SRC = $(SRC_DIR)/vista.c
JUGADOR_SRC = $(SRC_DIR)/jugador.c
MASTER_SRC = $(SRC_DIR)/master.c
SHMEMORY_SRC = $(SRC_DIR)/shmemory.c
LOGICA_SRC = $(SRC_DIR)/logica.c
ARGUMENTOS_SRC = $(SRC_DIR)/argumentos.c

# Archivos objeto
VISTA_OBJ = $(BIN_DIR)/vista.o
JUGADOR_OBJ = $(BIN_DIR)/jugador.o
MASTER_OBJ = $(BIN_DIR)/master.o
SHMEMORY_OBJ = $(BIN_DIR)/shmemory.o
LOGICA_OBJ = $(BIN_DIR)/logica.o
ARGUMENTOS_OBJ = $(BIN_DIR)/argumentos.o

# Regla por defecto: compilar todo
all: $(MASTER) $(VISTA) $(JUGADOR)

# Compilar master
$(MASTER): $(MASTER_OBJ) $(SHMEMORY_OBJ) $(LOGICA_OBJ) $(ARGUMENTOS_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(MASTER) $(MASTER_OBJ) $(SHMEMORY_OBJ) $(LOGICA_OBJ) $(ARGUMENTOS_OBJ)

# Compilar vista
$(VISTA): $(VISTA_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(VISTA) $(VISTA_OBJ)

# Compilar jugador
$(JUGADOR): $(JUGADOR_OBJ) $(LOGICA_OBJ) $(SHMEMORY_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(JUGADOR) $(JUGADOR_OBJ) $(LOGICA_OBJ) $(SHMEMORY_OBJ)

# Regla genérica para compilar archivos .c en .o
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Crear el directorio bin si no existe
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Limpiar archivos generados
clean:
	rm -rf $(BIN_DIR)

# Ejecutar con nuestro master
run: all
	chmod +x $(MASTER)
	./$(MASTER) -w 10 -h 10 -v ./$(VISTA) -p ./$(JUGADOR)

# Ejecutar con master de la cátedra (ChompChamps)
run_chomp:
	chmod +x ChompChamps
	./ChompChamps -w 10 -h 10 -v ./$(VISTA) -p ./$(JUGADOR)