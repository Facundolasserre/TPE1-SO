#ifndef SHMEMORY_H
#define SHMEMORY_H

#include "game_state.h"

extern EstadoJuego *estado;
extern Sincronizacion *sincro;



void sincronizar_lectura();
void liberar_lectura();
void * crear_shms(const char * nombre, size_t size);
void inicializar_shms(EstadoJuego **estado, Sincronizacion ** sincro, int ancho, int largo, int cant_jugadores, size_t semilla);
void inicializar_sincro(Sincronizacion * sincro);



#endif