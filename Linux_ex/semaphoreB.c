#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#if defined(_SEM_SEMUN_UNDEFINED) && _SEM_SEMUN_UNDEFINED

union ssu_sema_undef{
	unsigned short int *array;
	struct semid_ds *buf;
	struct seminfo *__buf;
	int val;
};

#endif

int main(void){
	union ssu_sema_undef sema;
	key_t key;
	int sema_id;

	//ssu_semademo.c 파일과 프로젝트 값 J로 key를 얻는다
	if((key=ftok("ssu_semademo.c",'J'))==-1){
		fprintf(stderr,"ftok error\n");
		exit(1);
	}

	//제거하고 나서 다시 실행하면 error 발생
	if((sema_id=semget(key,1,0))==-1){
		fprintf(stderr,"semget error\n");
		exit(1);
	}

	//사용 끝난 세마포어를 커널에서 제거한다
	if(semctl(sema_id,0,IPC_RMID,sema)==-1){
		fprintf(stderr,"semctl error\n");
		exit(1);
	}
	exit(0);
}
