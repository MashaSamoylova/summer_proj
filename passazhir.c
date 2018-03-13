#include "passazhir.h"

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

void delete_passazh(struct passazhir* Tom) {
    struct client_t *Ivan = (struct client_t*)Tom;
    Ivan->bus->n_passzh--;
    delete_client(Ivan);
}

int detect_disconnect(struct passazhir* Tom) {
    Tom->ufds.events = POLLOUT;
    return poll(&Tom->ufds, 1, 10000);
}

int try_write(struct passazhir* Tom, char* message) {
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

int write_passazh(struct passazhir *Tom, char* message) {
    int k = 3;
    while(k > 0) {
        if(try_write(Tom, message) != -1)
            return 0;
       k--;
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

int try_read_answer(struct passazhir* Tom, char *buffer, int size) {
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

int read_answer(struct passazhir* Tom, char *buffer, int size) {
    int k = 3;
    while(k > 0) { 
    if(try_read_answer(Tom, buffer, size) != -1)
            return 0;
       k--;
    }
    return -1; 
}

int open_window(struct client_t *Ivan) {
    struct passazhir *Tom = (struct passazhir*)Ivan;
    char buff[100] = "\0";
    write_passazh(Tom, "1.открыть окно\n2.ударить бабку\n");
    read_answer(Tom, buff, 10);
    printf("%s\n", buff);
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
    strcpy(Ivan->client.role, "passazhir");
    bus->n_passzh++;
    hello(Ivan);
    return 0;
}
