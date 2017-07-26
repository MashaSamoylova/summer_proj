all: main.c client.h
	gcc -pthread main.c -o main
