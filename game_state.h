#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"
#define MAX_INTENTOS 8

typedef struct {
    char nombre[16];
    unsigned int puntaje;
    unsigned int mov_invalidos;
    unsigned int mov_validos;
    unsigned short x, y;
    pid_t pid;
    bool bloqueado;
} Jugador;

typedef struct {
    unsigned short ancho;
    unsigned short alto;
    unsigned int num_jugadores;
    Jugador jugadores[9];
    bool juego_terminado;
    int tablero[];
} EstadoJuego;

typedef struct {
    sem_t A, B, C, D, E;
    unsigned int F;
} Sincronizacion;

#endif