#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int hello() {
	FILE *bus = fopen("bus", "r");
	if(bus == NULL) {
		printf("not found the bus:(\n");
		exit(1);
	}
	char p;
	while( (p = fgetc(bus)) != EOF) {
		printf("%c", p);
	}
	fclose(bus);

	printf("МАРШРУТКА № 8\n");
}

int main(int argc, char *argv[]) {
	int socket_desc, n;
	int sockfd, newsockfd;
	socklen_t client;
	char buffer[256];
	
	struct sockaddr_in server, cli_addr;
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);

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

	listen(socket_desc, 5);

	client = sizeof(cli_addr);
    newsockfd = accept(socket_desc, (struct sockaddr *) &cli_addr, &client);
	if (newsockfd < 0) {
		printf("ERROR on accept\n");
		exit(1);
	}
		       
	bzero(buffer,256);
	n = read(newsockfd,buffer,255);
	
	if (n < 0) {
		printf("ERROR reading from socket\n");
		exit(1);
	}
	
	printf("Here is the message: %s\n",buffer);
	
	n = write(newsockfd,"I got your message",18);
	
	if (n < 0) {
		printf("ERROR writing to socket\n");
		exit(1);
	}
	
	close(newsockfd);
	close(sockfd);

	return 0;
}

