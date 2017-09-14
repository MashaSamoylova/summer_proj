#pragma once
#include "main.h"
/*for create a thread*/
/*typedef struct { 
    marshrutka_t *bus;
    int i;
} thread_arg;*/

/*очистка памяти*/
/*void scrabwoman(marshrutka_t* bus) {
    struct client* arrow = bus->first_client;
    struct client* cup;
    
    while(arrow) {
        cup = arrow;
        free(arrow->context.uc_stack.ss_sp);
        arrow = arrow->next_client;
        free(cup);
    }

    free(bus);
}*/

/*отключение всех клиентов*/
/*void vypnut_passazhirov(marshrutka_t *bus) {
    struct client* arrow = bus->first_client;
    
    while(arrow) {
        close(arrow->connection);
        arrow = arrow->next_client;
    }
}*/


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

}
*/

void* otpravit_marshrutku(void* arg) {
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    printf("поееехааали\n");
    loop(bus->first_client);
	return NULL;
}


void start_marshrutaka(marshrutka_t* bus) {
    loop(bus->first_client);
}

void init_marshrutka(marshrutka_t* bus) {
    bus->first_client = NULL;
    bus->last_client = NULL;
    bus->n_clients = 0;
    bus->n_babok = 0;
    bus->status_window = 1;
}

