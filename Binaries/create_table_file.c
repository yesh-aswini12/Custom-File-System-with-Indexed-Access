#include<table_file.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	char *filename = argv[1];
    int num_rows = atoi(argv[2]);
    int num_columns = atoi(argv[3]);
    int record_size = atoi(argv[4]);
	create_string_table_file(filename, num_rows, num_columns, record_size);
	return 0;
}