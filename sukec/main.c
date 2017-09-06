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

void gears(marshrutka_t* bus) {
    struct client* Ivan = bus->first_client;
    struct babka* Katya = bus->first_babka;

    while(1) {
        swapcontext(&bus->toggle, &Ivan->context);
        swapcontext(&bus->toggle, &Katya->context);

        Ivan = Ivan->next_client;
        Katya = Katya->next_babka;
        sleep(0.5);
    }
    
}

void baba_handler(marshrutka_t* bus, struct babka* Katya) {
    
    while(1) {
        printf("baba id %d\n", Katya->id);
        swapcontext(&Katya->context, &bus->toggle);
    }
}

void handler(marshrutka_t* bus, struct client* Ivan) {
    
    while(1) {
        printf("client id %d\n", Ivan->id);
        swapcontext(&Ivan->context, &bus->toggle);
    }
}

void* otpravit_marshrutku(void* arg) {
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    broadcast(bus, "\nотправляемся\n"); 
    gears(bus);
    vypnut_passazhirov(bus);
    scrabwoman(bus);
	return NULL;
}

void init_contexts(marshrutka_t* bus) {
    printf("инициализация контекстов\n");
    
    struct client* Ivan = bus->first_client;
    do {
        Ivan->context.uc_link = &main_context;
        Ivan->context.uc_stack.ss_sp = calloc(2*SIGSTKSZ, sizeof(char));
        Ivan->context.uc_stack.ss_size = 2*SIGSTKSZ * sizeof(char);

        getcontext(&Ivan->context);
        makecontext(&Ivan->context, (void (*)(void))handler,
            2, bus, Ivan);

        Ivan->read_context.uc_link = &main_context;
        Ivan->read_context.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
        Ivan->read_context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

        getcontext(&Ivan->read_context);
        makecontext(&Ivan->read_context, (void (*)(void))read_answer,
                1, Ivan); 

        Ivan = Ivan->next_client;
    } while(Ivan != bus->first_client);

    struct babka* Katya = bus->first_babka;
    do {
        Katya->context.uc_link = &main_context;
        Katya->context.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
        Katya->context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

        getcontext(&Katya->context);
        makecontext(&Katya->context, (void (*)(void))baba_handler,
            2, bus, Katya);
        Katya = Katya->next_babka;
    } while(Katya != bus->first_babka);

    bus->toggle.uc_link = &main_context;
    bus->toggle.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
    bus->toggle.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

    getcontext(&bus->toggle);
    makecontext(&bus->toggle, (void (*)(void))gears,
            1, bus);

    printf("инициализация контекстов окончена\n");

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
        init_contexts(avtobus_442);
      
        ta.bus = avtobus_442;
	   //FIXME next bus will not go before the last is will not come	
	    pthread_create(&id, NULL, &otpravit_marshrutku, &ta);
  	  //  pthread_join(id, NULL);
        //printf("ДЖОИН\n");
    }

    return 0;
}
