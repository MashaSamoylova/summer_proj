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
    printf("%d\n", bus->first_client->id);

    do {
        write(arrow->connection, message, strlen(message));
        printf("lol\n");
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

void spawn_babka(marshrutka_t* bus) {
    
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

void next_client(
        marshrutka_t* bus,
        ucontext_t* next_context, 
        ucontext_t* other_context,
        struct client** current_client) {
    
    int i = 0;
    int count = bus->count_of_clnts;

    while(1){
        *current_client = &(bus->all_clnts[i]);
        i = (i + 1) % count;
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

    struct client* current_client;
    makecontext(&next_context, (void (*)(void))next_client,
            4, bus, &next_context, &toggle_context2, &current_client);


    getcontext(&toggle_context1);

    int i = 0;
    int k = 0;
    while(1) {
        swapcontext(&toggle_context2, &next_context);
        printf("iteration № %d, client id %d ", i, current_client->id);
        int result = empty(current_client);
        if(result) {
            printf("queue is empty\n");
        }
        else {
            printf("queue is not empty\n");
            handling(current_client);
        }
        i += 1;
        char buffer[10];
        memset(buffer, 0, 10);
        struct timeval tv;
        fd_set rfds;

    tv.tv_sec = 5;
    tv.tv_usec = 0;


        int sockfd = current_client->connection;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        
        /*FIXME здесь почему-то не по пять секунд ждет,
         * после того как один из пользователей отключился
         * начинаются проблемы*/
        
        write(sockfd, "generate an event 1 or 2:\n", 26);
        /*FIXME замени уже*/
        select(sockfd + 1, &rfds, NULL, NULL, &tv);
        
        if(FD_ISSET(sockfd, &rfds)) {
            int n = read(sockfd, buffer, 10);
            buffer[n] = 0;
        }
        
        int answer = atoi(buffer);
       /*это просто тестик 1 - генерация соседу в очередьб 2 - себе же*/ 
        if(answer == 1) {
            generate_event(&(bus->all_clnts[(k+1)%2]), answer, current_client->id);
        }
        if(answer == 2) {
            generate_event(current_client, answer, current_client->id);
        }
 //       sleep(1);
        
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


void* otpravit_marshrutku(void* arg) {	
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    broadcast(bus, "\nотправляемся\n"); 
    toggle(bus);
    vypnut_passazhirov(bus);
    scrabwoman(bus);
	return NULL;
}


void add_passanger(struct client* cl, marshrutka_t* bus) {
    
    cl->connection = new_client(bus->dvigatel);
    cl->ticket = 0;
    cl->id = bus->count_of_clnts;
    cl->first_event = NULL;
    cl->next_client = NULL;
    spawn_passazhir(bus, cl);
    bus->count_of_clnts++;
}

/*приветсвие и посдка происходит без корутин*/
void init_marshrutka(marshrutka_t* bus) {
    bus->count_of_clnts = 0;
    
    bus->first_client = calloc(1, sizeof( struct client ));
    add_passanger(bus->first_client, bus);
    bus->last_client = bus->first_client;

    for(int i = 1; i < MAX_CLIENTS; i++) {

        bus->last_client->next_client = calloc(1, sizeof( struct client ));
        bus->last_client = bus->last_client->next_client;
        add_passanger(bus->last_client, bus);
    }

    spawn_babka(bus);

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
