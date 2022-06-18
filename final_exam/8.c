#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int value;
void *ssu_thread(void *param);

int main(int argc, char *argv[]){
	pthread_t pid;
	int param;
	int status;

	param=atoi(argv[1]);
	pthread_create(&pid,NULL,ssu_thread,(void *)&param);
	pthread_join(pid,(void *)&status);

	printf("I am the parent: Square = %d\n",value);
}

void *ssu_thread(void *param){
	int *t;

	t=((int *)param);
	value=*t;
	printf("I am the child, passed value %d\n",value);
	value=value*value;
	return NULL;
}

