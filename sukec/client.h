#pragma once
#include "event.h"
#include <ucontext.h>

struct client {
	int connection;
	int seat;
	int ticket;
	int id;
    char buffer[1024];
    struct event* first_event;
    struct client* next_client;
    ucontext_t context;
    ucontext_t read_context;
};

int empty(struct client* Ivan) {
    
    if(Ivan->first_event == NULL) {
        return 1;
    }
    return 0;
}

void delete_event(struct client* Ivan) {
    struct event* cup = Ivan->first_event;
    Ivan->first_event = Ivan->first_event->next_event;
    free(cup);
}

void add_event(struct client* Ivan, struct event* sc) {
    
    if( empty(Ivan) ) { //queue is empty
        Ivan->first_event = sc;
        return;
    }

    struct event* last = Ivan->first_event;
    while(last->next_event != NULL) {
        last = last->next_event;
    }
    last = sc;
}

/*generating event to normal client from babka with id = ask_id*/
void generate_event(struct client* Ivan, int code, int ask_id) { 
    struct event* sc = calloc(1, sizeof(struct event));
    
    sc->code = code;
    sc->ask_id = ask_id;
    sc->next_event = NULL;

    add_event(Ivan, sc);
}

void hello(struct client* qw) {
    char buff[256];
    char p[1000];
	*p = 0;
    FILE *bus_ascii = fopen("bus", "r");
    if(bus_ascii == NULL) {
        printf("not found the bus:(\n");
        exit(1);
    }

    while(fgets(buff, 256, bus_ascii)) {
        strncat(p, buff, strlen(buff));
    }
    write(qw->connection, p, strlen(p));
    fclose(bus_ascii);

    char msg[40] = "\nМАРШРУТКА № 442\n";
    write(qw->connection, msg, strlen(msg));
    return;	
}

void write_to_client(struct client* Ivan, char* message) {
    write(Ivan->connection, message, strlen(message)); 
}

void read_answer(struct client* Ivan) {
    printf("сюда доходит\n");
    int flags = fcntl(Ivan->connection, F_GETFL, 0);
    if(fcntl(Ivan->connection, F_SETFL, flags | O_NONBLOCK)) {
        printf("something wrong\n");
    }

    int result;
    while(1) {

        result = read(Ivan->connection, Ivan->buffer, 1024);
        
        if(result != -1 ) {
            printf("%s\n", Ivan->buffer);
        }
        swapcontext(&Ivan->read_context, &Ivan->context);
    }
}


void open_window(struct client* Ivan) {
    write_to_client(Ivan, "Молодой человек, откройте окно!\n\n1. открыть окно\n 2.ударить бабку\n");
    int answer = 0;
    
    /*переклячатся на другого пользователя пока тупит этот*/
    while(!answer) {
        swapcontext(&Ivan->context, &Ivan->read_context);
        answer = atoi(Ivan->buffer);
        swapcontext(&Ivan->context, &Ivan->next_client->context);
    }

    printf("открытие окна %d %d\n");


}

void handling(struct client* Ivan) {
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
}


