#pragma once
#include "main.h"
//*очистка памяти*/

int clean_events(struct client_t *Ivan) {
    printf("чистка событий\n");
    struct event *arrow = Ivan->first_event;
    struct event *cup;

    while(arrow) {
        cup = arrow;
        arrow = arrow->next_event;
        free(cup);
    }
    return 0;
}

int kill_babki(marshrutka_t* bus) {
    int k = 0;
    struct babka *Katya = (struct babka*)bus->first_client;
    struct babka *cup;
    while( k < bus->n_babok) {
       cup = Katya;
       clean_events((struct client_t*)Katya);
       free(Katya->client.context.uc_stack.ss_sp);
       Katya = (struct babka*)Katya->client.next_client;
       free(cup);
       k++;
    }
    bus->first_client = (struct client_t*)Katya;
    printf("бабки удалены\n");
    return 0;
}

int kill_passazh(marshrutka_t* bus) {
    int k = 0;
    struct passazhir *Tom = (struct passazhir*)bus->first_client;
    struct passazhir *cup;
    while( k < bus->n_clients) {
       cup = Tom;
       clean_events((struct client_t*)Tom);
       free(Tom->client.context.uc_stack.ss_sp);
       Tom = (struct passazhir*)Tom->client.next_client;
       free(cup);
       k++;
    }
    bus->first_client = (struct client_t*)Tom;
    printf("пассажиры удалены\n");
    return 0;
}


void scrabwoman(marshrutka_t* bus) {

    /*удалять именно в таком порядке -__-*/
    kill_passazh(bus);
    kill_babki(bus);
    free(bus);
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

/*void broadcast(marshrutka_t *bus, char* message) {
    struct client* arrow = bus->first_client;

    do {
        write(arrow->connection, message, strlen(message));
        arrow = arrow->next_client;
    } while(arrow != bus->first_client);

}*/

void* otpravit_marshrutku(marshrutka_t *bus) {
    printf("поееехааали\n");
    loop(bus->first_client);
	return NULL;
}

