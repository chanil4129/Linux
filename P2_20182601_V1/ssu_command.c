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
        return atoll(unit_argv);
    }
    //kb,mb,gb
    if(!strcmp(unit,"KB")||!strcmp(unit,"kb")) KMG=3;
    else if(!strcmp(unit,"MB")||!strcmp(unit,"mb")) KMG=6;
    else if(!strcmp(unit,"GB")||!strcmp(unit,"gb")) KMG=9;
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
	
    return atoll(arr[0]);
}

//파일 사이즈 추출
/*
long long file_size(char *fname){
	int fd;

	if((fd=open(fname,O_RDONLY))<0){
		fprintf(stderr,"file_size extract open error for %s\n",fname);
	}
	
	
}
*/
