#pragma once
#include <ucontext.h>
#include "config.h"

typedef struct {
    struct client_t* first_client;
    struct client_t* last_client;
    int n_clients;
    int n_babok;
    int dvigatel;
    int status_window; //1 is open, 0 is close
} marshrutka_t;


struct event {
    int code;//code of event, e.g. 1 - babka prosit opustit' okno
    int ask_id;
    struct event* next_event;
};

//TODO add a ticket/travel card
struct client_t {
    marshrutka_t *bus;
    struct client_t *next_client;
    int id;
    struct event *first_event;
    struct event *last_event;    
    int (*handler) (struct client_t*);
    int (*generator) (struct client_t*);
    ucontext_t context;
};

struct passazhir {
    struct client_t client;
    int connection;
};

struct babka {
    struct client_t client;
    int hp;
};

/*for create a thread*/
typedef struct { 
    marshrutka_t *bus;
    int i;
} thread_arg;
