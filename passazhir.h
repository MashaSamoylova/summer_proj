#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "main.h"
#include "client.h"

void vypnut_passazhirov(marshrutka_t *bus);
void delete_passazh(struct passazhir* Tom);
int detect_disconnect(struct passazhir* Tom);
int try_write(struct passazhir* Tom, char* message);
int write_passazh(struct passazhir *Tom, char* message);
void hello(struct passazhir* Tom);
int try_read_answer(struct passazhir* Tom, char *buffer, int size);
int read_answer(struct passazhir* Tom, char *buffer, int size);
int new_client(int dvigatel);
int add_passzhir(marshrutka_t* bus);


//возможные события у персонажа
int open_window(struct client_t *Ivan);

