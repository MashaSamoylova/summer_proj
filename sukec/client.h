#pragma once
#include <ucontext.h>

#include "event.h"
#include "main.h"

/*int empty(struct client* Ivan) {
    
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
*/
/*generating event to normal client from babka with id = ask_id*/
/*void generate_event(struct client* Ivan, int code, int ask_id) { 
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

*/
/*void open_window(struct client* Ivan) {
    write_to_client(Ivan, "Молодой человек, откройте окно!\n\n1. открыть окно\n 2.ударить бабку\n");
    int answer = 0;
    
    while(!answer) {
        swapcontext(&Ivan->context, &Ivan->read_context);
        answer = atoi(Ivan->buffer);
        swapcontext(&Ivan->context, &Ivan->next_client->context);
    }

    printf("открытие окна %d %d\n");


}*/

/*void handling(struct client* Ivan) {
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
*/

/*int new_client(int dvigatel) {

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


void add_passanger(struct client* Ivan, int dvigatel) {
    
    Ivan->connection = new_client(dvigatel);
    Ivan->ticket = 0;
    Ivan->first_event = NULL;
    Ivan->next_client = NULL;
    
    hello(Ivan);
}
*/

int loop(struct client_t* Ivan) {
    while(1) {
        printf("client id %d\n", Ivan->id);
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


