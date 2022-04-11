#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <openssl/md5.h>

#define TRUE 1
#define FALSE 0
#define ARGMAX 5
#define BUFMAX 1024
#define FILEMAX 255
#define PATHMAX BUFMAX*4
#define REGFILE 1
#define DIRECTORY 2
#define EXTENTIONMAX 7

//BFS 디렉토리 정보
typedef struct dirinfo{
	char path[PATHMAX];
	int depth;
} dirinfo;

//파일 정보
typedef struct fileinfo{
	char filename[FILEMAX];
	char path[PATHMAX];
	long long size;
	int depth;
	unsigned char md[MD5_DIGEST_LENGTH];
} fileinfo;

//디렉토리정보 node
typedef struct d_node{
	dirinfo data;
	struct d_node *next;
} d_node;

//파일정보 node
typedef struct f_node{
	fileinfo data;
	struct f_node *next;
} f_node;
	
typedef struct lpath_queue{
	d_node *front;
	d_node *rear;
} lpath_queue;

typedef lpath_queue *path_queue;

fileinfo target_dir;//[TARGET_DIRECTORY]
int minsize_tilde;//[MINSIZE] ~ 일때 구분
int maxsize_tilde;//[MAXSIZE] ~ 일때 구분
long long minsize,maxsize;//[MINSIZE],[MAXSIZE] 범위
char extention[EXTENTIONMAX];//[EXTENSION] 확장명


void read_directory(char *dirname);
void Qinit(path_queue pq);
int Qempty(path_queue pq);
void Qpush(path_queue pq, dirinfo data);
void Qpop(path_queue pq);
void Qpeek(path_queue pq,dirinfo *f_dir);
int split(char *string, char *seperator, char *argv[]);
long long unit_to_byte(char *argv);
long long file_size(char *path);
