#ifndef UTILS_H
#define UTILS_H

#include <semaphore.h>

// Definición de estructuras (EstadoJuego, Jugador, Sincronizacion)
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
    int *tablero;
} EstadoJuego;

typedef struct {
    sem_t sem_vista;
    sem_t sem_master;
    sem_t mutex_estado;
    sem_t mutex_juego;
    sem_t mutex_lectores;
    unsigned int lectores_activos;
} Sincronizacion;

// Constantes
#define SHM_GAME_STATE "/game_state"
#define SHM_GAME_SYNC "/game_sync"

// Funciones comunes (declaraciones)
void init_shared_memory();
void init_semaphores();

#endif