#include <stdio.h>
#include <stdlib.h> //realpath
#include <sys/stat.h> //stat
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h> 
#include <dirent.h> //scandir
#include <sys/time.h>   //gettimeofday
#include <string.h> //strcmp
#include <time.h>
#include <stdbool.h>

#define TEMP 1024//임시 크기
#define PATH_SIZE 10000//경로 길이
#define ENTER 0//Enter키만 쳤을경우 매크로
#define FIND 1//find 명령어 매크로
#define EXIT 2//exit 명령어 매크로
#define HELP 3//help 명령어 매크로

struct timeval start_time,end_time; //Runtime calculate
struct s_find_file{//탐색할 파일 정보
	char filename[TEMP];
	char path[PATH_SIZE];
	char file_path[PATH_SIZE];
} s_file;
struct s_store_file{//탐색된 파일 정보
	char filename[TEMP];
	char path[PATH_SIZE];
	char file_path[PATH_SIZE];
	char mode[20];
	int size;
} s_store[TEMP];
struct file_content {//파일 내용 정보
    int line[TEMP];
    char buf[TEMP];
};
struct s_diff_dir{//디렉토리끼리 비교할 때 탐색된 파일 정보
	char filename[TEMP];
	char path[PATH_SIZE];
	char file_path[PATH_SIZE];
	char mode[20];
	int lowpath;
} s_dir[2][TEMP];

int file_num;//파일 개수
int file_index;//파일 번호

void find_dfs(char *);//탐색할 파일 찾기(dfs로 구현)
int find_choose(void);//탐색된 파일을 가지고 어떤 명령을 실행할지
int dir_weight_dfs(char*,int);//탐색된 디렉토리 사이즈 구하기
void print_file_info(struct stat ,int);//탐색된 파일 정보 출력하기
void find_option_q_file(int,int);//정규파일끼리 비교(option q)
void find_option_s_file(int,int);//정규파일끼리 비교(option s)
void find_option_q_dir(int);//디렉토리끼리 비교(option q)
void find_option_s_dir(int);//디렉토리끼리 비교(option q)
void file_diff(int,int,int);//정규파일끼리 내용 비교(이때 none option, option i, option r, 디렉토리 내부의 정규파일끼리 비교할때 사용)
void dir_diff(int,int,int);//디렉토리끼리 비교(none option,option i,option r)
int dir_file_same(int,int);//파일 내용이 같은지 비교
int dir_diff_cal(int,int,char *,int,int);//디렉토리끼리 비교(none option,option i,optionr)
void row(struct file_content,int,char*);//파일 내용에서 줄단위 버퍼 저장
void file_to_buf(int,int*,char*);//구조체에 파일 내용을 저장(정규파일끼리 비교시,none option)
void file_to_buf_dir(int,int,int*,char*);//구조체에 파일 내용을 저장(디렉토리 내부의 정규파일끼리 비교할 때,none option)
void file_to_buf_i(int,int*,char*);//구조체에 파일 내용을 저장(정규파일끼리 비교시,option i)
void file_to_buf_dir_i(int,int,int *,char *);//구조체에 파일 내용을 저장(디렉토리 내부의 정규파일끼리 비교할 때, option i)
void file_name_extract(char*,char*,int,int);//파일끼리 비교할 때 파일 이름 추출하기
void dir_name_extract(char*,char*,int,int);//디렉토리끼리 비교할 때 파일 이름 추출하기
void division_FILENAME_PATH(char*);//사용자 입력으로 받은 내용을 파일이름과 경로 구분
void find_command(void);//find 명령어 실행
void exit_command(void);//exit 명령어 실행
void help_command(void);//help 명령어 실행
int command_classify(char*);//사용자 입력으로 받은 내용을 find,exit,help, 그외 상황 구분

int main(void){
	char user_input[TEMP];//사용자로부터 입력
	int command_num;//사용자로부터 입력받은 내용 구분 받기

	gettimeofday(&start_time,NULL);//타이머 시작
	
	while(1){
		printf("20182601> ");
		memset(&s_file,0,sizeof(struct s_find_file));
		memset(&s_store,0,sizeof(struct s_store_file));
		command_num=command_classify(user_input);//사용자로부터 입력받은 내용 구분

		if(command_num==ENTER) continue;//Enter만 입력하면 재출력
		else if(command_num==FIND){//find 명령어라면
			division_FILENAME_PATH(user_input);//파일 이름과 경로 구분
			find_command();//find 명령어 실행
		}
		else if(command_num==EXIT) exit_command();//exit 명령어라면 exit 명령어 실행
		else if(command_num==HELP) help_command();//help 명령어라면 help 명령어 실행
		else {//예외상황
			fprintf(stderr,"input error");
			exit(1);
		}
	}
	return 0;
} 

