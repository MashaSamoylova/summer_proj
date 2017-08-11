#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "main.h"
#include "config.h"

void dozhdatsya_passazhirov(marshrutka_t *bus) {
    for(int i = 0; i < bus->count_of_clnts; i++) {
        pthread_join(bus->id_of_threads[i], NULL);
    }
}

void vypnut_passazhirov(marshrutka_t *bus) {
    for(int i = 0; i < bus->count_of_clnts; i++) {
        close(bus->all_clnts[i].connection);
    }
}

void* sit_down(void *arg) {
    marshrutka_t *bus = ((thread_arg*)arg)->bus;
    int i = ((thread_arg*)arg)->i;

    int st;
    pthread_mutex_lock(&bus->mutex_seat);
    do {
        st = rand() % MAX_SEATS;
    }while(bus->seats[st]); 
    bus->seats[st] = 1;
    bus->all_clnts[i].seat = st;
    pthread_mutex_unlock(&bus->mutex_seat);
}

void* hello(void *arg) {
    marshrutka_t *bus = ((thread_arg*)arg)->bus;
    int i = ((thread_arg*)arg)->i;

    char buff[256];
    char p[1000];
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

    write(bus->all_clnts[i].connection, "\nМАРШРУТКА № 442\n", 28);
    char msg[40];
    sprintf(msg, "Ты выбрал место: %d\n", bus->all_clnts[i].seat);
    write(bus->all_clnts[i].connection, msg, 32);
}

int read_answer(int sockfd) {

    char buffer[10];
    memset(buffer, 0, 10);
    read(sockfd, buffer, 10); 
	
	return atoi(buffer);
}

void seat_layout(marshrutka_t *bus, int sockfd) {
    int k = 0;
	char msg[50];
	char part2[25];
	write(sockfd, "\nПОЗИЦИЯ ПАССАЖИРОВ:\n", 39);
    while(k < MAX_SEATS) { //тут 10 для того, чтобы красиво выводилось))) не надо исправлять
        if(k < 10) {
            sprintf(msg, "  %d,%d) %d  %d       ", k, k+1, bus->seats[k], bus->seats[k+1]);
        }
        else {
            sprintf(msg, "%d,%d) %d  %d       ", k, k+1, bus->seats[k], bus->seats[k+1]);
        }
		k+=2;
        if(k < 10) {
            sprintf(part2, "  %d,%d) %d  %d\n", k, k+1, bus->seats[k], bus->seats[k+1]);
			strncat(msg, part2, strlen(part2));
        }
        else {
            sprintf(part2, "%d,%d) %d  %d\n", k, k+1, bus->seats[k], bus->seats[k+1]);
			strncat(msg, part2, strlen(part2));
        }
		write(sockfd, msg, strlen(msg));
        k+=2;
    }
}

void take_seat(marshrutka_t *bus, int i) {
	
	int sockfd = bus->all_clnts[i].connection;	

	seat_layout(bus, sockfd);
	char msg[100];
	int client_seat = bus->all_clnts[i].seat;
	sprintf(msg, "ты сидишь на %d, куда ты хочешь пересесть?\n", client_seat);
	write(sockfd, msg, strlen(msg));
	int answer = read_answer(sockfd);
	if(0 < answer && answer < MAX_SEATS) {
		pthread_mutex_lock(&bus->mutex_seat);
		
		if(bus->seats[answer]) {
			write(sockfd, "занято\n", 17);
		}
		else {
			bus->seats[answer] = 1;
			bus->seats[client_seat] = 0;
		}
		pthread_mutex_unlock(&bus->mutex_seat);
	}
	else  
		write(sockfd, "нет\n", 7);

}

void* menu(void* arg) {

	while(442) {
		marshrutka_t *bus = ((thread_arg*)arg)->bus;
		int k = ((thread_arg*)arg)->i;
		int sockfd = bus->all_clnts[k].connection;

		write(sockfd, "1. Чатик\n2. Заплатить за проезд\n3. Пересесть\n4. Выйти на следующей остановке\n", 133);
		int answer = read_answer(sockfd);

		switch(answer){
			case 1:
				break;
			case 2:
				break;
			case 3:
				take_seat(bus, k);
				break;
			case 4: 
				break;
			default:
				write(sockfd, "Ты несешь какаю-то дичь\n", 43);
		}
	}
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

void broadcast(marshrutka_t *bus, char* message) {
    int i;
    for(i = 0; i < bus->count_of_clnts; i++) {
        write(bus->all_clnts[i].connection, message, strlen(message));
    }
}

int zavesti_marshrutku(marshrutka_t *bus) {

#ifdef PRAVILNIE_ZAPCHASTI
    int result = 0;
#else
    int result = 1;
#endif

    memset(bus, 0, sizeof(marshrutka_t));
    pthread_mutex_init(&bus->mutex_seat, NULL);

    socklen_t client_ss;
    
    struct sockaddr_in server, cli_addr;
    bus->dvigatel = socket(AF_INET, SOCK_STREAM, 0);

    if (bus->dvigatel == -1) {
        fprintf(stderr, "Could not create socket\n");
        result = 0;
        goto errout;
    }

    int yes = 1;
    setsockopt(bus->dvigatel, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(bus->dvigatel, (struct sockaddr *) &server, sizeof(server)) < 0) {
        fprintf(stderr, "ERROR on binding\n");
        result = 0;
        goto errout;
    }

errout:
    if ( &bus->mutex_seat)
        pthread_mutex_destroy(&bus->mutex_seat);

    return result;
}

int main(int argc, char *argv[]) {

    marshrutka_t avtobus_442;
    while (!zavesti_marshrutku(&avtobus_442)) {
        char nuegonahuy;
        puts("Тыр-тыр-тыр-тыр... Не завелась! Может быть, ну его?..");
        while ((nuegonahuy=getchar()) != '\n')
            if (nuegonahuy == 'y')
                exit(-1);
    }

    while(442) {
        avtobus_442.count_of_clnts = 0;
        memset(avtobus_442.all_clnts, 0, sizeof(client) * MAX_CLIENTS);
        memset(avtobus_442.seats, 0, sizeof(int) * MAX_SEATS);

        thread_arg tas[MAX_CLIENTS];
        for(int i = 0; i < MAX_CLIENTS; i++) {
            avtobus_442.all_clnts[i].connection = new_client(avtobus_442.dvigatel);
            avtobus_442.all_clnts[i].ticket = 0;
            avtobus_442.all_clnts[i].id = i;
            tas[i].bus = &avtobus_442;
            tas[i].i = i;

            pthread_create( &(avtobus_442.id_of_threads[avtobus_442.all_clnts[i].id]), NULL, &sit_down, tas+i);
            pthread_create( &(avtobus_442.id_of_threads[avtobus_442.all_clnts[i].id]), NULL, &hello, tas+i);

            avtobus_442.count_of_clnts++;
        }

        dozhdatsya_passazhirov(&avtobus_442);

        broadcast(&avtobus_442, "\nОтправляемся\n");

        for(int i = 0; i < avtobus_442.count_of_clnts; i++) {
            tas[i].i = i;
            pthread_create( &(avtobus_442.id_of_threads[avtobus_442.all_clnts[i].id]), NULL, &menu, tas+i);
        }

        dozhdatsya_passazhirov(&avtobus_442);
        vypnut_passazhirov(&avtobus_442);
    }

    return 0;
}
