#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

int hello() {
	FILE *bus = fopen("bus", "r");
	if(NULL == bus) {
		printf("not found the bus:(\n");
		exit(12312);
	}
	char p;
	while( (p = fgetc(bus)) != EOF) {
		printf("%c", p);
	}
	fclose(bus);

	printf("МАРШРУТКА № 8\n");
}

int main() {
	hello();
}
