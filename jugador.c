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


    pid_t my_pid = getpid();
    for (int i = 0; i < estado->num_jugadores; i++) {
        if (estado->jugadores[i].pid == my_pid) {
            mi_indice = i;
            break;
        }
    }
    if (mi_indice == -1) {
        fprintf(stderr, "No se encontró el índice del jugador con PID %d\n", my_pid);
        exit(EXIT_FAILURE);
    }

    int ultima_suma_valida= -1;
    int actual_suma_valida;

    //este es el principal cambio que hicimos, ahora chequea que el juego no haya finaliado adentro del while, luego de haber sincronizado la lectura
    while (1) {
        //Cambio: bucle de espera activa para verificar si se procesó el mov anterior
        do{
            sincronizar_lectura(); //sem para lectura segura
            //cambio: se verifican condiciones de salida
            if (estado->juego_terminado || estado->jugadores[mi_indice].bloqueado) {
                liberar_lectura(); 
                goto cleanup;      
            }
            actual_suma_valida= estado->jugadores[mi_indice].mov_validos + estado->jugadores[mi_indice].mov_invalidos;
            liberar_lectura();
        } while(actual_suma_valida == ultima_suma_valida);
        
        ultima_suma_valida = actual_suma_valida;
        unsigned char mov= mov_random();
        write(STDOUT_FILENO, &mov, sizeof(mov));
        
    }

    cleanup:
    munmap(estado, state_size);
    munmap(sincro, sizeof(Sincronizacion));
    close(fd_state);
    close(fd_sync);
    return 0;
}