void division_FILENAME_PATH(char *user_input){//파일 이름과 경로 구분
	int i=5,j=0; //i:"find" except

	while(user_input[i]!=' ') s_file.filename[j]=user_input[i],i++,j++;
	s_file.filename[j]=0;j=0,i++;
	while(user_input[j]!=0) s_file.path[j]=user_input[i],i++,j++;
	s_file.path[j]=0;
}

void find_command(void){//find 명령어 실행
	char real_path[TEMP];//절대경로 반환받을 버퍼
	
	//상대경로 -> 절대경로
	if(realpath(s_file.path,real_path)==NULL){
		printf("(None path)\n");
		return ;
	}
	//초기화 및 경로 작성
	memset(s_file.path,0,sizeof(s_file.path));
	strcpy(s_file.path,real_path);
	//초기화 및 명령어 실행
   	file_num=0, file_index=0;
	s_store[0].size=-1;
	find_dfs(s_file.path);
	file_num=file_index;
	if(!file_index) fprintf(stderr,"not file\n");
	else if(file_index==1) printf("(None)\n");
	else while(!find_choose());
	return;
}

void find_dfs(char *path){//dfs로 파일 탐색
	struct stat file;//파일 정보 구조체
	struct dirent **namelist;//디렉토리의 하위 파일 이름
	int str_length;//문자열 길이 구하기
	char file_path[PATH_SIZE];//경로와 파일 이름 
	char lowpath[TEMP];//디렉토리의 하위 경로
	int dir_weight=0;//디렉토리일때 사이즈 크기
	int count;//개수

	//파일+경로
	if(!strcmp(path,"/")){
		strcpy(file_path,"/");
	}
	strcpy(file_path,path);
	strcat(file_path,"/");
	strcat(file_path,s_file.filename);

	dir_weight=dir_weight_dfs(path,dir_weight);
	//파일 찾으면
	if(stat(file_path,&file)==0&&(file_index==0||s_store[0].size==file.st_size||dir_weight==s_store[0].size)){
		strcpy(s_store[file_index].filename,s_file.filename);
		strcpy(s_store[file_index].path,path);
		strcpy(s_store[file_index].file_path,file_path);
		if((file.st_mode&S_IFMT)==S_IFDIR) strcpy(s_store[file_index].mode,"directory");
		else if((file.st_mode&S_IFMT)==S_IFREG) strcpy(s_store[file_index].mode,"regular file");
		print_file_info(file,dir_weight);
	}
	//하위 리스트
	if((count=scandir(path,&namelist,NULL,alphasort))<0){
//		fprintf(stderr,"scandir error\n");
		return;
	}
	for(int i=0;i<count;i++){
		memset(lowpath,0,sizeof(lowpath));
		//몇몇 경로 제외
        if(!strcmp(namelist[i]->d_name,"proc")) continue;
        if(!strcmp(namelist[i]->d_name,"X11")) continue;
        if(!strcmp(namelist[i]->d_name,"run")) continue;
        if(!strcmp(namelist[i]->d_name,"snap")) continue;
        if(!strcmp(namelist[i]->d_name,"sys")) continue;
      if(!strcmp(namelist[i]->d_name,"usr")) continue;
		if(namelist[i]->d_name[0]=='.') continue;//부모, 자기디렉토리 제외
		else{
			str_length=strlen(path);
			if(path[str_length-1]=='/'){
				strcpy(lowpath,path);
			}
			else{
				strcpy(lowpath,path);
				strcat(lowpath,"/");
			}
			strcat(lowpath,namelist[i]->d_name);
		}
		//초기화 및 디렉토리가 아니면 찾지 않기
		memset(&file,0,sizeof(stat));
		stat(lowpath,&file)<0;
		if((file.st_mode&S_IFMT)!=S_IFDIR) continue;//디렉토리가 아니면 더이상 찾지 않는다
		find_dfs(lowpath);//dfs
	}
	//할당 해제
	for(int i=0;i<count;i++) free(namelist[i]);
	free(namelist);
	return;
}

void print_file_info(struct stat file,int dir_weight){//정보 입력
	struct tm *t;
    char *rwx[8]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};

    if(!file_index) printf("Index Size Mode       Blocks Links UID  GID  Access            Change            Modify            Path\n"); 
    printf("%-6d",file_index);//Index
	//Size
    if(S_ISDIR(file.st_mode)){
        printf("%-5d",dir_weight);
        printf("d");
		s_store[file_index].size=dir_weight;
    }
    else{
        printf("%-5ld",(long)file.st_size);
        printf("-");
		s_store[file_index].size=file.st_size;
    }
	//Mode
    printf("%s%s%s ",rwx[(file.st_mode&0700)>>6],rwx[(file.st_mode&070)>>3],rwx[file.st_mode&07]);
    //blocks
   	printf("%-7lld",(long long)file.st_blocks);
    //Link
	printf("%-6ld",(long)file.st_nlink);
    //uid
	printf("%-5ld",(long)file.st_uid);
    //gid
	printf("%-5ld",(long)file.st_gid);
    //access time
	t=localtime(&file.st_atime);
    printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    //change time
	t=localtime(&file.st_ctime);
    printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    //modify time
	t=localtime(&file.st_mtime);
    printf("%02d-%02d-%02d %02d:%02d:%02d ",t->tm_year-100,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
    //path
	printf("%s\n",s_store[file_index++].file_path);
    return;
}

