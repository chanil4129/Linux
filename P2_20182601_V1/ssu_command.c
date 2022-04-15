#include "ssu_h.h"

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

//단위를 바이트로 바꿔 주는 함수
long long unit_to_byte(char *argv){
    char *unit;
    int length;
    char *arr[20];
    int count;
    int i=0;
	int KMG;//KB,MB,GB 구분
	char *unit_argv=(char *)malloc(sizeof(argv));
	strcpy(unit_argv,argv);

    length=strlen(unit_argv);
    for(i=0;i<length;i++) {
		if(unit_argv[i]>='0'&&unit_argv[i]<='9'||unit_argv[i]=='.') 
			continue;
		break;
	}

    unit=unit_argv+i;
    //byte
    if((length-i)==0||!strcmp(unit,"BYTE")||!strcmp(unit,"byte")){
		/*
		unit_argv[i]=0;
		if(!strcmp(unit_argv,"0")){
			return 0;
		}
        return atoll(unit_argv)?atoll(unit_argv):-1;
		*/
		return atoll(unit_argv);
    }
    //kb,mb,gb
    if(!strcmp(unit,"KB")||!strcmp(unit,"kb")) KMG=3;
    else if(!strcmp(unit,"MB")||!strcmp(unit,"mb")) KMG=6;
    else if(!strcmp(unit,"GB")||!strcmp(unit,"gb")) KMG=9;
	else return-1;
	unit_argv[i]='\0';
//	unit='\0';

	//"숫자"혹은"." 외에 들어오면 에러처리
    count=split(unit_argv,".",arr);
	for(i=0;i<count;i++){
		for(int j=0;j<strlen(arr[i]);j++){
			if(arr[i][j]>='0'&&arr[i][j]<='9'||arr[i][j]=='.')
				continue;
			return -1;
		}
	}
	if(count==1){
		for(i=0;i<KMG;i++)
			strcat(arr[0],"0");
	}
	else if(count==2){
		length=strlen(arr[1]);
		for(i=0;i<KMG-length;i++)
			strcat(arr[1],"0");
		strcat(arr[0],arr[1]);
	}
	else 
		return -1;

	/*
	for(i=0;i<strlen(arr[0]);i++){
		if(arr[0][i]!='0')
			break;
		return 0;
	}

	return
		atoll(arr[0])?atoll(arr[0]):-1;
		*/
	return atoll(arr[0]);
}

//파일 사이즈 , 붙여서 나타내기
void print_size(long long size,char *p_size){
    char temp[26];
    char *p;
    int i;

    p=temp;
    for(i=0;i<20&&size>0;i++){
        if(i&&(i%3)==0)
            *p++=',';
        *p++=(size%10)+'0';
        size/=10;
    }
    p--;

    while(p>=temp)
        *p_size++=*p--;
    *p_size=0;
}

//시간 출력
char* get_time(time_t stime){
    char *time=(char *)malloc(sizeof(char)*BUFMAX);
    struct tm *tm;

    tm=localtime(&stime);
    sprintf(time,"%04d-%02d-%02d %02d:%02d:%02d",1900+tm->tm_year,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);

    return time;
}

//최근 수정 시간 정수로 나타내기
long long Integer_time(time_t stime){
	char *time=(char *)malloc(sizeof(char)*BUFMAX);
	struct tm *tm;
	
	tm=localtime(&stime);
    sprintf(time,"%04d%02d%02d%02d%02d%02d",tm->tm_year,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	
	return atoll(time);
}

void home_dir(char *home){
	struct passwd *pwd;
	char buf[1024];
	size_t bufsize;
	int ret;
	int uid=0;

	if(getuid()==0){
		setuid(1000);
		uid++;
	}
	errno=0;

	if((pwd=getpwuid(getuid()))==NULL){
		if(errno==0||errno==ENOENT||errno==ESRCH||errno==EBADF||errno==EPERM){
			printf("Error : getpwuid\n");
			exit(1);
		}
		else{
			printf("Error: %s\n",strerror(errno));
			exit(1);
		}
	}
	strcpy(home,pwd->pw_dir);
	if(uid){
		setuid(0);
	}
}
