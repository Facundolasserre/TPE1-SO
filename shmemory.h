#ifndef SHMEMORY_H
#define SHMEMORY_H

#include "game_state.h"

extern EstadoJuego *estado;
extern Sincronizacion *sincro;



void sincronizar_lectura();
void liberar_lectura();


#endif