#include "game_logic.h"
#include "shmemory.h"
#include <stdio.h>
#include <unistd.h>     // Para usleep
#include <time.h>       // Para time
#include <stdlib.h>     // Para srand, rand, exit

int mi_indice = -1;

int main(int argc, char *argv[]) {
    (void)argc;  // Marcar parámetros como usados
    (void)argv;
    
    srand(time(NULL) ^ getpid());
    inicializar_shm();
    sleep(100000);
    encontrar_indice_jugador();

    // Verificar si estado es válido
    if (estado == NULL) {
        fprintf(stderr, "Error: Memoria compartida no inicializada\n");
        exit(EXIT_FAILURE);
    }

    while (!estado->juego_terminado) {
        sincronizar_lectura();
        if (mi_indice != -1 && !estado->jugadores[mi_indice].bloqueado) {
            generar_movimiento_adaptativo();
        }
        liberar_lectura();
        sleep(10000 + (rand() % 20000));
    }
    
    return 0;
}