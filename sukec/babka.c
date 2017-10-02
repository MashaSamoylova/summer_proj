#include "babka.h"

void ask_open(struct client_t* Katya) {
   //нужно решить как генерировать события, если id пассажиров 0 1 2 , 1 уходит, то здесь с сегфолтом упадет через 
    int number = rand()%Katya->bus->n_passzh;
    printf("generate_open number %d id babi %d\n", number, Katya->id);
    generate_event(number, "open window", Katya);
}

void add_babka(marshrutka_t* bus) {
    struct babka *Katya = calloc(1, sizeof(struct babka));
    struct client_t *client = (struct client_t*)Katya;

    Katya->client.bus = bus;
    insert_client(client);

    Katya->hp = 100;
    strcpy(Katya->client.role, "babka");
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
