#ifndef ARGUMENTOS_H
#define ARGUMENTOS_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>

#define MAX_JUGADORES 9

void cargar_argumentos(int argc, char * argv[], int * ancho, int * largo, int * retardo, int * timeout, size_t * semilla, char ** path_vista, char * path_jugadores[], int * cant_jugadores);
void print_argumentos(int ancho, int largo, int retardo, int timeout, size_t seed, char * path_vista, char * path_jugadores[], int cant_jugadores);

#endif