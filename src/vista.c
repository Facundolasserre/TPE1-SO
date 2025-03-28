//esqueleto
#include "utils.h"

int main(int argc, char *argv[]) {
    // Conectar a shmem y semáforos
    while (1) {
        sem_wait(&sincronizacion.sem_vista); // Esperar señal del máster
        // Imprimir estado del juego
        sem_post(&sincronizacion.sem_master); // Notificar al máster
        if (estado.juego_terminado) break;
    }
    return 0;
}