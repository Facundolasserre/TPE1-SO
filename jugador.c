#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>
#include "game_state.h"
#include "logica.h"
#include "shmemory.h"

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"
#define MAX_INTENTOS 8


EstadoJuego *estado;
Sincronizacion *sincro;
int mi_indice = 0;



unsigned char mov_random(){
    return rand() % 8;
}


int main(int argc, char *argv[]) {
    srand(time(NULL) ^ getpid());

    int ancho = atoi(argv[1]);
    int alto = atoi(argv[2]);
    size_t state_size = sizeof(EstadoJuego) + (ancho * alto * sizeof(int));

    //inicializar shared memories
    int fd_state = shm_open(SHM_STATE_NAME, O_RDONLY, 0666);
    estado = mmap(NULL, state_size, 
                PROT_READ, MAP_SHARED, fd_state, 0);
    
    int fd_sync = shm_open(SHM_SYNC_NAME, O_RDWR, 0666);
    sincro = mmap(NULL, sizeof(Sincronizacion), 
                PROT_READ | PROT_WRITE, MAP_SHARED, fd_sync, 0);

    usleep(10000);
    

    while (!estado->juego_terminado) {
        sincronizar_lectura();
        
        if (mi_indice != -1 && !estado->jugadores[mi_indice].bloqueado) {
            unsigned char move = mov_random();
            write(STDOUT_FILENO, &move, sizeof(move));
        }
        
        liberar_lectura();
        usleep(10000); 
    }

    munmap(estado, state_size);
    munmap(sincro, sizeof(Sincronizacion));
    close(fd_state);
    close(fd_sync);
    return 0;
}