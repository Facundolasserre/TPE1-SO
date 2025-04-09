#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"

// Estructuras deben coincidir EXACTAMENTE con el master
typedef struct {
    char nombre[16];
    unsigned int puntaje;
    unsigned int mov_invalidos;
    unsigned int mov_validos;
    unsigned short x, y;
    pid_t pid;
    bool bloqueado;
} Jugador;

typedef struct {
    unsigned short ancho;
    unsigned short alto;
    unsigned int num_jugadores;
    Jugador jugadores[9];
    bool juego_terminado;
    int tablero[]; // Flexible array member para el tablero
} EstadoJuego;

typedef struct {
    sem_t A; //print_needed
    sem_t B; //print_done
    sem_t C;
    sem_t D;
    sem_t E;
    unsigned int F;
} Sincronizacion;

EstadoJuego *estado;
Sincronizacion *sincro;

void liberar_recursos(int signum) {
    (void)signum;
    munmap(estado, sizeof(EstadoJuego) + (estado->ancho * estado->alto * sizeof(int)));
    munmap(sincro, sizeof(Sincronizacion));
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <ancho> <alto>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, liberar_recursos);
    
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
        sem_wait(&sincro->A); // Esperar señal del master
        
        // Imprimir estado crudo para diagnóstico
        printf("=== ESTADO ACTUAL ===\n");
        printf("Jugadores activos: %u\n", estado->num_jugadores);
        
        // Imprimir tablero línea por línea
        for (int y = 0; y < estado->alto; y++) {
            for (int x = 0; x < estado->ancho; x++) {
                int val = estado->tablero[y * estado->ancho + x];
                if (val > 0) printf("[%2d]", val);
                else if (val <= 0) printf(" P%d ", -val);
                //else printf(" P0 ");
            }
            printf("\n");
        }
        
        
       fflush(stdout);
        
        sem_post(&sincro->B); // Notificar al master
    }

    return 0;
}