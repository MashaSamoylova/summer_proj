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

ucontext_t main_context, next_context; 

void scrabwoman(marshrutka_t* bus) {
    free(bus);
}

void vypnut_passazhirov(marshrutka_t *bus) {
    struct client* arrow = bus->first_client;
    struct client* cup;
    do {
        close(arrow->connection);
        cup = arrow;
        arrow = arrow->next_client;
        free(cup);
    } while(arrow);
}


void broadcast(marshrutka_t *bus, char* message) {
    struct client* arrow = bus->first_client;

    do {
        write(arrow->connection, message, strlen(message));
        arrow = arrow->next_client;
    } while(arrow);
}

void sit_down(marshrutka_t *bus, struct client* qw) {
    int st;
    do {
        st = rand() % MAX_SEATS;
    }while(bus->seats[st]); 
    
    bus->seats[st] = 1;
    qw->seat = st;
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

/*приветсвие и посадка*/
void spawn_passazhir(marshrutka_t* bus, struct client* qw) {

    hello(qw);
    sit_down(bus, qw);

	char msg[40];
    sprintf(msg, "Ты выбрал место: %d\n", qw->seat);
    write(qw->connection, msg, strlen(msg));

    return;
}

void spawn_babka(struct babka* nemolodaya) {
    nemolodaya->hp = 100;
    nemolodaya->next_babka = NULL;
}

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

void generate_event(struct client* Ivan, int code, int ask_id) {
    struct event* sc = calloc(1, sizeof(struct event));
    
    sc->code = code;
    sc->ask_id = ask_id;
    sc->next_event = NULL;

    add_event(Ivan, sc);
}


void next_client(marshrutka_t* bus) {

    while(1) {
        
        struct client* arrow = bus->first_client;
        
        while(arrow) {
            printf("прыгнул\n");
            swapcontext(&next_context, &arrow->context);
            printf("вернулся\n");
            arrow = arrow->next_client;
            sleep(1);
        }

    }
}

void handler(struct client* Ivan) {
    while(1) {
        printf("дошел до цели\n");
        printf("client id %d\n", Ivan->id);
        swapcontext(&Ivan->context, &next_context);
    }
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

void* otpravit_marshrutku(void* arg) {
    getcontext(&main_context);
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    broadcast(bus, "\nотправляемся\n"); 
    next_client(bus);
    vypnut_passazhirov(bus);
    scrabwoman(bus);
	return NULL;
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


void add_passanger(struct client* cl, marshrutka_t* bus) {
    
    cl->connection = new_client(bus->dvigatel);
    cl->ticket = 0;
    cl->id = bus->count_of_clnts;
    cl->first_event = NULL;
    cl->next_client = NULL;
    spawn_passazhir(bus, cl);

    char* stack = calloc( SIGSTKSZ, sizeof(char) );

    cl->context.uc_link = &main_context;
    cl->context.uc_stack.ss_sp = stack;
    cl->context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

    getcontext(&cl->context);
    makecontext(&cl->context, (void (*)(void))handler,
        2, cl, &next_context);
    bus->count_of_clnts++;
}

/*инициалтзация глобальных контекстов*/
void init_context(marshrutka_t* bus) {
    char* stack = calloc( SIGSTKSZ, sizeof(char) );

    next_context.uc_link = &main_context;
    next_context.uc_stack.ss_sp = stack;
    next_context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

    getcontext(&next_context);
    makecontext(&next_context, (void (*)(void))next_client,
            1, bus);
}

/*приветсвие и посдка происходит без корутин*/
void init_marshrutka(marshrutka_t* bus) {
    /*инициализация пассажиров*/
    bus->count_of_clnts = 0;
    
    bus->first_client = calloc(1, sizeof( struct client ));
    add_passanger(bus->first_client, bus);
    bus->last_client = bus->first_client;

    for(int i = 1; i < MAX_CLIENTS; i++) {

        bus->last_client->next_client = calloc( 1, sizeof(struct client) );
        bus->last_client = bus->last_client->next_client;
        add_passanger(bus->last_client, bus);
    }
    init_context(bus);

    return;	
}

int main() {
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
