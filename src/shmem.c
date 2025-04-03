/*#include "utils.h"
#include <sys/mman.h>

void init_shared_memory() {
    // Crear/mapear SHM_GAME_STATE y SHM_GAME_SYNC
    int fd = shm_open(SHM_GAME_STATE, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(EstadoJuego));
    EstadoJuego *estado = mmap();
}*/