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

#include "main.h"

#include "passazhir.h"
#include "marshrutka.h"
/*
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

*/
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

    marshrutka_t* avtobus_442 = calloc(1, sizeof(marshrutka_t));
    avtobus_442->dvigatel = dvigatel;
    init_marshrutka(avtobus_442, (int)density);
    
    for(int i = 0; i < MAX_CLIENTS; i++) {
        add_passzhir(avtobus_442);
    }

    loop(avtobus_442->first_client);
  /*  pthread_t id;
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
*/
    return 0;
}
