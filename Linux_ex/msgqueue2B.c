#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

struct ssu_msgbuf{
	char msg_text[BUFFER_SIZE];
	long msg_type;
};

int main(void){
	struct ssu_msgbuf buf;
	key_t key;
	int msg_queueid;

	//ssu_msgque_1A.c파일과 프로젝트 값 B로 key를 얻는다
	if((key=ftok("ssu_msgqueue_1A.c",'B'))==-1){
		fprintf(stderr,"ftok error\n");
		exit(1);
	}

	//key에 해당한는 메시지 큐 ID 얻는다
	if((msg_queueid=msgget(key,0644))==-1){
		fprintf(stderr,"msgget error\n");
		exit(1);
	}

	printf("spock: ready to receive messages, captain.\n");

	//메시지 큐에 있는 정보를 출력한다
	while(1){
		if(msgrcv(msg_queueid,&buf,sizeof(buf.msg_text),0,0)==-1){
			fprintf(stderr,"msgrcv: Identifier removed\n");
			exit(1);
		}

		printf("spock: \"%s\"\n",buf.msg_text);
	}

	exit(0);
}

