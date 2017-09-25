all: main.c marshrutka.h marshrutka.o client.h client.o babka.h babka.o passazhir.h passazhir.o main.h 
	gcc -pthread main.c -o main -ggdb -g3 -Wall -Wextra -std=gnu11 marshrutka.o client.o babka.o passazhir.o

client: client.h client.c main.h
	gcc -c client.c

babka: babka.h babka.c client.h main.h 
	gcc -c babka.c

passazhir: passazhir.h passazhir.c client.h main.h
	gcc -c passazhir.c

marshrutka: marshrutka.h marshrutka.c main.h
	gcc -c marshrutka.c

clean: 
	rm main; rm *.o
