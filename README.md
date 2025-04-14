
# TPE1-SO: ChompChamps

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

1. **Compilar los programas** (vista y jugador):
   
   compilar vista: gcc -g -o vista vista.c -lrt -pthread

    compilar jugador: gcc -g -o jugador jugador.c logica.c shmemory.c -lrt -pthread

    compilar (nuestro master): gcc -Wall -Werror -pthread -lrt master.c shmemory.c logica.c argumentos.c -o master


    correr (master catedra): ./ChompChamps -w 10 -h 10 -v ./vista -p ./jugador 

    
    correr (nuestro master): ./master -w 10 -h 10 -v ./vista -p ./jugador 




