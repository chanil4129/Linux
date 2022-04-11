#include "ssu_h.h"

int main(int argc,char *argv[]){
	char dirname[PATHMAX];
	struct stat statbuf;
	int length;

	if(argc!=ARGMAX){
		printf("ERROR: Arguments error\n");
		exit(1);
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
        exit(1);
    }   
    
    //[MINSIZE]
    minsize_tilde=0;
    length=strlen(argv[2]);
    if(length==1||argv[2][0]=='~') minsize_tilde=1;
    else minsize=unit_to_byte(argv[2]);
    if(minsize==-1){
        printf("ERROR: MINSIZE error\n");
        exit(1);
    }   

    //[MAXSIZE]
    maxsize_tilde=0;
    length=strlen(argv[3]);
    if(length==1||argv[3][0]=='~') maxsize_tilde=1;
    else maxsize=unit_to_byte(argv[3]);
    if(maxsize==-1){
        printf("ERROR: MAXSIZE error\n");
        exit(1);
    }

    //[TARGET_DIR]
    if(realpath(argv[4],dirname)==NULL){
        printf("ERROR: Path exist error\n");
        exit(1);
    }

    if(lstat(dirname,&statbuf)<0){
        fprintf(stderr,"lstat error for %s\n",dirname);
        exit(1);
    }

    if(!S_ISDIR(statbuf.st_mode)){
        printf("ERROR: Path must be directory\n");
        exit(1);
    }

	read_directory(dirname);

	exit(0);
}

//디렉토리,파일 탐색하면서 파일 내용 저장
void read_directory(char *dirname){
	int count;
	struct stat statbuf;
	struct dirent **namelist;
	lpath_queue q;
	int depth=-1;
	dirinfo root_dir;
	strcpy(root_dir.path,dirname);
	root_dir.depth=depth;

	//BFS
	Qinit(&q);
	Qpush(&q,root_dir);
	while(!Qempty(&q)){
		dirinfo f_dir;
		Qpeek(&q,&f_dir);
		Qpop(&q);
		depth=f_dir.depth+1;
		
		if(access(f_dir.path,F_OK)!=0){
			if(errno==13)
				return;
			fprintf(stderr,"access error for %s\n",f_dir.path);
			exit(1);
		}

		if((count=scandir(f_dir.path,&namelist,NULL,alphasort))==-1){
			if(errno==1)
				return;
			fprintf(stderr,"scandir error for %s\n",f_dir.path);
			exit(1);
		}

		for(int i=0;i<count;i++){
			dirinfo r_dir;
			if(!strcmp(namelist[i]->d_name,".")||!strcmp(namelist[i]->d_name,"..")) continue;

			strcpy(r_dir.path,f_dir.path);
			if(!strcmp(f_dir.path,"/")){
				if(!strcmp(namelist[i]->d_name,"proc")) continue;
				if(!strcmp(namelist[i]->d_name,"run")) continue;
                if(!strcmp(namelist[i]->d_name,"sys")) continue;
            }
            else
                strcat(r_dir.path,"/");
            strcat(r_dir.path,namelist[i]->d_name);

			if(!strcmp(r_dir.path,root_dir.path))
				continue;

			if((lstat(r_dir.path,&statbuf)<0)&&(!access(r_dir.path,F_OK))){
				if(errno==1)
					continue;
				fprintf(stderr,"lstat error for %s\n",r_dir.path);
                exit(1);
            }

			if(S_ISDIR(statbuf.st_mode)){
				r_dir.depth=depth;
				Qpush(&q,r_dir);
				printf("%d: %s\n",depth,r_dir.path);
			}
			else if(S_ISREG(statbuf.st_mode)){
				
			}
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
void Qpush(path_queue pq, dirinfo data){
    d_node *newNode=(d_node *)malloc(sizeof(d_node));
    newNode->next=NULL;
    newNode->data=data;
	strcpy(newNode->data.path,data.path);
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
void Qpop(path_queue pq){
    d_node *delNode;

    if(Qempty(pq)){
        printf("Queue Memory Error\n");
        exit(1);
    }

    delNode=pq->front;
    pq->front=pq->front->next;

    free(delNode);
}

//큐 peek 함수
void Qpeek(path_queue pq,dirinfo *dir){
    if(Qempty(pq)){
        printf("Queue Memory Error\n");
        exit(1);
    }   
	strcpy(dir->path,pq->front->data.path);
	dir->depth=pq->front->data.depth;
}
