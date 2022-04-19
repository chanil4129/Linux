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

#define TRUE 1
#define FALSE 0
#define ARGMAX 5
#define BUFMAX 1024
#define BUFSIZE 1024*16
#define FILEMAX 255
#define PATHMAX BUFMAX*4
#define REGFILE 1
#define DIRECTORY 2
#define EXTENSIONMAX 7
#define HASHMAX 200

//BFS 디렉토리 정보
typedef struct dirinfo{
	char path[PATHMAX];
	int depth;
} dirinfo;

//파일 정보
typedef struct fileinfo{
	char name[FILEMAX];
	char path[PATHMAX];
	long long size;
	int depth;
	unsigned char md[HASHMAX];
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
	
//큐
typedef struct lpath_queue{
	d_node *front;
	d_node *rear;
} lpath_queue;

typedef lpath_queue *path_queue;

fileinfo target_dir;//[TARGET_DIRECTORY]
int minsize_tilde;//[MINSIZE] ~ 일때 구분
int maxsize_tilde;//[MAXSIZE] ~ 일때 구분
long long minsize,maxsize;//[MINSIZE],[MAXSIZE] 범위
char extension[EXTENSIONMAX];//[EXTENSION] 확장명


void read_directory(char *dirname);//디렉토리를 탐색하며 파일들을 파일리스트에 넣는 함수
void Qinit(path_queue pq);//큐 초기화 함수
int Qempty(path_queue pq);//큐 비어있는지 확인하는 함수
void Qpush(path_queue pq, dirinfo data);//큐에 노드를 넣는 함수
void Qpop(path_queue pq);//큐에 노드를 빼는 함수
void Qpeek(path_queue pq,dirinfo *f_dir);//큐에 뽑아낼 순서의 노드 정보를 가져오는 함수
void Dpush(fileinfo f);//노드를 추가하여 정규파일의 정보를 저장하고 파일리스트에 추가
void Dpop(int idx,int s_idx);//중복 리스트에 있는 노드를 삭제하고, 해당 파일도 삭제
void Dtrash(int idx,int s_idx);//중복 리스트에 있는 노드를 삭제하고, 해당 파일을 휴지통으로 이동
int split(char *string, char *seperator, char *argv[]);//문자열을 공백에 따라 분리해주는 함수
long long unit_to_byte(char *argv);//단위를 모두 byte로 바꿔주는 함수
void Qsort(int start, int end);//파일리스트를 사이즈, 사이즈가 같다면 경로에 따라서 정렬해주는 함수
void print_dup(char *dirname);//중복리스트를 생성·추가 해주고, 그 리스트를 출력해주는 함수
void print_size(long long size,char *p_size);//출력할 파일의 사이즈를 3자리 마다 ,를 찍는 함수
char* get_time(time_t stime);//파일 시간 출력을 위한 함수
void path_file_extract(char *f,int idx, int d_idx);//중복 리스트를 탐색하여 경로와 파일 이름을 문자열에 저장해주는 함수이다.
long long Integer_time(time_t stime);//최근 수정 시간을 정수로 나타내는 함수
void file_extract(char *f,int idx,int d_idx);//중복 리스트를 탐색하여 파일 이름만 문자열에 저장해주는 함수이다.
