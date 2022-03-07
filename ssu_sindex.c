#include <stdio.h>

typedef struct FILE{
	var index;
	var Mode;
	var Blocks;
	var Links;
	var UID;
	var GID;
	var Access;
	var Change;
	var Modify;
	var Path;
} FILE;

var find(var FILENAME, var PATH);
var dir_find();
var file_find();
var absolute_path();
var relative_path();
var exit();
var help();

int main(void){
	printf("20182601> ");
	printf("Index Size Mode        Blocks Links UID  GID  Access          Change        Modify         Path\n");
	return 0;
}
