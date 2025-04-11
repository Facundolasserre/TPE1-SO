#include "game_logic.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>  // Para dprintf (si se usa)

// Implementaciones completas
void encontrar_indice_jugador() {
    for (unsigned int i = 0; i < estado->num_jugadores; i++) {
        if (estado->jugadores[i].pid == getpid()) {
            mi_indice = i;
            break;
        }
    }
}

bool dentro_del_tablero(int x, int y) {
    return (x >= 0 && y >= 0 && x < estado->ancho && y < estado->alto);
}

bool celda_disponible(int x, int y) {
    int val = estado->tablero[y * estado->ancho + x];
    return val > 0 || val == -mi_indice;
}

void barajar_direcciones(unsigned char *direcciones) {
    for (int i = 7; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned char temp = direcciones[i];
        direcciones[i] = direcciones[j];
        direcciones[j] = temp;
    }
}

void generar_movimiento_adaptativo() {
    // Implementación completa aquí
}