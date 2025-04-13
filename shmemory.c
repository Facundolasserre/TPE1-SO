#include "shmemory.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>


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