int dir_weight_dfs(char *path,int dir_weight){//디렉토리의 사이즈 구하기
	struct stat file;
	struct dirent **namelist;
	int str_length;
	char file_path[PATH_SIZE];
	char lowpath[TEMP];
	int count;
	char buf[TEMP];
	int length;
	int k=0;
	//경로 구하기
	length=strlen(path)-1;
	while(path[length--]!='/') k++;
	length++;
	for(int i=0;i<k;i++) buf[i]=path[++length];
	buf[k]=0;
	if(!strcmp(s_file.filename,buf)){
		dir_weight=0;
	}
	
	//파일+경로
	if(!strcmp(path,"/")){
		strcpy(file_path,"/");
	}
	strcpy(file_path,path);
	strcat(file_path,"/");
	strcat(file_path,s_file.filename);
	
	//하위 리스트
	if((count=scandir(path,&namelist,NULL,alphasort))<0){
//		fprintf(stderr,"scandir error\n");
		return dir_weight;
	}
	for(int i=0;i<count;i++){
		memset(lowpath,0,sizeof(lowpath));
        if(!strcmp(namelist[i]->d_name,"proc")) continue;
        if(!strcmp(namelist[i]->d_name,"X11")) continue;
        if(!strcmp(namelist[i]->d_name,"run")) continue;
        if(!strcmp(namelist[i]->d_name,"snap")) continue;
        if(!strcmp(namelist[i]->d_name,"sys")) continue;
      if(!strcmp(namelist[i]->d_name,"usr")) continue;
		if(namelist[i]->d_name[0]=='.') continue;//부모, 자기디렉토리 제외
		else{
			str_length=strlen(path);
			if(path[str_length-1]=='/'){
				strcpy(lowpath,path);
			}
			else{
				strcpy(lowpath,path);
				strcat(lowpath,"/");
			}
			strcat(lowpath,namelist[i]->d_name);
		}
		
		memset(&file,0,sizeof(stat));
		stat(lowpath,&file)<0;
		if((file.st_mode&S_IFMT)==S_IFREG) dir_weight+=(int)file.st_size;//무게 합치기
		dir_weight=dir_weight_dfs(lowpath,dir_weight);
		if(!strcmp(namelist[i]->d_name,s_file.filename)) break;
	}
	for(int i=0;i<count;i++) free(namelist[i]);
	free(namelist);
	return dir_weight;
}

int find_choose(){//사용자로부터 입력받아 탐색된 파일들을 어떤 옵션으로 명령할지 구분
	char index[TEMP];
	char input[TEMP];
	char option=0;
	int index_num;
	char c;
	int i=0;

	//명령어 입력 받고 구분
	printf(">> ");
	while((c=getc(stdin))!='\n') input[i]=c,i++;
	input[i]=0,i=0;
	while(input[i]!=0){
		index[i]=input[i],i++;
		if(input[i]==' '){
			option=input[i+1];
			if(input[i+2]!=0){
				fprintf(stderr,"many option\n");
				return 0;
			}
			break;
		}
	}
	index[i]=0;
	for(i=0;index[i]!=0;i++) if(index[i]<'0'||index[i]>'9') return 0;
	index_num=atoi(index);
	if(index_num>=file_num){
		fprintf(stderr,"index size error\n");
		return 0;
	}
	if(index_num==0){
		fprintf(stderr,"anoter index num please..\n");
		return 0;
	}
	//입력 받은 것을 경우에 따라 실행
	if(!strcmp(s_store[0].mode,"regular file")&&!strcmp(s_store[index_num].mode,"regular file")){//정규 파일끼리
		if(option=='\0') file_diff(0,index_num,0);//none option
        else if(option=='q') find_option_q_file(0,index_num);//q option
        else if(option=='s') find_option_s_file(0,index_num);//s option
        else if(option=='i') file_diff(0,index_num,1);//i option
        else if(option=='r') file_diff(0,index_num,0);//r option
        else {
            fprintf(stderr,"your type option is non_option\n");
            return 0;
        }
        return 1;
    }
	else if(!strcmp(s_store[0].mode,"directory")&&!strcmp(s_store[index_num].mode,"directory")){//디렉토리끼리
		if(option=='\0') dir_diff(0,index_num,2);//none option
        else if(option=='q') find_option_q_dir(index_num);//q option
        else if(option=='s') find_option_s_dir(index_num);//s option
        else if(option=='i') dir_diff(0,index_num,3);//i option
        else if(option=='r') dir_diff(0,index_num,2);//r option
        else {
            fprintf(stderr,"your type option is non_option\n");
            return 0;
        }
        return 1;
	}
	else//정규파일과 디렉토리라면
		printf("File %s is a %s while file %s is a %s\n",s_store[0].filename,s_store[0].mode,s_store[index_num].filename,s_store[index_num].mode);
}
	
