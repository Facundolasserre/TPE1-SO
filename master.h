#ifndef MASTER_H
#define MASTER_H

#include "game_state.h"
#include "argumentos.h"

#define SHM_STATE_GAME "/game_state"
#define SHM_SYNC_GAME "/game_sync"


#define ANCHO_DEFAULT 10
#define LARGO_DEFAULT 10
#define RETARDO_DEFAULT 200
#define TIMEOUT_DEFAULT 10

void limpia_recursos(int pipes_jugadores[MAX_JUGADORES][2], int cant_jugadores, EstadoJuego * estado, Sincronizacion * sincro, pid_t pid_vista, pid_t pid_jugadores[]);
pid_t crear_proceso(const char *path, char *ancho, char *alto);


#endif