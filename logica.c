#include "logica.h"
#include "game_state.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>  
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include "shmemory.h"
#include <errno.h>

const short int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
const short int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};


void inicializar_tablero(EstadoJuego *estado, size_t semilla){
    srand(semilla);
    for(int i = 0; i < estado->ancho * estado->alto; i++){
        estado->tablero[i] = (rand() % 9) + 1;
    }
}


void ubicar_jugadores(EstadoJuego *estado){
    for (int i = 0; i < estado->num_jugadores; i++) {
		estado->jugadores[i].x = estado->ancho / (rand() % estado->ancho);
		estado->jugadores[i].y = estado->alto / (rand() % estado->alto);
		estado->jugadores[i].puntaje = 0;
		estado->jugadores[i].mov_invalidos = 0;
		estado->jugadores[i].mov_validos = 0;
		estado->jugadores[i].bloqueado = false;
		snprintf(estado->jugadores[i].nombre, sizeof(estado->jugadores[i].nombre),"Jugador %d", i + 1);
	}
}

void crear_jugadores(EstadoJuego *estado, int pipes_jugadores[MAX_JUGADORES][2], char * path_jugadores[], int num_jugadores, char * str_ancho, char * str_alto, pid_t pids_jugadores[MAX_JUGADORES]){
    for(int i = 0; i < num_jugadores ; i++){
        pids_jugadores[i] = fork();
        if(pids_jugadores[i] == -1){
            perror("fork");
            break;
        }
        if(pids_jugadores[i] == 0){ //estamos en el proceso hijo
            if(dup2(pipes_jugadores[i][1], STDOUT_FILENO) < 0){
                perror("dup2");
                exit(EXIT_FAILURE);
            }
            close(pipes_jugadores[i][1]);  
            if (close(pipes_jugadores[i][0]) < 0) {
                perror("cerrar el extremo de lectura");
                exit(EXIT_FAILURE);
            }

            for(int j=i+1; j<num_jugadores; j++){          
                close(pipes_jugadores[j][1]);
                close(pipes_jugadores[j][0]);  
            }
            for(int k=0 ; k<i ; k++){
                close(pipes_jugadores[k][0]);
            }

            if(execl(path_jugadores[i], path_jugadores[i], str_ancho, str_ancho, NULL) == -1){
                fprintf(stderr, "execl fallo por %s: %s\n", path_jugadores[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        estado->jugadores[i].pid = pids_jugadores[i];
        if (close(pipes_jugadores[i][1]) < 0) {
                perror("cerrar el extremo derecho del padre");
                exit(EXIT_FAILURE);
            }
    }
}

bool mov_valido(EstadoJuego * estado, int index_jugador, unsigned char mov){
    int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
	int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

    int dir = mov;

    int prox_x= estado->jugadores[index_jugador].x + dx[dir];
    int prox_y= estado->jugadores[index_jugador].y + dy[dir];

    return (prox_x >= 0 && prox_x < estado->ancho && prox_y >= 0 && prox_y < estado->alto && estado->tablero[prox_y * estado->ancho + prox_x] > 0);
}

bool tiene_mov_valido(EstadoJuego *estado, int index_jugador) {
	for (int mov = 0; mov < 8; mov++) {
		if (mov_valido(estado, index_jugador, mov)) {
			return true;
		}
	}
	return false;
}

void chequeo_timeouts(EstadoJuego * estado, time_t * tiempo_ult_mov, int timeout, int * jugadores_bloqueados, int cant_jugadores){
    
    time_t tiempo_actual = time(NULL);
    
    for (int i = 0; i < cant_jugadores; i++) {
		if (!estado->jugadores[i].bloqueado) {
			if (!tiene_mov_valido(estado, i)) {
				estado->jugadores[i].bloqueado=1;
				(*jugadores_bloqueados)++;
			}
			else if (tiempo_actual - tiempo_ult_mov[i] >= timeout) {
				estado->jugadores[i].bloqueado=1;
				(*jugadores_bloqueados)++;
			}
		}
	}
}

void procesar_movimiento(EstadoJuego * estado, int index_jugador, unsigned char mov){
    if(!mov_valido(estado, index_jugador, mov)){
        estado->jugadores[index_jugador].mov_invalidos++;
    }else{
        int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
		int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

        int prox_x= estado->jugadores[index_jugador].x + dx[mov];
        int prox_y= estado->jugadores[index_jugador].y + dy[mov];

        estado->jugadores[index_jugador].puntaje += estado->tablero[prox_y * estado->ancho + prox_x];
		estado->jugadores[index_jugador].x = prox_x;
		estado->jugadores[index_jugador].y = prox_y;
		estado->jugadores[index_jugador].mov_validos++;
		estado->tablero[prox_y * estado->ancho + prox_x] = 0 - index_jugador; 

    }
}

void leer_mov(EstadoJuego * estado, Sincronizacion * sincro, int pipes_jugadores[MAX_JUGADORES][2], fd_set * leerfds, time_t *tiempo_ult_mov, int * jugadores_bloqueados, int cant_jugadores, pid_t pid_vista){
    
    for(int i=0 ; i<cant_jugadores ; i++){
        if(!estado->jugadores[i].bloqueado && FD_ISSET(pipes_jugadores[i][0], leerfds)){
            unsigned char mov;
 			ssize_t lectura_bytes = read(pipes_jugadores[i][0], &mov, sizeof(mov));

             if (lectura_bytes <= 0) {
                estado->jugadores[i].bloqueado=1;
                (*jugadores_bloqueados)++;
            }else{
                if(sem_wait(&sincro->mutex_estado) == -1){
                    perror("sem_wait");
                    exit(EXIT_FAILURE);
                }
                if(sem_wait(&sincro->mutex_juego) == -1){
                    perror("sem_wait");
                    exit(EXIT_FAILURE);
                }

                procesar_movimiento(estado, i, mov);

                if (sem_post(&sincro->mutex_juego) == -1) {
                    perror("sem_post");
                    exit(EXIT_FAILURE);
                }
                if(pid_vista){
                    
                    if (sem_post(&sincro->sem_imprimir) == -1) {
                        perror("sem_post");
                        exit(EXIT_FAILURE);
                    }
                    if (sem_wait(&sincro->sem_impreso) == -1) {
                        perror("sem_wait");
                        exit(EXIT_FAILURE);
                    }
                    
                }

                if(sem_post(&sincro->mutex_estado) == -1){
                    perror("sem_wait");
                    exit(EXIT_FAILURE);
                }

                tiempo_ult_mov[i] = time(NULL);
            }


            
        }
    }
}

 
void juego(EstadoJuego *estado, Sincronizacion *sincro, int pipes_jugadores[MAX_JUGADORES][2], time_t *tiempo_ult_mov, int cant_jugadores, int timeout, int retardo, pid_t pid_vista){
    
	fd_set leerfds;
    int jugadores_bloqueados=0;
    struct timeval tv;
    int fd_max = -1;


    for (int i = 0; i < cant_jugadores; i++) {
		if (pipes_jugadores[i][0] > fd_max){
			fd_max = pipes_jugadores[i][0];
		}
	}

    while(!estado->juego_terminado){

        chequeo_timeouts(estado, tiempo_ult_mov, timeout, &jugadores_bloqueados, cant_jugadores);
        if (jugadores_bloqueados == cant_jugadores) {
			estado->juego_terminado = 1;
			if (pid_vista) {
                if (sem_post(&sincro->sem_imprimir) == -1) {
                    perror("sem_post");
                    exit(EXIT_FAILURE);
                }
                if (sem_wait(&sincro->sem_impreso) == -1) {
                    perror("sem_wait");
                    exit(EXIT_FAILURE);
                }
	          
			}
		}
        

        FD_ZERO(&leerfds);
		
		for (int i = 0; i < cant_jugadores; i++){
			if (!estado->jugadores[i].bloqueado){
				FD_SET(pipes_jugadores[i][0], &leerfds);
			}
		}
        
        tv.tv_sec  = 0;
		tv.tv_usec = 150000;

        int listo = select(fd_max + 1, &leerfds, NULL, NULL, &tv);
		if (listo == -1) {
			perror("select");
			break;
		}

        leer_mov(estado, sincro, pipes_jugadores, &leerfds, tiempo_ult_mov, &jugadores_bloqueados, cant_jugadores, pid_vista);

        if (pid_vista) {
			usleep(retardo * 1000);
		}
	}

}


