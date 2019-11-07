CC=gcc
CFLAGS=-I. -pthread -std=c99 -lpthread


example: example.c
	$(CC) -o $@ $^ $(CFLAGS)

semaphores: semaphores.c
	$(CC) -o $@ $^ $(CFLAGS)

