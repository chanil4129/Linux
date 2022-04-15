#include "ssu_h.h"

#if !defined(_OSD_POSIX) && !defined(__DJGPP__)
//int read(int, void *, unsigned int);
#endif

f_node **file_list;
int file_list_count;

f_node **dup_list;
int dup_list_count;
int *set_count;

void fmd5(char *file_path,unsigned char *md);

int main(int argc,char *argv[]){
	char dirname[PATHMAX];
	struct stat statbuf;
	int length;
	struct timeval begin_t,end_t;
	char dirbuf[PATHMAX];
	char *argv_ptr;

	if(argc!=ARGMAX){
		printf("ERROR: Arguments error\n");
		exit(1);
	}

    //[FILE_EXTENSION]
    char *ptr;
    length=strlen(argv[1]);
    if(!strcmp(argv[1],"*")) strcpy(extension,argv[1]);
    else if(argv[1][0]=='*'&&argv[1][1]=='.'){
        ptr=argv[1]+2;
        strcpy(extension,ptr);
    }   
    else{
        printf("ERROR: FILE_EXTENSION error\n");
        exit(1);
    }   
    
    //[MINSIZE]
    minsize_tilde=0;
    length=strlen(argv[2]);
    if(length==1&&argv[2][0]=='~') minsize_tilde=1;
    else {
		minsize=unit_to_byte(argv[2]);
		if(minsize==-1){
			printf("ERROR: MINSIZE error\n");
			exit(1);
		}   
		else if(minsize==0){
			printf("ERROR: MINSIZE>0\n");
			exit(1);
		}
	}

	//[MAXSIZE]
	maxsize_tilde=0;
	length=strlen(argv[3]);
	if(length==1&&argv[3][0]=='~') maxsize_tilde=1;
	else {
		maxsize=unit_to_byte(argv[3]);
		if(maxsize==-1){
			printf("ERROR: MAXSIZE error\n");
			exit(1);
		}
		else if(maxsize==0){
			printf("ERROR: MAXSIZE>0\n");
			exit(1);
		}
	}

	//[TARGET_DIR]
	dirbuf[0]=0;
	argv_ptr=argv[4];
	if(argv[4][0]=='~'){
		home_dir(dirbuf);
		argv_ptr++;
	}
	strcat(dirbuf,argv_ptr);

    if(realpath(dirbuf,dirname)==NULL){
        printf("ERROR: Path exist error\n");
        exit(1);
    }
	printf("%s\n",dirname);

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

	while(1){
		char path_file[PATHMAX];
		char input[BUFMAX];
		char *arr[10];
		int count;
		int idx;
		int d_idx;
		int input_error=0;
		int y_n;
		int *del_list;
		long long *I_time;

		if(dup_list_count==0)
			exit(0);

		printf(">> ");
		fgets(input,sizeof(input),stdin);
		count=split(input," ",arr);

		if(!strcmp(arr[0],"exit\n")){
			printf("Back to Prompt\n");
			exit(0);
		}

		if(count!=2&&count!=3){
			printf("ERROR: Arguments error\n");
			continue;
		}

		if(atoi(arr[0])<0||atoi(arr[0])>dup_list_count){
			printf("ERROR: out of Identical files index\n");
			continue;
		}

		if(count==3){
			if(atoi(arr[2])<1||atoi(arr[2])>set_count[atoi(arr[0])]){
				printf("ERROR: out of Identical files index\n");
				continue;
			}
			d_idx=atoi(arr[2]);
		}

		idx=atoi(arr[0]);

		//option d
		if(arr[1][0]=='d'){
			path_file_extract(path_file,idx,d_idx);
			printf("\"%s\" has been deleted in #%d\n\n",path_file,idx);
			Dpop(idx,d_idx);
			print_dup(dirname);
		}
		//option i
		else if(arr[1][0]=='i'){
			del_list=(int *)malloc(sizeof(int)*(set_count[idx]+1));
			memset(del_list,0,sizeof(del_list));
			for(int i=1;i<=set_count[idx];i++){
				path_file_extract(path_file,idx,i);
				printf("Delete \"%s\"? [y/n] ",path_file);
				y_n=fgetc(stdin);
				if(y_n=='y'||y_n=='Y')
					del_list[i]=1;
				else if(y_n!='n'&&y_n!='N'){
					printf("Input Error\n");
					input_error=1;
					break;
				}
				getchar();
			}
			printf("\n");
			if(input_error==0){
				int temp=0;
				for(int i=1;i<=set_count[idx];i++){
					if(del_list[i]==1){
						Dpop(idx,i-temp);
						temp++;
					}
				}
				print_dup(dirname);
			}
		}
		//option f와 option t
		else if(arr[1][0]=='f'||arr[1][0]=='t'){
//			left_list=(f_node **)malloc(sizeof(f_node *));
			f_node *left;
			struct stat t_statbuf;
			long long max=0;
			int i;
			int temp=0;

			I_time=(long long *)malloc(sizeof(long long)*(set_count[idx]+1));
			memset(I_time,0,sizeof(I_time));
			for(i=1;i<=set_count[idx];i++){
				path_file_extract(path_file,idx,i);
				if(lstat(path_file,&t_statbuf)<0){
					fprintf(stderr,"lstat error for %s\n",path_file);
					exit(1);
				}
				I_time[i]=Integer_time(t_statbuf.st_mtime);
			}
			max=I_time[1];
			d_idx=1;
//			qsort(I_time,sizeof(I_time)/sizeof(long long),sizeof(long long),time_compare);
			for(i=1;i<=set_count[idx];i++){
				if(I_time[i]>max){
					max=I_time[i];
					d_idx=i;
				}
			}
			path_file_extract(path_file,idx,d_idx);
			if(arr[1][0]=='f'){
				for(i=1;i<d_idx;i++){
					Dpop(idx,i-temp);
					temp++;
				}
				temp=0;
				for(i=d_idx+1;i<=set_count[idx];i++){
					Dpop(idx,2);
					temp++;
				}
				printf("Left file in #%d : %s (%s)\n\n",idx,path_file,get_time(t_statbuf.st_mtime));
			}
			else if(arr[1][0]=='t'){
				printf("1\n");//debug
				for(i=1;i<d_idx;i++){
				printf("2\n");//debug
					Dtrash(idx,i-temp);
					temp++;
				}
				temp=0;
				for(i=d_idx+1;i<=set_count[idx];i++){
				printf("3\n");//debug
					Dtrash(idx,2);
					temp++;
				}
				printf("All files in #%d have moved to Trash except \"%s\" (%s)\n\n",idx,path_file,get_time(t_statbuf.st_mtime));
			}
			print_dup(dirname);
		}
		else
			printf("Input Error\n");
	}
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
//				printf("dir: %s\n",namelist[i]->d_name);
				//FILESIZE가 0이면 삭제
				if(statbuf.st_size==0){
					remove(r_dir.path);
					continue;
				}

				//[EXTENSION] 확인
				char filename[FILEMAX];
				char *ext="";
				strcpy(filename,namelist[i]->d_name);
				if(strstr(filename,".")!=NULL){
					ext=strrchr(filename,'.');
					ext++;
				}
				
				if(strcmp(extension,"*")&&strcmp(extension,ext))
					continue;
				
				//[MINSIZE],[MAXSIZE] 확인
				if(minsize_tilde==0&&(long long)statbuf.st_size<minsize||maxsize_tilde==0&&statbuf.st_size>maxsize)
					continue;

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

void Dpop(int idx,int s_idx){
	f_node *cur=dup_list[idx];
	f_node *pop;
	char path_file[PATHMAX];

	path_file_extract(path_file,idx,s_idx);
	pop=cur->next;
	cur->next=pop->next;

	remove(path_file);
	free(pop);
}

void Dtrash(int idx,int s_idx){
	f_node *cur=dup_list[idx];
    f_node *pop;
    char file[FILEMAX];
	char path_file[PATHMAX];
	char home_path[BUFMAX];
	char trash_file[PATHMAX];
	char *ptr;
	int i=0;

	path_file_extract(path_file,idx,s_idx);
	file_extract(file,idx,s_idx);
    pop=cur->next;
    cur->next=pop->next;
	printf("%s\n",path_file);

	while(1){
		home_dir(home_path);
		sprintf(trash_file,"%s/.local/share/Trash/files/version%d%s",home_path,i,file);
		i++;
		if(access(trash_file,F_OK)==0)
			continue;
		if(rename(path_file,trash_file)<0){
			fprintf(stderr, "rename error\n");
			exit(1);
		}
		break;
	}
	
    free(pop);
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
	char print_file_size[26];
	int i=0;
	struct stat statbuf;
	char p_file[PATHMAX];

	free(dup_list);
	free(set_count);
	dup_list_count=0;

	dup_list=(f_node **)malloc(sizeof(f_node *));
	set_count=(int *)malloc(sizeof(int));
	for(i=0;i<file_list_count;i++){
		if(file_list[i]->next->next==NULL)
			continue;

		dup_list_count++;
		dup_list=realloc(dup_list,sizeof(f_node *)*(dup_list_count+1));
		dup_list[dup_list_count]=file_list[i];
		f_node *cur=dup_list[dup_list_count]->next;
		set_count=realloc(set_count,sizeof(int)*dup_list_count+1);
		set_count[dup_list_count]=0;
		int j=1;

		print_size(dup_list[dup_list_count]->data.size,print_file_size);
		printf("---- Identical files #%d (%s bytes - ",dup_list_count,print_file_size);
		for(int t=0;t<MD5_DIGEST_LENGTH;t++)
			printf("%02x",dup_list[dup_list_count]->data.md[t]);
		printf(") ----\n");

		while(cur!=NULL){
			strcpy(p_file,cur->data.path);
			strcat(p_file,"/");
			strcat(p_file,cur->data.name);

			if((lstat(p_file,&statbuf)<0)&&(!access(p_file,F_OK))){
				if(errno==1)
					continue;
				fprintf(stderr,"lstat error for %s\n",p_file);
				exit(1);
			}
			printf("[%d] %s ",j,p_file);
			printf("(mtime : %s) (atime : %s)\n",get_time(statbuf.st_mtime),get_time(statbuf.st_atime));
			set_count[dup_list_count]++;
			j++;
			cur=cur->next;
		}
		printf("\n");
	}
	if(dup_list_count==0){
		printf("No duplicates in %s\n",dirname);
		free(dup_list);
		free(set_count);
	}
}

void path_file_extract(char *f,int idx, int d_idx){
	f_node *cur=dup_list[idx];
	
	memset(f,0,sizeof(f));
	for(int i=0;i<d_idx;i++){
		if(cur==NULL)
			printf("Error : node access\n");
		cur=cur->next;
	}
	strcpy(f,cur->data.path);
	strcat(f,"/");
	strcat(f,cur->data.name);
}

void file_extract(char *f,int idx,int d_idx){
	f_node *cur=dup_list[idx];
	
	memset(f,0,sizeof(f));
	for(int i=0;i<d_idx;i++){
		if(cur==NULL)
			printf("Error : node access\n");
		cur=cur->next;
	}
	strcat(f,cur->data.name);
}
