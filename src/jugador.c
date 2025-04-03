#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"
#define MAX_INTENTOS 8

typedef struct {
    char nombre[16];
    unsigned int puntaje;
    unsigned int mov_invalidos;
    unsigned int mov_validos;
    unsigned short x, y;
    pid_t pid;
    bool bloqueado;
} Jugador;

typedef struct {
    unsigned short ancho;
    unsigned short alto;
    unsigned int num_jugadores;
    Jugador jugadores[9];
    bool juego_terminado;
    int tablero[];
} EstadoJuego;

typedef struct {
    sem_t A, B, C, D, E;
    unsigned int F;
} Sincronizacion;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


const int movimientos[4] = {0, 2, 4, 6}; 

int main(int argc, char *argv[]) {
    srand(time(NULL)); 
    
    
    int ancho = atoi(argv[1]);
    int alto = atoi(argv[2]);

    while (1) {
        int direccion = movimientos[rand() % 4]; 
        write(STDOUT_FILENO, &direccion, 1);
        usleep(100000); 
    }
    
    return 0;
}