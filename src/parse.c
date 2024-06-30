
#include "parse.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

void delete_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
                     int id) {
  strncpy(employees[id].name, employees[dbhdr->count - 1].name,
          sizeof(employees[dbhdr->count - 1].name));
  strncpy(employees[id].address, employees[dbhdr->count - 1].address,
          sizeof(employees[dbhdr->count - 1].address));
  employees[id].hours = employees[dbhdr->count - 1].hours;
}

int change_empoyee_hours(struct dbheader_t *dbhdr, struct employee_t *employees,
                         char *namehours) {
  char *name = strtok(namehours, ",");
  char *hours = strtok(NULL, ",");
  int i = find_empoyee(dbhdr, employees, name);
  if (i == -1) {
    printf("Unable to find empoyee\n");
    return STATUS_ERROR;
  }

  employees[i].hours = atoi(hours);
  return STATUS_SUCCESS;
}

int find_empoyee(struct dbheader_t *dbhdr, struct employee_t *employees,
                 char *sname) {
  int i = 0;
  for (; i < dbhdr->count; i++) {
    if (strcmp(employees[i].name, sname) == 0) {
      return i;
    }
  }
  return STATUS_ERROR;
}

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
  int i = 0;
  for (; i < dbhdr->count; i++) {
    printf("Employee %d\n", i);
    printf("\tName: %s\n", employees[i].name);
    printf("\tAddress: %s\n", employees[i].address);
    printf("\tHours: %d\n", employees[i].hours);
  }
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
                 char *addstring) {
  char *name = strtok(addstring, ",");
  char *addr = strtok(NULL, ",");
  char *hours = strtok(NULL, ",");

  strncpy(employees[dbhdr->count - 1].name, name,
          sizeof(employees[dbhdr->count - 1].name));
  strncpy(employees[dbhdr->count - 1].address, addr,
          sizeof(employees[dbhdr->count - 1].address));
  employees[dbhdr->count - 1].hours = atoi(hours);

  return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr,
                   struct employee_t **employeesOut) {
  if (fd < 0) {
    printf("Got a bad FD from user\n");
    return STATUS_ERROR;
  }

  int count = dbhdr->count;

  struct employee_t *employees = calloc(count, sizeof(struct employee_t));
  if (employees == (void *)-1) {
    printf("Malloc failed to create employees\n");
    return STATUS_ERROR;
  }

  read(fd, employees, count * sizeof(struct employee_t));
  int i = 0;
  for (; i < count; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }
  *employeesOut = employees;
  return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **headerOut) {
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == (void *)-1) {
    printf("Malloc failed to create db header\n");
    return STATUS_ERROR;
  }
  header->magic = HEADER_MAGIC;
  header->version = 0x1;
  header->count = 0;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd < 0) {
    printf("Got a bad FD from user\n");
    return STATUS_ERROR;
  }
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == (void *)-1) {
    printf("Malloc failed to create db header\n");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct dbheader_t)) !=
      sizeof(struct dbheader_t)) {
    perror("read");
    free(header);
    return STATUS_ERROR;
  }

  header->magic = ntohl(header->magic);
  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->filesize = ntohl(header->filesize);

  if (header->version != 1) {
    printf("Improper header version\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->magic != HEADER_MAGIC) {
    printf("Improper header magic\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat dbstat = {0};
  fstat(fd, &dbstat);
  if (header->filesize != dbstat.st_size) {
    printf("Currapted database\n");
    free(header);
    return STATUS_ERROR;
  }

  *headerOut = header;
  return STATUS_SUCCESS;
}

void output_file(int fd, struct dbheader_t *dbhdr,
                 struct employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad FD from user\n");
    return;
  }

  int realcount = dbhdr->count;

  dbhdr->magic = htonl(dbhdr->magic);
  dbhdr->version = htons(dbhdr->version);
  dbhdr->count = htons(dbhdr->count);
  dbhdr->filesize = htonl(sizeof(struct dbheader_t) +
                          (sizeof(struct employee_t) * realcount));

  lseek(fd, 0, SEEK_SET);

  write(fd, dbhdr, sizeof(struct dbheader_t));

  int i = 0;
  for (; i < realcount; i++) {
    employees[i].hours = htonl(employees[i].hours);
    write(fd, &employees[i], sizeof(struct employee_t));
  }

  return;
}
