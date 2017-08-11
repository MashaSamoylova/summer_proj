all: main.c main.h
	gcc -pthread main.c -o main -ggdb -g3 -Wall -Wextra -std=c99
