#include "game_state.h"
#include "shmemory.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void liberar_recursos(int signum) {
    (void)signum;
    if (estado) {
        munmap(estado, sizeof(EstadoJuego) + (estado->ancho * estado->alto * sizeof(int)));
    }
    if (sincro) {
        munmap(sincro, sizeof(Sincronizacion));
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    (void)argc;  // Marcar parámetros como usados
    (void)argv;
    
    signal(SIGINT, liberar_recursos);
    inicializar_shm();
    
    while (1) {
        sem_wait(&sincro->A);
        // Lógica de impresión ACTUALIZADA (implementación original)
        printf("=== ESTADO ACTUAL ===\n");
        for (int y = 0; y < estado->alto; y++) {
            for (int x = 0; x < estado->ancho; x++) {
                int val = estado->tablero[y * estado->ancho + x];
                if (val > 0) printf("[%2d]", val);
                else if (val <= 0) printf(" P%d ", -val);
            }
            printf("\n");
        }
        fflush(stdout);
        sem_post(&sincro->B);
    }
    return 0;
}