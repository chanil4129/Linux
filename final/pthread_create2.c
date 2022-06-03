#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 8

struct thread_data{
	int thread_index;
	int sum;
	char *message;
};

void *ssu_printhello(void *argv);

struct thread_data thread_data_array[THREAD_NUM];
char *messages[THREAD_NUM];

int main(void){
	pthread_t tid[THREAD_NUM];
	int sum;
	int i;

	sum=0;
	messages[0]="English: hello World!";
	messages[1]="French: hello World!";
	messages[2]="Spanish: hello World!";
	messages[3]="Klingon: hello World!";
	messages[4]="German: hello World!";
	messages[5]="Russia: hello World!";
	messages[6]="Japan: hello World!";
	messages[7]="Latin: hello World!";

	for(i=0;i<THREAD_NUM;i++){
		sum=sum+i;
		thread_data_array[i].thread_index=i;
		thread_data_array[i].sum=sum;
		thread_data_array[i].message=messages[i];
		printf("create thread %d\n",i);

		if(pthread_create(&tid[i],NULL,ssu_printhello,(void *)&thread_data_array[i])!=0){
			fprintf(stderr,"pthread_create error\n");
			exit(1);
		}
	}
	sleep(5);
	exit(0);
}

void *ssu_printhello(void *arg){
	struct thread_data *data;
	char *message;
	int thread_index;
	int sum;

	sleep(1);
	data=(struct thread_data *)arg;
	thread_index=data->thread_index;
	sum=data->sum;
	message=data->message;
	printf("Thread %d:%s   Sum=%d\n",thread_index,message,sum);
	return NULL;
}
