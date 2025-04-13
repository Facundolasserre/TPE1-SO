#ifndef VISTA_H
#define VISTA_H

#include "game_state.h"
void imprimir_tablero(EstadoJuego *estado);

#define ROJO "\033[31m"
#define VERDE "\033[32m"
#define AZUL "\033[34m"
#define AMARILLO "\033[33m"
#define MAGENTA "\033[35m"
#define CIAN "\033[36m"
#define VIOLETA "\033[38;5;54m"  
#define NARANJA "\033[38;5;208m"
#define MARRON "\033[38;5;94m"
#define BLANCO "\033[37m"
#define RESET "\033[0m"

void imprimir_final(EstadoJuego *estado);


#endif 