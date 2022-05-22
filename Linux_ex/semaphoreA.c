#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#define MAX_RETRIES 8

union ssu_sema_undef{
	struct semid_ds *buf;
	ushort *array;
	int val;
};

int ssu_initsema(key_t key, int sema_nums);

int main(void){
	struct sembuf sema_buf;
	key_t key;
	int sema_id;

	sema_buf.sem_num=0;
	sema_buf.sem_op=-1;
	sema_buf.sem_flg=SEM_UNDO;

	//ssu_semademo.c 파일과 프로젝트 값 B로 key를 얻는다.
	if((key=ftok("ssu_semademo.c",'J'))==-1){
		fprintf(stderr,"ftok error\n");
		exit(1);
	}

	//세마포어 ID를 얻는다
	if((sema_id=ssu_initsema(key,1))==-1){
		fprintf(stderr,"initsem error\n");
		exit(1);
	}

	//엔터를 쳐서 다음단계로 넘어간다.
	printf("Press return to lock: ");
	getchar();
	printf("Trying to lock...\n");

	//key에 해당한는 세마포어를 얻 값을 1로 설정해준 뒤, P()와 V()를 수행한다.
	if(semop(sema_id,&sema_buf,1)==-1){
		fprintf(stderr,"semop error\n");
		exit(1);
	}

	//enter 입력을 받고 다음단계로 넘어간다
	printf("Locked. \n");
	printf("Press return to unlock: ");
	getchar();
	sema_buf.sem_op=1;

	if(semop(sema_id,&sema_buf,1)==-1){
		fprintf(stderr,"semop error\n");
		exit(1);
	}

	printf("Unlocked \n");
	exit(0);
}

int ssu_initsema(key_t key, int sema_nums){
	union ssu_sema_undef sema;
	struct semid_ds buf;
	struct sembuf sema_buf;
	int i;
	int sema_id;

	//세마포어 id를 얻는다
	sema_id=semget(key,sema_nums,IPC_CREAT|IPC_EXCL|0666);

	if(sema_id>=0){
		sema_buf.sem_op=1;
		sema_buf.sem_flg=0;
		sema.val=1;
		printf("press return \n");
		getchar();

		for(sema_buf.sem_num=0;sema_buf.sem_num<sema_nums;sema_buf.sem_num++){
			if(semop(sema_id,&sema_buf,1)==-1){
				int err_num=errno;
				semctl(sema_id,0,IPC_RMID);
				errno=err_num;
				return -1;
			}
		}
	}
	else if(errno==EEXIST){
		int ready=0;
		
		sema_id=semget(key, sema_nums,0);

		if(sema_id<0)
			return sema_id;

		sema.buf=&buf;

		for(i=0;i<MAX_RETRIES && !ready; i++){
			semctl(sema_id,sema_nums-1,IPC_STAT,sema);
			
			if(sema.buf->sem_otime!=0)
				ready=1;
			else
				sleep(1);
		}

		if(!ready){
			errno=ETIME;
			return -1;
		}
	}
	return sema_id;
}

