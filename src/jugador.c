#include "utils.h"

//ESQUELETO
int main(int argc, char *argv[]) {
    // Tenemos que conectar a shmem y semaforos
    while (!estado.juego_terminado) {
        sem_wait(&sincronizacion.mutex_lectores);
        // 
        sem_post(&sincronizacion.mutex_lectores);

        // Calcular movimiento válido
        unsigned char direccion = rand() % 8;
        write(STDOUT_FILENO, &direccion, 1); // Enviar al pipe
    }
    return 0;
}