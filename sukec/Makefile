all: main.c marshrutka.h client.h babka.h 
	gcc -pthread main.c -o main -ggdb -g3 -Wall -Wextra -std=gnu11 

client: client.h client.c
	gcc -c client.c

babka: babka.h babka.c
	gcc -c babka.c
