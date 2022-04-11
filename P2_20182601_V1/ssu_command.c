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

//파일 사이즈 추출
long long file_size(char *fname){
	FILE *fp;
	char *buf;
	long unit;
	unsigned int total_size=0;
	unsigned int current_size=0;

	if((fp=fopen(fname,"rb"))==NULL){
		fprintf(stderr,"fopen error for %s\n",fname);
		exit(1);
	}

	fseek(fp,0,SEEK_END);
	unit=ftell(fp);
	fseek(fp,0,SEEK_SET);

	buf=(char*)malloc(sizeof(char)*unit);

	while((current_size=fread(&buf[total_size],sizeof(char),unit-total_size,fp))>0){
		total_size+=current_size;
	}

	if(total_size!=unit){
		fprintf(stderr,"not read all file");
		exit(1);
	}
}

