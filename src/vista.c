#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"

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
    sem_t A; 
    sem_t B;
    sem_t C;
    sem_t D;
    sem_t E;
    unsigned int F;
} Sincronizacion;

EstadoJuego *estado;

int main(int argc, char *argv[]){
    int fd= shm_open("/game_state", O_RDONLY, 0666);

    estado= mmap(NULL, sizeof(EstadoJuego), PROT_READ, MAP_SHARED, fd, 0);
    while(1){
        printf("\n--- TABLERO ---\n");
        for(int y = 0; y < estado->alto; y++) {
            for(int x = 0; x < estado->ancho; x++) {
                int valor = estado->tablero[y * estado->ancho + x];

                if(valor > 0)       printf("[%2d]", valor);  // Casilla con puntos
                else if(valor < 0)  printf(" P%d ", -valor); // Jugador
                else                printf(" ██ ");         // Casilla ocupada
            }
            printf("\n");
        }
    }
    return 0;
}