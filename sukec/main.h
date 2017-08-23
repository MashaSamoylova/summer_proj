#include "config.h"

struct event{
    int code;//code of event, e.g. 1 - babka prosit opustit' okno
    int ask_id;
    struct event* next_event;
};

struct client {
	int connection;
	int seat;
	int ticket;
	int id;
    struct event* first_event;
    struct client* next_client;
};

typedef struct{
    struct client* skeleton;
    int hp;
}babka;

typedef struct {
    struct client* first_client;
    struct client* last_client;
    struct client all_clnts[MAX_PASSAZHIR];
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