void file_diff(int index1,int index2,int option){//정규파일끼리 내용 비교
    struct file_content f1;//파일 내용 저장
    struct file_content f2;
    char buf[TEMP];//임시 버퍼
    char bo1[TEMP],bo2[TEMP],bn1[TEMP],bn2[TEMP];//각 줄별로 임시 버퍼
    int length1,length2;//파일 길이
    int line1=1,line2=1;//라인 개수
    bool last_line1=false,last_line2=false;
    int o1=1,o2=1,n1=1,n2=1;//포인터(줄마다 가리키는 부분)
    int same=1;//같은지 다른지

	//구조체에 내용 저장
    if(option==1) file_to_buf_i(index1,&length1,buf);//정규파일기리 비교시. 옵션 i
	else if(option==2) file_to_buf_dir(0,index1,&length1,buf);//디렉토리의 정규파일끼리 비교시
	else if(option==3) file_to_buf_dir_i(0,index1,&length1,buf);//디렉토리의 정규파일 비교시. 옵션 i
    else file_to_buf(index1,&length1,buf);//정규파일끼리 비교시
    strcpy(f1.buf,buf);
    memset(buf,0,length1);

    if(option==1) file_to_buf_i(index2,&length2,buf);
	else if(option==2)file_to_buf_dir(1,index2,&length2,buf);
	else if(option==3)file_to_buf_dir_i(1,index2,&length2,buf);
    else file_to_buf(index2,&length2,buf);
    strcpy(f2.buf,buf);

    if(length1!=length2) same=0;
    else for(int i=0;i<length1;i++) if(f1.buf[i]!=f2.buf[i]) same=0;
    if(same) return;

    for(int i=0;i<length1;i++) if(f1.buf[i]==10) {
        f1.buf[i]=0;
        line1++;
    }
    for(int i=0;i<length2;i++)  if(f2.buf[i]==10) {
        f2.buf[i]=0;
        line2++;
    }
    if(f1.buf[length1]!=0) f1.buf[length1]=0,last_line1=true;
    if(f2.buf[length2]!=0) f2.buf[length2]=0,last_line2=true;
    line1=1,line2=1;
    f1.line[1]=-1,f2.line[1]=-1;
    for(int i=0;i<length1;i++) if(f1.buf[i]==0) f1.line[++line1]=i;
    for(int i=0;i<length2;i++) if(f2.buf[i]==0) f2.line[++line2]=i;
    line1--,line2--;

	//줄별로 내용 비교하여 출력
	//내용이 없을 경우
    if(line1==0&&f1.buf[0]==0){
        printf("0a1,%d\n",line2);
        for(int i=1;i<=line2;i++){
            row(f2,i,bo1);
            printf("> %s\n",bo1);
        }
		if(!last_line2) printf("\\ No newline at end of file\n");
        return;
    }
    if(line2==0&&f2.buf[0]==0){
        printf("1,%dd0\n",line1);
        for(int i=1;i<=line1;i++){
            row(f1,i,bo1);
            printf("> %s\n",bo1);
        }
		if(!last_line1) printf("\\ No newline at end of file\n");
        return;
    }
	//내용이 있을 경우
    while(1){//
        int i=1;
        row(f1,o1,bo1);
        row(f2,n1,bn1);
        if(o1==line1&&n1==line2&&strcmp(bo1,bn1)){
            printf("%dc%d\n",o1,n1);
            printf("< %s\n",bo1);
            if(o1>=line1) if(!last_line1) printf("\\ No newline at end of file\n");
            printf("---\n");
            printf("> %s\n",bn1);
			if(n1>=line2) if(!last_line2) printf("\\ No newline at end of file\n");
        }
        if(o1>=line1&&n1>=line2) break;
        if(o2>line1) o2=o1,n2++;
        row(f1,o2,bo2);
        row(f2,n2,bn2);
        if(n2!=line2&&strcmp(bo2,bn2)){o2++;continue;}//마지막 라인이 아니고, 다르면 o2++
        if(n2==line2&&o2<line1&&strcmp(bo2,bn2)){o2++;continue;}
        if(o2==line1&&n2<line2){n2++;continue;}
        if(n2!=line2&&bo2[0]==0&&bn2[0]==0) {o2++;continue;}//개행만 있는거 무시
        //printf("%d %d %d  %d\n",o1,o2,n1,n2);
        if(o2==1&&o2<n2&&n1!=n2){//top a
            printf("0a1,%d\n",n2-1);
            while(n1!=n2){
                printf("> %s\n",bn1);
                row(f2,++n1,bn1);
            }
            if(n2>=line2) if(!last_line2) printf("\\ No newline at end of file\n");
        	if(o1>=line1&&n1>=line2) break;
            continue;
        }
        if(n2==1&&o2>n2&&o1!=o2){//top d
            printf("1,%dd0\n",o2-1);
            while(o1!=o2){
                printf("< %s\n",bo1);
                row(f1,++o1,bo1);
            }
			//printf("o2 : %d\nline1 :%d\n",o2,line1);
            if(o2>=line1) if(!last_line1) printf("\\ No newline at end of file\n");
        	if(o1>=line1&&n1>=line2) break;
            continue;
        }
		
        if(o1!=line1&&n1!=line2&&o1==o2&&n1==n2) {o1++,n1++,o2++,n2++;continue;}//같은 문장
		
        while(1){//개행 같은거 처리
            row(f1,o2-i,bo2),row(f2,n2-i,bn2);
            if(bo2[0]!=0||bn2[0]!=0) break;
            i++;
        }
        if(o2==line1&&n2==line2)i--;

        if((n1+i)==n2&&(o1+i)<o2){//(o1),(o2)c(n1)
            printf("%d,%dc%d\n",o1,o2-i,n1);
            while(o1<=(o2-i)){
                printf("< %s\n",bo1); row(f1,++o1,bo1);
            }
            if(o2>=line1) if(!last_line1) printf("\\ No newline at end of file\n");
            printf("---\n");
            printf("> %s\n",bn1);
            if(n2>=line2) if(!last_line2) printf("\\ No newline at end of file\n");
            o1=o2,n1=n2;
        	if(o1>=line1&&n1>=line2) break;
            continue;
        }
        if((n1+i)==n2&&(o1+i)==o2){//(o1)c(n1)
            printf("%dc%d\n",o1,n1);
            printf("< %s\n",bo1);
            if(o2>=line1) if(!last_line1) printf("\\ No newline at end of file\n");
            printf("---\n");
            printf("> %s\n",bn1);
            if(n2>=line2) if(!last_line2) printf("\\ No newline at end of file\n");
            o1=o2,n1=n2;
        	if(o1>=line1&&n1>=line2) break;
            continue;
        }
        if((n1+i)<n2&&(o1+i)==o2){//(o1)c(n1),(n2)
            printf("%dc%d,%d\n",o1,n1,n2-i);
            printf("< %s\n",bo1);
            if(o2>=line1) if(!last_line1) printf("\\ No newline at end of file\n");
            printf("---\n");
            while(n1<=(n2-i)){
                printf(">%s\n",bn1);
                row(f2,++n1,bn1);
            }
            if(n2>=line2) if(!last_line2) printf("\\ No newline at end of file\n");
            o1=o2,n1=n2;
        	if(o1>=line1&&n1>=line2) break;
            continue;
        }
        if((n1+i)<n2&&(o1+i)<o2){//(o1),(o2)c(n1),(n2)
            printf("%d,%dc%d,%d\n",o1,o2,n1,n2);
            while(o1<=(o2-i)){
                printf("< %s\n",bo1);
                row(f1,++o1,bo1);
            }
            if(o2>=line1) if(!last_line1) printf("\\ No newline at end of file\n");
            printf("---\n");
            while(n1<=(n2-i)){
                printf(">%s\n",bn1);
                row(f2,++n1,bn1);
            }
            if(n2>=line2) if(!last_line2) printf("\\ No newline at end of file\n");
            o1=o2,n1=n2;
        	if(o1>=line1&&n1>=line2) break;
            continue;
        }
    }
}

