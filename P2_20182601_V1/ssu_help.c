#include <stdio.h>
#include <stdlib.h>

int main(void){
	printf("Usage:\n");
    printf("   > fmd5/fsha1 [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n");
	printf("     : show duple files. + MINSIZE<duple files<MAXSIZE in TARGET_DIRECTORY\n");
    printf("      >> [SET_INDEX] [OPTION ...]\n");
	printf("         exit : goto main display\n");
    printf("         [OPTION ...]\n");
    printf("         d [LIST_IDX] : delete [LIST_IDX] file\n");
    printf("         i : ask for confirmation before delete\n");
    printf("         f : force delete except the recently modified file\n");
    printf("         t : force move to Trash except the recently modified file\n");
    printf("   > help : show command\n");
    printf("   > exit : prompt end\n\n");
	exit(0);
}
