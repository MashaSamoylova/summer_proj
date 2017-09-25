#include "babka.h"
#include <stdio.h>
#include <stdlib.h>

void generate_open(struct client_t* Katya) {
   //нужно решить как генерировать события, если id пассажиров 0 1 2 , 1 уходит, то здесь с сегфолтом упадет через 
    int number = rand()%Katya->bus->n_passzh;
    printf("generate_open number %d id babi %d\n", number, Katya->id);
    generate_event(number, 1, Katya);
}

int babka_generator(struct client_t* Katya){
    
    //window is open
    if(Katya->bus->status_window) {
        generate_open(Katya);
    }
    else{
        //generate_close(Katya);
    }
    return 0;
}

int baba_handler(struct client_t* Katya) {
    
    struct babka *Klava = (struct babka*)Katya;
    
    while( !empty(Katya) ) {
        int code = Katya->first_event->code;
        printf("id %d\n", Katya->id);
        /*и здесь также добавить обработчики на события*/
        switch(code) {
            case 101:
                printf("event 101\n");
                break;
            case 102:
                printf("event 102\n");
                break;
            default:
                break;
        }
        delete_event(Katya);
        printf("event is handled\n");
    }

    
    printf("baba action %d\n", Katya->id);
    return 0;
}

void add_babka(marshrutka_t* bus) {
    struct babka *Katya = calloc(1, sizeof(struct babka));
    struct client_t *client = (struct client_t*)Katya;

    Katya->client.bus = bus;
    insert_client(client);

    Katya->hp = 100;
    Katya->client.handler = baba_handler;
    Katya->client.generator = babka_generator;
    bus->n_babok++;
}

void delete_babka(struct babka* Katya) {
    struct client_t *Klava = (struct client_t*)Katya;
    Klava->bus->n_babok--;
    delete_client(Klava);
}

/*void generate_close( struct babka* Katya, struct client* Ivan ) {
    generate_event(Ivan, 2, Katya->id);    
}

*/