void row(struct file_content f,int target_line,char *buf){//라인별로 버퍼 전달
    int i=0;
    int j=f.line[target_line]+1;
    memset(buf,0,TEMP);
    while(f.buf[j]!=0){
        buf[i]=f.buf[j];
        i++,j++;
    }
    buf[j]=0;
}

void file_to_buf(int index, int *length,char *buf){//정규파일끼리 비교시 버퍼 전달
    int fd;
    if((fd=open(s_store[index].file_path,O_RDONLY))<0){
        //      fprintf(stderr,"file to buf open  error\n");
    }
    if((*length=read(fd,buf,TEMP))<0){
        //      fprintf(stderr,"file to buf read error\n");
    }
    buf[*length]=0;
}

void file_to_buf_dir(int N,int index,int *length,char *buf){//디렉토리의 정규파일끼리 비교시 버퍼 전달
	int fd;
	if((fd=open(s_dir[N][index].file_path,O_RDONLY))<0){
        //      fprintf(stderr,"file to buf open  error\n");
	}
    if((*length=read(fd,buf,TEMP))<0){
        //      fprintf(stderr,"file to buf read error\n");
	}
    buf[*length]=0;
}

void file_to_buf_dir_i(int N,int index,int *length,char *buf){//디렉토리의 정규파일끼리 비교시 버퍼 전달(option i)
	int fd;
	if((fd=open(s_dir[N][index].file_path,O_RDONLY))<0){
        //      fprintf(stderr,"file to buf open  error\n");
	}
    if((*length=read(fd,buf,TEMP))<0){
        //      fprintf(stderr,"file to buf read error\n");
	}
    buf[*length]=0;
    for(int i=0;i<*length;i++) if(buf[i]>='A'&&buf[i]<='Z') buf[i]+=32;
}

