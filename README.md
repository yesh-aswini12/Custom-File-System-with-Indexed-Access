# Custom-File-System-with-Indexed-Access
# Table File Operations

This project provides a library and a set of executables for working with file-based tables. It allows creating, deleting, and managing table files, rows, and columns, along with user-accessible functions for reading and writing records. The project ensures that all functions and binaries can be used system-wide, just like standard utilities.

## Features

### Library Functions

#### Table File Operations
- **Create:** `int create_string_table_file(char *filename, int num_rows, int num_columns, int record_size);`
- **Delete:** `int delete_string_table_file(char *filename);`

#### Row and Column Management
- **Create Row:** `int create_row(char *filename, char *row_name);`
- **Delete Row:** `int delete_row(char *filename, char *row_name);`
- **Create Column:** `int create_column(char *filename, char *column_name);`
- **Delete Column:** `int delete_column(char *filename, char *column_name);`

#### Open, Read, and Write Records
- **Open:** `struct File_ID open_string_table_file(char *filename, char *row_name, char *column_name);`
- **Read:** `void* read_table(struct File_ID fid);`
- **Write:** `int write_table(struct File_ID fid, void *buffer, size_t size);`

### Executable Binaries

Six executables are provided:

- `create_table_file`: Create a table file.
- `delete_table_file`: Delete a table file.
- `create_row`: Add a new row to the table file.
- `delete_row`: Remove a row from the table file.
- `create_column`: Add a new column to the table file.
- `delete_column`: Remove a column from the table file.

## Setup Process

### 1. Building the Static Library

To allow global usage of `#include <table_file.h>` in any program, we build a static library and place it in the system's library path.

**Steps:**
```bash
# Compile to object file
gcc -c table_file.c -o table_file.o

# Create the static library
ar rcs libtable_file.a table_file.o

# Move the static library
sudo mv libtable_file.a /usr/local/lib/

# Move the header file
sudo cp table_file.h /usr/local/include/

# Update linker cache
sudo ldconfig

# Change permission for header
sudo chmod 644 /usr/local/include/table_file.h
```

### 2. Using the Static Library

Once the library and header are in place, you can use `#include <table_file.h>` in your programs.

Example `main.c` compilation:
```bash
gcc -o test_program main.c -ltable_file
```

### 3. Compiling the Executable Binaries

Compile each utility with the library:
```bash
gcc -o create_table_file create_table_file.c -ltable_file
gcc -o delete_table_file delete_table_file.c -ltable_file
gcc -o create_row create_row.c -ltable_file
gcc -o delete_row delete_row.c -ltable_file
gcc -o create_column create_column.c -ltable_file
gcc -o delete_column delete_column.c -ltable_file
```

Move the executables to `/usr/local/bin` for global access:
```bash
sudo mv create_table_file /usr/local/bin/
sudo mv delete_table_file /usr/local/bin/
sudo mv create_row /usr/local/bin/
sudo mv delete_row /usr/local/bin/
sudo mv create_column /usr/local/bin/
sudo mv delete_column /usr/local/bin/
```

## Usage Instructions

### Table File Management
- **Create a Table File:**
  ```bash
  create_table_file <filename> <num_rows> <num_columns> <record_size>
  ```
- **Delete a Table File:**
  ```bash
  delete_table_file <filename>
  ```

### Row and Column Management
- **Add a Row:**
  ```bash
  create_row <filename> <row_name>
  ```
- **Remove a Row:**
  ```bash
  delete_row <filename> <row_name>
  ```
- **Add a Column:**
  ```bash
  create_column <filename> <column_name>
  ```
- **Remove a Column:**
  ```bash
  delete_column <filename> <column_name>
  ```

### Reading and Writing Records

Use the `open_string_table_file`, `read_table`, and `write_table` functions in your programs to manipulate table records programmatically.

## Summary

- The library provides functions for table file operations.
- The binaries offer command-line utilities for managing tables, rows, and columns.
- The setup process integrates the library and utilities into the system, making them accessible globally.

This documentation should help you build and use this project seamlessly on your system.
