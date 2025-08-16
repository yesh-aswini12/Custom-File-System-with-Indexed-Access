#include<table_file.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	char *filename = argv[1];
	delete_string_table_file(filename);
	return 0;
}