
# TPE1-SO: ChompChamps

# Integrantes:
   Franco Ferrari 63094
   Octavio Zacagnino 64255
   Facundo Lasserre 62165

## Descripción
Este proyecto implementa el juego "ChompChamps", un juego concurrente en el que múltiples jugadores (bots) compiten por recolectar recompensas en un tablero compartido. El sistema utiliza memoria compartida y semáforos para coordinar las acciones entre un proceso master, los jugadores, y una vista que muestra el estado del juego.

### Objetivo
- **Master**: Controla el estado del juego, actualiza el tablero y coordina a los jugadores y la vista.
- **Jugadores**: Bots que se mueven en el tablero buscando maximizar su puntaje al ocupar celdas con recompensas.
- **Vista**: Muestra el estado actual del tablero y los jugadores activos.


## Compilación y Ejecución


### Requisitos
- Tener el binario del máster (`ChompChamps`) en el mismo directorio.
- Compilador `gcc` y librerías estándar de C.

### Pasos:

Asegúrese de que los archivos fuente (master.c, vista.c, jugador.c, shmemory.c, logica.c, argumentos.c) y sus encabezados estén en el directorio de trabajo.

**Ejecute el comando:** `make`

Esto compila los tres binarios (master, vista, jugador) y los coloca en bin/.

**Con nuestro máster:** `make run`

Ejecuta: ./bin/master -w 10 -h 10 -v ./bin/vista -p ./bin/jugador ./bin/jugador ./bin/jugador

**Con el máster de la cátedra:*** `make run_chomp`

Ejecuta: ./ChompChamps -w 10 -h 10 -v ./bin/vista -p ./bin/jugador ./bin/jugador ./bin/jugador

**Para limpiar los archivos generados:** `make clean`