void file_to_buf_i(int index, int *length,char *buf){//정규파일끼리 비교시(option i)
    int fd;
    if((fd=open(s_store[index].file_path,O_RDONLY))<0){
        //      fprintf(stderr,"file to buf open  error\n");
    }
    if((*length=read(fd,buf,TEMP))<0){
        //      fprintf(stderr,"file to buf read error\n");
    }
    buf[*length]=0;
    for(int i=0;i<*length;i++) if(buf[i]>='A'&&buf[i]<='Z') buf[i]+=32;

}

void dir_diff(int index1,int index2,int option){//디렉토리끼리 비교시
	int length_1=0;
	int length_2=0;
	char p1_path[TEMP];
	char p2_path[TEMP];

	for(int i=0;i<TEMP;i++){
		memset(&s_dir[0][i],0,sizeof(struct s_diff_dir));
		memset(&s_dir[1][i],0,sizeof(struct s_diff_dir));
	}
	//디렉토리의 파일 정보들 구조체에 저장
	length_1=dir_diff_cal(0,index1,s_store[index1].file_path,option,length_1);
	length_2=dir_diff_cal(1,index2,s_store[index2].file_path,option,length_2);

	for(int i=0;i<length_1;i++){
		for(int j=0;j<length_2;j++){
			dir_name_extract(p1_path,p2_path,i,j);//경로에서 다른 부분만 출력하기 위해 버퍼 받기
			if(!strcmp(s_dir[0][i].filename,s_dir[1][j].filename)){//파일 이름 같으면
				if(!strcmp(s_dir[0][i].mode,s_dir[1][j].mode)){//모드가 같을 때
					if(!strcmp(s_dir[0][i].mode,"regular file")){//둘다 정규 파일이면
						if(dir_file_same(i,j)) break;//같으면 생략
						if(option==1)//r 옵션이라면
							printf("diff -r %s/%s %s/%s\n",p1_path,s_dir[0][i].filename,p2_path,s_dir[1][j].filename);
						else//아무 옵션도 없다면
							printf("diff %s/%s %s/%s\n",p1_path,s_dir[0][i].filename,p2_path,s_dir[1][j].filename);
						file_diff(i,j,option);break;
					}
					else{//둘다 디렉토리이면
						if(option==1&&s_dir[0][i].lowpath&&s_dir[1][j].lowpath){//r모드일때 하위 디렉토리가 없어야 출력
							break;
						}
						printf("Common subdirectories: %s/%s and %s/%s\n",p1_path,s_dir[0][i].filename,p2_path,s_dir[1][j].filename);break;
					}
				}//파일 종류가 다르면
				printf("File %s/%s is a %s while file %s/%s is a %s\n",p1_path,s_dir[0][i].filename,s_dir[0][i].mode,p2_path,s_dir[1][j].filename,s_dir[1][j].mode);break;
			}
			if(j==length_2-1){
				printf("Only in %s: %s\n",p1_path,s_dir[0][i].filename);
			}
		}
	}
	for(int j=0;j<length_2;j++){
		for(int i=0;i<length_1;i++){
			dir_name_extract(p1_path,p2_path,i,j);
			if(!strcmp(s_dir[0][i].filename,s_dir[1][j].filename))
				break;
			if(i==length_1-1){
				printf("Only in %s: %s\n",p2_path,s_dir[1][j].filename);
			}
		}
	}

}

int dir_file_same(int index1,int index2){//파일의 내용이 같다면 1, 다르면 0 리턴
	char buf1[TEMP];
	char buf2[TEMP];
	char p1_file[PATH_SIZE];
	char p2_file[PATH_SIZE];
    int length1,length2;
    int same=1;
	//버퍼에 파일 내용 저장
    file_to_buf_dir(0,index1,&length1,buf1);
    file_to_buf_dir(1,index2,&length2,buf2);
	//비교
    if(length1!=length2) same=0;
    else for(int i=0;i<length1;i++) if(buf1[i]!=buf2[i]) same=0;
	return same;
}


