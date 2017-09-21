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
#include "babka.h"
#include "marshrutka.h"

int init_marshrutka(struct server_t *server) {
    marshrutka_t* avtobus_442 = calloc(1, sizeof(marshrutka_t));
    avtobus_442->dvigatel = server->dvigatel;
    
    avtobus_442->first_client = NULL;
    avtobus_442->last_client = NULL;
    avtobus_442->n_passzh = 0;
    avtobus_442->n_clients = 0;
    avtobus_442->n_babok = 0;
    avtobus_442->status_window = 1;

    for(int i = 0; i < MAX_CLIENTS; i++) {
        add_passzhir(avtobus_442);
        avtobus_442->n_passzh++;
    }
    
    for(int i = 0; i < server->density; i++) {
        add_babka(avtobus_442);
        avtobus_442->n_babok++;
    }

    avtobus_442->n_clients = avtobus_442->n_passzh + avtobus_442->n_babok;
   
    pthread_mutex_unlock(&server->stop);
    otpravit_marshrutku(avtobus_442);
    printf("автобус вернулся в парк\n");
    vypnut_passazhirov(avtobus_442);
    scrabwoman(avtobus_442);

    return 0;
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

    struct server_t *server = malloc(sizeof( struct server_t ));
    if( server == NULL ) {
        printf("Failed to alloc server\n");
        exit(-1);
    }

    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    
    server->dvigatel = dvigatel;
    server->density = density;
    server->thread_counter = 0;

    while(442) {
        if(server->thread_counter >= MAX_THREADS) {
            printf("no bases in the garage\n");
        }

        else {
            pthread_mutex_lock(&server->stop);
            pthread_t thread;
            pthread_create(&thread, &thread_attr, (void *(*)(void*))&init_marshrutka, server);
            server->thread_counter++;
        }
    }
    return 0;
}
