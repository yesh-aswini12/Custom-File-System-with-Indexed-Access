#include <table_file.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char *filename = argv[1];
    char *column_name = argv[2];
    create_column(filename, column_name);
    return 0;
}