int dir_diff_cal(int N,int index,char *path,int option,int length){//디렉토리 하위 파일들의 정보를 구조체에 저장하기
	struct stat file;
	struct dirent **namelist;
	char lowpath[TEMP];
	int count;

	s_dir[N][length].lowpath=0;//하위 디렉토리가 있는지 검사하는것. 일단 초기화
	if((count=scandir(path,&namelist,NULL,alphasort))<0){
//		fprintf(stderr,"scandir error\n");
		return length;
	}
	for(int i=0;i<count;i++){
		memset(lowpath,0,sizeof(lowpath));
		if(namelist[i]->d_name[0]=='.') continue;
        if(!strcmp(namelist[i]->d_name,"proc")) continue;
        if(!strcmp(namelist[i]->d_name,"X11")) continue;
        if(!strcmp(namelist[i]->d_name,"run")) continue;
        if(!strcmp(namelist[i]->d_name,"snap")) continue;
        if(!strcmp(namelist[i]->d_name,"sys")) continue;
     	if(!strcmp(namelist[i]->d_name,"usr")) continue;

		strcpy(lowpath,path);
		strcat(lowpath,"/");
		strcat(lowpath,namelist[i]->d_name);
		//만약 정규파일 혹은 디렉토리라면 구조체에 저장
		if(stat(lowpath,&file)==0){
			if((file.st_mode&S_IFMT)==S_IFDIR||(file.st_mode&S_IFMT)==S_IFREG){
				s_dir[N][length].lowpath++;
				strcpy(s_dir[N][length].filename,namelist[i]->d_name);
				strcpy(s_dir[N][length].path,s_store[index].file_path);
				strcpy(s_dir[N][length].file_path,lowpath);
				if((file.st_mode&S_IFMT)==S_IFDIR)
					strcpy(s_dir[N][length].mode,"directory");
				else
					strcpy(s_dir[N][length].mode,"regular file");
				length++;
			}
		}
		if(option==1){//옵션 r일 경우에 dfs 탐색
			length=dir_diff_cal(N,index,lowpath,option,length);
		}
	}
	for(int i=0;i<count;i++) free(namelist[i]);
	free(namelist);
	return length;
}




void find_option_q_file(int index1,int index2){//정규파일끼리 비교시. option q
	char buf1[TEMP];
	char buf2[TEMP];
	char p1_file[PATH_SIZE];
	char p2_file[PATH_SIZE];
    int length1,length2;
    int same=1;

    file_name_extract(p1_file,p2_file,index1,index2);//경로에서 다른부분만 출력하기 위한 버퍼
	//버퍼에 내용 저장
    file_to_buf(index1,&length1,buf1);
    file_to_buf(index2,&length2,buf2);
	//다르면 출력
    if(length1!=length2) same=0;
    else for(int i=0;i<length1;i++) if(buf1[i]!=buf2[i]) same=0;
    if(same) {
        return;
    }
    else{
        printf("Files %s and %s differ\n",p1_file,p2_file);
        return;
    }
}
void find_option_s_file(int index1,int index2){//정규파일끼리 비교시 option s
    char buf1[TEMP];
    char buf2[TEMP];
    char p1_file[PATH_SIZE];
    char p2_file[PATH_SIZE];
    int length1,length2;
    int same=1;

    file_name_extract(p1_file,p2_file,index1,index2);//경로에서 다른 부분만 출력하기 위한 버퍼
	//버퍼에 내용 저장
    file_to_buf(index1,&length1,buf1);
    file_to_buf(index2,&length2,buf2);
	//같으면 출력
    if(length1!=length2) same=0;
    else for(int i=0;i<length1;i++) if(buf1[i]!=buf2[i]) same=0;
    if(same) {
        printf("Files %s and %s are identical\n",p1_file,p2_file);
        return;
    }
    else{
        return;
    }

}


void find_option_q_dir(int index){//디렉토리끼리 비교시 option q
	int length_1=0;
	int length_2=0;
	char p1_path[TEMP];
	char p2_path[TEMP];
	int read_length_1=0;
	int read_length_2=0;
	char buf1[TEMP];
	char buf2[TEMP];
	int same=1;

	for(int i=0;i<TEMP;i++){
		memset(&s_dir[0][i],0,sizeof(struct s_diff_dir));
		memset(&s_dir[1][i],0,sizeof(struct s_diff_dir));
	}
	//디렉토리 하위 정보 구조체에 저장
	length_1=dir_diff_cal(0,0,s_store[0].file_path,0,length_1);
	length_2=dir_diff_cal(1,index,s_store[index].file_path,0,length_2);

	//비교하여 출력
	for(int i=0;i<length_1;i++){
		for(int j=0;j<length_2;j++){
			dir_name_extract(p1_path,p2_path,i,j);
			same=1;
			if(!strcmp(s_dir[0][i].mode,"directory")&&!strcmp(s_dir[1][j].mode,"directory")) break;
			if(!strcmp(s_dir[0][i].filename,s_dir[1][j].filename)){
				file_to_buf_dir(0,i,&read_length_1,buf1);
				file_to_buf_dir(1,j,&read_length_2,buf2);
				if(read_length_1==read_length_2) same=0;
				else for(int k=0;k<length_1;k++) if (buf1[k]!=buf2[k]) same=0;
				memset(buf1,0,TEMP);memset(buf2,0,TEMP);
				if(same) break;
				else{
					printf("Files %s/%s and %s/%s differ\n",p1_path,s_dir[0][i].filename,p2_path,s_dir[1][j].filename); break;
				}
			}
		}
	}
}

