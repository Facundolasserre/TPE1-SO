#include "argumentos.h"

void cargar_argumentos(int argc, char * argv[], int * ancho, int * largo, int * retardo, int * timeout, size_t * semilla, char ** path_vista, char * path_jugadores[], int * cant_jugadores) {
    int opt;

    while ((opt = getopt(argc, argv, "w:h:d:t:s:v:p:")) != -1) {
        switch (opt) {
            case 'w':
                *ancho = atoi(optarg);
                if (*ancho < 10) {
                    fprintf(stderr, "El ancho debe ser de al menos 10.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'h':
                *largo = atoi(optarg);
                if (*largo < 10) {
                    fprintf(stderr, "El largo minimo debe ser de 10.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'd':
                *retardo = atoi(optarg);
                if (*retardo < 0) {
                    fprintf(stderr, "El retardo debe ser un numero positivo.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                *timeout = atoi(optarg);
                if (*timeout < 0) {
                    fprintf(stderr, "El timeout debe ser un numero positivo.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                *semilla = atoi(optarg);
                break;
            case 'v':
                *path_vista = optarg;
                break;
            case 'p':
                optind--;
                while (optind < argc && argv[optind][0] != '-') {
                    if (*cant_jugadores >= MAX_JUGADORES) {
                        fprintf(stderr, "El maximo de jugadores permitidos es %d.\n", MAX_JUGADORES);
                        exit(EXIT_FAILURE);
                    }
                    if (argv[optind] == NULL || strlen(argv[optind]) == 0) {
                        fprintf(stderr, "El path al jugador es inválido.\n");
                        exit(EXIT_FAILURE);
                    }
                    path_jugadores[(*cant_jugadores)++] = argv[optind++];
                }
                break;
            default:
                fprintf(stderr,
                        "Usage: %s [-w ancho] [-h largo] [-d retardo] "
                        "[-t timeout] [-s semilla] [-v path vista] "
                        "[-p path jugadores...]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (*cant_jugadores == 0) {
        fprintf(stderr, "Debe haber al menos un jugador.\n");
        exit(EXIT_FAILURE);
    }
}

void print_argumentos(int ancho, int largo, int retardo, int timeout, size_t seed, char * path_vista, char * path_jugadores[], int cant_jugadores) {
    printf("Parametros:\n");
    printf("Ancho: %d\n", ancho);
    printf("Largo: %d\n", largo);
    printf("Retardo: %d ms\n", retardo);
    printf("Timeout: %d s\n", timeout);
    printf("Semilla: %zu\n", seed);
    printf("Path vista: %s\n", path_vista ? path_vista : "Nulo");
    printf("Path jugadores:\n");
    for (int i = 0; i < cant_jugadores; i++) {
        printf("  %s\n", path_jugadores[i]);
    }
}