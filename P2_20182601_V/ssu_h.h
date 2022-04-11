#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

#define TRUE 1
#define FALSE 0
#define ARGMAX 5
#define BUFMAX 1024
#define FILEMAX 255
#define PATHMAX BUFMAX*4
#define REGFILE 1
#define DIRECTORY 2
#define FMD5 3
#define FSHA1 4
#define EXTENTIONMAX 7

//파일 정보
typedef struct fileinfo{
	char filename[FILEMAX];
   	char path[PATHMAX];
	char extention[7];
	long long size;
	int type;
	int depth;
	unsigned char md[MD5_DIGEST_LENGTH];
	unsigned char sh[SHA_DIGEST_LENGTH];
	struct fileinfo *next;
	struct fileinfo *set;
}fileinfo;

typedef struct node{
	fileinfo data;
	struct node *next;
	struct node *set;
} node;

//해쉬값 같은 것들 연결리스트
typedef struct ldup_list{
	node *front;
	node *rear;
	node *parent;
	node *son;
}ldup_list;

//하위 디렉토리 탐색할 때 Queue
typedef struct lpath_queue{
	node *front;
	node *rear;
}lpath_queue;

typedef ldup_list *dup_list;
typedef lpath_queue *path_queue;

fileinfo target_dir;//[TARGET_DIRECTORY]
int minsize_tilde;//[MINSIZE] ~ 일때 구분
int maxsize_tilde;//[MAXSIZE] ~ 일때 구분
long long minsize,maxsize;//[MINSIZE],[MAXSIZE] 범위
int md5_sha1;//FMD5과 FSHA1 사용 구분
char extention[EXTENTIONMAX];//[EXTENSION] 확장명

void command_find(int argc, char *argv[]);
void command_md5(char *file_path,char * f_md);
void command_sha1(char *file_path,char *f_md);
void command_help(void);
void read_directory();
int split(char *string, char *seperator, char *argv[]);
long long unit_to_byte(char *argv);
void Qinit(path_queue pq);
int Qempty(path_queue pq);
void Qpush(path_queue pq, fileinfo data);
void Qpop(path_queue pq);
void Qpeek(path_queue pq,fileinfo *dir);
void Binit(dup_list dl);
int Bempty(dup_list dl);
void Bpush(dup_list dl,fileinfo data);
void Bpop(dup_list dl);
void Bpeek(dup_list dl,fileinfo *data);
