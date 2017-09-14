#pragma once
#include <ucontext.h>

#include "event.h"
#include "main.h"

/*
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

*/

int empty(struct client_t *Ivan) {
    
    if(Ivan->first_event == NULL) {
        return 1;
    }
    return 0;
}

void add_event(struct client_t *Ivan, struct event *sc) {
    
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

void delete_event(struct client_t *Ivan) {
    struct event* cup = Ivan->first_event;
    Ivan->first_event = Ivan->first_event->next_event;
    free(cup);
}

void generate_event(int number, int code, struct client_t *Ivan) {
    
    struct client_t *arrow = Ivan->bus->first_client;
   
    //поиск клиента, которому хотят добавить событие
    while(number) {
        arrow = arrow->next_client;
        number--;
    }

    struct event *sc = calloc(1, sizeof(struct event));
    sc->code = code;
    sc->ask_id = Ivan->id;
    sc->next_event = NULL;

    add_event(arrow, sc);
}

int loop(struct client_t* Ivan) {
    while(1) {
        printf("client id %d\n", Ivan->id);
        
        if(!empty(Ivan)) {
            Ivan->handler(Ivan);
        }
        
        Ivan->generator(Ivan);
        swapcontext(&Ivan->context, &Ivan->next_client->context);
    }
}

int init_context(struct client_t *Ivan) {
    Ivan->context.uc_link = NULL;
    Ivan->context.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
    Ivan->context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

    getcontext(&Ivan->context);
    makecontext(&Ivan->context, (void (*)(void))loop,
        1, Ivan);
    return 0;

}

int insert_client(struct client_t* Ivan) {
    
    if(!Ivan->bus) {
        printf("клиент без автобуса\n");
        return -1;
    }

    printf("добавление\n");
    Ivan->id = rand()%1000;
    printf("%d\n", Ivan->id);
    Ivan->first_event = NULL;
    Ivan->last_event = NULL;

    Ivan->bus->last_client->next_client = Ivan;
    Ivan->bus->last_client = Ivan;
    Ivan->next_client = Ivan->bus->first_client;
   
    init_context(Ivan);
    return 0;
}


