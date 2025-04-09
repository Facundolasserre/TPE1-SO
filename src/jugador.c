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
    int val = estado->tablero[y * estado->ancho + x];
    return val > 0 || val == -mi_indice; // Celdas con recompensa o propias
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
        estado->jugadores[mi_indice].bloqueado) {
       // dprintf(STDERR_FILENO, "Jugador %d: No puede mover (índice: %d, juego terminado: %d, bloqueado: %d)\n", 
               // mi_indice, mi_indice, estado->juego_terminado, estado->jugadores[mi_indice].bloqueado);
        return;
    }

    Jugador *yo = &estado->jugadores[mi_indice];
    unsigned char direcciones[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    barajar_direcciones(direcciones);

    int max_recompensa = 0;
    unsigned char mejores[8];
    int num_mejores = 0;
    unsigned char alternativas[8];
    int num_alternativas = 0;

    for (int i = 0; i < 8; i++) {
        unsigned char dir = direcciones[i];
        int nx = yo->x + dx[dir];
        int ny = yo->y + dy[dir];
        if (dentro_del_tablero(nx, ny)) {
            int recompensa = estado->tablero[ny * estado->ancho + nx];
            if (recompensa > 0 && (nx != yo->x || ny != yo->y)) {
                if (recompensa > max_recompensa) {
                    max_recompensa = recompensa;
                    mejores[0] = dir;
                    num_mejores = 1;
                } else if (recompensa == max_recompensa) {
                    mejores[num_mejores++] = dir;
                }
            } else if (recompensa == 0 && (nx != yo->x || ny != yo->y)) {
                alternativas[num_alternativas++] = dir;
            } else {
               // dprintf(STDERR_FILENO, "Jugador %d: Celda (%d, %d) no disponible, valor: %d\n", 
                        //mi_indice, nx, ny, recompensa);
            }
        }
    }

    unsigned char movimiento;
    int nx, ny;
    if (num_mejores > 0) {
        int idx = rand() % num_mejores;
        movimiento = mejores[idx];
        nx = yo->x + dx[movimiento];
        ny = yo->y + dy[movimiento];
       // dprintf(STDERR_FILENO, "Jugador %d: Decidió moverse a (%d, %d) con recompensa %d desde (%d, %d), dir=%d\n", 
                //mi_indice, nx, ny, max_recompensa, yo->x, yo->y, movimiento);
    } else if (num_alternativas > 0) {
        int idx = rand() % num_alternativas;
        movimiento = alternativas[idx];
        nx = yo->x + dx[movimiento];
        ny = yo->y + dy[movimiento];
        //dprintf(STDERR_FILENO, "Jugador %d: Decidió moverse a (%d, %d) sin recompensa (celda propia) desde (%d, %d), dir=%d\n", 
                //mi_indice, nx, ny, yo->x, yo->y, movimiento);
    } else {
        //dprintf(STDERR_FILENO, "Jugador %d: Sin movimientos válidos desde (%d, %d) - esperando terminación del juego.\n", 
             //   mi_indice, yo->x, yo->y);
        return; // No enviar movimiento, indicar que está bloqueado
    }

    // Revalidar el movimiento justo antes de enviarlo
    sincronizar_lectura();
    if (estado->tablero[ny * estado->ancho + nx] > 0 || estado->tablero[ny * estado->ancho + nx] == -mi_indice) {
        //dprintf(STDERR_FILENO, "Jugador %d: Enviando movimiento a (%d, %d), dir=%d\n", mi_indice, nx, ny, movimiento);
        write(STDOUT_FILENO, &movimiento, 1);
    } else {
        //dprintf(STDERR_FILENO, "Jugador %d: Movimiento a (%d, %d) ya no es válido, valor: %d\n", 
               // mi_indice, nx, ny, estado->tablero[ny * estado->ancho + nx]);
    }
    liberar_lectura();
}

void sincronizar_lectura() {
    sem_wait(&sincro->C);
    sem_wait(&sincro->E);
    sincro->F++;
    if (sincro->F == 1) sem_wait(&sincro->D);
    sem_post(&sincro->E);
    sem_post(&sincro->C);
}

void liberar_lectura() {
    sem_wait(&sincro->E);
    sincro->F--;
    if (sincro->F == 0) sem_post(&sincro->D);
    sem_post(&sincro->E);
}

int main(int argc, char *argv[]) {
    srand(time(NULL) ^ getpid());
    
    int fd_state = shm_open(SHM_STATE_NAME, O_RDONLY, 0666);
    estado = mmap(NULL, sizeof(EstadoJuego) + (100 * sizeof(int)), 
                PROT_READ, MAP_SHARED, fd_state, 0);
    
    int fd_sync = shm_open(SHM_SYNC_NAME, O_RDWR, 0666);
    sincro = mmap(NULL, sizeof(Sincronizacion), 
                PROT_READ | PROT_WRITE, MAP_SHARED, fd_sync, 0);

    usleep(100000);
    encontrar_indice_jugador();

    while (!estado->juego_terminado) {
        sincronizar_lectura();
        
        if (mi_indice != -1 && !estado->jugadores[mi_indice].bloqueado) {
            generar_movimiento_adaptativo();
        }
        
        liberar_lectura();
        usleep(10000 + (rand() % 20000)); // 10-30 ms
    }

    munmap(estado, sizeof(EstadoJuego) + (100 * sizeof(int)));
    munmap(sincro, sizeof(Sincronizacion));
    close(fd_state);
    close(fd_sync);
    return 0;
}