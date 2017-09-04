#ifndef _CONFIG

#define _CONFIG

#define MAX_CLIENTS 4 
#define MAX_PASSAZHIR 4
#define MAX_SEATS   11
#define MAX_THREADS 10
#define MAX_ROLES   50
#define MAX_STOPS   10
#define MAX_LENGTH  100 //длни названия остановки
#define PORT        1234

#endif

#include <ucontext.h>
ucontext_t main_context, next_context; 

