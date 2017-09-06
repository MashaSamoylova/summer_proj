#pragma once
#include "client.h"
#include "config.h"
#include "babka.h"

typedef struct {
    struct client* first_client;
    struct client* last_client;
    int count_of_clnts;
    int dvigatel;
    struct babka* first_babka;
    struct babka* last_babka;
    ucontext_t toggle;
} marshrutka_t;

/*for create a thread*/
typedef struct { 
    marshrutka_t *bus;
    int i;
} thread_arg;

/*очистка памяти*/
void scrabwoman(marshrutka_t* bus) {
    struct client* arrow = bus->first_client;
    struct client* cup;
    
    while(arrow) {
        cup = arrow;
        free(arrow->context.uc_stack.ss_sp);
        arrow = arrow->next_client;
        free(cup);
    }

    free(bus);
}

/*отключение всех клиентов*/
void vypnut_passazhirov(marshrutka_t *bus) {
    struct client* arrow = bus->first_client;
    
    while(arrow) {
        close(arrow->connection);
        arrow = arrow->next_client;
    }
}


/*возвращает сокет для всего сервиса*/
int zavesti_marshrutku() {
    /*
#ifndef PRAVILNIE_ZAPCHASTI
    return 0;
#endif
*/

    struct sockaddr_in server;
    int dvigatel = socket(AF_INET, SOCK_STREAM, 0);

    if (dvigatel == -1) {
        fprintf(stderr, "Could not create socket\n");
        return 0; 
    }

    int yes = 1;
    setsockopt(dvigatel, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(dvigatel, (struct sockaddr *) &server, sizeof(server)) < 0) {
        fprintf(stderr, "ERROR on binding\n");
        return 0;
    }
    listen(dvigatel, 5);
    return dvigatel;
}

/*заорать на весь автобус*/
void broadcast(marshrutka_t *bus, char* message) {
    struct client* arrow = bus->first_client;

    do {
        write(arrow->connection, message, strlen(message));
        arrow = arrow->next_client;
    } while(arrow != bus->first_client);

}

void spawn_babki(marshrutka_t* bus, int density) {

    density--;
    bus->first_babka = calloc(1, sizeof( struct babka ));
    add_babka(bus->first_babka, density);
    bus->last_babka = bus->first_babka;
    density--;

    while(density != -1) {
        bus->last_babka->next_babka = calloc(1, sizeof(struct babka));
        bus->last_babka = bus->last_babka->next_babka;
        add_babka(bus->last_babka, density);
        density--;
    }

    bus->last_babka->next_babka = bus->first_babka;
}

void spawn_passazhiri( marshrutka_t* bus) {
    bus->count_of_clnts = 0;
    
    bus->first_client = calloc(1, sizeof( struct client ));
    add_passanger(bus->first_client, bus->dvigatel);
    bus->last_client = bus->first_client;

    for(int i = 1; i < MAX_CLIENTS; i++) {

        bus->last_client->next_client = calloc( 1, sizeof(struct client) );
        bus->last_client = bus->last_client->next_client;
        add_passanger(bus->last_client, bus->dvigatel);
    }

    bus->last_client->next_client = bus->first_client;
  
}

void init_marshrutka(marshrutka_t* bus, int density) {
    spawn_passazhiri(bus); 
    spawn_babki(bus, density);
}


