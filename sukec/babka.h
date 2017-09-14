#pragma once
#include "main.h"
#include "client.h"

void generate_open(struct client_t* Katya) {
    int number = rand()%Katya->bus->n_clients;
    printf("generate_open number %d id babi %d\n", number, Katya->id);
    generate_event(number, 1, Katya);
}

int babka_generator(struct client_t* Katya){
    
    //window is open
    if(Katya->bus->status_window) {
        generate_open(Katya);
    }
    else{
        //generate_close(Katya);
    }
    return 0;
}

int baba_handler(struct client_t* Katya) {
    printf("baba action %d\n", Katya->id);
    return 0;
}

void add_babka(marshrutka_t* bus) {
    printf("добавление бабки\n");
    struct babka *Katya = calloc(1, sizeof(struct babka));
    struct client_t *client = (struct client_t*)Katya;

    Katya->client.bus = bus;

    if(bus->first_client == NULL) {
        bus->first_client = client;
        bus->last_client = client;
        insert_client(client);
    }
    else {
        insert_client(client);
    }

    Katya->hp = 100;
    Katya->client.handler = baba_handler;
    Katya->client.generator = babka_generator;
}


/*void generate_close( struct babka* Katya, struct client* Ivan ) {
    generate_event(Ivan, 2, Katya->id);    
}

*/
