#ifndef FILE_H
#define FILE_H

int create_db_file(char* filename);  // Create database
int open_db_file(char* filename);    // Open database
int recreate_db_file(
    int fd, char* filepath);  // Recreate database to get rid of deleted data

#endif
