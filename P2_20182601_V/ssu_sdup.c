#include "ssu_h.h"

int main(void){
	int argc=0;
	char input[BUFMAX];
	char* argv[ARGMAX];

	//프롬프트 시작
	while(1) {
		printf("20182601> ");
		fgets(input,sizeof(input),stdin);
		input[strlen(input)-1]='\0';
		argc=split(input," ",argv);

		if(argc==0)
			continue;
		if(!strcmp(argv[0],"fmd5"))
			;//고치기
		else if(!strcmp(argv[0],"fsha1"))
			;//고치기
		else if(!strcmp(argv[0],"help"))
			command_help();
		else if(!strcmp(argv[0],"exit")){
			printf("Prompt End\n");
			break;
		}
		else
			command_help();
	}

	exit(0);
}

//입력 문자열 토크나이징 함수
int split(char* string, char* seperator, char* argv[]){
	int argc=0;
	char* ptr=NULL;

	ptr=strtok(string,seperator);
	while(ptr!=NULL){
		argv[argc++]=ptr;
		ptr=strtok(NULL," ");
	}

	return argc;
}

void command_help(void){
	printf("Usage:\n");
	printf("   > fmd5/fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n");
	printf("      >> [SET_INDEX] [OPTION ...]\n");
	printf("         [OPTION ...]\n");
	printf("         d [LIST_IDX] : delete [LIST_IDX] file\n");
	printf("         i : ask for confirmation before delete\n");
	printf("         f : force delete except the recently modified file\n");
	printf("         t : force move to Trash except the recently modified file\n");
	printf("   > help\n");
	printf("   > exit\n\n");
}

