#pragma once
#include <ucontext.h>

struct event {
    int code;//code of event, e.g. 1 - babka prosit opustit' okno
    int ask_id;
    struct event* next_event;
};


//TODO add a ticket/travel card
struct client_t {
    struct client *next_client;
    struct client *prev_client;
    int id;
    struct event* first_event;
    struct event* last_event;    
}

struct passazhir {
    struct client_t client;
    int connection;
}

struct babka {
    struct client_t client;
    int hp;
}
