#include <table_file.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    char *row_name = argv[2];
    create_row(filename, row_name);
    return 0;
}
