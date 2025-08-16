#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "table_file.h"

int create_string_table_file(char *filename, int num_rows,int num_columns,int record_size){ // Time complexity = O(R + C), R is num_rows, C is num_columns
    size_t file_size = num_rows*num_columns*record_size;
    int fd = open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);
    if(fd==-1){
        fprintf(stderr,"Error in creating file %s\n",filename);
        return -1;
    }
    int status = ftruncate(fd,file_size);
    if(status==-1){
        fprintf(stderr,"Error in creating file %s\n",filename);
        if(close(fd)==-1){
            fprintf(stderr,"Error in closing file %s\n",filename);
            return -1;
        }
        return -1;
    }
    if(close(fd)==-1){
        fprintf(stderr,"Error in closing file %s\n",filename);
        return -1;
    }
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return -1;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);

    int index_fd = open(index_filename,O_WRONLY|O_CREAT,0644);
    if(index_fd==-1){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }

    char attributes[50];
    snprintf(attributes,sizeof(attributes),"%d %d %d\n",num_rows,num_columns,record_size);
    ssize_t bytes_written = write(index_fd,attributes,strlen(attributes));
    if(bytes_written==-1){
        fprintf(stderr,"Error in writing to file %s\n",index_filename);
        close(index_fd);
        return -1;
    }

    char buffer[1024];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", num_rows);
    write(index_fd, buffer, len);
    for (int i = 0; i < num_rows; ++i) {
        len = snprintf(buffer, sizeof(buffer), "%d", i);
        write(index_fd, buffer, len);
        if(i<num_rows-1){
            write(index_fd," ",1);
        }else{
            write(index_fd, "\n", 1);
        }
    }

    len = snprintf(buffer, sizeof(buffer), "%d\n", num_columns);
    write(index_fd, buffer, len);
    for (int i = 0; i < num_columns; ++i) {
        len = snprintf(buffer, sizeof(buffer), "%d", i);
        write(index_fd, buffer, len);
        if(i<num_columns-1){
            write(index_fd," ",1);
        }else{
            write(index_fd, "\n", 1);
        }
    }

    for(int i=0;i<num_rows;i++){
        len = snprintf(buffer, sizeof(buffer), "%d None\n", i);
        write(index_fd, buffer, len);
    }
    for(int i=0;i<num_columns;i++){
        len = snprintf(buffer, sizeof(buffer), "%d None\n", i);
        write(index_fd, buffer, len);
    }

    if(close(index_fd)==-1){
        fprintf(stderr,"Error in closing file %s\n",index_filename);
        return -1;
    }
    fprintf(stdout,"Successfully created files %s & %s\n",filename,index_filename);
    return 0;
    

}

int delete_string_table_file(char*filename){ // Time complexity = O(1)
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return -1;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);
    if(remove(filename)!=0){
        fprintf(stderr,"Unable to delete file %s\n",filename);
        return -1;
    }
    if(remove(index_filename)!=0){
        fprintf(stderr,"Unable to delete index file of %s\n",filename);
        return -1;
    }
    printf("successfully deleted files %s & %s\n",filename,index_filename);
    return 0;
}

