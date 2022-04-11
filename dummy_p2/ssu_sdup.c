#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
		if(!strcmp(argv[0],"fmd5")){
//			command_find(argc,argv);
		}
		else if(!strcmp(argv[0],"fsha1")){
//			command_find(argc,argv);
		}
		else if(!strcmp(argv[0],"help"))
//			command_help();
		else if(!strcmp(argv[0],"exit")){
			printf("Prompt End\n");
			break;
		}
		else
//			command_help();
	}

	exit(0);
}

//파일 탐색
void command_find(int argc, char *argv[]){
	char dirname[PATHMAX];
	struct stat statbuf;
//	fileinfo filelist[BUFMAX];
	int length;

	if(argc!=ARGMAX){
		printf("ERROR: Arguments error\n");
		return;
	}

	//[FILE_EXTENSION]
	char *ptr;
	length=strlen(argv[1]);
	if(!strcmp(argv[1],"*")) strcpy(extention,argv[1]);
	else if(argv[1][0]=='*'&&argv[1][1]=='.'){
		ptr=argv[1]+2;
		strcpy(extention,ptr);
	}
	else{
		printf("ERROR: FILE_EXTENSION error\n");
		return;
	}
	
	//[MINSIZE]
	minsize_tilde=0;
	length=strlen(argv[2]);
	if(length==1||argv[2][0]=='~') minsize_tilde=1;
	else minsize=unit_to_byte(argv[2]);
	if(minsize==-1){
		printf("ERROR: MINSIZE error\n");
		return;
	}

	//[MAXSIZE]
	maxsize_tilde=0;
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
//	filelist[0]=target_dir;

//	read_directory(0,dirname,filelist);	
	read_directory();	
	return;//temp

//	if(){}//해쉬 동일한 값이 없다면..
	
	while(1){//옵션 적용하기
	}

//	extention_tilde=minsize_tilde=maxsize_tilde=0;
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
//void read_directory(int depth,char *dirname,fileinfo *filelist){
void read_directory(){
	int count;
//	char pathname[PATHMAX];
	struct stat statbuf;
	struct dirent **namelist;
	lpath_queue q;
	int depth=0;

	/*
	if(access(dirname, F_OK)!=0){
		if(errno==13)
			return;
		fprintf(stderr,"access error for %s\n",dirname);
		exit(1);
	}
	*/
	
	//BFS
	Qinit(&q);
	Qpush(&q,target_dir);
	while(!Qempty(&q)){
		fileinfo f_dir;
		Qpeek(&q,&f_dir);
//		printf("%s\n",f_dir.path);
		Qpop(&q);
		
		if(access(f_dir.path,F_OK)!=0){
			if(errno==13)
				return;
			fprintf(stderr,"access error for %s\n",f_dir.path);
			exit(1);
		}

//		printf("f_dir.path: %s\n",f_dir.path);
		if((count=scandir(f_dir.path,&namelist,NULL,alphasort))==-1){
			if(errno==1)
				return;
			fprintf(stderr,"scandir error for %s\n",f_dir.path);
			exit(1);
		}

		for(int i=0;i<count;i++){
			fileinfo r_file;
			if(!strcmp(namelist[i]->d_name,".")||!strcmp(namelist[i]->d_name,"..")) continue;

			/*
			if(!strcmp(dirname,"/"))
				sprintf(r_file.path,"%s%s",f_dir.path,namelist[i]->d_name);
			else
				sprintf(r_file.path,"%s/%s",f_dir.path,namelist[i]->d_name);
			*/
			strcpy(r_file.path,f_dir.path);
			if(!strcmp(f_dir.path,"/")){
				if(!strcmp(namelist[i]->d_name,"proc")) continue;
				if(!strcmp(namelist[i]->d_name,"run")) continue;
				if(!strcmp(namelist[i]->d_name,"sys")) continue;
			}
			else
				strcat(r_file.path,"/");
			strcat(r_file.path,namelist[i]->d_name);
				

			//원본 파일이면 스킵
			if(!strcmp(r_file.path,target_dir.path))
				continue;

//			memset(&statbuf,0,sizeof(struct stat));
			if((lstat(r_file.path,&statbuf)<0)&&(!access(r_file.path,F_OK))){
				if(errno==1)
					continue;
				fprintf(stderr,"lstat error for %s\n",r_file.path);
				exit(1);
			}

			if(S_ISDIR(statbuf.st_mode)){
				r_file.type=DIRECTORY;
				r_file.depth=depth;
				Qpush(&q,r_file);
			}
			else if(S_ISREG(statbuf.st_mode)){
				//extention 추가. extension에 *또는 확장명 들어있음
				if(minsize_tilde==0&&statbuf.st_size<=minsize||maxsize_tilde==0&&statbuf.st_size>=maxsize) continue;
				strcpy(r_file.filename,r_file.path);
				strcpy(r_file.path,f_dir.path);
				r_file.size=statbuf.st_size;
				r_file.type=REGFILE;
				r_file.depth=++depth;
				//리스트 push
				if(md5_sha1==FMD5){
					command_md5(r_file.filename,r_file.md);
					/*
					printf("%s: ",r_file.filename);
					for(i=0;i<MD5_DIGEST_LENGTH;i++)
						printf("%02x",r_file.md[i]);
					printf("\n");
					*/
				}
				else if(md5_sha1==FSHA1){
					command_sha1(r_file.filename,r_file.sh);
					/*
					printf("%s: ",r_file.filename);
					for(i=0;i<SHA_DIGEST_LENGTH;i++)
						printf("%02x",r_file.sh[i]);
					printf("\n");
					*/
				}
			}
//			printf("%s: %s\n",r_file.path,r_file.md);

			/*
			printf("%s: ",r_file.path);
			for(i=0;i<MD5_DIGEST_LENGTH;i++)
				printf("%02x",r_file.md[i]);
			printf("\n");
			*/
			/*
			printf("%s: ",r_file.path);
			char s[MD5_DIGEST_LENGTH];
			sprintf(s,"%X\n",r_file.md);
			printf(s);
			*/

//			printf("%s\n",r_file.path);
		}
	}
}

void Binit(dup_list dl){
	dl->front=(node*)malloc(sizeof(node));//더미노드 생성
//	dl->front->next=NULL;
	dl->rear=NULL;
	dl->parent=NULL;
	dl->son=NULL;

}
int Bempty(dup_list dl){
	if(dl->front->next==NULL)
		return TRUE;
	else
		return FALSE;
}
void Bpush(dup_list dl,fileinfo data){
	node *comp=dl->front;
	node *newNode=(node *)malloc(sizeof(node));
	newNode->next=NULL;
	newNode->set=NULL;
	newNode->data=data;
	strcpy(newNode->data.filename,data.filename);
	strcpy(newNode->data.path,data.path);
	strcpy(newNode->data.extention,data.extention);
	newNode->data.size=data.size;
	newNode->data.type=data.type;
	if(md5_sha1==FMD5){
		strcpy(newNode->data.md,data.md);
	}
	else if(md5_sha1==FSHA1){
		strcpy(newNode->data.sh,data.sh);
	}
	
	if(Bempty(dl)){
		dl->front->next=newNode;
		dl->rear->next=newNode;
	}
	else{
		/*
		dl->rear->next=newNode;
		dl->rear-newNode;
		*/
		while(comp->next==NULL){
			if(comp->next->data.size==newNode->data.size){
				if((md5_sha1==FMD5&&!strcmp(newNode->data.md,comp->next->data.md))||(md5_sha1==FSHA1&&!strcmp(newNode->data.sh,comp->next->data.sh))){
					int check=0;
					if(comp->next->set==NULL){
						comp->next->set=(node*)malloc(sizeof(node));
						comp->next->set->set=NULL;
						comp->next->set->data.depth=-1;
					}
					else {
					//	node s_comp=comp->next->set;
						while(comp->next->set->set==NULL){
							if(comp->next->set->next->data.depth>newNode->data.depth){
								newNode->next=comp->next->set->next;
								comp->next->set->next=newNode;
								check=1;
								break;
							}
							comp->next->set=comp->next->set->set;
						}
					}
					if(check) break;
				}
				else{
					if(comp->next->data.depth>newNode->data.depth){
						newNode->next=comp->next;
						comp->next=newNode;
						break;
					}
				}
			}
			if(comp->next->data.size>newNode->data.size){
				newNode->next=comp->next;
				comp->next=newNode;
				break;
			}
			/*
			if(md5_sha1==FMD5){
				if(strcmp(newNode->data.md,comp->data.md)
			}
			else if(md5_sha1==FSHA1){
				strcmp(newNode->data.sh,comp->data.sh);
			}
			*/
			comp=comp->next;
		}
	}
}
			
void Bpop(dup_list dl){

}
void Bpeek(dup_list dl,fileinfo *data){
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
//	printf("%s\n",data.path);

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
void Qpop(path_queue pq){
	node *delNode;

	if(Qempty(pq)){
		printf("Queue Memory Error\n");
		exit(1);
	}

	delNode=pq->front;
	pq->front=pq->front->next;

	free(delNode);
}

//큐 peek 함수
void Qpeek(path_queue pq,fileinfo *dir){
	if(Qempty(pq)){
		printf("Queue Memory Error\n");
		exit(1);
	}
	strcpy(dir->path,pq->front->data.path);
	dir->type=pq->front->data.type;
	dir->depth=pq->front->data.depth;
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


