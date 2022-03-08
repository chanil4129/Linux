#include <stdio.h>
#include <stdlib.h> //realpath
#include <sys/stat.h> //stat
#include <sys/types.h>
#include <unistd.h> 
#include <dirent.h> //scandir
#include <sys/time.h>   //gettimeofday
#include <string.h>	//strcmp

#define INPUT_SIZE 1023
#define ENTER 0
#define FIND 1
#define EXIT 2
#define HELP 3
#define FILE_NUM 1023

/*
typedef struct Print_File {
    var index;
    var Mode;
    var Blocks;
    var Links;
    var UID;
    var GID;
    var Access;
    var Change;
    var Modify;
    var Path;
} Print_File;
*/

struct timeval start_time,end_time; //Runtime calculate

int command_classify(char *);
void find_command(char *, char *);
void division_FILENAME_PATH(char *, char *, char *);
//var dir_find();
//var file_find();
void exit_command();
void help_command();

int main(void) {
    struct stat *buf;   //File Attribute
    char user_input[INPUT_SIZE];
	int command_num;
    char FILENAME[INPUT_SIZE], PATH[INPUT_SIZE];

    gettimeofday(&start_time,NULL); //timer start

    while(1){
        printf("20182601> ");
		command_num=command_classify(user_input);

		if(command_num==ENTER) continue;
		else if(command_num==FIND){
			division_FILENAME_PATH(user_input,FILENAME, PATH);
			find_command(FILENAME, PATH);
		}
		else if(command_num==EXIT) exit_command();
		else if(command_num==HELP) help_command();
		else {
			fprintf(stderr,"input error");
			exit(1);
		}
    }
    return 0;
}

void division_FILENAME_PATH(char *user_input, char *FILENAME, char *PATH){
	int i=0,j=0;	//i:"find" except
	while(user_input[i+4]!='/') FILENAME[i]=user_input[i], i++;
	FILENAME[i+1]='\0';
	while(user_input[i]!='\0') PATH[j]=user_input[i],i++,j++;
	PATH[j]='\0';
	printf("%s\n%s\n",FILENAME,PATH);
}

void find_command(char *FILENAME, char *PATH){   //scandir(디렉토리 목록 조회), realpath(상대경로=>절대경로)
	char REAL_PATH[INPUT_SIZE];
	int index;
	if(realpath(PATH,REAL_PATH)==NULL){
		printf("(None)");
		return;
	}
	printf("Index Size Mode        Blocks Links UID  GID  Access          Change        Modify         Path\n");
	index=find_dfs(FILENAME, REAL_PATH, index);
	if(index==0) printf("(None)");
	else find_option();
	return;
}

int find_dfs(char *FILENAME, char *PATH, int index){
}

void find_option(){
	printf(">> ");
}

int command_classify(char *result){
	int c;
	int i=0;
	while((c=getc(stdin))!='\n'){
		if(c==' ') continue;	//space(ascii 32)
		result[i]=c, i++;	//command store
	}
	result[i]='\0';
	if(result[0]=='\0') return ENTER;
	else if(result[0]=='f'&&result[1]=='i'&&result[2]=='n'&&result[3]=='d') return FIND;
	else if(result[0]=='e'&&result[1]=='x'&&result[2]=='i'&&result[3]=='t'&&result[4]=='\0') return EXIT;
	else return HELP;
}

//var dir_find(){}
//var file_find(){}

void exit_command(void){
    double Runtime_sec,Runtime_usec;
    gettimeofday(&end_time,NULL); //timer end
    Runtime_sec=end_time.tv_sec-start_time.tv_sec;
    Runtime_usec=end_time.tv_usec-start_time.tv_usec;
    printf("Prompt End\nRuntime: %d:%d(sec:usec)\n",(int)Runtime_sec,(int)Runtime_usec);
    exit(0);
}

void help_command(void){
    printf("Usage:\n\t> find [FILENAME] [PATH]\n\t\t>> [INDEX] [OPTION ...]\n\t> help\n\t exit\n\n");
    printf("\t[OPTION ...]");
    printf("\n\t q : report only when files differ");
    printf("\n\t s : report when two files are the same");
    printf("\n\t i : ignore case differences in file contents");
    printf("\n\t r : recursively compare any subdirectories found\n");
}
