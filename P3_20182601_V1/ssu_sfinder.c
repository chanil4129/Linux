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
#include <limits.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

#define NAMEMAX 255
#define STRMAX 10000
#define ARGMAX 11
#define BUFMAX 1024
#define FILEMAX 255
#define PATHMAX 4096
#define THREADMAX 4
#define TIMEMAX 15

#define DIRECTORY 1
#define REGFILE 2

#define REMOVE 1
#define DELETE 2
#define RESTORE 3

#define KB 1000
#define MB 1000000
#define GB 1000000000
#define KiB 1024
#define MiB 1048576
#define GiB 1073741824
#define SIZE_ERROR -2

#define HASHMAX 33

typedef struct fileInfo {
	char path[PATHMAX];
	struct stat statbuf;
	struct fileInfo *next;
} fileInfo;

typedef struct fileList {
	long long filesize;
	char hash[HASHMAX];
	fileInfo *fileInfoList;
	struct fileList *next;
} fileList;

typedef struct dirList {
	char dirpath[PATHMAX];
	struct dirList *next;
} dirList;

typedef struct trashInfo {
	char path[PATHMAX];
	char restorepath[PATHMAX];
	char date[TIMEMAX];
	char time[TIMEMAX];
	struct stat statbuf;
} trashInfo;

typedef struct thread_data{
	char *hash;
	char *filename;
	char *fullpath;
} thread_data;

char extension[10];
char same_size_files_dir[PATHMAX];
char trash_path[PATHMAX];
char trash_path_info[PATHMAX];
char log_path[PATHMAX];
long long minbsize;
long long maxbsize;
fileList *dups_list_h;
fileList *dups_list_l;
trashInfo *trash_list[STRMAX];
int	trash_length;
thread_data thread_data_array[THREADMAX];
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int thread_num;
int work_thread;
int t;

void fileinfolist_free(fileInfo *head);//fileinfolist free 시킴
void dupslist_free(void);//dups_list를 free시킴
void command_fmd5(int argc,char *argv[]);//fmd5 명령어
void command_list(int argc,char *argv[]);//list 명령어
void set_swap(fileList *a,fileList *b);//세트끼리 데이터 바꾸기
void list_swap(fileInfo *a,fileInfo *b);//리스트들끼리 데이터 바꾸기
void command_trash(int argc,char *argv[]);//trash 명령어
void command_restore(int argc,char *argv[]);//restore 명령어
int filename_compare(const void *v1, const void *v2);//list를 filename에 따라 정렬
int size_compare(const void *v1, const void *v2);//set를 size에 따라 정렬
int date_compare(const void *v1, const void *v2);//list를 date에 따라 정렬
int time_compare(const void *v1, const void *v2);//list를 time에 따라 정렬
void trashlist_build(void);//trash 파일 정보 넣기
void command_help(void);//help 명령어
void fileinfo_append(fileInfo *head, char *path);//파일 정보 추가
fileInfo *fileinfo_delete_node(fileInfo *head, char *path);//fileinfo 노드를 삭제
int fileinfolist_size(fileInfo *head);//해당 리스트 개수 세주는거
void filelist_append(fileList *head, long long filesize, char *path, char *hash);//filelist 추가와 동시에 fileInfo추가
void filelist_delete_node(fileList *head, char *hash);//파일 리스트 없애기
int filelist_size(fileList *head);//filelist 개수 구해주기
int filelist_search(fileList *head, char *hash);//filelist 찾기
void dirlist_append(dirList *head, char *path);//dirlist 추가
void dirlist_print(dirList *head, int index);//dirlist 출력
void dirlist_delete_all(dirList *head);//dirlist 삭제
void get_path_from_home(char *path, char *path_from_home);//'~' 절대경로 찾아주기
int is_dir(char *target_dir);//디렉토리인지
long long get_size(char *filesize);//파일 사이즈 얻기
int get_file_mode(char *target_file, struct stat *statbuf);//디렉토리 정규파일 구분
void get_fullpath(char *target_dir, char *target_file, char *fullpath);//path+filename
int get_dirlist(char *target_dir, struct dirent ***namelist);//하위 디렉토리 확인
char *get_extension(char *filename);//확장자 뒷부분 포인터 or NULL
void get_filename(char *path, char *filename);//파일이름만(.이랑 / 다 빼고)
void get_new_file_name(char *org_filename, char *new_filename);//버전 높여가면서 새로운 이름짓기(휴지통 관련)
void remove_files(char *dir);//파일 삭제
void get_same_size_files_dir(void);//중복파일 기록
void get_trash_path(void);//휴지통 경로 얻기
void get_log_path(void);//로그파일 경로 얻기
void filesize_with_comma(long long filesize, char *filesize_w_comma);//파일 사이즈에 3자리마다 콤마넣기
void sec_to_ymdt(struct tm *time, char *ymdt);//시간 출력
void filelist_print_format(fileList *head);//중복리스트 출력
int md5(char *target_path, char *hash_result);//md5 해시값 얻기
void dir_traverse(dirList *dirlist);//BFS 재귀 탐색
void *record_thread(void *arg);//쓰레드 코드(미구현)
void find_duplicates(void);//중복 찾기
void remove_no_duplicates(void);//중복 아닌거 삭제
time_t get_recent_mtime(fileInfo *head, char *last_filepath);//가장 최근 수정한 파일 찾기
void delete_prompt(void);//delete 프롬프트 실행
void logging(int cmd,char *log_path);//로그 기록 남기기
int split(char *string, char *seperator, char *argv[]);//구분자 기준으로 토큰 얻기

