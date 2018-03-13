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
#include <search.h>

#include "main.h"

#include "passazhir.h"
#include "babka.h"
#include "marshrutka.h"

static char *actions[] = {
    "ask open window",
    "open window"
};

static int (*handlers[])(struct client_t*) = {
    ask_open,
    open_window,
};

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
    }
    
    for(int i = 0; i < server->density; i++) {
        add_babka(avtobus_442);
    }

    pthread_mutex_unlock(&server->stop);
    otpravit_marshrutku(avtobus_442);
    printf("автобус вернулся в парк\n");
    vypnut_passazhirov(avtobus_442);
    scrabwoman(avtobus_442);

    return 0;
}


int create_table() {
    int size = sizeof(actions)/sizeof(char*) + 1;
    
    ENTRY e, *ep;
    hcreate(size+30);

    for(int i = 0; i < size; i++) {
        e.key = actions[i];
        e.data = (void*)handlers[i];
        ep = hsearch(e, ENTER);
        if( ep == NULL) {
            fprintf(stderr, "entry failed\n");
            return -1;
        }
    }
    return 0;
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
    
    if( create_table() == -1) {
        return 0;
    };
    
    return dvigatel;
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
