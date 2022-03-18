#include <stdio.h>
#include <stdlib.h> //realpath
#include <sys/stat.h> //stat
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h> 
#include <dirent.h> //scandir
#include <sys/time.h>   //gettimeofday
#include <string.h>	//strcmp
#include <time.h>
#include <stdbool.h>

#define INPUT_SIZE 1023
#define PATH_SIZE 10000
#define ENTER 0
#define FIND 1
#define EXIT 2
#define HELP 3
#define FILE_NUM 1023

struct file_content {
	int line[INPUT_SIZE];
	char buf[INPUT_SIZE];
};
struct timeval start_time,end_time; //Runtime calculate
char Index_file[FILE_NUM][PATH_SIZE];
int file_index;
long dir_weight;

int command_classify(char *);
void find_command(char *, char *);
void division_FILENAME_PATH(char *, char *, char *);
int find_dfs(char *, char *, int);
void print_file_info(char *,struct stat, struct tm *,int);
int find_index();
void find_only_index(int);
void file_diff(int,int);
void dir_diff(int);
//char* file_to_buf(int,int*);
void file_to_buf(int,int*,char *);
void row(struct file_content ,int,char *);
void find_option_q(int);
void find_option_s(int);
void find_option_i(int);
void find_option_r(int);
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
	int i=5,j=0;	//i:"find " except
	while(user_input[i]!=' ') FILENAME[j]=user_input[i],i++,j++;
	FILENAME[j]='\0',j=0,i++;
	while(user_input[j]!='\0') PATH[j]=user_input[i],i++,j++;
	PATH[j]='\0';
//	printf("F: %s\nP: %s\n",FILENAME,PATH);
}

void find_command(char *FILENAME, char *PATH){   //scandir(디렉토리 목록 조회), realpath(상대경로=>절대경로)
	char REAL_PATH[INPUT_SIZE];
	int index=0;
	int realpath_length,path_length;

	 if(realpath(PATH,REAL_PATH)==NULL){
		fprintf(stderr,"(None)\n");	//modify=>exception
		return;
	}
	dir_weight=0;
	index=find_dfs(FILENAME, REAL_PATH, index);
	if(index==1) printf("(None)\n");
	else while(!find_index(index));
	return;
}

int find_dfs(char *FILENAME, char *PATH, int index){
	struct stat file;
	struct stat d_file;
	struct stat is_dir;
	struct tm *t;
	DIR *dir_ptr=NULL;
	struct dirent *file_ptr=NULL;
	struct dirent **namelist;
	char lowpath[PATH_SIZE];
	char PATH_FILE[PATH_SIZE];
	int count,str_length;

	if(!strcmp(PATH,"/")){
	   	strcpy(PATH_FILE,"/");
		strcat(PATH_FILE,FILENAME);
	}
	else{
		strcpy(PATH_FILE,PATH);
		strcat(PATH_FILE,"/");
		strcat(PATH_FILE,FILENAME);
	}
//	printf("%s\n",PATH_FILE);//tmp
	if(stat(PATH_FILE,&file)==0){
		print_file_info(PATH_FILE,file,t,index);
		index++;
	}

	if((count=scandir(PATH,&namelist,NULL,alphasort))==-1){
		fprintf(stderr,"scandir error\n");
		return index;
	}
	for(int i=0;i<count;i++){
		memset(lowpath,0,sizeof(lowpath));
		if(!strcmp(namelist[i]->d_name,"..")) continue;	
		if(!strcmp(namelist[i]->d_name,".")) continue;
		if(!strcmp(namelist[i]->d_name,"proc")) continue;
		if(!strcmp(namelist[i]->d_name,"run")) continue;
		if(!strcmp(namelist[i]->d_name,"snap")) continue;
		if(!strcmp(namelist[i]->d_name,"sys")) continue;
//		if(!strcmp(namelist[i]->d_name,"usr")) continue;
		if(namelist[i]->d_name[0]=='.') continue;	
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

		
		if(stat(lowpath,&is_dir)==-1){
//			fprintf(stderr,"%s\n",lowpath);
		}
		if((is_dir.st_mode&S_IFMT)!=S_IFDIR) continue;
		
		if(((dir_ptr=opendir(lowpath))==NULL)) continue;
		while((file_ptr=readdir(dir_ptr))==NULL){
			fprintf(stderr,"readdir error\n");
		}
		closedir(dir_ptr);
		index=find_dfs(FILENAME,lowpath,index);
	}
	for(int i=0;i<count;i++) free(namelist[i]);
	free(namelist);
	return index;
}

void print_file_info(char *PATH_FILE,struct stat file, struct tm *t,int index){
	char *rwx[8]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};

	if(!index) printf("Index Size Mode       Blocks Links UID  GID  Access            Change            Modify            Path\n"); //if index=0
	printf("%-6d",index);
	if(S_ISDIR(file.st_mode)){
		printf("%-5ld",(long)file.st_size);
	    printf("d");
	}
	else{
		printf("%-5ld",(long)file.st_size);
		printf("-");
	}
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
	printf("%s\n",PATH_FILE);

	strcpy(Index_file[file_index++],PATH_FILE);
	return;

}