int main(void){
	int argc=0;
	char input[BUFMAX];
	char *argv[ARGMAX];
	
	while(1){
		printf("20182601> ");
		fgets(input,sizeof(input),stdin);
		input[strlen(input)-1]='\0';
		argc=split(input," ",argv);

		//trash path와 trash info 경로를 얻음
		get_trash_path();
		get_log_path();
		
		if(argc==0)
			continue;

		//각 내장명령어에 따라 함수 실행
		if(!strcmp(argv[0], "fmd5"))
			command_fmd5(argc,argv);

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

void dupslist_free(void){
	fileList *filelist_cur=dups_list_h->next;
	fileList *tmp;

	while(filelist_cur!=NULL){
		tmp=filelist_cur->next;
		fileinfolist_free(filelist_cur->fileInfoList);
		free(filelist_cur);
		filelist_cur=tmp;
	}
	dups_list_h->next=NULL;
}

void fileinfolist_free(fileInfo *head){
	fileInfo *fileinfo_cur = head->next;
	fileInfo *tmp;

	while(fileinfo_cur!=NULL){
		tmp=fileinfo_cur->next;
		free(fileinfo_cur);
		fileinfo_cur=tmp;
	}
	head->next=NULL;
}

void command_fmd5(int argc, char *argv[]){
	int flag_e=0,flag_l=0,flag_h=0,flag_d=0,flag_t=0;
	int opt;
	char *e_argv,*l_argv,*h_argv,*d_argv,*t_argv;
	char target_dir[PATHMAX];
	dirList *dirlist=(dirList *)malloc(sizeof(dirList));

	dirlist->next=NULL;
	if(dups_list_h!=NULL){
		dupslist_free();
		free(dups_list_h);
	}

	dups_list_h=(fileList *)malloc(sizeof(fileList));
	dups_list_h->next=NULL;

	if(argc>11){
		printf("Usage: find -e [FILE_EXTENSION] -l [MINSIZE] -h [MAXSIZE] -d [DIRECTORY] -t [THREAD NUM]\n");
		return;
	}

	optind=0;
	while((opt=getopt(argc,argv,"e:l:h:d:t:"))!=EOF){
		switch(opt){
			case 'e':
				flag_e=1;
				e_argv=optarg;
				break;
			case 'l':
				flag_l=1;
				l_argv=optarg;
				break;
			case 'h':
				flag_h=1;
				h_argv=optarg;
				break;
			case 'd':
				flag_d=1;
				d_argv=optarg;
				break;
			case 't':
				flag_t=1;
				t_argv=optarg;
				break;
			case '?':
				printf("Unknown option : %c\n",optopt);
				return;
		}
	}

	if(flag_e==0||flag_l==0||flag_h==0||flag_d==0){
		printf("option error: must be included '-e -l -h -d'\n");
		return;
	}

	if(flag_t==0){
		t_argv="1";
	}
	thread_num=atoi(t_argv);
	
	if(strchr(e_argv,'*')==NULL){
		printf("ERROR: [FILE_EXTENSION] should be '*' or '*.extension'\n");
		return;
	}

	if (strchr(e_argv, '.') != NULL){
		strcpy(extension, get_extension(argv[1]));

		if (strlen(extension) == 0){
			printf("ERROR: There should be extension\n");
			return;
		}
	}

	minbsize = get_size(l_argv);

	if (minbsize == -1)
		minbsize = 0;

	maxbsize = get_size(h_argv);

	if (minbsize == SIZE_ERROR || maxbsize == SIZE_ERROR){
		printf("ERROR: Size wrong -min size : %s -max size : %s\n", l_argv, h_argv);
		return;
	}

	if (maxbsize != -1 && minbsize > maxbsize){
		printf("ERROR: [MAXSIZE] should be bigger than [MINSIZE]\n");
		return;
	}

	memset(target_dir,0,sizeof(target_dir));
	if (strchr(d_argv, '~') != NULL)
		get_path_from_home(d_argv, target_dir);
	else{
		if (realpath(d_argv, target_dir) == NULL){
			printf("ERROR: [TARGET_DIRECTORY] should exist\n");
			return;
		}
	}

	if (access(target_dir, F_OK) == -1){
		printf("ERROR: %s directory doesn't exist\n", target_dir);
		return;
	}

	if (!is_dir(target_dir)){
		printf("ERROR: [TARGET_DIRECTORY] should be a directory\n");
		return;
	}

	if(strchr(d_argv,'~')!=NULL)
		get_path_from_home(d_argv,target_dir);
	else
		realpath(d_argv,target_dir);

	get_same_size_files_dir();
	
	struct timeval begin_t,end_t;

	gettimeofday(&begin_t,NULL);

	dirlist_append(dirlist,target_dir);
	dir_traverse(dirlist);
	free(dirlist);
	find_duplicates();
	remove_no_duplicates();

	gettimeofday(&end_t,NULL);

	end_t.tv_sec-=begin_t.tv_sec;
	if(end_t.tv_usec<begin_t.tv_usec){
		end_t.tv_sec--;
		end_t.tv_usec+=1000000;
	}

	end_t.tv_usec-=begin_t.tv_usec;

	//중복리스트가 없다면
	if (dups_list_h->next == NULL)
		printf("No duplicates in %s\n", target_dir);
	//있으면 추가
	else 
		filelist_print_format(dups_list_h);

	printf("Searching time: %ld:%06ld(sec:usec)\n\n", end_t.tv_sec, end_t.tv_usec);

	delete_prompt();
}

void command_list(int argc,char *argv[]){
	int flag_l=0,flag_c=0,flag_o=0;
	int opt;
	char *l_argv,*c_argv,*o_argv;

	if(argc>7){
		printf("usage: list -l [LIST_TYPE] -c [CATEGORY] -o [ORDER]\n");
		return;
	}

	optind=0;
	while((opt=getopt(argc,argv,"l:c:o:"))!=EOF){
		switch(opt){
			case 'l':
				flag_l=1;
				l_argv=optarg;
				break;
			case 'c':
				flag_c=1;
				c_argv=optarg;
				break;
			case 'o':
				flag_o=1;
				o_argv=optarg;
				break;
			case '?':
				printf("Unknown option : %c\n",optopt);
				return;
		}
	}

	if(flag_l==0)
		l_argv="fileset";
	if(flag_c==0)
		c_argv="size";
	if(flag_o==0)
		o_argv="1";

	if(strcmp(l_argv,"fileset")&&strcmp(l_argv,"filelist")){
		printf("'-l' option error\n");
		return;
	}

	if(strcmp(c_argv,"size")&&strcmp(c_argv,"filename")&&strcmp(c_argv,"uid")&&strcmp(c_argv,"gid")&&strcmp(c_argv,"mode")){
		printf("'-c' option error\n");
		return;
	}

	if(strcmp(o_argv,"1")&&strcmp(o_argv,"-1")){
		printf("'-o' option error\n");
		return;
	}

	fileList *filelist=dups_list_h;
	int compare;
	if(!strcmp(l_argv,"fileset")){
		for(int i=0;i<filelist_size(filelist);i++){
			fileList *filelist_pre=filelist->next;
			fileList *filelist_pst=filelist_pre->next;
			for(int j=0;j<filelist_size(filelist)-(i+1);j++){
				if(!strcmp(c_argv,"size"))
					compare=filelist_pre->filesize<filelist_pst->filesize;

				if(!strcmp(o_argv,"1")&&!compare)
					set_swap(filelist_pre,filelist_pst);
				else if(!strcmp(o_argv,"-1")&&compare)
					set_swap(filelist_pre,filelist_pst);

				filelist_pre=filelist_pre->next;
				filelist_pst=filelist_pst->next;
			}
		}
	}
	else{
		fileList *filelist_cur=filelist->next;
		for(int k=0;k<filelist_size(filelist);k++){
			for(int i=0;i<fileinfolist_size(filelist_cur->fileInfoList);i++){
				fileInfo *fileinfo_pre=filelist_cur->fileInfoList->next;
				fileInfo *fileinfo_pst=fileinfo_pre->next;
				for(int j=0;j<fileinfolist_size(filelist_cur->fileInfoList)-(i+1);j++){
					if(!strcmp(c_argv,"filename")){
						compare=strcmp(fileinfo_pre->path,fileinfo_pst->path);
						if(compare<=0)
							compare=1;
						else
							compare=0;
					}
					if(!strcmp(c_argv,"uid"))
						compare=fileinfo_pre->statbuf.st_uid<fileinfo_pst->statbuf.st_uid;
					if(!strcmp(c_argv,"gid"))
						compare=fileinfo_pre->statbuf.st_gid<fileinfo_pst->statbuf.st_gid;
					if(!strcmp(c_argv,"mode"))
						compare=fileinfo_pre->statbuf.st_mode<fileinfo_pst->statbuf.st_mode;


					if(!strcmp(o_argv,"1")&&!compare)
						list_swap(fileinfo_pre,fileinfo_pst);
					else if(!strcmp(o_argv,"-1")&&compare)
						list_swap(fileinfo_pre,fileinfo_pst);

					fileinfo_pre=fileinfo_pre->next;
					fileinfo_pst=fileinfo_pst->next;
				}
			}
			filelist_cur=filelist_cur->next;
		}
	}

	filelist_print_format(dups_list_h);
}

void set_swap(fileList *a,fileList *b){
	long long filesize;
	char hash[HASHMAX];
	fileInfo *fileInfoList;
	
	filesize=a->filesize;
	a->filesize=b->filesize;
	b->filesize=filesize;

	strcpy(hash,a->hash);
	strcpy(a->hash,b->hash);
	strcpy(b->hash,hash);

	fileInfoList=a->fileInfoList;
	a->fileInfoList=b->fileInfoList;
	b->fileInfoList=fileInfoList;
}

void list_swap(fileInfo *a,fileInfo *b){
	char path[PATHMAX];
	struct stat statbuf;

	strcpy(path,a->path);
	strcpy(a->path,b->path);
	strcpy(b->path,path);

	statbuf=a->statbuf;
	a->statbuf=b->statbuf;
	b->statbuf=statbuf;
}

void command_trash(int argc,char *argv[]){
	int flag_c=0,flag_o=0;
	int opt;
	char *c_argv,*o_argv;

	if(argc>5){
		printf("usage: trash -c [CATEGORY] -o [ORDER]\n");
		return;
	}

	optind=0;
	while((opt=getopt(argc,argv,"c:o:"))!=EOF){
		switch(opt){
			case 'c':
				flag_c=1;
				c_argv=optarg;
				break;
			case 'o':
				flag_o=1;
				o_argv=optarg;
				break;
			case '?':
				printf("Unknown option : %c\n",optopt);
				return;
		}
	}

	if(flag_c==0)
		c_argv="filename";
	if(flag_o==0)
		o_argv="1";

	if(strcmp(c_argv,"filename")&&strcmp(c_argv,"size")&&strcmp(c_argv,"date")&&strcmp(c_argv,"time")){
		printf("'-c' option error\n");
		return;
	}
	if(strcmp(o_argv,"1")&&strcmp(o_argv,"-1")){
		printf("'-o' option error\n");
		return;
	}
	
	trashlist_build();

	if(!strcmp(c_argv,"filename"))
		qsort(*trash_list,trash_length,sizeof(trashInfo *),filename_compare);
	else if(!strcmp(c_argv,"size"))
		qsort(*trash_list,trash_length,sizeof(trashInfo *),size_compare);
	else if(!strcmp(c_argv,"date"))
		qsort(*trash_list,trash_length,sizeof(trashInfo *),date_compare);
	else if(!strcmp(c_argv,"time"))
		qsort(*trash_list,trash_length,sizeof(trashInfo *),time_compare);
	
	if(trash_length==0)
		printf("empty trash\n");
	else{
		printf("      %-80s%-20s%-20s%-20s\n","FILENAME","SIZE","DELETION DATE","DELETION TIME");
		if(!strcmp(o_argv,"-1")){
			for(int i=0;i<trash_length/2;i++){
				trashInfo *temp;
				temp=trash_list[i];
				trash_list[i]=trash_list[trash_length-1-i];
				trash_list[trash_length-1-i]=temp;
			}
		}
		for(int i=0;i<trash_length;i++)
			printf("[%3d] %-80s%-20lld%-20s%-20s\n",i+1,trash_list[i]->restorepath,(long long)trash_list[i]->statbuf.st_size,trash_list[i]->date,trash_list[i]->time);
	}
}

int filename_compare(const void *v1, const void *v2){
	trashInfo *t1=(trashInfo *)v1;
	trashInfo *t2=(trashInfo *)v2;
	return strcmp(t1->restorepath,t2->restorepath);
}

int size_compare(const void *v1, const void *v2){
	trashInfo *t1=(trashInfo *)v1;
    trashInfo *t2=(trashInfo *)v2;
	return t1->statbuf.st_size-t2->statbuf.st_size;
}

int date_compare(const void *v1, const void *v2){
	trashInfo *t1=(trashInfo *)v1;
    trashInfo *t2=(trashInfo *)v2;
	return strcmp(t1->date,t2->date);
}

int time_compare(const void *v1, const void *v2){
	trashInfo *t1=(trashInfo *)v1;
    trashInfo *t2=(trashInfo *)v2;
	return strcmp(t1->time,t2->time);
}


void trashlist_build(void){
	struct dirent **namelist;
	int listcnt;
	char input[BUFMAX];
	int argc=0;
	char *argv[ARGMAX];

	for(int i=0;i<trash_length;i++)
		free(trash_list[i]);
	memset(trash_list,0,sizeof(trash_list));
	trash_length=0;

	listcnt=get_dirlist(trash_path_info,&namelist);

	for(int i=0;i<listcnt;i++){
		FILE *fp;
		char fullpath[PATHMAX]={0,};
		char filename[NAMEMAX]={0,};
		char path[PATHMAX]={0,};
	
		trashInfo *new=(trashInfo *)malloc(sizeof(trashInfo));
		
		if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		get_fullpath(trash_path_info,namelist[i]->d_name,fullpath);
		get_filename(fullpath,filename);
		get_fullpath(trash_path,filename,path);

		if((fp=fopen(fullpath,"r"))==NULL){
			printf("open error\n");
			continue;
		}

		fgets(input,sizeof(input),fp);
		input[strlen(input)-1]='\0';
		argc=split(input,"#",argv);
		
		strcpy(new->path,path);
		strcpy(new->restorepath,argv[0]);
		strcpy(new->date,argv[1]);
		strcpy(new->time,argv[2]);
		lstat(new->path,&new->statbuf);

		trash_list[trash_length++]=new;
	}
}

void command_restore(int argc,char *argv[]){
	int index;
	char fullpath[PATHMAX];
	char filename[NAMEMAX];
	char hash[HASHMAX];
	char *pt=NULL;

	if(argc!=2){
		printf("usage: restore [index]\n");
		return;
	}

	index=atoi(argv[1]);
	if(index<1||index>trash_length){
		printf("index error\n");
		return;
	}

	//log남기기
	logging(RESTORE,trash_list[--index]->restorepath);

	//복원
	rename(trash_list[index]->path,trash_list[index]->restorepath);

	//info파일 없애기
	pt=get_extension(trash_list[index]->path);
	get_filename(trash_list[index]->path,filename);
	strcpy(fullpath,trash_path_info);
	strcat(fullpath,filename);
	strcat(fullpath,".");
	strcat(fullpath,pt);
	strcat(fullpath,".trashinfo");
	remove(fullpath);

	trashlist_build();
	qsort(*trash_list,trash_length,sizeof(trashInfo *),filename_compare);
	if(trash_length==0)
		printf("empty trash\n");
	else{
		printf("      %-80s%-20s%-20s%-20s\n","FILENAME","SIZE","DELETION DATE","DELETION TIME");
		for(int i=0;i<trash_length;i++)
			printf("[%3d] %-80s%-20lld%-20s%-20s\n",i+1,trash_list[i]->restorepath,(long long)trash_list[i]->statbuf.st_size,trash_list[i]->date,trash_list[i]->time);
	}
}

void fileinfo_append(fileInfo *head, char *path){
	fileInfo *fileinfo_cur;

	fileInfo *newinfo = (fileInfo *)malloc(sizeof(fileInfo));
	memset(newinfo, 0, sizeof(fileInfo));
	strcpy(newinfo->path, path);
	lstat(newinfo->path, &newinfo->statbuf);
	newinfo->next = NULL;

	if (head->next == NULL)
		head->next = newinfo;
	else {
		fileinfo_cur = head->next;
		while (fileinfo_cur->next != NULL)
			fileinfo_cur = fileinfo_cur->next;

		fileinfo_cur->next = newinfo;
	}

}

fileInfo *fileinfo_delete_node(fileInfo *head, char *path){
	fileInfo *deleted;

	if (!strcmp(head->next->path, path)){
		deleted = head->next;
		head->next = head->next->next;
		return head->next;
	}
	else {
		fileInfo *fileinfo_cur = head->next;

		while (fileinfo_cur->next != NULL){
			if (!strcmp(fileinfo_cur->next->path, path)){
				deleted = fileinfo_cur->next;

				fileinfo_cur->next = fileinfo_cur->next->next;
				return fileinfo_cur->next;
			}

			fileinfo_cur = fileinfo_cur->next;
		}
	}
}

int fileinfolist_size(fileInfo *head){
	fileInfo *cur = head->next;
	int size = 0;

	while (cur != NULL){
		size++;
		cur = cur->next;
	}

	return size;
}

void filelist_append(fileList *head, long long filesize, char *path, char *hash){
    fileList *newfile = (fileList *)malloc(sizeof(fileList));
    memset(newfile, 0, sizeof(fileList));

    newfile->filesize = filesize;
    strcpy(newfile->hash, hash);

    newfile->fileInfoList = (fileInfo *)malloc(sizeof(fileInfo));
    memset(newfile->fileInfoList, 0, sizeof(fileInfo));

    fileinfo_append(newfile->fileInfoList, path);
    newfile->next = NULL;

    if (head->next == NULL) {
        head->next = newfile;
    }
    else {
        fileList *cur_node = head->next, *prev_node = head, *next_node;

        while (cur_node != NULL && cur_node->filesize < newfile->filesize) {
            prev_node = cur_node;
            cur_node = cur_node->next;
        }

        newfile->next = cur_node;
        prev_node->next = newfile;
    }
}

void filelist_delete_node(fileList *head, char *hash){
	fileList *deleted;

	if (!strcmp(head->next->hash, hash)){
		deleted = head->next;
		head->next = head->next->next;
	}
	else {
		fileList *filelist_cur = head->next;

		while (filelist_cur->next != NULL){
			if (!strcmp(filelist_cur->next->hash, hash)){
				deleted = filelist_cur->next;

				filelist_cur->next = filelist_cur->next->next;

				break;
			}

			filelist_cur = filelist_cur->next;
		}
	}

	free(deleted);
}

int filelist_size(fileList *head){
	fileList *cur = head->next;
	int size = 0;

	while (cur != NULL){
		size++;
		cur = cur->next;
	}

	return size;
}

int filelist_search(fileList *head, char *hash){
	fileList *cur = head;
	int idx = 0;

	while (cur != NULL){
		if (!strcmp(cur->hash, hash))
			return idx;
		cur = cur->next;
		idx++;
	}

	return 0;
}

void dirlist_append(dirList *head, char *path){
	dirList *newFile = (dirList *)malloc(sizeof(dirList));

	strcpy(newFile->dirpath, path);
	newFile->next = NULL;

	if (head->next == NULL)
		head->next = newFile;
	else{
		dirList *cur = head->next;

		while(cur->next != NULL)
			cur = cur->next;

		cur->next = newFile;
	}
}

void dirlist_print(dirList *head, int index){
	dirList *cur = head->next;
	int i = 1;

	while (cur != NULL){
		if (index)
			printf("[%d] ", i++);
		printf("%s\n", cur->dirpath);
		cur = cur->next;
	}
}

void dirlist_delete_all(dirList *head){
	dirList *dirlist_cur = head->next;
	dirList *tmp;

	while (dirlist_cur != NULL){
		tmp = dirlist_cur->next;
		free(dirlist_cur);
		dirlist_cur = tmp;
	}

	head->next = NULL;
}

//입력에 '~'가 왔을 때 절대 경로로 바꿔주기
void get_path_from_home(char *path, char *path_from_home){
	char path_without_home[PATHMAX] = {0,};
	char *home_path;

    home_path = getenv("HOME");

    if (strlen(path) == 1){
		strncpy(path_from_home, home_path, strlen(home_path));
	}
    else {
        strncpy(path_without_home, path + 1, strlen(path)-1);
        sprintf(path_from_home, "%s%s", home_path, path_without_home);
    }
}

int is_dir(char *target_dir){
    struct stat statbuf;

    if (lstat(target_dir, &statbuf) < 0){
        printf("ERROR: lstat error for %s\n", target_dir);
        return 1;
    }
    return S_ISDIR(statbuf.st_mode) ? DIRECTORY : 0;

}

int get_file_mode(char *target_file, struct stat *statbuf){
	if (lstat(target_file, statbuf) < 0){
        printf("ERROR: lstat error for %s\n", target_file);
        return 0;
    }

    if (S_ISREG(statbuf->st_mode))
    	return REGFILE;
    else if(S_ISDIR(statbuf->st_mode))
    	return DIRECTORY;
    else
    	return 0;
}

void get_fullpath(char *target_dir, char *target_file, char *fullpath){
	strcat(fullpath, target_dir);

	if(fullpath[strlen(target_dir) - 1] != '/')
		strcat(fullpath, "/");

	strcat(fullpath, target_file);
	fullpath[strlen(fullpath)] = '\0';
}

int get_dirlist(char *target_dir, struct dirent ***namelist){
	int cnt = 0;

	if ((cnt = scandir(target_dir, namelist, NULL, alphasort)) == -1){
		printf("ERROR: scandir error for %s\n", target_dir);
		return -1;
	}

	return cnt;
}

char *get_extension(char *filename){
	char *tmp_ext;

	if ((tmp_ext = strstr(filename, ".tar")) != NULL || (tmp_ext = strrchr(filename, '.')) != NULL)
		return tmp_ext + 1;
	else
		return NULL;
}

void get_filename(char *path, char *filename){
	char tmp_name[NAMEMAX];
	char *pt = NULL;

	memset(tmp_name, 0, sizeof(tmp_name));

	if (strrchr(path, '/') != NULL)
		strcpy(tmp_name, strrchr(path, '/') + 1);
	else
		strcpy(tmp_name, path);

	if ((pt = get_extension(tmp_name)) != NULL)
		pt[-1] = '\0';

	if (strchr(path, '/') == NULL && (pt = strrchr(tmp_name, '.')) != NULL)
		pt[0] = '\0';

	strcpy(filename, tmp_name);
}

void get_new_file_name(char *org_filename, char *new_filename){
	char new_trash_path[PATHMAX];
	char tmp[NAMEMAX];
	struct dirent **namelist;
	int trashlist_cnt;
	int same_name_cnt = 1;

	get_filename(org_filename, new_filename);
	trashlist_cnt = get_dirlist(trash_path, &namelist);

	for (int i = 0; i < trashlist_cnt; i++){
		if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		memset(tmp, 0, sizeof(tmp));
		get_filename(namelist[i]->d_name, tmp);

		if (!strcmp(new_filename, tmp))
			same_name_cnt++;
	}

	sprintf(new_filename + strlen(new_filename), ".%d", same_name_cnt);

	if (get_extension(org_filename) != NULL)
		sprintf(new_filename + strlen(new_filename), ".%s", get_extension(org_filename));
}

void remove_files(char *dir){
	struct dirent **namelist;
	int listcnt = get_dirlist(dir, &namelist);

	for (int i = 0; i < listcnt; i++){
		char fullpath[PATHMAX] = {0, };

		if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		get_fullpath(dir, namelist[i]->d_name, fullpath);

		remove(fullpath);
	}
}

void get_same_size_files_dir(void){
	get_path_from_home("~/20182601", same_size_files_dir);

	if (access(same_size_files_dir, F_OK) == 0)
		remove_files(same_size_files_dir);
	else
		mkdir(same_size_files_dir, 0755);
}

void get_trash_path(void){
	if (getuid() == 0){
		get_path_from_home("~/.Trash/files/", trash_path);
		get_path_from_home("~/.Trash/info/", trash_path_info);

		if (access(trash_path, F_OK) == 0)
			remove_files(trash_path);
		else
			mkdir(trash_path, 0755);

		if (access(trash_path_info,F_OK)==0)
			remove_files(trash_path_info);
		else
			mkdir(trash_path_info,0755);
	}
	else{
		get_path_from_home("~/.local/share/Trash/files/", trash_path);
		get_path_from_home("~/.local/share/Trash/info/", trash_path_info);
	}
}

//log파일 위치
void get_log_path(void){
	if (getuid() == 0){
		get_path_from_home("~/.duplicate_20182601.log", log_path);

		if (access(log_path, F_OK) != 0)
			creat(log_path,0666);
	}
	else{
		get_path_from_home("~/.duplicate_20182601.log", log_path);

		if (access(log_path, F_OK) != 0)
			creat(log_path,0666);
	}
}
	

void filesize_with_comma(long long filesize, char *filesize_w_comma){
	char filesize_wo_comma[STRMAX] = {0, };
	int comma;
	int idx = 0;

	sprintf(filesize_wo_comma, "%lld", filesize);
	comma = strlen(filesize_wo_comma)%3;

	for (int i = 0 ; i < strlen(filesize_wo_comma); i++){
		if (i > 0 && (i%3) == comma)
			filesize_w_comma[idx++] = ',';

		filesize_w_comma[idx++] = filesize_wo_comma[i];
	}

	filesize_w_comma[idx] = '\0';
}

void sec_to_ymdt(struct tm *time, char *ymdt){
	sprintf(ymdt, "%04d-%02d-%02d %02d:%02d:%02d", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec);
}

void filelist_print_format(fileList *head){
	fileList *filelist_cur = head->next;
	int set_idx = 1;

	while (filelist_cur != NULL){
		fileInfo *fileinfolist_cur = filelist_cur->fileInfoList->next;

		if(fileinfolist_cur->next==NULL){
			filelist_cur=filelist_cur->next;
			continue;
		}

		char mtime[STRMAX];
		char atime[STRMAX];
		char filesize_w_comma[STRMAX] = {0, };
		int i = 1;
		int uid=fileinfolist_cur->statbuf.st_uid;
		int gid=fileinfolist_cur->statbuf.st_gid;
		int mode=fileinfolist_cur->statbuf.st_mode;

		filesize_with_comma(filelist_cur->filesize, filesize_w_comma);

		printf("---- Identical files #%d (%s bytes - %s) ----\n", set_idx++, filesize_w_comma, filelist_cur->hash);

		while (fileinfolist_cur != NULL){
			sec_to_ymdt(localtime(&fileinfolist_cur->statbuf.st_mtime), mtime);
			sec_to_ymdt(localtime(&fileinfolist_cur->statbuf.st_atime), atime);
			printf("[%d] %s (mtime : %s) (atime : %s) (uid : %d) (gid : %d) (mode : %o)\n", i++, fileinfolist_cur->path, mtime, atime,uid,gid,mode);

			fileinfolist_cur = fileinfolist_cur->next;
		}

		printf("\n");

		filelist_cur = filelist_cur->next;
	}
}

int md5(char *target_path, char *hash_result){
	FILE *fp;
	unsigned char hash[MD5_DIGEST_LENGTH];
	unsigned char buffer[SHRT_MAX];
	int bytes = 0;
	MD5_CTX md5;

	if ((fp = fopen(target_path, "rb")) == NULL){
		printf("ERROR: fopen error for %s\n", target_path);
		return 1;
	}

	MD5_Init(&md5);

	while ((bytes = fread(buffer, 1, SHRT_MAX, fp)) != 0)
		MD5_Update(&md5, buffer, bytes);

	MD5_Final(hash, &md5);

	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
		sprintf(hash_result + (i * 2), "%02x", hash[i]);
	hash_result[HASHMAX-1] = 0;

	fclose(fp);

	return 0;
}

void dir_traverse(dirList *dirlist){
	dirList *cur = dirlist->next;
	dirList *subdirs = (dirList *)malloc(sizeof(dirList));
	pthread_t tid[THREADMAX];
	int status;
	t=0;

	for(int i=0;i<THREADMAX;i++)
		memset(&thread_data_array[i],0,sizeof(thread_data));
	memset(subdirs, 0 , sizeof(dirList));

	while (cur != NULL){
		struct dirent **namelist;
		int listcnt;

		listcnt = get_dirlist(cur->dirpath, &namelist);
		pthread_mutex_lock(&mutex);
		//하위 디렉토리(fullpath)
		for (int i = 0; i < listcnt; i++){
			char fullpath[PATHMAX] = {0, };
			struct stat statbuf;
			int file_mode;
			long long filesize;

			if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
				continue;

			get_fullpath(cur->dirpath, namelist[i]->d_name, fullpath);

			if (!strcmp(fullpath,"/proc") || !strcmp(fullpath, "/run") || !strcmp(fullpath, "/sys") || !strcmp(fullpath, trash_path))
				continue;

			file_mode = get_file_mode(fullpath, &statbuf);

			if ((filesize = (long long)statbuf.st_size) == 0)
				continue;

			if (filesize < minbsize)
				continue;

			if (maxbsize != -1 && filesize > maxbsize)
				continue;


			if (file_mode == DIRECTORY)
				dirlist_append(subdirs, fullpath);
			else if (file_mode == REGFILE){
				FILE *fp;
				char filename[PATHMAX*2];
				char *path_extension;
				char hash[HASHMAX];

				sprintf(filename, "%s/%lld", same_size_files_dir, filesize);

				memset(hash, 0, HASHMAX);
				md5(fullpath, hash);//해쉬 얻기

				path_extension = get_extension(fullpath);

				if (strlen(extension) != 0){
					if (path_extension == NULL || strcmp(extension, path_extension))
						continue;
				}

				if ((fp = fopen(filename, "a")) == NULL){
					printf("ERROR: fopen error for %s\n", filename);
					return;
				}

				fprintf(fp, "%s %s\n", hash, fullpath);

				fclose(fp);
			}
		}
		cur = cur->next;
		pthread_mutex_unlock(&mutex);
	}

	dirlist_delete_all(dirlist);

	dirlist->next=subdirs->next;
	free(subdirs);
	if (dirlist->next != NULL)
		dir_traverse(dirlist);

}

void *record_thread(void *arg){
	FILE *fp;
	struct thread_data *data;
	char *filename;
	char *fullpath;
	char *hash;

	data=(struct thread_data *)arg;
	filename=data->filename;
	fullpath=data->fullpath;
	hash=data->hash;

	if ((fp = fopen(filename, "a")) == NULL){
		printf("ERROR: fopen error for %s\n", filename);
		exit(1);
	}

	fprintf(fp, "%s %s\n", hash, fullpath);

	fclose(fp);
}

void *regfile_thread(void *arg){
	FILE *fp;
	char *filename;
	char *fullpath;
	char *hash;
	thread_data *data;

//	pthread_mutex_lock(&mutex);
	data=(thread_data *)arg;
	filename=data->filename;
	fullpath=data->fullpath;
	hash=data->hash;
	printf("thread :%s\n",filename);
	if(hash==NULL)exit(1);
	if ((fp = fopen(filename, "a")) == NULL){
		printf("ERROR: fopen error for %s\n", filename);
	//	return;
	}

	fprintf(fp, "%s %s\n", hash, fullpath);

	fclose(fp);
//	pthread_mutex_unlock(&mutex);
}

void find_duplicates(void){
	struct dirent **namelist;
	int listcnt;
	char hash[HASHMAX];
	FILE *fp;

	listcnt = get_dirlist(same_size_files_dir, &namelist);

	for (int i = 0; i < listcnt; i++){
		char filename[PATHMAX*2];
		long long filesize;
		char filepath[PATHMAX];
		char hash[HASHMAX];
		char line[STRMAX];

		if (!strcmp(namelist[i]->d_name, ".") || !strcmp(namelist[i]->d_name, ".."))
			continue;

		filesize = atoll(namelist[i]->d_name);
		sprintf(filename, "%s/%s", same_size_files_dir, namelist[i]->d_name);

		if ((fp = fopen(filename, "r")) == NULL){
			printf("ERROR: fopen error for %s\n", filename);
			continue;
		}

		while (fgets(line, sizeof(line), fp) != NULL){
			int idx;

			strncpy(hash, line, HASHMAX);
			hash[HASHMAX-1] = '\0';

			strcpy(filepath, line+HASHMAX);
			filepath[strlen(filepath)-1] = '\0';

			//중복리스트에 없는거면 중복리스트에 추가
			if ((idx = filelist_search(dups_list_h, hash)) == 0)
				filelist_append(dups_list_h, filesize, filepath, hash);
			//중복리스트에 이미 있는거면 있는거에 추가
			else{
				fileList *filelist_cur = dups_list_h;
				while (idx--){
					filelist_cur = filelist_cur->next;
				}
				fileinfo_append(filelist_cur->fileInfoList, filepath);
			}
		}

		fclose(fp);
	}
}

//중복리스트에 1개만 있는거면 삭제
void remove_no_duplicates(void){
	fileList *filelist_cur = dups_list_h->next;

	while (filelist_cur != NULL){
		fileInfo *fileinfo_cur = filelist_cur->fileInfoList;

		if (fileinfolist_size(fileinfo_cur) < 2)
			filelist_delete_node(dups_list_h, filelist_cur->hash);

		filelist_cur = filelist_cur->next;
	}
}

time_t get_recent_mtime(fileInfo *head, char *last_filepath){
	fileInfo *fileinfo_cur = head->next;
	time_t mtime = 0;

	while (fileinfo_cur != NULL){
		if (fileinfo_cur->statbuf.st_mtime > mtime){
			mtime = fileinfo_cur->statbuf.st_mtime;
			strcpy(last_filepath, fileinfo_cur->path);
		}
		fileinfo_cur = fileinfo_cur->next;
	}
	return mtime;
}

//삭제 옵션 명령하는 프롬프트
void delete_prompt(void){
	while (filelist_size(dups_list_h) > 0){
		int flag_l=0,flag_d=0,flag_i=0,flag_f=0,flag_t=0;
		char *l_argv,*d_argv;
		int opt;
		char input[STRMAX];
		char last_filepath[PATHMAX];
		char modifiedtime[STRMAX];
		char *argv[3];
		int argc;
		int set_idx;
		time_t mtime = 0;
		fileList *target_filelist_p;
		fileInfo *target_infolist_p;

		optind=0;
		printf(">> ");

		fgets(input, sizeof(input), stdin);
		input[strlen(input)-1]='\0';

		if (!strcmp(input, "exit")){
			printf(">> Back to Prompt\n");
			break;
		}
		else if(!strcmp(input, "\n"))
			continue;

		argc = split(input," ",argv);

		if(argc>5||argc<4){
			printf("usage: delete -l [SET_IDX] -d [LIST_IDX]\n");
			printf("usage: delete -l [SET_IDX] -i\n");
			printf("usage: delete -l [SET_IDX] -f\n");
			printf("usage: delete -l [SET_IDX] -t\n");
			continue;
		}

		while((opt=getopt(argc,argv,"l:d:ift"))!=EOF){
			switch(opt){
				case 'l':
					flag_l=1;
					l_argv=optarg;
					break;
				case 'd':
					flag_d=1;
					d_argv=optarg;
					break;
				case 'i':
					flag_i=1;
					break;
				case 'f':
					flag_f=1;
					break;
				case 't':
					flag_t=1;
					break;
				case '?':
					printf("Unknown option : %c\n",optopt);
					continue;
			}
		}

		if(flag_l==0){
			printf("option error: must be included '-l'\n");
			continue;
		}

		if (!atoi(l_argv)){
			printf("ERROR: [SET_INDEX] should be a number\n");
			continue;
		}

		if (atoi(l_argv) < 0 || atoi(l_argv) > filelist_size(dups_list_h)){
			printf("ERROR: [SET_INDEX] out of range\n");
			continue;
		}

		target_filelist_p = dups_list_h->next;

		set_idx = atoi(l_argv);

		while (--set_idx){
			target_filelist_p = target_filelist_p->next;
		}

		target_infolist_p = target_filelist_p->fileInfoList;

		mtime = get_recent_mtime(target_infolist_p, last_filepath);
		sec_to_ymdt(localtime(&mtime), modifiedtime);

		set_idx = atoi(l_argv);

		if (flag_f==1){
			fileInfo *tmp;
			fileInfo *deleted = target_infolist_p->next;

			while (deleted != NULL){
				tmp = deleted->next;

				if (!strcmp(deleted->path, last_filepath)){
					deleted = tmp;
					continue;
				}
				logging(DELETE,deleted->path);
				remove(deleted->path);
				free(deleted);
				deleted = tmp;
			}

			filelist_delete_node(dups_list_h, target_filelist_p->hash);
			printf("Left file in #%d : %s (%s)\n\n", atoi(l_argv), last_filepath, modifiedtime);
		}
		else if(flag_t==1){
			FILE *fp;
			time_t curTime=time(NULL);
			char now[STRMAX];
			fileInfo *tmp;
			fileInfo *deleted = target_infolist_p->next;
			char move_to_trash[PATHMAX];
			char write_trash_info[PATHMAX];
			char filename[PATHMAX];


			while (deleted != NULL){
				tmp = deleted->next;

				if (!strcmp(deleted->path, last_filepath)){
					deleted = tmp;
					continue;
				}

				memset(move_to_trash, 0, sizeof(move_to_trash));
				memset(write_trash_info,0,sizeof(write_trash_info));
				memset(filename, 0, sizeof(filename));

				sprintf(move_to_trash, "%s%s", trash_path, strrchr(deleted->path, '/') + 1);
				sprintf(write_trash_info,"%s%s",trash_path_info,strrchr(deleted->path,'/')+1);

				if (access(move_to_trash, F_OK) == 0){
					get_new_file_name(deleted->path, filename);

					strncpy(strrchr(move_to_trash, '/') + 1, filename, strlen(filename));
					strncpy(strrchr(write_trash_info, '/') + 1, filename, strlen(filename));
				}
				else
					strcpy(filename, strrchr(deleted->path, '/') + 1);

				if (rename(deleted->path, move_to_trash) == -1){
					printf("ERROR: Fail to move duplicates to Trash\n");
					continue;
				}
				logging(REMOVE,deleted->path);
				strcat(write_trash_info,".trashinfo");

				if((fp=fopen(write_trash_info,"w"))==NULL){
					fprintf(stderr,"open error for %s\n",write_trash_info);
					continue;
				}

				sec_to_ymdt(localtime(&curTime),now);

				fprintf(fp,"%s#%s",deleted->path,now);

				free(deleted);
				deleted = tmp;
				fclose(fp);
			}

			filelist_delete_node(dups_list_h, target_filelist_p->hash);
			printf("All files in #%d have moved to Trash except \"%s\" (%s)\n\n", atoi(l_argv), last_filepath, modifiedtime);
		}
		else if(flag_i==1){
			char ans[STRMAX];
			fileInfo *fileinfo_cur = target_infolist_p->next;
			fileInfo *deleted_list = (fileInfo *)malloc(sizeof(fileInfo));
			fileInfo *tmp;
			int listcnt = fileinfolist_size(target_infolist_p);

			while (fileinfo_cur != NULL && listcnt--){
				printf("Delete \"%s\"? [y/n] ", fileinfo_cur->path);
				memset(ans, 0, sizeof(ans));
				fgets(ans, sizeof(ans), stdin);

				if (!strcmp(ans, "y\n") || !strcmp(ans, "Y\n")){
					logging(DELETE,fileinfo_cur->path);
					remove(fileinfo_cur->path);
					fileinfo_cur = fileinfo_delete_node(target_infolist_p, fileinfo_cur->path);
				}
				else if (!strcmp(ans, "n\n") || !strcmp(ans, "N\n"))
					fileinfo_cur = fileinfo_cur->next;
				else {
					printf("ERROR: Answer should be 'y/Y' or 'n/N'\n");
					break;
				}
			}
			printf("\n");

			if (fileinfolist_size(target_infolist_p) < 2)
				filelist_delete_node(dups_list_h, target_filelist_p->hash);

		}
		else if(flag_d==1){
			fileInfo *deleted;
			int list_idx;

			if (d_argv == NULL || (list_idx = atoi(d_argv)) == 0){
				printf("ERROR: There should be an index\n");
				continue;
			}

			if (list_idx < 0 || list_idx > fileinfolist_size(target_infolist_p)){
				printf("ERROR: [LIST_IDX] out of range\n");
				continue;
			}

			deleted = target_infolist_p;

			while (list_idx--)
				deleted = deleted->next;

			printf("\"%s\" has been deleted in #%d\n\n", deleted->path, atoi(l_argv));
			logging(DELETE,deleted->path);
			remove(deleted->path);
			fileinfo_delete_node(target_infolist_p, deleted->path);

			if (fileinfolist_size(target_infolist_p) < 2)
				filelist_delete_node(dups_list_h, target_filelist_p->hash);
		}
		else {
			printf("ERROR: Only f, t, i, d options are available\n");
			continue;
		}

		filelist_print_format(dups_list_h);
	}
}

//로그 기록 남기기
void logging(int cmd,char *log_file){
	FILE *fp;
	time_t curTime=time(NULL);
	char now[STRMAX];
	struct passwd *pwd;

	if((fp=fopen(log_path,"a"))==NULL){
		fprintf(stderr,"fopen error for %s\n",log_path);
		exit(1);
	}

	if(cmd==REMOVE)
		fprintf(fp,"%s ","[REMOVE]");
	else if(cmd==DELETE)
		fprintf(fp,"%s ","[DELETE]");
	else if(cmd==RESTORE)
		fprintf(fp,"%s ","[RESTORE]");

	sec_to_ymdt(localtime(&curTime),now);

	if((pwd=getpwuid(getuid()))==NULL){
		if(errno==0||errno == ENOENT || errno == ESRCH || errno == EBADF || errno == EPERM) { 
			printf("미등록 된 사용자입니다.\n"); 
			return ; 
		} 
		else { 
			printf("error: %s\n", strerror(errno)); 
			return ; 
		} 
	}
	
	fprintf(fp,"%s %s %s\n",log_file,now,pwd->pw_name);
	fclose(fp);
}

//단위를 byte로 바꿔주는 함수
long long get_size(char *filesize){
	double size_bytes = 0;
	char size[STRMAX] = {0, };
	char size_unit[4] = {0, };
	int size_idx = 0;

	if (!strcmp(filesize, "~"))
		size_bytes = -1;
	else {
		for (int i = 0; i < strlen(filesize); i++){
			if (isdigit(filesize[i]) || filesize[i] == '.'){
				size[size_idx++] = filesize[i];
				if (filesize[i] == '.' && !isdigit(filesize[i + 1]))
					return SIZE_ERROR;
			}
			else {
				strcpy(size_unit, filesize + i);
				break;
			}
		}

		size_bytes = atof(size);

		if (strlen(size_unit) != 0){
			if (!strcmp(size_unit, "kb") || !strcmp(size_unit, "KB"))
				size_bytes *= KB;
			else if (!strcmp(size_unit, "mb") || !strcmp(size_unit, "MB"))
				size_bytes *= MB;
			else if (!strcmp(size_unit, "gb") || !strcmp(size_unit, "GB"))
				size_bytes *= GB;
			else if (!strcmp(size_unit, "kib") || !strcmp(size_unit, "KiB"))
				size_bytes *= KiB;
			else if (!strcmp(size_unit, "mib") || !strcmp(size_unit, "MiB"))
				size_bytes *= MiB;
			else if (!strcmp(size_unit, "gib") || !strcmp(size_unit, "GiB"))
				size_bytes *= GiB;
			else
				return SIZE_ERROR;
		}
	}

	return (long long)size_bytes;
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
	printf("Usage:\n > fmd5 -e [FILE_EXTENSION] -l [MINSIZE] -h [MAXSIZE] -d [TARGET_DIRECTORY] -t [THREAD_NUM]\n");
	printf("      *fmd5 is file dup list find and print*\n");
	printf("      >> delete -l [SET_INDEX] -d [OPTARG] -i -f -t\n");
	printf("             *delete is file dup list delete*\n");
	printf(" > list -l [FILETYPE] -c [CATEGORY] -o [ORDER]\n");
	printf("      *list is file dup list sort and print*\n");
	printf(" > trash -c [CATEGORY] -o [ORDER]\n");
	printf("      *trash is user_trash print*\n");
	printf(" > restore [RESTORE_INDEX]\n");
	printf("      *restore is the file trash to origin path*\n");
	printf(" > help : prompt command explicate\n");
	printf(" > exit : ssu_sfinder exit\n");
}

