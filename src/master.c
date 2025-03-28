//ESQUELETO
#include "utils.h"
#include <sys/select.h>

int main(int argc, char *argv[]) {
    // Parsear argumentos
    // Inicializar shmem y semáforos
    // Crear pipes para jugadores
    // Generar tablero
    // Forkear procesos jugadores y vista

    // Loop principal
    while (!juego_terminado) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        // Agregar pipes de jugadores a read_fds
        select(/* ... */);

        // Procesar movimientos válidos
        // Actualizar estado del juego
        sem_post(&sincronizacion.sem_vista); // Notificar a la vista
        sem_wait(&sincronizacion.sem_master); // Esperar a que la vista termine
    }

    // Liberar recursos
    return 0;
}