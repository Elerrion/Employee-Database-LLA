#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void free_items(int dbfd, struct dbheader_t *dbhdr,
                struct employee_t *employees) {
  if (dbfd != -1) {
    close(dbfd);
  }

  if (dbhdr != NULL) {
    free(dbhdr);
  }

  if (employees != NULL) {
    free(employees);
  }
  return;
}
// Program options
void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n - creat new database file\n");
  printf("\t -f - (required) path to database file\n");
  printf("\t -a - (name,address,hourse) add employee\n");
  printf("\t -s - (name) find empoyee\n");
  printf("\t -h - (name,hours) update employee hours\n");
  printf("\t -l - list all empoyees\n");
  return;
}

int main(int argc, char *argv[]) {
  int c;
  int dbfd = -1;
  int id = -1;
  char *filepath = NULL;
  char *addstring = NULL;
  char *sname = NULL;
  char *namehours = NULL;
  char *dname = NULL;
  bool newfile = false;
  bool list = false;

  struct employee_t *employees = NULL;
  struct dbheader_t *dbhdr = NULL;

  while ((c = getopt(argc, argv, "nf:a:s:h:d:l")) != -1) {
    switch (c) {
      case 'n':
        newfile = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case 'a':
        addstring = optarg;
        break;
      case 's':
        sname = optarg;
        break;
      case 'h':
        namehours = optarg;
        break;
      case 'd':
        dname = optarg;
        break;
      case 'l':
        list = true;
        break;
      case '?':
        printf("Unknown option - %c\n", c);
        break;
      default:
        return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);
    return -1;
  }

  // Open or Create new file and database header
  if (newfile) {
    dbfd = create_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to create database file\n");
      return -1;
    }

    if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      printf("Unable to create database header\n");
      free_items(dbfd, dbhdr, employees);
      return -1;
    }

  } else {
    dbfd = open_db_file(filepath);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to open database file\n");
      return -1;
    }

    if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      printf("Unable to validate database header\n");
      free_items(dbfd, dbhdr, employees);
      return -1;
    }
  }

  // Read database employees
  if (read_employees(dbfd, dbhdr, &employees) == STATUS_ERROR) {
    printf("Unable to read employees\n");
    free_items(dbfd, dbhdr, employees);
    return -1;
  }

  // Add employee to database
  if (addstring) {
    dbhdr->count++;
    employees = realloc(employees, dbhdr->count * sizeof(struct employee_t));
    add_employee(dbhdr, employees, addstring);
  }

  // Find selected employee by name in database
  if (sname) {
    if (find_empoyee(dbhdr, employees, sname) == STATUS_ERROR) {
      printf("Unable to find empoyee\n");
    }
  }

  // Update employee hours by name in database
  if (namehours) {
    if (change_empoyee_hours(dbhdr, employees, namehours) == STATUS_ERROR) {
      printf("Unable to find empoyee\n");
    }
  }

  // Delete selected employee by name in database
  if (dname) {
    id = find_empoyee(dbhdr, employees, dname);
    if (id == STATUS_ERROR) {
      printf("Unable to find empoyee\n");
    } else {
      delete_employee(dbhdr, employees, id);
      dbhdr->count--;
      employees = realloc(employees, dbhdr->count * sizeof(struct employee_t));
    }
  }
  // List all employees in database
  if (list) {
    list_employees(dbhdr, employees);
  }
  // Recreate db file to get rid of deleted data and print new data.
  dbfd = recreate_db_file(dbfd, filepath);
  output_file(dbfd, dbhdr, employees);

  free_items(dbfd, dbhdr, employees);
  return 0;
}
