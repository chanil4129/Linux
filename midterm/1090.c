if((dirp=opendir(argv[1])==NULL||chdir(argv[1])==-1));
while((dentry=readdir(dirp))!=NULL){
	if(dentry->d_ino==0)
		continue;

	memcpy(filename,dentry->d_name,DIRECTORY_SIZE);

	while((length=read(fd,buf[count],BUFFER_SIZE))>0){
		buf[count][length]=0;

		for(i=0;i<BUFFERPSID;i++){
			if(buf[count][i]=='\n'){
				offset=offset+i+1;

				lseek(fd,offset,SEEK_SET);
				t
					ount++;
			}