void find_option_s_dir(int index){//디렉토리끼리 비교시 option s
	int length_1=0;
	int length_2=0;
	char p1_path[TEMP];
	char p2_path[TEMP];
	int read_length_1=0;
	int read_length_2=0;
	char buf1[TEMP];
	char buf2[TEMP];
	int same=1;

	for(int i=0;i<TEMP;i++){
		memset(&s_dir[0][i],0,sizeof(struct s_diff_dir));
		memset(&s_dir[1][i],0,sizeof(struct s_diff_dir));
	}
	length_1=dir_diff_cal(0,0,s_store[0].file_path,0,length_1);
	length_2=dir_diff_cal(1,index,s_store[index].file_path,0,length_2);

	for(int i=0;i<length_1;i++){
		for(int j=0;j<length_2;j++){
			dir_name_extract(p1_path,p2_path,i,j);
			same=1;
			if(!strcmp(s_dir[0][i].mode,"directory")&&!strcmp(s_dir[1][j].mode,"directory")) break;
			if(!strcmp(s_dir[0][i].filename,s_dir[1][j].filename)){
				file_to_buf_dir(0,i,&read_length_1,buf1);
				file_to_buf_dir(1,j,&read_length_2,buf2);
				if(read_length_1==read_length_2) same=0;
				else for(int k=0;k<length_1;k++) if (buf1[k]!=buf2[k]) same=0;
				memset(buf1,0,TEMP);memset(buf2,0,TEMP);
				if(same) {
					printf("Files %s/%s and %s/%s are identical\n",p1_path,s_dir[0][i].filename,p2_path,s_dir[1][j].filename);
				   	break;
				}
				else break;
			}
		}
	}
}

void file_name_extract(char *file_1,char *file_2,int index1,int index2){//파일 이름 추출(출력할 부분). 같은 부분 제외
	int i=0;
	int j=0;
	int k=0;
	memset(file_1,0,TEMP);
	memset(file_2,0,TEMP);
	while(s_store[index1].file_path[i]==s_store[index2].file_path[i])i++;
	while(s_store[index1].file_path[i]!='/')i--;
	i++;j=i;
	while(s_store[index1].file_path[i]!=0) file_1[k++]=s_store[index1].file_path[i++];
	k=0;
	while(s_store[index2].file_path[j]!=0) file_2[k++]=s_store[index2].file_path[j++];
}

void dir_name_extract(char *file_1,char *file_2,int index1,int index2){//파일 이름 추출(출력할 부분). 같은 부분 제외
	int i=0,j=0,k=0;
	memset(file_1,0,TEMP);memset(file_2,0,TEMP);
	//printf("extract : %s\nextract : %s\n",s_dir[0][index1].file_path,s_dir[1][index2].file_path);//temp
	while(s_dir[0][index1].file_path[i]==s_dir[1][index2].file_path[i])i++;
	while(s_dir[0][index1].file_path[i]!='/')i--;
	i++;j=i;
	while(s_dir[0][index1].file_path[i]!=0) file_1[k++]=s_dir[0][index1].file_path[i++];
	while(s_dir[0][index1].file_path[i]!='/')file_1[--k]=0,i--;
	k=0;
	//printf("extract : %s\n",file_1);//temp
	while(s_dir[1][index2].file_path[j]!=0) file_2[k++]=s_dir[1][index2].file_path[j++];
	while(s_dir[1][index2].file_path[j]!='/')file_2[--k]=0,j--;
}

int command_classify(char *result){ //classify command
    int c;
    int i=0;
    while((c=getc(stdin))!='\n') result[i]=c, i++;  //command store

    result[i]='\0';
    if(result[0]=='\0') return ENTER;
    else if(result[0]=='f'&&result[1]=='i'&&result[2]=='n'&&result[3]=='d') return FIND;
    else if(result[0]=='e'&&result[1]=='x'&&result[2]=='i'&&result[3]=='t'&&result[4]=='\0') return EXIT;
    else return HELP;
}

void exit_command(void){    //exit command
    double Runtime_sec,Runtime_usec;
    gettimeofday(&end_time,NULL); //timer end
    Runtime_sec=end_time.tv_sec-start_time.tv_sec;
    Runtime_usec=end_time.tv_usec-start_time.tv_usec;
    if(Runtime_usec<0){
        Runtime_sec-=1.0;
        Runtime_usec+=1000000.0;
    }
    printf("Prompt End\nRuntime: %d:%d(sec:usec)\n",(int)Runtime_sec,(int)Runtime_usec);
    exit(0);
}

void help_command(void){    //help command
    printf("Usage:\n   > find [FILENAME] [PATH]\t : find file and show info\n");
	printf("       >> [INDEX] [OPTION ...]\t : compare file\n");
	printf("   > help : show command\n");
	printf("   > exit : show runtime and exit\n\n");
    printf("   [OPTION ...]");
    printf("\n    q : report only when files differ");
    printf("\n    s : report when two files are the same");
    printf("\n    i : ignore case differences in file contents");
    printf("\n    r : recursively compare any subdirectories found\n");
}
