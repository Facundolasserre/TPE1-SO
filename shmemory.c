#include "shmemory.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include "logica.h"


void sincronizar_lectura() {
    sem_wait(&sincro->mutex_estado);
    sem_wait(&sincro->mutex_lectura);
    sincro->cant_lectores++;
    if (sincro->cant_lectores == 1) sem_wait(&sincro->mutex_juego);
    sem_post(&sincro->mutex_lectura);
    sem_post(&sincro->mutex_estado);
}

void liberar_lectura() {
    sem_wait(&sincro->mutex_lectura);
    sincro->cant_lectores--;
    if (sincro->cant_lectores == 0) sem_post(&sincro->mutex_juego);
    sem_post(&sincro->mutex_lectura);
}

void * crear_shms(const char * nombre, size_t size){
    int fd = shm_open(nombre, O_CREAT | O_RDWR, 0666);
    if(fd == -1){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, size) == -1) {
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}
    void * puntero = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (puntero == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
    return puntero;
}

void inicializar_shms(EstadoJuego **estado, Sincronizacion ** sincro, int ancho, int largo, int cant_jugadores, size_t semilla){
    inicializar_sincro(*sincro);

    (*estado)->ancho = ancho;
    (*estado)->alto = largo;
    (*estado)->num_jugadores = cant_jugadores;
    (*estado)->juego_terminado = false;
    (*estado)->jugadores[0].pid = getpid();
    (*estado)->jugadores[0].bloqueado = false;

    inicializar_tablero(*estado, semilla);
    ubicar_jugadores(*estado);

}

void inicializar_sincro(Sincronizacion *sincro) {
    sincro->cant_lectores = 0;
    sem_init(&sincro->sem_imprimir, 1, 0);
    sem_init(&sincro->sem_impreso, 1, 0);
    sem_init(&sincro->mutex_estado, 1, 1);
    sem_init(&sincro->mutex_juego, 1, 1);
    sem_init(&sincro->mutex_lectura, 1, 1);
}

// pid_t crear_proceso(const char *path, char *ancho, char *alto) {
//     pid_t pid = fork();
//     if (pid == 0) {
//         execl(path, path, ancho, alto, NULL);
//         perror("execl");
//         exit(EXIT_FAILURE);
//     }
//     return pid;
// }