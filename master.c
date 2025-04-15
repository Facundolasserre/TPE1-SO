#include "master.h"

#include <fcntl.h>
#include <getopt.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "shmemory.h"
#include "game_state.h"
#include "logica.h"
#include "argumentos.h"

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"

EstadoJuego *estado;
Sincronizacion *sincro;

int main(int argc, char * argv[]) {
    int ancho = ANCHO_DEFAULT;
    int largo = LARGO_DEFAULT;
    int retardo = RETARDO_DEFAULT;
    int timeout = TIMEOUT_DEFAULT;
    size_t semilla = time(NULL);
    char * path_vista = NULL;
    char * path_jugadores[MAX_JUGADORES];
    int cant_jugadores = 0;

    cargar_argumentos(argc, argv, &ancho, &largo, &retardo, &timeout, &semilla, &path_vista, path_jugadores, &cant_jugadores);
    print_argumentos(ancho, largo, retardo, timeout, semilla, path_vista, path_jugadores, cant_jugadores);
    sleep(1);

	//Se crean las shared memories
	EstadoJuego *estado = crear_shms(SHM_STATE_NAME, sizeof(EstadoJuego) + (ancho * largo * sizeof(int)));
	Sincronizacion *sincro = crear_shms(SHM_SYNC_NAME, sizeof(Sincronizacion));

	//Se inicializan las shared memories creadas
	inicializar_shms(&estado, &sincro, ancho, largo, cant_jugadores, semilla);

    int pipes_jugadores[MAX_JUGADORES][2];
    
	//inicializar pipes
    for(int i=0 ; i<cant_jugadores ; i++){
        if(pipe(pipes_jugadores[i]) == -1){
            perror("pipe");
            for(int j=0 ; j<i ; j++){
                close(pipes_jugadores[j][0]);
                close(pipes_jugadores[j][1]);
            }
            exit(EXIT_FAILURE);
        }
    }

	char ancho_str[10], alto_str[10];
	sprintf(ancho_str, "%d", ancho);
	sprintf(alto_str, "%d", largo);

    //pid proceso jugador
    pid_t pids_jugadores[MAX_JUGADORES];
    crear_jugadores(estado, pipes_jugadores, path_jugadores, cant_jugadores, ancho_str, alto_str, pids_jugadores);

    //pid proceso vista
	pid_t pid_vista = 0;
	if(path_vista){
	    pid_vista= crear_proceso(path_vista, ancho_str, alto_str);
	} 

    time_t *tiempo_ult_mov = malloc(cant_jugadores * sizeof(time_t));

    if (tiempo_ult_mov == NULL) {
		fprintf(stderr,"Fallo el malloc para tiempo_ult_mov.\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < cant_jugadores; i++) {
		tiempo_ult_mov[i] = time(NULL);
	}

    
    juego(estado, sincro, pipes_jugadores, tiempo_ult_mov, cant_jugadores, timeout, retardo, pid_vista);

    free(tiempo_ult_mov);

    limpia_recursos(pipes_jugadores, cant_jugadores, estado, sincro, pid_vista, pids_jugadores);

    return 0;

}

void pid_procesos(int num_jugadores, pid_t pids_jugadores[], EstadoJuego *estado, pid_t pid_vista){
	
	printf("Master Chomp Champs PID: %d\n", getpid());

	if(pid_vista){
		int status_salida_vista;
		waitpid(pid_vista, &status_salida_vista, 0);
		printf("Vista || PID %d finalizó con status %d\n", pid_vista, status_salida_vista);
	}
	for(int i = 0; i < num_jugadores; i++){
	
		int status;
		waitpid(pids_jugadores[i], &status, 0);
		
		if(WIFEXITED(status)){
			printf("Jugador %d || PID: %d || Status de salida: %d || Puntaje: %d\n", i+1, pids_jugadores[i], WEXITSTATUS(status), estado->jugadores[i].puntaje);
		}
		else if(WIFSIGNALED(status)){
			printf("Jugador %d || PID: %d || Muerto por señal: %d\n", i+1, pids_jugadores[i], WTERMSIG(status));
		}
		else if(WIFSTOPPED(status)){
			printf("Jugador %d || PID: %d || Cortado por señal: %d\n", i+1, pids_jugadores[i], WSTOPSIG(status));
		}
		else {
			printf("Jugador %d || PID: %d || Status desconocido\n", i+1, pids_jugadores[i]);
		}
	}
}

void limpia_recursos(int pipes_jugadores[MAX_JUGADORES][2], int cant_jugadores, EstadoJuego * estado, Sincronizacion * sincro, pid_t pid_vista, pid_t pid_jugadores[]){
    
    for (int i = 0 ; i < cant_jugadores ; i++){
		close(pipes_jugadores[i][0]);
	}
    
    pid_procesos(cant_jugadores, pid_jugadores, estado, pid_vista);

    sem_destroy(&sincro->sem_imprimir);
	sem_destroy(&sincro->sem_impreso);
	sem_destroy(&sincro->mutex_estado);
	sem_destroy(&sincro->mutex_juego);
	sem_destroy(&sincro->mutex_lectura);

    munmap(estado, sizeof(EstadoJuego) + (estado->ancho * estado->alto * sizeof(int)));
    shm_unlink(SHM_STATE_GAME);

    munmap(sincro, sizeof(Sincronizacion));
    shm_unlink(SHM_SYNC_GAME);

}




    
    






