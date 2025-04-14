#ifndef LOGICA_H
#define LOGICA_H

#include "game_state.h"

extern EstadoJuego *estado;
extern int mi_indice;

extern const short int dx[8];
extern const short int dy[8];

void encontrar_indice_jugador();

void ubicar_jugadores(EstadoJuego *estado);

void inicializar_tablero(EstadoJuego *estado, size_t semilla);

void crear_jugadores(EstadoJuego *estado, int pipes_jugadores[MAX_JUGADORES][2], char * path_jugadores[], int num_jugadores, char * str_ancho, char * str_alto, pid_t pids_jugadores[MAX_JUGADORES]);

void chequeo_timeouts(EstadoJuego * estado, time_t * tiempo_ult_mov, int timeout, int * jugadores_bloqueados, int cant_jugadores);

void leer_mov(EstadoJuego * estado, Sincronizacion * sincro, int pipes_jugadores[MAX_JUGADORES][2], fd_set * leerfds, time_t *tiempo_ult_mov, int * jugadores_bloqueados, int cant_jugadores, pid_t pid_vista);

void juego(EstadoJuego * estado, Sincronizacion *sincro, int pipes_jugadores[MAX_JUGADORES][2], time_t *tiempo_ult_mov, int cant_jugadores, int timeout, int retardo, pid_t pid_vista);

bool tiene_mov_valido(EstadoJuego * estado, int index_jugador);





#endif