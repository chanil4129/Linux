#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void ssu_echo_exit(int status);

int main(void){
	//자식 프로세스들의 종료 상태를 출력하는 프로그램
	pid_t pid;
	int status;

	//프로세스 생성
	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	//자식 프로세스가 Argument list too long 상태로 정상 종료
	else if(pid==0)
		exit(7);

	//종료된 자식 프로세스의 pid가 깉은지 확인하며, 자식 프로세스의 종료를 기다린다.
	//자식 프로세스가 종료되면 status 인자가 종료 상태 값을 갖게 된다.
	if(wait(&status)!=pid){
		fprintf(stderr,"wait error\n");
		exit(1);
	}

	//사용자가 정의한 ssu_echo_exit에 exit(7)로 종료한 자식 프로세스의 상태 status를 전달해주며
	//정상종료 되었으므로 정상종료를 출력하고, statloc 인자의 종료 상태 값 7 정보를 출력한다.
	ssu_echo_exit(status);

	//프로세스 생성
	if((pid=fork())<0){
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	//자식 프로세스가 비정상 종료. 이때 버퍼를 삭제하고 열린 파일을 닫는다. 시그널에 의해 종료된다.
	else if(pid==0)
		abort();

	//종료된 자식 프로세스의 pid가 같은지 확인하며, 자식 프로세스의 종료를 기다린다.
	//비정상 종료이므로 시그널 번호6을 보낸다.
	if(wait(&status)!=pid){
		fprintf(stderr,"wait error\n");
		exit(1);
	}

	//사용자가 정의한 ssu_echo_exit에 비정상 종료로 판단하며.
	//시그널 번호는 6이고, 코어파일은 생성 됐으므로 WCOREDUMP가 참이 되어 (core file generated)가 같이 출력된다.
	ssu_echo_exit(status);

	//프로세스 생성
	if((pid=fork())<0){
		fprintf(stderr,"fork error\n");
		exit(1);
	}
	//자식 프로세스가 비정상 종료. 0으로 나눌 수 없기 때문에 비정상 종료된다. 시그널에 의해 종료된다.
	else if(pid==0)
		status/=0;

	//종료된 자식 프로세스의 pid가 같은지 확인하며, 자식 프로세스의 종료를 기다린다.
	//비정상 종료이므로 시그널 번호8을 보낸다.
	if(wait(&status)!=pid){
		fprintf(stderr,"wait error\n");
		exit(1);
	}
	
	//사용자가 정의한 ssu_echo_exit에 비정상 종료로 판단하며,
	//시그널 번호는 8이고, 코어파일은 생성 됐으므로 WCOREDUMP가 참이 되어 (core file generated)가 같이 출력된다.
	ssu_echo_exit(status);
	exit(0);
}

void ssu_echo_exit(int status){
	//정상 종료라면 상태값 출력
	if(WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
				WEXITSTATUS(status));
	//비정상 종료라면 상태값 출력하고, 코어파일이 생성 되었으면 시그널 번호를 출력.
	else if(WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status)?" (core file generated)" : "");
#else
				"");
#endif
	else if(WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",WSTOPSIG(status));
}
