#pragma once
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

#include "main.h"


int clean_events(struct client_t *Ivan);
int empty(struct client_t *Ivan);
int delete_client(struct client_t * Ivan);
void add_event(struct client_t *Ivan, struct event *sc);
void delete_event(struct client_t *Ivan);
void generate_event(int number, char* name, struct client_t *Ivan);
int loop(struct client_t* Ivan);
int init_context(struct client_t *Ivan);
int insert_client(struct client_t* Ivan);
