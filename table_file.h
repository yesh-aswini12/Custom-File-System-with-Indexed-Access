#ifndef TABLE_FILE_H    // Include guard
#define TABLE_FILE_H

#include <stddef.h>    // For size_t

struct File_ID {
  int fd;
  int row_index;
  int col_index;
  int rec_size;
  int nrows;
  int ncols;
};

int create_string_table_file(char *filename, int num_rows, int num_columns, int record_size);

int create_row(char *filename,char* stringname);

int create_column(char *filename,char* stringname);

int delete_string_table_file(char*filename);

int delete_row(char *filename,char* stringname);

int delete_column(char *filename,char* stringname);

struct File_ID open_string_table_file(char*filename, char* row_name, char* column_name);

void* read_table(struct File_ID fid);

int write_table(struct File_ID fid, void *buffer, size_t size);

#endif // TABLE_FILE_H
