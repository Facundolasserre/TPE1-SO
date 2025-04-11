#include "shmemory.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <semaphore.h> 

EstadoJuego *estado = NULL;
Sincronizacion *sincro = NULL;

void inicializar_shm() {
    int fd_state = shm_open(SHM_STATE_NAME, O_RDONLY, 0666);
    estado = mmap(NULL, sizeof(EstadoJuego) + (100 * sizeof(int)), 
               PROT_READ, MAP_SHARED, fd_state, 0);
    
    int fd_sync = shm_open(SHM_SYNC_NAME, O_RDWR, 0666);
    sincro = mmap(NULL, sizeof(Sincronizacion), 
               PROT_READ | PROT_WRITE, MAP_SHARED, fd_sync, 0);
}

void sincronizar_lectura() {
    sem_wait(&sincro->C);
    sem_wait(&sincro->E);
    sincro->F++;
    if (sincro->F == 1) sem_wait(&sincro->D);
    sem_post(&sincro->E);
    sem_post(&sincro->C);
}

void liberar_lectura() {
    sem_wait(&sincro->E);
    sincro->F--;
    if (sincro->F == 0) sem_post(&sincro->D);
    sem_post(&sincro->E);
}