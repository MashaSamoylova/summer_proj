#pragma once
#include "event.h"
#include "client.h"

#include <ucontext.h>

struct babka{
    int hp;
    int id;
    struct event* first_event;
    ucontext_t context;
    struct babka* next_babka;
};

void generate_open( struct babka* Katya, struct client* Ivan ) {
   generate_event(Ivan, 1, Katya->id);
}

void generate_close( struct babka* Katya, struct client* Ivan ) {
    generate_event(Ivan, 2, Katya->id);    
}

void add_babka(struct babka* Katya, int id) {
    Katya->next_babka = NULL;
    Katya->first_event = NULL;
    Katya->id = id;
    Katya->hp = 100;
    
}
