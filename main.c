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

int new_client(int socket_desc) {

		listen(socket_desc, 5);

		struct sockaddr_in cli_addr;
		int newsockfd;
		socklen_t client_ss;

		client_ss = sizeof(cli_addr);
		newsockfd = accept(socket_desc, (struct sockaddr *) &cli_addr, &client_ss);
		if (newsockfd < 0) {
			printf("ERROR on accept\n");
			exit(1);
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

	pthread_t id_of_threds[3];

	int socket_desc, n;
	int sockfd, newsockfd;
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
		
		int err = pthread_create(&(id_of_threds[all_clnts[i].id]), NULL, &hello, all_clnts[i].connection);

		count_of_clnts++;
	}

	for(i = 0; i < count_of_clnts; i++) {
		pthread_join(id_of_threds[i], NULL);
	}

	broad_cast("\nОтправляемся\nВаше слово:\n");

	
	close(newsockfd);
	close(sockfd);

	return 0;
}
