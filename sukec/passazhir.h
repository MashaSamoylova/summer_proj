#pragma once
#include "main.h"
#include "client.h"

int passazhir_handler(struct client_t* Ivan) {

    struct passazhir *Tom = (struct passazhir*)Ivan;
    
    while( !empty(Ivan) ) {
        int code = Ivan->first_event->code;
        printf("id %d\n", Ivan->id);
        switch(code) {
            case 1:
                printf("event 1\n");
                break;
            case 2:
                printf("event 2\n");
                break;
            default:
                break;
        }
        delete_event(Ivan);
        printf("event is handled\n");
    }

    sleep(0.5);
    return 0;
}

int passazhir_generator(struct client_t* Ivan) {
    struct passazhir *Tom = (struct passazhir*)Ivan;
    printf("generate TOM %d\n", Tom->client.id);
    return 0;
}

int new_client(int dvigatel) {

    struct sockaddr_in cli_addr;
    int newsockfd;
    socklen_t client_ss;

    client_ss = sizeof(cli_addr);
    newsockfd = accept(dvigatel, (struct sockaddr *) &cli_addr, &client_ss);
    if (newsockfd < 0) {
        printf("ERROR on accept\n");
    }
    return newsockfd;
}

int add_passzhir(marshrutka_t* bus) {

    printf("добавление пассажира\n");
    struct passazhir *Ivan = calloc(1, sizeof(struct passazhir));
    struct client_t* client = (struct client_t*)Ivan;

    Ivan->client.bus = bus;
    if(bus->first_client == NULL) {
        bus->first_client = client; 
        bus->last_client = client;
        insert_client(client);
    }
    else {
        insert_client(client);
    }
    
    Ivan->connection = new_client(bus->dvigatel);
    Ivan->client.handler = passazhir_handler;
    Ivan->client.generator = passazhir_generator;
    return 0;
}
