#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include "game_state.h"
#include "vista.h"
#include "game_logic.h"

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"

const char *colores[]={ROJO, VERDE, AZUL, AMARILLO, MAGENTA, CIAN, VIOLETA, NARANJA, MARRON };

EstadoJuego *estado;
Sincronizacion *sincro;

// void liberar_recursos(int signum) {
//     (void)signum;
//     munmap(estado, sizeof(EstadoJuego) + (estado->ancho * estado->alto * sizeof(int)));
//     munmap(sincro, sizeof(Sincronizacion));
//     exit(0);
// }



void imprimir_tablero(EstadoJuego *estado){
    system("clear");
    printf("\033[36m"
     "     ___  _   _  _____  __  __  ____   ___  _   _    __    __  __  ____  ___ \n"
     "    / __)( )_( )(  _  )(  \\/  )(  _ \\ / __)( )_( )  /__\\  (  \\/  )(  _ \\/ __)\n" 
     "   ( (__  ) _ (  )(_)(  )    (  )___/( (__  ) _ (  /(__)\\  )    (  )___/\\__ \\ \n"
    "    \\___)(_) (_)(_____)(_/\\/\\_)(__)   \\___)(_) (_)(__)(__)(_/\\/\\_)(__)  (___/ \n"
    "\033[0m\n");
    
    printf("Jugadores: %d\n", estado->num_jugadores);
            
    
    // Imprimir tablero línea por línea
    for (int y = 0; y < estado->alto; y++) {
        for (int x = 0; x < estado->ancho; x++) {
            
            int val = estado->tablero[y * estado->ancho + x];
            if (val > 0) {
                // Celda con recompensa en blanco
                printf(BLANCO "[%2d]" RESET, val);
            } else if (val <=0) {
                int idx = -val;  
                
                if (estado->jugadores[idx].bloqueado) {
                    printf("%s[ X]" RESET, colores[idx]);
                } else {
                    printf("%s[P%d]" RESET, colores[idx], idx+1);
                }
                
            }
        }
        printf("\n");
    }
    fflush(stdout);
}

void imprimir_final(EstadoJuego *estado) {
    printf("\n \033[36m===================================GAME OVER===================================\033[0m");
    printf("\n\033[1mPuntajes:\033[0m\n");
    int indices[MAX_JUGADORES];
    for (int i = 0; i < estado->num_jugadores; i++)
        indices[i] = i;
    for (int i = 0; i < estado->num_jugadores - 1; i++) {
        for (int j = 0; j < estado->num_jugadores - i - 1; j++) {
            if (estado->jugadores[indices[j]].puntaje < estado->jugadores[indices[j + 1]].puntaje){
                int swap = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = swap;
            }
        }
    }
    for (int i = 0; i < estado->num_jugadores; i++) {
        int idx = indices[i];
        const char *nombre = estado->jugadores[idx].nombre[0] ? estado->jugadores[idx].nombre : "Unknown";
        printf(" %s[%s](%d)\033[0m: %dpts con %d solicitudes de mov. validas y %d solicitudes de mov. invalidas.\n", colores[idx], nombre, idx+1 ,estado->jugadores[idx].puntaje, estado->jugadores[idx].mov_validos, estado->jugadores[idx].mov_invalidos);
    }
    printf("\n\033[36m=========================================================="
           "===========================\033[0m\n\n");
}




int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <ancho> <alto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    
    
    // Obtener dimensiones reales del tablero
    unsigned short ancho = atoi(argv[1]);
    unsigned short alto = atoi(argv[2]);

    
    // Mapear estado
    int fd_state = shm_open(SHM_STATE_NAME, O_RDONLY, 0666);
    size_t state_size = sizeof(EstadoJuego) + (ancho * alto * sizeof(int));
    estado = mmap(NULL, state_size, PROT_READ, MAP_SHARED, fd_state, 0);
    
    // Mapear sincronización
    int fd_sync = shm_open(SHM_SYNC_NAME, O_RDWR, 0666);
    sincro = mmap(NULL, sizeof(Sincronizacion), PROT_READ | PROT_WRITE, MAP_SHARED, fd_sync, 0);

    while (1) {
        if(estado->juego_terminado){
            sem_post(&sincro->sem_impreso);
            break;
        }
        printf("\033[2J\033[H"); //Limpia y mueve cursor al inicio
        
        sem_wait(&sincro->sem_imprimir); // Esperar señal del master
        
        imprimir_tablero(estado);
        
        sem_post(&sincro->sem_impreso); // Notificar al master
    }

    imprimir_final(estado);

    return 0;
}

