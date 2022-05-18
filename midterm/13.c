#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define STDERR 2
#define BUFLEN 512

void compile_program(char *filename);
void redirection(char *command, int new, int old);

int main(int argc, char *argv[]){
	char buf[BUFLEN];
	if(argc<2){
		exit(1);
	}

	compile_program(argv[1]);
}

void compile_program(char *filename){
	char question_name[BUFLEN];	//문제이름
	char command[BUFLEN];	//실행 명령어를 위한 버퍼
	char tmp[BUFLEN];		//문자열 생성을 위한 임시 버퍼
	int fd;					//에러 파일의 파일 디스크립터
	int size; 				//에러 파일의 크기

	strcpy(question_name,filename);
	question_name[strlen(filename) - strlen(strrchr(filename,'.'))]='\0';


	sprintf(tmp,"%s.exe", question_name);
	sprintf(command, "gcc -o %s %s -lpthread",tmp,filename);

	sprintf(tmp,"%s_error.txt",question_name);

	if((fd=creat(tmp,0666))<0){
		fprintf(stderr,"creat error for %s\n",tmp);
		exit(1);
	}

	redirection(command,fd,STDERR);
	size=lseek(fd,0,SEEK_END);
	close(fd);

	if(size >0)
		printf("compile error! error file size : %d\n",size);
	else{
		printf("compile success!\n");
		unlink(tmp);
	}


}

void redirection(char *command, int new, int old){
	int saved_fd;

	saved_fd=dup(old);
	dup2(new,old);
	system(command);
}


