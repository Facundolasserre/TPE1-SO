#include "logica.h"
#include "game_state.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>  
#include <stdlib.h>
#include <semaphore.h>
#include "shmemory.h"

const short int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const short int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};


// //Movimiento Jugador________________________________________________________________


void encontrar_indice_jugador() {
    for (unsigned int i = 0; i < estado->num_jugadores; i++) {
        if (estado->jugadores[i].pid == getpid()) {
            mi_indice = i;
            break;
        }
    }
}

void inicializar_tablero(EstadoJuego *estado, size_t semilla){
    srand(semilla);
    for(int i = 0; i < estado->ancho * estado->alto; i++){
        estado->tablero[i] = (rand() % 9) + 1;
    }
}


void ubicar_jugadores(EstadoJuego *estado){
    for (int i = 0; i < estado->num_jugadores; i++) {
		estado->jugadores[i].x = (estado->ancho / (estado->num_jugadores + 1)) * (i + 1);
		estado->jugadores[i].y = estado->alto / 2;
		estado->jugadores[i].puntaje = 0;
		estado->jugadores[i].mov_invalidos = 0;
		estado->jugadores[i].mov_validos = 0;
		estado->jugadores[i].bloqueado = false;
		snprintf(estado->jugadores[i].nombre, sizeof(estado->jugadores[i].nombre),
				 "Jugador %d", i + 1);
	}
}


// bool dentro_del_tablero(int x, int y) {
//     return (x >= 0 && y >= 0 && x < estado->ancho && y < estado->alto);
// }

// bool celda_disponible(int x, int y) {
//     int val = estado->tablero[y * estado->ancho + x];
//     return val > 0 || val == -mi_indice;
// }

// void barajar_direcciones(unsigned char *direcciones) {
//     for (int i = 7; i > 0; i--) {
//         int j = rand() % (i + 1);
//         unsigned char temp = direcciones[i];
//         direcciones[i] = direcciones[j];
//         direcciones[j] = temp;
//     }
// }

// void generar_movimiento_adaptativo() {
//     if (mi_indice == -1 || estado->juego_terminado || 
//         estado->jugadores[mi_indice].bloqueado) {
//         return;
//     }

//     Jugador *yo = &estado->jugadores[mi_indice];
//     unsigned char direcciones[8] = {0, 1, 2, 3, 4, 5, 6, 7};
//     barajar_direcciones(direcciones);

//     int max_recompensa = 0;
//     unsigned char mejores[8];
//     int num_mejores = 0;
//     unsigned char alternativas[8];
//     int num_alternativas = 0;

//     for (int i = 0; i < 8; i++) {
//         unsigned char dir = direcciones[i];
//         int nx = yo->x + dx[dir];
//         int ny = yo->y + dy[dir];
//         if (dentro_del_tablero(nx, ny)) {
//             int val = estado->tablero[ny * estado->ancho + nx];
//             if (val > 0 && (nx != yo->x || ny != yo->y)) { // Celda con recompensa
//                 if (val > max_recompensa) {
//                     max_recompensa = val;
//                     mejores[0] = dir;
//                     num_mejores = 1;
//                 } else if (val == max_recompensa) {
//                     mejores[num_mejores++] = dir;
//                 }
//             } else if ((val == 0 || val == -mi_indice) && (nx != yo->x || ny != yo->y)) {
//                 // Celda vacía o propia como alternativa
//                 alternativas[num_alternativas++] = dir;
//             }
//         }
//     }

//     unsigned char movimiento;
//     if (num_mejores > 0) {
//         int idx = rand() % num_mejores;
//         movimiento = mejores[idx];
//     } else if (num_alternativas > 0) {
//         int idx = rand() % num_alternativas;
//         movimiento = alternativas[idx];
//     } else {
//         return; // Sin movimientos posibles
//     }

//     // Enviar movimiento directamente, dejar que el máster valide
//     write(STDOUT_FILENO, &movimiento, 1);
// }
// //___________________________________________________________________________


