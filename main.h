#include "config.h"

typedef struct {
	int connection;
	int seat;
	int ticket;
	int id;
	char role[MAX_ROLES];
} client;

typedef struct {
    client all_clnts[MAX_CLIENTS];
    int count_of_clnts;
    int seats[MAX_SEATS];
    int dvigatel;
	char stops[MAX_STOPS][MAX_LENGTH];
    pthread_t id_of_threads[MAX_THREADS];
    pthread_mutex_t mutex_seat;
} marshrutka_t;

typedef struct {
    marshrutka_t *bus;
    int i;
} thread_arg;
