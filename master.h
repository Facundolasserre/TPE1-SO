#ifndef MASTER_H
#define MASTER_H

#include "game_state.h"
#include "argumentos.h"

#define SHM_STATE_GAME "/game_state"
#define SHM_SYNC_GAME "/game_sync"

void limpia_recursos(int pipes_jugadores[MAX_JUGADORES][2], int cant_jugadores, EstadoJuego * estado, Sincronizacion * sincro, pid_t pid_vista, pid_t pid_jugadores[]);



#endif