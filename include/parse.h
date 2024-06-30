#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

struct dbheader_t {
  unsigned int magic;      // Magic number
  unsigned short version;  // Database standar version
  unsigned short count;    // Number of employees in db
  unsigned int filesize;   // Size of db for validation
};

struct employee_t {
  char name[256];
  char address[256];
  unsigned int hours;
};

int create_db_header(
    int fd, struct dbheader_t **headerOut);  // Create db header and return it
int validate_db_header(int fd,
                       struct dbheader_t **headerOut);  // Validate existing db
                                                        // header and return it
void output_file(int fd, struct dbheader_t *dbhdr,
                 struct employee_t *employees);  // Output database to file

int read_employees(int fd, struct dbheader_t *dbhdr,
                   struct employee_t **employeesOut);  // Read employees in db
int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
                 char *addstring);  // Add employees in db
void list_employees(struct dbheader_t *dbhdr,
                    struct employee_t *employees);  // List employees in db

int find_empoyee(struct dbheader_t *dbhdr, struct employee_t *employees,
                 char *sname);  // Find employee by name in db

int change_empoyee_hours(
    struct dbheader_t *dbhdr, struct employee_t *employees,
    char *namehours);  // Update employee hours by name in db

void delete_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
                     int id);  // Delete employee by name in db
#endif
