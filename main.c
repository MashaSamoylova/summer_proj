#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // for sleep
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "client.h"

client all_clnts[10];
int count_of_clnts = 0;
pthread_t id_of_threds[3];

pthread_mutex_t mutex;

void threads_are_terminated() {

	int i;	
	for(i = 0; i < count_of_clnts; i++) {
		pthread_join(id_of_threds[i], NULL);
	}

}

void disconnect_all_clients() {
	
	int i;
	for(i = 0; i < count_of_clnts; i++) {
		close(all_clnts[i].connection);
	}
}

int hello(int newsockfd) {
	FILE *bus = fopen("bus", "r");
	char buff[256];
	if(bus == NULL) {
		printf("not found the bus:(\n");
		exit(1);
	}

	
	char p;
	int n = 7;
	while( 0 <= n) {
		fgets(buff, 255, bus);
		write(newsockfd, buff, strlen(buff));
		n--;
	}
	fclose(bus);

	write(newsockfd, "\nМАРШРУТКА № 8\n", 27);

}

void menu(int sockfd) {
	
	write(sockfd, "1. Чатик\n2. Заплатить за проезд\n3. Пересесть\n4. Выйти на следующей остановке\n", 133);
}

void read_answer(int sockfd) {
	
	menu(sockfd);
	char buffer[10];
	memset(buffer, 0, 10);
	read(sockfd, buffer, 10); //наверно здесь нужно два, а не 10
	write(sockfd, "Ты написал:\n", 21);
	write(sockfd, buffer, 10);
	printf("%s\n", buffer);

}

int new_client(int socket_desc) {

	listen(socket_desc, 5);

	struct sockaddr_in cli_addr;
	int newsockfd;
	socklen_t client_ss;

	client_ss = sizeof(cli_addr);
	newsockfd = accept(socket_desc, (struct sockaddr *) &cli_addr, &client_ss);
	if (newsockfd < 0) {
		printf("ERROR on accept\n");
	}
	return newsockfd;
}


int broad_cast(char* message) {

	int i;
	for(i = 0; i < count_of_clnts; i++) {
		write(all_clnts[i].connection, message, strlen(message));
	}

}

int main(int argc, char *argv[]) {

	pthread_mutex_init(&mutex, NULL);

	int socket_desc, n;
	int sockfd;
	socklen_t client_ss;
	char buffer[256];
	
	struct sockaddr_in server, cli_addr;
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

	int yes = 1;
	setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	if (socket_desc == -1) {
		printf("Could not create socket\n");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(1234);

	if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printf("ERROR on binding\n");
		exit(1);
	}

	int i;
	
	for(i = 0; i < 3; i++) {
		all_clnts[i].connection = new_client(socket_desc);
		all_clnts[i].seat = rand();
		all_clnts[i].ticket[0] = '\0';
		all_clnts[i].id = i;
		
		int err = pthread_create( &(id_of_threds[all_clnts[i].id]), NULL, &hello, all_clnts[i].connection);

		count_of_clnts++;
	}

	threads_are_terminated();

	broad_cast("\nОтправляемся\n");

	for(i = 0; i < count_of_clnts; i++) {
		pthread_create( &(id_of_threds[all_clnts[i].id]), NULL, &read_answer, all_clnts[i].connection);
	}

	threads_are_terminated();
	disconnect_all_clients();

	
	close(sockfd);

	return 0;
}

