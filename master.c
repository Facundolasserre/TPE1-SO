#include "master.h"
#include "argumentos.h"

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
#include "game_state.h"
#include "shmemory.h"

#define SHM_STATE_NAME "/game_state"
#define SHM_SYNC_NAME "/game_sync"

EstadoJuego *estado;
Sincronizacion *sincro;

int main(int argc, char * argv[]) {
    int ancho = 10;
    int largo = 10;
    int retardo = 10;
    int timeout = 10;
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
//////////////hasta aca bien//////////////////////////

	char ancho_str[10], alto_str[10];
	sprintf(ancho_str, "%d", ancho);
	sprintf(alto_str, "%d", alto);

	


    return 0;
}

