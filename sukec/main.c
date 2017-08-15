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
#include "main.h"
#include "config.h"
#include <ucontext.h>

void broadcast(marshrutka_t *bus, char* message) {

    int i;
    for(i = 0; i < bus->count_of_clnts; i++) {
        write(bus->all_clnts[i].connection, message, strlen(message));
    }
}

void sit_down(marshrutka_t *bus, int i) {
    int st;
    do {
        st = rand() % MAX_SEATS;
    }while(bus->seats[st]); 
    bus->seats[st] = 1;
    bus->all_clnts[i].seat = st;
}

void* hello(marshrutka_t *bus, int i) {
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
    write(bus->all_clnts[i].connection, p, strlen(p));
    fclose(bus_ascii);

    char msg[40] = "\nМАРШРУТКА № 442\n";
    write(bus->all_clnts[i].connection, msg, strlen(msg));
    return NULL;	
}

/*приветсвие и посадка*/
void* spawn_passazhir(void *arg) {
    marshrutka_t *bus = ((thread_arg*)arg)->bus;
    int i = ((thread_arg*)arg)->i;

    hello(bus, i);
    sit_down(bus, i);

	char msg[40];
    sprintf(msg, "Ты выбрал место: %d\n", bus->all_clnts[i].seat);
    write(bus->all_clnts[i].connection, msg, strlen(msg));

    return NULL;
}

/*flag - 1 ответ от пользователя считан
 * flag - 0 иначе*/
void read_answer(
        marshrutka_t* bus,
        ucontext_t *read_context, 
        ucontext_t *other_context, 
        int *flag) 
{
    
    char buffer[10];
    memset(buffer, 0, 10);
    
    struct timeval tv;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    int i = 0;
    fd_set rfds;
    int count = bus->count_of_clnts;
    int sockfd;
    
    while(1) {
        sockfd = bus->all_clnts[i].connection;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        *flag = 0;
        
        /*FIXME здесь почему-то не по пять секунд ждет,
         * после того как один из пользователей отключился
         * начинаются проблемы*/
        
        int result = select(sockfd + 1, &rfds, NULL, NULL, &tv);
        
        if(FD_ISSET(sockfd, &rfds)) {
            *flag = 1;
            int n = read(sockfd, buffer, 10);
            buffer[n] = 0;
            printf("%d : %s", i, buffer);
        }

        i = (i + 1) % count;
        swapcontext(read_context, other_context);
    }
}

int passanger(marshrutka_t* bus) {
    ucontext_t main_context1, main_context2, read_context;
    volatile int flag;
    
    char stack[SIGSTKSZ];

    getcontext(&read_context);

    read_context.uc_link = &main_context1;
    read_context.uc_stack.ss_sp = stack;
    read_context.uc_stack.ss_size = sizeof(stack);

    makecontext(&read_context, (void (*)(void))read_answer, 
            4, bus, &read_context, &main_context2, &flag);


    getcontext(&main_context1);


    while (1) {
        swapcontext(&main_context2, &read_context);
        if(flag) {
            /*здесь обработка того, что написал*/
            printf("**********************\n");
        }
    }
    return 0;
}

int new_client(int dvigatel) {
    listen(dvigatel, 5);

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
    return dvigatel;
}


void* otpravit_marshrutku(void* arg) {	
	marshrutka_t *bus = ((thread_arg*)arg)->bus;
    broadcast(bus, "\nотправляемся\n"); 
    passanger(bus);
	return NULL;
}


/*приветсвие и посдка происходит без корутин*/
void init_marshrutka(marshrutka_t* bus) {

    thread_arg* tas = malloc( MAX_CLIENTS*sizeof(thread_arg) );

    bus->count_of_clnts = 0;
    for(int i = 0; i < MAX_CLIENTS; i++) {
        bus->all_clnts[i].connection = new_client(bus->dvigatel);
        bus->all_clnts[i].ticket = 0;
        bus->all_clnts[i].id = i;
        tas[i].bus = bus;
        tas[i].i = i;

		spawn_passazhir(tas+i);

        bus->count_of_clnts++;
    }
    return;	
}

int main() {
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
		init_marshrutka(avtobus_442);
      
       ta.bus = avtobus_442;
	   //FIXME next bus will not go before the last is will not come	
	    pthread_create(&id, NULL, &otpravit_marshrutku, &ta);
  //	    pthread_join(id, NULL);
    //    printf("ДЖОИН\n");
    }

    return 0;
}
