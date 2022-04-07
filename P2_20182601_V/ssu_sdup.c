#include "ssu_h.h"

int main(void){
	int argc=0;
	char input[BUFMAX];
	char *argv[ARGMAX];

	//프롬프트 시작
	while(1) {
		printf("20182601> ");
		fgets(input,sizeof(input),stdin);
		input[strlen(input)-1]='\0';
		argc=split(input," ",argv);

		if(argc==0)
			continue;
		if(!strcmp(argv[0],"fmd5")||!strcmp(argv[0],"sha1"))
			command_find(argc,argv);
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

//파일 탐색
void command_find(int argc, char *argv[]){
	char dirname[PATHMAX];
	struct stat statbuf;
	fileinfo filelist[BUFMAX];
	long long minsize,maxsize;
	int length;

	if(argc!=ARGMAX){
		printf("ERROR: Arguments error\n");
		return;
	}

	//[FILE_EXTENSION]
	char *ptr;
	length=strlen(argv[1]);
	if(length==1||argv[1][0]=='*') extention_tilde=1;
	else if(argv[1][0]=='*'&&argv[1][1]=='.'){
		ptr=argv[1]+2;
		strcpy(extention,ptr);
	}
	else{
		printf("ERROR: FILE_EXTENSION error\n");
		return;
	}
	
	//[MINSIZE]
	length=strlen(argv[2]);
	if(length==1||argv[2][0]=='~') minsize_tilde=1;
	else minsize=unit_to_byte(argv[2]);
	if(minsize==-1){
		printf("ERROR: MINSIZE error\n");
		return;
	}

	//[MAXSIZE]
	length=strlen(argv[3]);
	if(length==1||argv[3][0]=='~') maxsize_tilde=1;
	else maxsize=unit_to_byte(argv[3]);
	if(maxsize==-1){
		printf("ERROR: MAXSIZE error\n");
		return;
	}

	//[TARGET_DIR]
	if(realpath(argv[4],dirname)==NULL){
		printf("ERROR: Path exist error\n");
		return;
	}

	if(lstat(dirname,&statbuf)<0){
		fprintf(stderr,"lstat error for %s\n",dirname);
		exit(1);
	}

	if(!S_ISDIR(statbuf.st_mode)){
		printf("ERROR: Path must be directory\n");
		return;
	}

	strcpy(target_dir.path,dirname);
	target_dir.type=DIRECTORY;
	target_dir.depth=-1;
	filelist[0]=target_dir;

	read_directory(0,dirname,filelist);	

//	if(){}//해쉬 동일한 값이 없다면..
	
	while(1){//옵션 적용하기
	}

	extention_tilde=minsize_tilde=maxsize_tilde=0;
	memset(extention,0,sizeof(char)*EXTENTIONMAX);

}

//단위를 바이트로 바꿔 주는 함수
long long unit_to_byte(char *argv){
	char *unit;
	int length;
	char *arr[20];
	int count;
	int i=0;

	length=strlen(argv);
	for(i=0;i<length;i++) if(argv[i]<'0'||argv[i]>'9'||argv[i]!='.') break;
	
	unit=argv+i;
	//byte
	if((length-i)==0||!strcmp(unit,"BYTE")||!strcmp(unit,"byte")){
		return atoll(argv);
	}
	//kb,mb,gb
	count=split(argv,".",arr);
	if(count!=1||count!=2){
		return -1;
	}
	if(!strcmp(unit,"KB")||!strcmp(unit,"kb")) {
		for(i=0;i<3;i++){
			if(count==1)
				arr[1][i]='0';
		}
		arr[1][3]='\0';
	}
	else if(!strcmp(unit,"MB")||!strcmp(unit,"mb")) {
		for(i=0;i<6;i++){
			if(count==1)
				arr[1][i]='0';
		}
		arr[1][6]='\0';
	}
	else if(!strcmp(unit,"GB")||!strcmp(unit,"gb")) {
		for(i=0;i<9;i++){
			if(count==1)
				arr[1][i]='0';
		}
		arr[1][9]='\0';
	}
	else
		return -1;
	strcat(arr[0],arr[1]);
	return atoll(arr[0]);
}

//하위 경로 탐색 함수
void read_directory(int depth,char *dirname,fileinfo *filelist){
	int count;
	char pathname[PATHMAX];
	struct stat statbuf;
	struct dirent **namelist;
	lpath_queue q;

	if(access(dirname, F_OK)!=0){
		if(errno==13)
			return;
		fprintf(stderr,"access error for %s\n",dirname);
		exit(1);
	}
	
	//BFS
	Qinit(&q);
	Qpush(&q,filelist[0]);
	while(!Qempty(&q)){
		if((count=scandir(dirname,&namelist,NULL,alphasort))==-1){
			if(errno==1)
				return;//고쳐야됨
			fprintf(stderr,"scandir error for %s\n",dirname);
		}
	}
}

//큐 init 함수
void Qinit(path_queue pq){
	pq->front=NULL;
	pq->rear=NULL;
}

//큐 empty 함수
int Qempty(path_queue pq){
	if(pq->front==NULL)
		return TRUE;
	else
		return FALSE;
}

//큐 push 함수
void Qpush(path_queue pq, fileinfo data){
	node *newNode=(node *)malloc(sizeof(node));
	newNode->next=NULL;
	newNode->data=data;
	strcpy(newNode->data.path,data.path);
	newNode->data.type=data.type;
	newNode->data.depth=data.depth;

	if(Qempty(pq)){
		pq->front=newNode;
		pq->rear=newNode;
	}
	else{
		pq->rear->next=newNode;
		pq->rear=newNode;
	}
}

//큐 pop 함수
fileinfo Qpop(path_queue pq){
	node *delNode;
	fileinfo retfile;

	if(Qempty(pq)){
		printf("Queue Memory Error\n");
		exit(1);
	}

	delNode=pq->front;
	retfile=delNode->data;
	pq->front=pq->front->next;

	free(delNode);
	return retfile;
}

//큐 peek 함수
fileinfo Qpeek(path_queue pq){
	if(Qempty(pq)){
		printf("Queue Memory Error\n");
		exit(1);
	}

	return pq->front->data;
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


