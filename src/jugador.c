#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <stdbool.h>
#include <time.h>

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"
#define MAX_INTENTOS 8

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
    int tablero[];
} EstadoJuego;

typedef struct {
    sem_t A, B, C, D, E;
    unsigned int F;
} Sincronizacion;

EstadoJuego *estado;
Sincronizacion *sincro;
int mi_indice = -1;

// Direcciones: 0=Arriba, 1=Arriba-Derecha, 2=Derecha, 3=Abajo-Derecha, 
// 4=Abajo, 5=Abajo-Izquierda, 6=Izquierda, 7=Arriba-Izquierda
const short dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const short dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

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
    return estado->tablero[y * estado->ancho + x] > 0;
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
    if (mi_indice == -1 || estado->juego_terminado || 
        estado->jugadores[mi_indice].bloqueado) return;

    Jugador *yo = &estado->jugadores[mi_indice];
    unsigned char direcciones[8] = {0,1,2,3,4,5,6,7};
    int mejores[3] = {-1, -1, -1};
    int max_recompensa = 0;

    // Barajar direcciones para exploración aleatoria
    barajar_direcciones(direcciones);

    // Buscar las 3 mejores opciones
    for (int i = 0; i < 8; i++) {
        unsigned char dir = direcciones[i];
        int nx = yo->x + dx[dir];
        int ny = yo->y + dy[dir];
        
        if (dentro_del_tablero(nx, ny)){
            int recompensa = estado->tablero[ny * estado->ancho + nx];
            
            if (recompensa > 0) {
                if (recompensa > max_recompensa) {
                    max_recompensa = recompensa;
                    mejores[0] = dir;
                } else if (recompensa == max_recompensa) {
                    if (mejores[1] == -1) mejores[1] = dir;
                    else mejores[2] = dir;
                }
            }
        }
    }

    // Seleccionar movimiento
    unsigned char movimiento = 8; // Inválido
    for (int i = 0; i < 3; i++) {
        if (mejores[i] != -1) {
            movimiento = mejores[i];
            break;
        }
    }

    // Si no encuentra, intentar movimiento de escape
    if (movimiento == 8) {
        for (int i = 0; i < 8; i++) {
            int nx = yo->x + dx[i];
            int ny = yo->y + dy[i];
            if (dentro_del_tablero(nx, ny)) {
                movimiento = i;
                break;
            }
        }
    }

    if (movimiento != 8) {
        write(STDOUT_FILENO, &movimiento, 1);
    }
}

void sincronizar_lectura() {
    sem_wait(&sincro->C);
    sem_wait(&sincro->E);
    sincro->F++;
    if (sincro->F == 1) sem_wait(&sincro->D);
    sem_post(&sincro->E);
}

void liberar_lectura() {
    sem_wait(&sincro->E);
    sincro->F--;
    if (sincro->F == 0) sem_post(&sincro->D);
    sem_post(&sincro->E);
    sem_post(&sincro->C);
}

int main(int argc, char *argv[]) {
    srand(time(NULL) ^ getpid());
    
    // Conectar a memorias compartidas
    int fd_state = shm_open(SHM_STATE_NAME, O_RDONLY, 0666);
    estado = mmap(NULL, sizeof(EstadoJuego) + (100 * sizeof(int)), 
                PROT_READ, MAP_SHARED, fd_state, 0);
    
    int fd_sync = shm_open(SHM_SYNC_NAME, O_RDWR, 0666);
    sincro = mmap(NULL, sizeof(Sincronizacion), 
                PROT_READ | PROT_WRITE, MAP_SHARED, fd_sync, 0);

    // Esperar inicialización
    usleep(100000);
    encontrar_indice_jugador();

    while (!estado->juego_terminado) {
        sincronizar_lectura();
        
        if (mi_indice != -1 && !estado->jugadores[mi_indice].bloqueado) {
            generar_movimiento_adaptativo();
        }
        
        liberar_lectura();
        usleep(30000 + (rand() % 40000)); // Tiempo variable entre movimientos
    }

    return 0;
}