int find_index(int index_size){
	char index[INPUT_SIZE];
	char input[INPUT_SIZE];
	char option='\0';
	int index_num;
	char c;
	int i=0;
	
	printf(">> ");
	while((c=getc(stdin))!='\n') input[i]=c,i++;
	input[i]='\0',i=0;
	while(input[i]!='\0') {
		index[i]=input[i],i++;
		if(input[i]==' '){
			option=input[i+1];
			if(input[i+2]!='\n') {
				fprintf(stderr,"many option");
				return 0;
			}
			break;
		}
	}
	index[i]='\0';
	for(i=0;index[i]!='\0';i++) if(index[i]<'0'||index[i]>'9') return 0;
	index_num=atoi(index);
	if(index_num>=index_size) {
		fprintf(stderr,"index size error\n");
		return 0;
	}
	if(option=='\0') find_only_index(index_num);
	else if(option=='q') find_option_q(index_num);
	else if(option=='s') find_option_s(index_num);
	else if(option=='i') find_option_i(index_num);
	else if(option=='r') find_option_r(index_num);
	else {
	fprintf(stderr,"your type option is non_option\n");
	return 0;
	}
	return 1;
}

void find_only_index(int index){
	struct stat sb1;
	struct stat sb2;

	if(stat(Index_file[0],&sb1)==-1){
		fprintf(stderr,"stat");
	}
	if(stat(Index_file[index],&sb2)==-1){
		fprintf(stderr,"stat");
	}

	if((sb1.st_mode&S_IFMT)==S_IFREG&&(sb2.st_mode&S_IFMT)==S_IFREG)
		file_diff(0,index);
	if((sb1.st_mode&S_IFMT)==S_IFDIR&&(sb2.st_mode&S_IFMT)==S_IFDIR)
		dir_diff(index);
}

void file_diff(int index1,int index2){
	struct file_content f1;
	struct file_content f2;
	char buf[INPUT_SIZE];
	char row1[INPUT_SIZE],row2[INPUT_SIZE];
	int length1,length2;
	int line1=1,line2=1;
	bool last_line1=false,last_line2=false;
	
	file_to_buf(index1,&length1,buf);
	strcpy(f1.buf,buf);
	memset(buf,0,length1);

	file_to_buf(index2,&length2,buf);
	strcpy(f2.buf,buf);
	
	for(int i=0;i<length1;i++) if(f1.buf[i]==10) {
		f1.buf[i]=0;	
		line1++;
	}
	for(int i=0;i<length2;i++) 	if(f2.buf[i]==10) {
		f2.buf[i]=0;
		line2++;
	}
	if(f1.buf[length1]!=0) f1.buf[length1]=0,last_line1=true;
	if(f2.buf[length2]!=0) f2.buf[length2]=0,last_line2=true;
	line1=1,line2=1;
	f1.line[1]=-1,f2.line[1]=-1;
	for(int i=0;i<length1;i++) if(f1.buf[i]==0) f1.line[++line1]=i;
	for(int i=0;i<length2;i++) if(f2.buf[i]==0) f2.line[++line2]=i;

	row(f1,1,row1);
	printf("%s\n",row1);
	row(f1,2,row1);
	printf("%s\n",row1);
	row(f1,3,row1);
	printf("%s\n",row1);
	row(f1,4,row1);
	printf("%s\n",row1);
	row(f1,5,row1);
	printf("%s\n",row1);
	
}

void row(struct file_content f,int target_line,char *buf){
	int i=0;
	int j=f.line[target_line]+1;
	memset(buf,0,INPUT_SIZE);
	while(f.buf[j]!=0){
		buf[i]=f.buf[j];
		i++,j++;
	}
	buf[j]=0;
}

//char* file_to_buf(int index, int *length){
void file_to_buf(int index, int *length,char *buf){
//	char buf[INPUT_SIZE];
	int fd;
	if((fd=open(Index_file[index],O_RDONLY))<0){
		fprintf(stderr,"file to buf open  error\n");
	}
	if((*length=read(fd,buf,INPUT_SIZE))<0){
		fprintf(stderr,"file to buf read error\n");
	}
//	for(int i=0;i<*length;i++) if(buf[i]==10) buf[i]='\n';
	buf[*length]=0;
}

void dir_diff(int index){
}
void find_option_q(int index){
}
void find_option_s(int index){
}
void find_option_i(int index){
}
void find_option_r(int index){
}

int command_classify(char *result){	//classify command
	int c;
	int i=0;
	while((c=getc(stdin))!='\n') result[i]=c, i++;	//command store
	
	result[i]='\0';
	if(result[0]=='\0') return ENTER;
	else if(result[0]=='f'&&result[1]=='i'&&result[2]=='n'&&result[3]=='d') return FIND;
	else if(result[0]=='e'&&result[1]=='x'&&result[2]=='i'&&result[3]=='t'&&result[4]=='\0') return EXIT;
	else return HELP;
}


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
