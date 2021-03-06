#pragma once
#include <ucontext.h>
#include <poll.h>
#include "config.h"
#include <pthread.h>

typedef struct {
    struct client_t* first_client;
    struct client_t* last_client;
    int n_clients;
    int n_passzh;
    int n_babok;
    int dvigatel;
    int status_window; //1 is open, 0 is close
} marshrutka_t;


struct event {
    int ask_id;
    struct event* next_event;
    char name[10];
};

//TODO add a ticket/travel card
struct client_t {
    marshrutka_t *bus;
    struct client_t *next_client;
    struct client_t *prev_client;
    int id;
    struct event *first_event;
    struct event *last_event;    
    char role[20];
    int (*handler) (struct client_t*);
    int (*generator) (struct client_t*);
    ucontext_t context;
 };

struct passazhir {
    struct client_t client;
    struct pollfd ufds;
    int connection;
};

struct babka {
    struct client_t client;
    int hp;
};

struct server_t {
    int dvigatel;
    int density;
    int thread_counter;
    pthread_mutex_t lock; //для переменной thread_counter
    pthread_mutex_t stop; //только одна маршрутка может быть припаркована к остановке
};

