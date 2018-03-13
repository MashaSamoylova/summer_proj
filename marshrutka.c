#include "marshrutka.h"

//*очистка памяти*/
void scrabwoman(marshrutka_t* bus) {

    int k = 0;
    struct client_t* arrow = bus->first_client;
    struct client_t* cup;
    while(k < bus->n_clients) {
        cup = arrow;
        clean_events(arrow);
        free(arrow->context.uc_stack.ss_sp);
        arrow = arrow->next_client;
        free(cup);
        k++;
    }
    free(bus);
}

/*void broadcast(marshrutka_t *bus, char* message) {
    struct client* arrow = bus->first_client;

    do {
        write(arrow->connection, message, strlen(message));
        arrow = arrow->next_client;
    } while(arrow != bus->first_client);

}*/

void* otpravit_marshrutku(marshrutka_t *bus) {
    printf("поееехааали\n");
    loop(bus->first_client);
	return NULL;
}

