#include "ssu_h.h"

int main(void){
	pid_t pid;
    int argc=0;
    char input[BUFMAX];
    char *argv[ARGMAX+1];
//	int status;

    //프롬프트 시작
    while(1) {
        printf("20182601> ");
        fgets(input,sizeof(input),stdin);
        input[strlen(input)-1]='\0';
        argc=split(input," ",argv);
		argv[ARGMAX]=NULL;

		/*
		for(int i=0;i<5;i++){
			printf("argv[%d]:%s\n",i,argv[i]);
		}
		*/

        if(argc==0)
			continue;

        if(!strcmp(argv[0],"exit")){
            printf("Prompt End\n");
            break;
        }
		
		if((pid=fork())<0){
			fprintf(stderr,"fork error\n");
			exit(1);
		}

		else if(pid==0){
			if(!strcmp(argv[0],"fmd5")){
				if(pid==0){
					execv("./ssu_find-md5",argv);
					fprintf(stderr,"couldn't execute: fmd5\n");
				}
			}
			else if(!strcmp(argv[0],"fsha1")){
				if(pid==0){
					execv("./ssu_find-sha1",argv);
					fprintf(stderr,"couldn't execute: fsha1\n");
				}
			}
			else
				if(pid==0){
					execl("./ssu_help","./ssu_help",NULL);
					fprintf(stderr,"couldn't execute: help\n");
				}
			exit(127);
		}

		while(wait((int *)0)!=-1);
    }

    exit(0);
}
