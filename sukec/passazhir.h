#pragma once
#include "main.h"
#include "client.h"

/*отключение всех клиентов*/
void vypnut_passazhirov(marshrutka_t *bus) {
    struct client_t* arrow = bus->first_client;
    int k = 0;
    
    while(k < bus->n_clients) {
        struct passazhir *Tom = (struct passazhir*)arrow;
        close(Tom->ufds.fd);
        arrow = arrow->next_client;
        k++;
    }
}

int write_passazh(struct passazhir* Tom, char* message) {
    Tom->ufds.events = POLLOUT;
    int n = poll(&Tom->ufds, 1, 10000);

    if (n < 0) {
        return -1; //poll error
    }

    if (n == 0) {
        return 1;
    }

    if(Tom->ufds.revents == POLLOUT) {
        n = send(Tom->ufds.fd, message, strlen(message), 0);
        if(n <= 0) return -1;
        return n;
    }
    return -1;
}

void hello(struct passazhir* Tom) {
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
    write_passazh(Tom, p);
    fclose(bus_ascii);

    char msg[40] = "\nМАРШРУТКА № 442\n";
    write_passazh(Tom, msg);
    return;	
}

int read_answer(struct passazhir* Tom, char *buffer, int size) {
    Tom->ufds.events = POLLIN;
    int n = poll(&Tom->ufds, 1, 10000);
    if(n < 0) {
        return -1; //poll error
    }

    if(n == 0) {
        return 0; //cant read yet
    }

    if(Tom->ufds.revents == POLLIN) {
        n = recv(Tom->ufds. fd, buffer, size, 0);
        if(n <= 0) return -1;
        return n;
    }

    return -1;
}

int open_window(struct passazhir *Tom) {
    char buff[100] = "\0";
    write_passazh(Tom, "1.открыть окно\n2.ударить бабку\n");
    read_answer(Tom, buff, 10);
    printf("%s\n", buff);
    return 0;
}

int passazhir_handler(struct client_t* Ivan) {

    struct passazhir *Tom = (struct passazhir*)Ivan;
    
    while( !empty(Ivan) ) {
        int code = Ivan->first_event->code;
        printf("id %d\n", Ivan->id);
        /*здесь нужно добавить обработчики на события, 
         * типо для кода 1, 2 и тд*/
        switch(code) {
            case 1:
                open_window(Tom);
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

    return 0;
}

int passazhir_generator(struct client_t* Ivan) {
    struct passazhir *Tom = (struct passazhir*)Ivan;
    printf("generate TOM %d\n", Tom->client.id);
    return 0;
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

int add_passzhir(marshrutka_t* bus) {

    struct passazhir *Ivan = calloc(1, sizeof(struct passazhir));
    struct client_t* client = (struct client_t*)Ivan;

    Ivan->client.bus = bus;
    insert_client(client);
    
    Ivan->ufds.fd = new_client(bus->dvigatel);
    Ivan->client.handler = passazhir_handler;
    Ivan->client.generator = passazhir_generator;
    hello(Ivan);
    return 0;
}
