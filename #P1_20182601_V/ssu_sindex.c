#include <stdio.h>
#include <stdlib.h> //realpath
#include <sys/stat.h> //stat
#include <sys/types.h>
#include <unistd.h> 
#include <dirent.h> //scandir
#include <sys/time.h>   //gettimeofday
#include <string.h>	//strcmp
#include <time.h>

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
int target_file(char *, char *, int);
int find_dfs(char *, char *, int);
void find_option();
//var dir_find();
//var file_find();
void exit_command();
void help_command();

int main(void) {
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

void division_FILENAME_PATH(char *user_input, char *FILENAME, char *PATH){	//divide FILENAME and PATH
	int i=4,j=0;	//i:"find" except
	while(user_input[i]!='/'){
		if(user_input[i]=='\0'){//not input path
			fprintf(stderr,"none path");
			return ;
		}
	   	FILENAME[j]=user_input[i],i++,j++;
	}
	FILENAME[j]='\0',j=0;
	while(user_input[i]!='\0') PATH[j]=user_input[i],i++,j++;
	PATH[j]='\0';
}

void find_command(char *FILENAME, char *PATH){   //scandir(디렉토리 목록 조회), realpath(상대경로=>절대경로)
	char REAL_PATH[INPUT_SIZE];
	int index=0;
	if(realpath(PATH,REAL_PATH)==NULL){
		printf("(None)\n");	//modify=>exception
		return;
	}
	index=target_file(FILENAME, REAL_PATH, index);
	index=find_dfs(FILENAME, REAL_PATH, index);
	if(index==0) printf("(None)\n");
	else find_option();
	return;
}

int find_dfs(char *FILENAME, char *PATH, int index){
	struct stat file;
	struct tm *t;
	char *rwx[8]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
	DIR *dir_ptr=NULL;
	struct dirent *file_ptr=NULL;
	struct dirent **namelist;
	char lowpath[200000];
	char PATH_FILE[200000];
	int count,str_length;

	if((count=scandir(PATH,&namelist,NULL,alphasort))==-1){
		fprintf(stderr,"scandir error\n");
		return index;
	}
	for(int i=0;i<count;i++){
		memset(lowpath,0,sizeof(lowpath));
		if(!strcmp(namelist[i]->d_name,"..")) continue;	
		if(!strcmp(namelist[i]->d_name,".")) continue;
		if(namelist[i]->d_name[0]=='.') continue;
		if(!strcmp(namelist[i]->d_name,"X11")) continue;	
		else {
			str_length=strlen(PATH);
			if(PATH[str_length-1]=='/') {
				strcpy(lowpath,PATH);
			}			
			else {
				strcpy(lowpath,PATH);
				strcat(lowpath,"/");
			}
			strcat(lowpath,namelist[i]->d_name);
		}

		if((dir_ptr=opendir(lowpath))==NULL) continue;
		while((file_ptr=readdir(dir_ptr))==NULL){
			fprintf(stderr,"readdir error\n");
		}
		memset(PATH_FILE,0,sizeof(PATH_FILE));
		strcpy(PATH_FILE,lowpath);
		strcat(PATH_FILE,"/");
		strcat(PATH_FILE,FILENAME);
		if(lstat(PATH_FILE,&file)==0){
			printf("%-6d",index++);
			printf("%-5ld",(long)file.st_size);
			S_ISDIR(file.st_mode) ? printf("d") : printf("-");
			printf("%s%s%s ",rwx[(file.st_mode&0700)>>6],rwx[(file.st_mode&070)>>3],rwx[file.st_mode&07]);
			printf("%-7lld",(long long)file.st_blocks);
			printf("%-6ld",(long)file.st_nlink);
			printf("%-5ld",(long)file.st_uid);
			printf("%-5ld",(long)file.st_gid);
			t=localtime(&file.st_atime);
			printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
			t=localtime(&file.st_ctime);
			printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
			t=localtime(&file.st_mtime);
			printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
			printf("%s\n",lowpath);
		}
		closedir(dir_ptr);
		index=find_dfs(FILENAME,lowpath,index);
	}
	for(int i=0;i<count;i++) free(namelist[i]);
	free(namelist);
	return index;
}

int target_file(char *FILENAME, char *PATH, int index){
	struct stat file;
	struct tm *t;
	char *rwx[8]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};
	if(stat(FILENAME,&file)==0){
		if(!index) printf("Index Size Mode       Blocks Links UID  GID  Access            Change            Modify            Path\n"); //if index=0
		printf("%-6d",index++);
		printf("%-5ld",(long)file.st_size);
		S_ISDIR(file.st_mode) ? printf("d") : printf("-");
		printf("%s%s%s ",rwx[(file.st_mode&0700)>>6],rwx[(file.st_mode&070)>>3],rwx[file.st_mode&07]);
		printf("%-7lld",(long long)file.st_blocks);
		printf("%-6ld",(long)file.st_nlink);
		printf("%-5ld",(long)file.st_uid);
		printf("%-5ld",(long)file.st_gid);
		t=localtime(&file.st_atime);
		printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		t=localtime(&file.st_ctime);
		printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		t=localtime(&file.st_mtime);
		printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		printf("%s\n",PATH);
	}
	return index;
}

void find_option(){
	printf(">> ");
}

int command_classify(char *result){	//classify command
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

void exit_command(void){	//exit command
    double Runtime_sec,Runtime_usec;
    gettimeofday(&end_time,NULL); //timer end
    Runtime_sec=end_time.tv_sec-start_time.tv_sec;
    Runtime_usec=end_time.tv_usec-start_time.tv_usec;
    printf("Prompt End\nRuntime: %d:%d(sec:usec)\n",(int)Runtime_sec,(int)Runtime_usec);
    exit(0);
}

void help_command(void){	//help command
    printf("Usage:\n\t> find [FILENAME] [PATH]\n\t\t>> [INDEX] [OPTION ...]\n\t> help\n\t exit\n\n");
    printf("\t[OPTION ...]");
    printf("\n\t q : report only when files differ");
    printf("\n\t s : report when two files are the same");
    printf("\n\t i : ignore case differences in file contents");
    printf("\n\t r : recursively compare any subdirectories found\n");
}
