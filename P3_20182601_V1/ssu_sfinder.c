#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#define ARGMAX 11
#define BUFMAX 1024
#define FILEMAX 255
#define PATHMAX BUFMAX*4

void command_fmd5(int argc,char *argv[]);
void command_fsha1(int argc,char *argv[]);
void command_list(int argc,char *argv[]);
void command_trash(int argc,char *argv[]);
void command_restore(int argc,char *argv[]);
void command_help(void);
int split(char *string, char *seperator, char *argv[]);

int main(void){
	int argc=0;
	char input[BUFMAX];
	char *argv[ARGMAX];
	
	while(1){
		printf("20182601> ");
		fgets(input,sizeof(input),stdin);
		input[strlen(input)-1]='\0';
		argc=split(input," ",argv);
		
		if(argc==0)
			continue;

		if(!strcmp(argv[0], "fmd5"))
			command_fmd5(argc,argv);

		else if(!strcmp(argv[0],"fsha1"))
			command_fsha1(argc,argv);

		else if(!strcmp(argv[0],"list"))
			command_list(argc,argv);

		else if(!strcmp(argv[0],"trash"))
			command_trash(argc,argv);

		else if(!strcmp(argv[0],"restore"))
			command_restore(argc,argv);

		else if(!strcmp(argv[0],"exit")){
			printf("Prompt End\n");
			break;
		}
		else
			command_help();
	}
	exit(0);
}

void command_fmd5(int argc, char *argv[]){
	int flag_e=0,flag_l=0,flag_h=0,flag_d=0,flag_t=0;
	int opt;
	
	if(argc>ARGMAX){
		printf("ERROR: Arguments error\n");
		return;
	}

	while((opt=getopt(argc,argv,"elhdt"))!=-1){
		switch(opt){
			case 'e':
				flag_e=1;
				break;
			case 'l':
				flag_l=1;
				break;
			case 'h':
				flag_h=1;
				break;
			case 'd':
				flag_d=1;
				break;
			case 't':
				flag_t=1;
				break;
			case '?':
				printf("Unknown option : %c\n",optopt);
				return;
		}
	}

	printf("%d\n%d\n%d\n%d\n%d\n",flag_e,flag_l,flag_h,flag_d,flag_t);

//	flag_e=flag_l=flag_h=flag_d=flag_t=0;
}

void command_fsha1(int argc,char *argv[]){
}
void command_list(int argc,char *argv[]){
}
void command_trash(int argc,char *argv[]){
}
void command_restore(int argc,char *argv[]){
}


//입력 문자열 토크나이징 함수
int split(char *string, char *seperator, char *argv[]){
    int argc=0;
    char *ptr=NULL;

    ptr=strtok(string,seperator);
    while(ptr!=NULL){
        argv[argc++]=ptr;
        ptr=strtok(NULL," ");
    }

    return argc;
}

//help
void command_help(void){
	printf("Usage:\n > fmd5/fsha1 -e [FILE_EXTENSION] -l [MINSIZE] -h [MAXSIZE] -d [TARGET_DIRECTORY] -t [THREAD_NUM]\n");
	printf("      >> delete -l [SET_INDEX] -d [OPTARG] -i -f -t\n");
	printf(" > trash -c [CATEGORY] -o [ORDER]\n");
	printf(" > restore [RESTORE_INDEX]\n");
	printf(" > help : \n");
	printf(" > exit : \n");
}
