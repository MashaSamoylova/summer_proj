#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "main.h"
#include "config.h"
#include <ucontext.h>

void broadcast(marshrutka_t *bus, char* message) {
    int i;
    for(i = 0; i < bus->count_of_clnts; i++) {
        write(bus->all_clnts[i].connection, message, strlen(message));
    }
}

void sit_down(marshrutka_t *bus, int i) {
    int st;
    do {
        st = rand() % MAX_SEATS;
    }while(bus->seats[st]); 
    bus->seats[st] = 1;
    bus->all_clnts[i].seat = st;
}

void* hello(marshrutka_t *bus, int i) {
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
    write(bus->all_clnts[i].connection, p, strlen(p));
    fclose(bus_ascii);

    char msg[40] = "\nМАРШРУТКА № 442\n";
    write(bus->all_clnts[i].connection, msg, strlen(msg));
    return NULL;	
}

/*приветсвие и посадка*/
void* spawn_passazhir(void *arg) {
    marshrutka_t *bus = ((thread_arg*)arg)->bus;
    int i = ((thread_arg*)arg)->i;

    hello(bus, i);
    sit_down(bus, i);

	char msg[40];
    sprintf(msg, "Ты выбрал место: %d\n", bus->all_clnts[i].seat);
    write(bus->all_clnts[i].connection, msg, strlen(msg));

    return NULL;
}


int empty(client* Ivan) {
    
    if(Ivan->last_event == NULL) {
        return 1;
    }
    return 0;
}

void delete_event(client* Ivan) {
    struct event* cup = Ivan->first_event;
    Ivan->first_event = Ivan->first_event->next_event;
    free(cup);

}

void handling(client* Ivan) {
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
    }
}

void add_event(client* Ivan, struct event* sc) {
    
    if( !empty(Ivan) ) { //queue is empty
        Ivan->first_event = sc;
        Ivan->last_event = sc;
        return;
    }

    Ivan->last_event->next_event = sc;
    Ivan->last_event = sc;
}

void generate_event(client* Ivan, int code, int ask_id) {
    struct event* sc = calloc(1, sizeof(struct event));
    
    sc->code = code;
    sc->ask_id = ask_id;
    sc->next_event = NULL;

    add_event(Ivan, sc);
}

void next_client(
        marshrutka_t* bus,
        ucontext_t* next_context, 
        ucontext_t* other_context,
        client* current_client) {
    
    int i = 0;
    int count = bus->count_of_clnts;

    while(1){
        *current_client = bus->all_clnts[i];
        printf("check123 %d\n", current_client->id);
        i = (i + 1) % count;
        printf("suka1\n");
        swapcontext(next_context, other_context);
    }
}

int toggle(marshrutka_t* bus) {
    ucontext_t toggle_context1, toggle_context2, next_context;

    char stack[SIGSTKSZ];

    getcontext(&next_context);

    next_context.uc_link = &toggle_context1;
    next_context.uc_stack.ss_sp = stack;
    next_context.uc_stack.ss_size = sizeof(stack);

    volatile client current_client;
    makecontext(&next_context, (void (*)(void))next_client,
            4, bus, &next_context, &toggle_context2, &current_client);


    getcontext(&toggle_context1);

    int i = 0;
    for(int k = 0; k < bus->count_of_clnts; ++k) {
        printf("check %d\n", bus->all_clnts[k].id);
    }
    while(1) {
        swapcontext(&toggle_context2, &next_context);
        printf("iteration № %d, client id %d\n", i, current_client.id);
        i += 1;
        sleep(1);
        
    }
}

int new_client(int dvigatel) {
    listen(dvigatel, 5);

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
    return dvigatel;
}


void* otpravit_marshrutku(void* arg) {	
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    broadcast(bus, "\nотправляемся\n"); 
    toggle(bus);
	return NULL;
}


/*приветсвие и посдка происходит без корутин*/
void init_marshrutka(marshrutka_t* bus) {

    thread_arg* tas = malloc( MAX_CLIENTS*sizeof(thread_arg) );

    bus->count_of_clnts = 0;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        bus->all_clnts[i].connection = new_client(bus->dvigatel);
        bus->all_clnts[i].ticket = 0;
        bus->all_clnts[i].id = i;
        bus->all_clnts[i].first_event = NULL;
        bus->all_clnts[i].last_event = NULL;
        tas[i].bus = bus;
        tas[i].i = i;

		spawn_passazhir(tas+i);

        bus->count_of_clnts++;
    }

    return;	
}

int main() {
    printf("suka\n");
    int dvigatel;
    while (!(dvigatel = zavesti_marshrutku())) {
        char nuegonahuy;
        puts("Тыр-тыр-тыр-тыр... Не завелась! Может быть, ну его?..");
        //system("spd-say -r -50 -l ru \"Тыр-тыр-тыр-тыр... Не завелась! Может быть, ну его?..\"");
        while ((nuegonahuy=getchar()) != '\n')
            if (nuegonahuy == 'y')
                exit(-1);
    }

    pthread_t id;
    thread_arg ta;
    ta.i = dvigatel;
    while(442) {
		marshrutka_t* avtobus_442 = calloc(1, sizeof(marshrutka_t));
		avtobus_442->dvigatel = dvigatel;
		init_marshrutka(avtobus_442);
      
       ta.bus = avtobus_442;
	   //FIXME next bus will not go before the last is will not come	
	    pthread_create(&id, NULL, &otpravit_marshrutku, &ta);
  //	    pthread_join(id, NULL);
    //    printf("ДЖОИН\n");
    }

    return 0;
}
