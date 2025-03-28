CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pthread
LDFLAGS = -lrt

SRC = src/master.c src/vista.c src/jugador.c src/shmem.c src/semaphores.c
OBJ = $(SRC:.c=.o)

all: master vista jugador

master: src/master.o src/shmem.o src/semaphores.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

vista: src/vista.o src/shmem.o src/semaphores.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

jugador: src/jugador.o src/shmem.o src/semaphores.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f master vista jugador src/*.o