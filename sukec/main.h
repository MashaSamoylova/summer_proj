#include "config.h"

typedef struct {
    int code;//code of event, e.g. 1 - babka prosit opustit' okno
    int (*handler)(int, int);
    int asc_id;
} event;

typedef struct {
	int connection;
	int seat;
	int ticket;
	int id;
	char role[MAX_ROLES];
    event quene[MAX_QUEUE];
} client;

typedef struct {
    client all_clnts[MAX_CLIENTS];
    int count_of_clnts;
    int seats[MAX_SEATS];
    int dvigatel;
	char stops[MAX_STOPS][MAX_LENGTH];
	int count_of_stps;
} marshrutka_t;

typedef struct {
    marshrutka_t *bus;
    int i;
} thread_arg;