int create_row(char* filename, char* stringname){
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return -1;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);
    FILE* file = fopen(index_filename,"r");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    int nrows,ncols,rec_size;
    fscanf(file,"%d %d %d",&nrows,&ncols,&rec_size);
    int nempty_rows;
    fscanf(file,"%d",&nempty_rows);
    if(nempty_rows==0){
        fprintf(stderr,"All rows are filled\n");
        fclose(file);
        return -1;
    }
    int empty_rows[nempty_rows];
    for(int i=0;i<nempty_rows;i++){
        fscanf(file,"%d",&empty_rows[i]);
    }
    int nempty_cols;
    fscanf(file,"%d",&nempty_cols);
    int empty_cols[nempty_cols];
    for(int i=0;i<nempty_cols;i++){
        fscanf(file,"%d",&empty_cols[i]);
    }

    int rows[nrows];
    char* row_string[nrows];
    for(int i=0;i<nrows;i++){
        row_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&rows[i],row_string[i]);
    }
    int columns[ncols];
    char* col_string[ncols];
    for(int i=0;i<ncols;i++){
        col_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&columns[i],col_string[i]);
    }
    fclose(file);
    int currrow = empty_rows[0];
    row_string[currrow] = stringname;
    nempty_rows = nempty_rows-1;
    int new_empty_rows[nempty_rows];
    for(int i=0;i<nempty_rows;i++){
        new_empty_rows[i] = empty_rows[i+1];
    }
    file = fopen(index_filename,"w");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    fprintf(file,"%d %d %d\n",nrows,ncols,rec_size);
    fprintf(file,"%d\n",nempty_rows);
    for(int i=0;i<nempty_rows;i++){
        fprintf(file,"%d",new_empty_rows[i]);
        if(i<nempty_rows-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    fprintf(file,"%d\n",nempty_cols);
    for(int i=0;i<nempty_cols;i++){
        fprintf(file,"%d",empty_cols[i]);
        if(i<nempty_cols-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    for(int i=0;i<nrows;i++){
        fprintf(file,"%d %s\n",i,row_string[i]);
    }
    for(int i=0;i<ncols;i++){
        fprintf(file,"%d %s\n",i,col_string[i]);
    }
    fclose(file);
    return 0;
}

int create_column(char* filename, char* stringname){
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return -1;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);
    FILE* file = fopen(index_filename,"r");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
    }
    int nrows,ncols,rec_size;
    fscanf(file,"%d %d %d",&nrows,&ncols,&rec_size);
    int nempty_rows;
    fscanf(file,"%d",&nempty_rows);
    int empty_rows[nempty_rows];
    for(int i=0;i<nempty_rows;i++){
        fscanf(file,"%d",&empty_rows[i]);
    }
    int nempty_cols;
    fscanf(file,"%d",&nempty_cols);
    if(nempty_cols==0){
        fprintf(stderr,"All columns are filled\n");
        fclose(file);
        return -1;
    }
    int empty_cols[nempty_cols];
    for(int i=0;i<nempty_cols;i++){
        fscanf(file,"%d",&empty_cols[i]);
    }

    int rows[nrows];
    char* row_string[nrows];
    for(int i=0;i<nrows;i++){
        row_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&rows[i],row_string[i]);
    }
    int columns[ncols];
    char* col_string[ncols];
    for(int i=0;i<ncols;i++){
        col_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&columns[i],col_string[i]);
    }
    fclose(file);
    int currcol = empty_cols[0];
    col_string[currcol] = stringname;
    nempty_cols = nempty_cols-1;
    int new_empty_cols[nempty_cols];
    for(int i=0;i<nempty_cols;i++){
        new_empty_cols[i] = empty_cols[i+1];
    }
    file = fopen(index_filename,"w");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    fprintf(file,"%d %d %d\n",nrows,ncols,rec_size);
    fprintf(file,"%d\n",nempty_rows);
    for(int i=0;i<nempty_rows;i++){
        fprintf(file,"%d",empty_rows[i]);
        if(i<nempty_rows-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    fprintf(file,"%d\n",nempty_cols);
    for(int i=0;i<nempty_cols;i++){
        fprintf(file,"%d",new_empty_cols[i]);
        if(i<nempty_cols-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    for(int i=0;i<nrows;i++){
        fprintf(file,"%d %s\n",i,row_string[i]);
    }
    for(int i=0;i<ncols;i++){
        fprintf(file,"%d %s\n",i,col_string[i]);
    }
    fclose(file);
    return 0;
}

int delete_row(char *filename,char* stringname){
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return -1;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);
    FILE* file = fopen(index_filename,"r");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    int nrows,ncols,rec_size;
    fscanf(file,"%d %d %d",&nrows,&ncols,&rec_size);
    int nempty_rows;
    fscanf(file,"%d",&nempty_rows);
    if(nempty_rows==nrows){
        fprintf(stderr,"All rows are empty\n");
        fclose(file);
        return -1;
    }
    int empty_rows[nempty_rows];
    for(int i=0;i<nempty_rows;i++){
        fscanf(file,"%d",&empty_rows[i]);
    }
    int nempty_cols;
    fscanf(file,"%d",&nempty_cols);
    int empty_cols[nempty_cols];
    for(int i=0;i<nempty_cols;i++){
        fscanf(file,"%d",&empty_cols[i]);
    }

    int rows[nrows];
    char* row_string[nrows];
    int ispresent=0;
    for(int i=0;i<nrows;i++){
        row_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&rows[i],row_string[i]);
        if(strcmp(stringname,row_string[i])==0){
            ispresent=1;
        }
    }
    if(ispresent==0){
        fprintf(stderr,"specified string is not a row\n");
        fclose(file);
        return -1;
    }
    int columns[ncols];
    char* col_string[ncols];
    for(int i=0;i<ncols;i++){
        col_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&columns[i],col_string[i]);
    }
    fclose(file);
    int currrow;
    for(int i=0;i<nrows;i++){
        if(strcmp(stringname,row_string[i])==0){
            currrow=i;
            break;
        }
    }
    char*none = "None";
    row_string[currrow] = none;
    nempty_rows = nempty_rows+1;
    int new_empty_rows[nempty_rows];
    int i=0;
    int j=0;
    while(j<nempty_rows-1&&empty_rows[j]<currrow){
        new_empty_rows[i] = empty_rows[j];
        i++;
        j++;
    }

    new_empty_rows[i] = currrow;
    i++;
    while(j<nempty_rows-1){
        new_empty_rows[i] = empty_rows[j];
        i++;
        j++;
    }
    file = fopen(index_filename,"w");
    if(file ==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    fprintf(file,"%d %d %d\n",nrows,ncols,rec_size);
    fprintf(file,"%d\n",nempty_rows);
    for(int i=0;i<nempty_rows;i++){
        fprintf(file,"%d",new_empty_rows[i]);
        if(i<nempty_rows-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    fprintf(file,"%d\n",nempty_cols);
    for(int i=0;i<nempty_cols;i++){
        fprintf(file,"%d",empty_cols[i]);
        if(i<nempty_cols-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    for(int i=0;i<nrows;i++){
        fprintf(file,"%d %s\n",i,row_string[i]);
    }
    for(int i=0;i<ncols;i++){
        fprintf(file,"%d %s\n",i,col_string[i]);
    }
    fclose(file);
    return 0;
}

int delete_column(char *filename,char* stringname){
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return -1;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);
    FILE* file = fopen(index_filename,"r");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    int nrows,ncols,rec_size;
    fscanf(file,"%d %d %d",&nrows,&ncols,&rec_size);
    int nempty_rows;
    fscanf(file,"%d",&nempty_rows);
    int empty_rows[nempty_rows];
    for(int i=0;i<nempty_rows;i++){
        fscanf(file,"%d",&empty_rows[i]);
    }
    int nempty_cols;
    fscanf(file,"%d",&nempty_cols);
    if(nempty_cols==ncols){
        fprintf(stderr,"All columns are empty\n");
        fclose(file);
        return -1;
    }
    int empty_cols[nempty_cols];
    for(int i=0;i<nempty_cols;i++){
        fscanf(file,"%d",&empty_cols[i]);
    }
    int rows[nrows];
    char* row_string[nrows];
    for(int i=0;i<nrows;i++){
        row_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&rows[i],row_string[i]);
    }

    int columns[ncols];
    char* col_string[ncols];
    int ispresent = 0;
    for(int i=0;i<ncols;i++){
        col_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&columns[i],col_string[i]);
        if(strcmp(stringname,col_string[i])==0){
            ispresent=1;
        }
    }
    if(ispresent==0){
        fprintf(stderr,"specified string is not a column\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    int currcol;
    for(int i=0;i<ncols;i++){
        if(strcmp(stringname,col_string[i])==0){
            currcol=i;
            break;
        }
    }
    char*none = "None";
    col_string[currcol] = none;
    nempty_cols = nempty_cols+1;
    int new_empty_cols[nempty_cols];
    int i=0;
    int j=0;
    while(j<nempty_cols-1&&empty_cols[j]<currcol){
        new_empty_cols[i] = empty_cols[j];
        i++;
        j++;
    }
    new_empty_cols[i] = currcol;
    i++;
    while(j<nempty_cols-1){
        new_empty_cols[i] = empty_cols[j];
        i++;
        j++;
    }

    file = fopen(index_filename,"w");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return -1;
    }
    fprintf(file,"%d %d %d\n",nrows,ncols,rec_size);
    fprintf(file,"%d\n",nempty_rows);
    for(int i=0;i<nempty_rows;i++){
        fprintf(file,"%d",empty_rows[i]);
        if(i<nempty_rows-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    fprintf(file,"%d\n",nempty_cols);
    for(int i=0;i<nempty_cols;i++){
        fprintf(file,"%d",new_empty_cols[i]);
        if(i<nempty_cols-1){
            fprintf(file," ");
        }else{
            fprintf(file,"\n");
        }
    }
    for(int i=0;i<nrows;i++){
        fprintf(file,"%d %s\n",i,row_string[i]);
    }
    for(int i=0;i<ncols;i++){
        fprintf(file,"%d %s\n",i,col_string[i]);
    }
    fclose(file);
    return 0;
}

struct File_ID open_string_table_file(char* filename,char* rname,char* cname){ // Time complexity = O(R + C), R is no of rows, C is no of columns
    struct File_ID error;
    error.fd = -1;
    error.col_index = -1;
    error.row_index = -1;
    error.rec_size = -1;
    error.ncols = -1;
    error.nrows = -1;
    int fd = open(filename,O_RDWR);
    if(fd==-1){
        fprintf(stderr,"Unable to open table file %s\n",filename);
        return error;
    }
    char*index_suffix = "_index.txt";
    size_t index_file_len = strlen(filename)+strlen(index_suffix)+1;
    char* index_filename = malloc(index_file_len);
    if(index_filename==NULL){
        fprintf(stderr,"Unable to allocate space for index_filename of %s\n",filename);
        return error;
    }
    snprintf(index_filename,index_file_len,"%s%s",filename,index_suffix);
    FILE* file = fopen(index_filename,"r");
    if(file==NULL){
        fprintf(stderr,"Error in opening file %s\n",index_filename);
        return error;
    }
    int nrows,ncols,rec_size;
    fscanf(file,"%d %d %d",&nrows,&ncols,&rec_size);
    int nempty_rows;
    fscanf(file,"%d",&nempty_rows);
    int empty_rows[nempty_rows];
    for(int i=0;i<nempty_rows;i++){
        fscanf(file,"%d",&empty_rows[i]);
    }
    int nempty_cols;
    fscanf(file,"%d",&nempty_cols);
    if(nempty_cols==ncols){
        fprintf(stderr,"All columns are empty\n");
        fclose(file);
        return error;
    }
    int empty_cols[nempty_cols];
    for(int i=0;i<nempty_cols;i++){
        fscanf(file,"%d",&empty_cols[i]);
    }
    int rows[nrows];
    int rownum = -1;
    char* row_string[nrows];
    for(int i=0;i<nrows;i++){
        row_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&rows[i],row_string[i]);
        if(strcmp(rname,row_string[i])==0){
            rownum = i;
        }
    }
    if(rownum==-1){
        fprintf(stderr,"specified string is not a row\n");
        fclose(file);
        return error;
    }
    int columns[ncols];
    char* col_string[ncols];
    int colnum = -1;
    for(int i=0;i<ncols;i++){
        col_string[i] = malloc(256*(sizeof(char)));
        fscanf(file,"%d %s",&columns[i],col_string[i]);
        if(strcmp(cname,col_string[i])==0){
            colnum = i;
        }
    }
    if(colnum==-1){
        fprintf(stderr,"specified string is not a column\n");
        fclose(file);
        return error;
    }

    fclose(file);
    
    off_t byte_offset = ((rownum*ncols)+colnum)*rec_size;
    if(lseek(fd,byte_offset,SEEK_SET)==-1){
        fprintf(stderr,"Error in changing file descripter's offset for file %s\n",index_filename);
        close(fd);
        return error;
    }
    struct File_ID myqd;
    myqd.fd = fd;
    myqd.row_index = rownum;
    myqd.col_index = colnum;
    myqd.rec_size = rec_size;
    myqd.nrows = nrows;
    myqd.ncols = ncols;
    return myqd;
}

void* read_table(struct File_ID fid) { // Time complexity = O(rec_size)
    void *record = malloc(fid.rec_size);
    if (!record) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    if (read(fid.fd, record, fid.rec_size) != fid.rec_size) {
        fprintf(stderr, "Error: Failed to read record from file\n");
        free(record);
        return NULL;
    }
    return record;
}

int write_table(struct File_ID fid, void *buffer, size_t size) { // Time complexity = O(size)
    int pos = lseek(fid.fd, 0, SEEK_CUR);
    if (pos == -1) {
        fprintf(stderr, "Error: Failed to get current file position\n");
        return -1;
    }
    
    int used = pos - ((fid.row_index * fid.ncols) + fid.col_index) * fid.rec_size;
    int available = fid.rec_size - used;

    if (size > available) {
        fprintf(stderr, "Warning: Record size exceeds available space; only %d bytes will be written\n", available);
        ssize_t written = write(fid.fd, buffer, available);
        if (written == -1) {
            fprintf(stderr, "Error: Failed to write to file\n");
        }
        return written;
    }
    
    ssize_t written = write(fid.fd, buffer, size);
    if (written == -1) {
        fprintf(stderr, "Error: Failed to write to file\n");
    }
    return written;
}
