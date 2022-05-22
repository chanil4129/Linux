#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHARED_MEMORY_SIZE 1024

int main(int argc, char *argv[]){
	key_t key;
	char *data;
	int shared_memory_id;

	if (argc > 2) {
		fprintf(stderr, "usage: %s [data_to_write] \n", argv[0]);
		exit(1);
	}

	//ssu_shmdemo.c 파일과 프로젝트 값 R로 key를 얻는다
	if ((key = ftok("ssu_shmdemo.c", 'R')) == -1) {
		fprintf(stderr, "ftok error\n");
		exit(1);
	}

	//세마포어 ID값 얻는다
	if ((shared_memory_id = shmget(key, SHARED_MEMORY_SIZE, 0644 | IPC_CREAT)) ==-1) {
		fprintf(stderr, "shmget error\n");
		exit(1);
	}

	//공유 메모리에 정보를 저장하고 그 정보를 다시 확인한다.
	//공유 메모리를 읽고 쓸 , 해당 문자열을 공유 메모리에 쓰고 인자가 없는 경우 현재 Tm인 메모리를 읽는다.
	if ((data = shmat(shared_memory_id, (void *)0, 0)) == (char *)(-1)) {
		fprintf(stderr, "shmat error\n");
		exit(1);
	}

	if (argc == 2) {
		printf("writing to segment: \"%s\" \n", argv[1]);
		strncpy(data, argv[1], SHARED_MEMORY_SIZE);
	}
	else
		printf("segment contains: \"%s\" \n", data);

	if (shmdt(data) == -1) {
		fprintf(stderr, "shmdt error\n");
		exit(1);
	}

	exit(0);
}
