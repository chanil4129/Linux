#include "ssu_h.h"

#if !defined(_OSD_POSIX) && !defined(__DJGPP__)
//int read(int, void *, unsigned int);
#endif

f_node **file_list;
int file_list_count;

void fmd5(char *file_path,unsigned char *md);

int main(int argc,char *argv[]){
	char dirname[PATHMAX];
	struct stat statbuf;
	int length;
	struct timeval begin_t,end_t;
	char dirbuf[PATHMAX];

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
	dirbuf[0]=0;
	if(argv[4][0]=='~'){
		strcpy(dirbuf,"/home");
		argv[4]++;
	}
	strcat(dirbuf,argv[4]);

	printf("%s\n",dirbuf);

    if(realpath(dirbuf,dirname)==NULL){
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

	gettimeofday(&begin_t,NULL);
	read_directory(dirname);
	gettimeofday(&end_t,NULL);

	end_t.tv_sec-=begin_t.tv_sec;
	if(end_t.tv_usec<begin_t.tv_usec){
		end_t.tv_sec--;
		end_t.tv_usec+=1000000;
		}
	end_t.tv_usec-=begin_t.tv_usec;

	Qsort(0,file_list_count-1);

	print_dup(dirname);
	printf("Seraching time: %ld:%06ld\n\n",end_t.tv_sec,end_t.tv_usec);
	/*
	for(int i=0;i<file_list_count;i++){
		printf("%lld  %s\n",file_list[i]->data.size,file_list[i]->next->data.name);
		//printf("%s\n",file_list[i]->data.md);
		//printf("%s\n",file_list[i]->next->data.md);
	}
	*/
		
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
//				printf("%d: %s\n",depth,r_dir.path);
			}
			else if(S_ISREG(statbuf.st_mode)){
				if(statbuf.st_size==0){
					remove(r_dir.path);
					continue;
				}
				//[EXTENSION],[MINSIZE],[MAXSIZE] 확인
				char *ext;
				strcpy(ext,namelist[i]->d_name);
				for(int i=0;i<strlen(namelist[i]->d_name);i++){
					if(ext[i]=='.') {
						ext+=(i+1);
						break;
					}
				}
				
				if(strcmp(extension,"*")){
					strcmp(extension,ext)
					continue;
				}
				
				

				//조건에 맞다면 file_list에 push
				fileinfo file;
				strcpy(file.name,namelist[i]->d_name);
				strcpy(file.path,f_dir.path);
				file.size=(long long)statbuf.st_size;
				file.depth=depth;
				fmd5(r_dir.path,file.md);
				/*
				//DEBUG
				printf("%s/%s   %lld    %d   ",file.path,file.name,file.size,file.depth);
				for(int i=0;i<MD5_DIGEST_LENGTH;i++)
					printf("%02x",file.md[i]);
				printf("\n");
				*/
				Dpush(file);
			}
		}
	}
}

//md5 명령어 실행 함수
void fmd5(char *file_path,unsigned char *md){
	FILE *IN;
	MD5_CTX c;
	int fd;
	int i;
	static unsigned char buf[BUFSIZE];
	
	IN=fopen(file_path,"r");
    if(IN==NULL){
        perror(file_path);
        fprintf(stderr,"md5 error for %s\n",file_path);
        exit(1);
    }

    fd=fileno(IN);
    MD5_Init(&c);
    while(1){
        i=read(fd,buf,BUFSIZE);
        if(i<=0) break;
        MD5_Update(&c,buf,(unsigned long)i);
    }
    MD5_Final(&(md[0]),&c);

    /*
    for(i=0;i<MD5_DIGEST_LENGTH;i++)
        printf("%02x",md[i]);
    printf("\n");
    */
    fclose(IN);
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

void Dpush(fileinfo f){
	f_node *newNode=(f_node *)malloc(sizeof(f_node));
	newNode->next=NULL;
	newNode->data=f;
	strcpy(newNode->data.name,f.name);
	strcpy(newNode->data.path,f.path);
	newNode->data.size=f.size;
	newNode->data.depth=f.depth;
	strcpy(newNode->data.md,f.md);

	if(file_list_count==0){
		f_node *flagNode=(f_node *)malloc(sizeof(f_node));
		flagNode->next=NULL;
		flagNode->data.size=f.size;
		flagNode->data.depth=f.depth;
		strcpy(flagNode->data.md,f.md);

		file_list=(f_node **)malloc(sizeof(f_node *));
		file_list[0]=flagNode;
		file_list_count++;

		flagNode->next=newNode;
	}
	else{
		f_node *cur;
		int i=0;
		for(i=0;i<file_list_count;i++){
			if(!strcmp(file_list[i]->data.md,f.md)){
				cur=file_list[i];
				while(cur->next!=NULL)
					cur=cur->next;
				cur->next=newNode;
				return;
			}
		}
		
		f_node *flagNode=(f_node *)malloc(sizeof(f_node));
		flagNode->next=NULL;
		flagNode->data.size=f.size;
		flagNode->data.depth=f.depth;
		strcpy(flagNode->data.md,f.md);
		
		file_list_count++;
		file_list=realloc(file_list,sizeof(f_node *)*file_list_count);
		file_list[i]=flagNode;

		flagNode->next=newNode;
	}
}

void Qsort(int start, int end){
	if(start>=end){
		return;
	}

	int key=start;
	int i=start+1,j=end;
	f_node *ptr;

	while(i<=j){
		while(i<=end&&file_list[i]->data.size<=file_list[key]->data.size){
			if(file_list[i]->data.size==file_list[key]->data.size&&file_list[i]->data.depth>file_list[key]->data.depth)
				break;
			i++;
		}
		while(j>start&&file_list[j]->data.size>=file_list[key]->data.size){
			if(file_list[j]->data.size==file_list[key]->data.size&&file_list[j]->data.depth<file_list[key]->data.depth)
				break;
			j--;
		}
		if(i>j){
			ptr=file_list[j];
			file_list[j]=file_list[key];
			file_list[key]=ptr;
		}
		else{
			ptr=file_list[i];
			file_list[i]=file_list[j];
			file_list[j]=ptr;
		}
	}
	Qsort(start,j-1);
	Qsort(j+1,end);
}

void print_dup(char *dirname){
	int count=0;
	int i=0;
	for(i=0;i<file_list_count;i++){
		if(file_list[i]->next->next==NULL)
			continue;

		count++;
		f_node *cur=file_list[i]->next;
		int j=1;

		printf("---- Identical files #%d (%lld bytes - ",count,file_list[i]->data    .size);
		for(int t=0;t<MD5_DIGEST_LENGTH;t++)
			printf("%02x",file_list[i]->data.md[t]);
		printf(") ----\n");

		while(cur!=NULL){
			printf("[%d] %s/%s\n",j,cur->data.path,cur->data.name);
			j++;
			cur=cur->next;
		}
		printf("\n");
	}
	if(count==0)
		printf("No duplicates in %s\n",dirname);
}
