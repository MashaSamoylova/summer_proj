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
#include <fcntl.h>

#include "marshrutka.h"

void baba_handler(marshrutka_t* bus, struct babka* Katya, struct client* Ivan) {
  
    while(1) {
        printf("baba id %d\n", Katya->id);
        swapcontext(&Katya->context, &Katya->next_babka->context);
        swapcontext(&Katya->context, &Ivan->context);
    }
}

void handler(marshrutka_t* bus, struct client* Ivan, struct babka* Katya) {
    
    while(1) {
        printf("client id %d\n", Ivan->id);
        swapcontext(&Ivan->context, &Ivan->next_client->context);
        sleep(0.3);
    }
}

void* otpravit_marshrutku(void* arg) {
    getcontext(&main_context);
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    broadcast(bus, "\nотправляемся\n"); 
    handler(bus, bus->first_client, bus->first_babka);
    vypnut_passazhirov(bus);
    scrabwoman(bus);
	return NULL;
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

void add_babka(struct babka* Katya, int id, marshrutka_t* bus) {
    Katya->next_babka = NULL;
    Katya->id = id;
    Katya->hp = 100;
    
    char* stack = calloc( SIGSTKSZ, sizeof(char) );

    Katya->context.uc_link = &main_context;
    Katya->context.uc_stack.ss_sp = stack;
    Katya->context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

    getcontext(&Katya->context);
    makecontext(&Katya->context, (void (*)(void))baba_handler,
        3, bus, Katya, bus->first_client);
}

void spawn_babki(marshrutka_t* bus, int density) {

    bus->first_babka = calloc(1, sizeof( struct babka ));
    add_babka(bus->first_babka, density, bus);
    bus->last_babka = bus->first_babka;
    density--;

    while(density) {
        bus->last_babka->next_babka = calloc(1, sizeof(struct babka));
        bus->last_babka = bus->last_babka->next_babka;
        add_babka(bus->last_babka, density, bus);
        density--;
    }

    bus->last_babka->next_babka = bus->first_babka;
}

void add_passanger(struct client* Ivan, marshrutka_t* bus) {
    
    Ivan->connection = new_client(bus->dvigatel);
    Ivan->ticket = 0;
    Ivan->id = bus->count_of_clnts;
    Ivan->first_event = NULL;
    Ivan->next_client = NULL;
    
    hello(Ivan);
    bus->count_of_clnts++;
}

void init_contexts(marshrutka_t* bus) {
    printf("инициализация контекстов\n");
    struct client* Ivan = bus->first_client;
    do {
        printf("huy\n");
        Ivan->context.uc_link = &main_context;
        Ivan->context.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
        Ivan->context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

        getcontext(&Ivan->context);
        makecontext(&Ivan->context, (void (*)(void))handler,
            3, bus, Ivan, bus->first_babka);

        Ivan->read_context.uc_link = &main_context;
        Ivan->read_context.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
        Ivan->read_context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

        getcontext(&Ivan->read_context);
        makecontext(&Ivan->read_context, (void (*)(void))read_answer,
                1, Ivan); 

        Ivan = Ivan->next_client;
    } while(Ivan != bus->first_client);
    printf("инициализация контекстов окончена\n");

}
/*приветсвие и посдка происходит без корутин*/
void init_marshrutka(marshrutka_t* bus, int density) {
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

    bus->last_client->next_client = bus->first_client;
    spawn_babki(bus, density);
    init_contexts(bus);
    return;	
}

int main(int argc, char* argv[]) {
     setbuf ( stdout , NULL ); 

    if(argc < 2) {
        printf("please, use ./main <density of babok>\n");
        exit(1);
    }
    
    char* p;
    long density = strtol(argv[1], &p, 10);
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
		init_marshrutka(avtobus_442, (int)density);
      
        ta.bus = avtobus_442;
	   //FIXME next bus will not go before the last is will not come	
	    pthread_create(&id, NULL, &otpravit_marshrutku, &ta);
  	  //  pthread_join(id, NULL);
        //printf("ДЖОИН\n");
    }

    return 0;
}
