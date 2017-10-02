#include "client.h"

int clean_events(struct client_t *Ivan) {
    printf("чистка событий\n");
    struct event *arrow = Ivan->first_event;
    struct event *cup;

    while(arrow) {
        cup = arrow;
        arrow = arrow->next_event;
        free(cup);
    }
    return 0;
}

int empty(struct client_t *Ivan) {
    
    if(Ivan->first_event == NULL) {
        return 1;
    }
    return 0;
}

int delete_client(struct client_t * Ivan) {
    clean_events(Ivan);
    free(Ivan->context.uc_stack.ss_sp);
    Ivan->prev_client->next_client = Ivan->next_client;
    struct client_t* cup = Ivan->next_client;
    Ivan->bus->n_clients--;
    free(Ivan);
    setcontext(&cup->context);
    return 0;
}

void add_event(struct client_t *Ivan, struct event *sc) {
    
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

void delete_event(struct client_t *Ivan) {
    struct event* cup = Ivan->first_event;
    Ivan->first_event = Ivan->first_event->next_event;
    free(cup);
}

void generate_event(int number, char* name, struct client_t *Ivan) {
    
    struct client_t *arrow = Ivan->bus->first_client;
   
    //поиск клиента, которому хотят добавить событие
    while(number) {
        arrow = arrow->next_client;
        number--;
    }

    struct event *sc = calloc(1, sizeof(struct event));
    strcpy(sc->name, name);
    sc->ask_id = Ivan->id;
    sc->next_event = NULL;

    add_event(arrow, sc);
}

int main_handler(struct client_t * Ivan) {
    while( !empty(Ivan) ) {

        if(!strcmp(Ivan->first_event->name, "ask open window")) {
            ask_open(Ivan);
        }
        
        if(!strcmp(Ivan->first_event->name, "open window")) {
            open_window(Ivan);
        }
        
        delete_event(Ivan);
        printf("event is handled\n");

    }
}

int loop(struct client_t* Ivan) {
    while(1) {
        if(Ivan->bus->n_passzh == 0) {
            sleep(10);
            return 0;
        }
        printf("client id %d\n", Ivan->id);
        
        if(!empty(Ivan)) {
            main_handler(Ivan);
        }
        
        Ivan->generator(Ivan);
        swapcontext(&Ivan->context, &Ivan->next_client->context);
    }
}

int init_context(struct client_t *Ivan) {
    Ivan->context.uc_link = NULL;
    Ivan->context.uc_stack.ss_sp = calloc(SIGSTKSZ, sizeof(char));
    Ivan->context.uc_stack.ss_size = SIGSTKSZ * sizeof(char);

    getcontext(&Ivan->context);
    makecontext(&Ivan->context, (void (*)(void))loop,
        1, Ivan);
    return 0;

}

int insert_client(struct client_t* Ivan) {
    
    if(!Ivan->bus) {
        printf("клиент без автобуса\n");
        return -1;
    }

    Ivan->id = rand()%1000;
    printf("%d\n", Ivan->id);
    Ivan->first_event = NULL;
    Ivan->last_event = NULL;

    if(Ivan->bus->first_client == NULL) {
        Ivan->bus->first_client = Ivan; 
        Ivan->bus->last_client = Ivan;
    }

    Ivan->bus->last_client->next_client = Ivan;
    Ivan->prev_client = Ivan->bus->last_client;
    Ivan->bus->last_client = Ivan;
    Ivan->next_client = Ivan->bus->first_client;
   
    init_context(Ivan);
    Ivan->bus->n_clients++;
    return 0;
}

