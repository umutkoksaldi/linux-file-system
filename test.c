#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "myfs.h"


#define TEN_BLOCK_DATA_COUNT (10 * BLOCKSIZE / sizeof(long))
#define TEN_BLOCK_RW_COUNT   (10 * BLOCKSIZE / MAXREADWRITE)
#define MAX_RW_DATA_COUNT    (MAXREADWRITE / sizeof(long))

#define TRUNCATE_COUNT       5
#define TRUNCATE_SIZE        (TRUNCATE_COUNT * sizeof(long))

#define MULTI_FILE_COUNT     50

#define FAIL_IF(expr) if ((expr)) return test_result(FAIL)

#define LOGFILE      "log.txt"
#define TRUE         1
#define FALSE        0
#define SUCCESS      1
#define FAIL         0

#define SAMPLEDISK   "disk3"
#define SAMPLEFILE   "samplefile"
#define EXPECTEDFILE "expected"
#define SAMPLETEXT   "What a cool project.\nCS 342 rocks 8)\n"

#define TESTDISK1    "disk1"
#define TESTDISK4    "disk4"
#define TESTDISK6    "disk6"

#define TESTFILE     "testfile"
#define TESTFILE4    "testfile4"

#define TEST_1_1     "Test 1.1"
#define TEST_1_2     "Test 1.2"
#define TEST_2_1     "Test 2.1"
#define TEST_2_2     "Test 2.2"
#define TEST_3_1     "Test 3.1"
#define TEST_3_2     "Test 3.2"
#define TEST_4_1     "Test 4.1"
#define TEST_4_2     "Test 4.2"
#define TEST_5_1     "Test 5.1"
#define TEST_5_2     "Test 5.2"
#define TEST_6_1     "Test 6.1"
#define TEST_6_2     "Test 6.2"


char *testname;

/* Logging */

void log_(char *message) {
  FILE *logfile;
  logfile = fopen(LOGFILE, "a");
  
  fprintf(logfile, "%s: %s\n", testname, message);

  fclose(logfile);
}

/* Error Handling */

int generic_handler(int result, char *str) {
  if (result < 0)
    log_(str);

  return result;
}

int Myfs_create(char *filename) {
  return generic_handler(myfs_create(filename), "Cannot create file.");
}

int Myfs_mount(char *diskname) {
  return generic_handler(myfs_mount(diskname), "Cannot mount disk.");
}

int Myfs_open(char *filename) {
  return generic_handler(myfs_open(filename), "Cannot open file.");
}

int Myfs_delete(char *filename) {
  return generic_handler(myfs_delete(filename), "Cannot delete file.");
}

int Myfs_close(int fd) {
  return generic_handler(myfs_close(fd), "Cannot close file.");
}

int Myfs_read(int fd, void *buf, int n) {
  return generic_handler(myfs_read(fd, buf, n), "Cannot read file.");
}

int Myfs_write(int fd, void *buf, int n) {
  return generic_handler(myfs_write(fd, buf, n), "Cannot write file.");
}

int Myfs_truncate(int fd, int size) {
  return generic_handler(myfs_truncate(fd, size), "Cannot truncate file.");
}

int Myfs_seek(int fd, int offset) {
  return generic_handler(myfs_seek(fd, offset), "Cannot seek.");
}

int Myfs_filesize(int fd) {
  return generic_handler(myfs_filesize(fd), "Cannot get file size.");
}

/* Utility functions */

void create_and_mount(char *diskname) {
  log_("Creating disk.");
  myfs_diskcreate(diskname);

  log_("Formatting disk.");
  myfs_makefs(diskname);

  log_("Mounting disk.");
  myfs_mount(diskname);
}

int test_result(int result) {
  if (result == SUCCESS)
    log_("TRUE\n");
  else
    log_("FALSE\n");

  return result;
}

long *generate_data(unsigned long count) {
  long i;
  long *data;

  data = (long *) malloc(count * sizeof(long));

  for (i=0; i < count; i++)
    data[i] = (i % 2 == 0 ? 1 : -1) * i*i*i; 

  return data;
}

/* Test cases */

/* Test 1.1: Create disk. Create 5 files */
int test_1_1() {
  int i, fd;
  char filename[MAXFILENAMESIZE];

  testname = TEST_1_1;

  create_and_mount(TESTDISK1);
  
  for (i=0; i < 5; i++) {
    snprintf(filename, MAXFILENAMESIZE, "%s%d", TESTFILE, i+1);

    log_("Creating file.");
    FAIL_IF((fd = Myfs_create(filename)) < 0);

    log_("Closing created file.");
    FAIL_IF(Myfs_close(fd) < 0);

  }
  
  myfs_umount();

  return test_result(SUCCESS);
}

/* Test 1.2: Mount disk. Print directory */
int test_1_2() {
  testname = TEST_1_2;

  myfs_mount(TESTDISK1);

  log_("Listing directory content.");
  freopen("output1", "w+", stdout);
  myfs_print_dir();
  freopen("/dev/tty", "w", stdout);

  myfs_umount();

  return test_result(SUCCESS);
}

/* Test 1.3: Diff command */

/* Test 2.1: Delete 3 files */
int test_2_1() {
  char filename[MAXFILENAMESIZE];
  int i;

  testname = TEST_2_1;
  
  FAIL_IF(Myfs_mount(TESTDISK1) < 0);

  for (i=0; i < 5; i += 2) {
    snprintf(filename, MAXFILENAMESIZE, "%s%d", TESTFILE, i+1);

    log_("Deleting file.");
    FAIL_IF(Myfs_delete(filename) < 0);

    log_("Trying to open deleted file. It should fail.");
    FAIL_IF(Myfs_open(filename) >= 0);
  }

  myfs_umount();

  return test_result(SUCCESS);
}

/* Test 2.2: Mount disk. Print directory */
int test_2_2() {
  testname = TEST_2_2;
  
  FAIL_IF(Myfs_mount(TESTDISK1) < 0);

  log_("Listing directory content.");
  freopen("output2", "w+", stdout);
  myfs_print_dir();
  freopen("/dev/tty", "w", stdout);

  myfs_umount();

  return test_result(SUCCESS);
}

/* Test 2.3: Diff command */

/* Test 3.1: Sample test case */
int test_3_1() {
  int fd;

  testname = TEST_3_1;

  create_and_mount(SAMPLEDISK);

  log_("Creating file.");
  FAIL_IF((fd = Myfs_create(SAMPLEFILE)) < 0);

  log_("Writing content.");
  FAIL_IF(Myfs_write(fd, SAMPLETEXT, strlen(SAMPLETEXT)) != strlen(SAMPLETEXT));

  log_("Closing file.");
  FAIL_IF(Myfs_close(fd) < 0);

  myfs_umount();
  
  return test_result(SUCCESS);
}

/* Test 3.2: Sample test case */
int test_3_2() {
  char buf[MAXREADWRITE+1];
  int fd, n;

  testname = TEST_3_2;

  FAIL_IF(Myfs_mount(SAMPLEDISK) < 0);

  log_("Opening file.");
  FAIL_IF((fd = Myfs_open(SAMPLEFILE)) < 0);

  log_("Reading content.");
  FAIL_IF((n = Myfs_read(fd, buf, MAXREADWRITE)) != strlen(SAMPLETEXT));

  log_("Closing file.");
  FAIL_IF(Myfs_close(fd) < 0);

  myfs_umount();
  
  buf[n] = '\0';
  FAIL_IF(strcmp(SAMPLETEXT, buf) != 0);
  
  return test_result(SUCCESS);
}

/* Test 4.1: Write file with 10 blocks */
int test_4_1() {
  int i, fd;
  long *data;

  testname = TEST_4_1;

  data = generate_data(TEN_BLOCK_DATA_COUNT);

  create_and_mount(TESTDISK4);

  log_("Creating file.");
  FAIL_IF((fd = Myfs_create(TESTFILE4)) < 0);

  log_("Writing content.");
  for (i=0; i < TEN_BLOCK_RW_COUNT; i++)
    FAIL_IF(Myfs_write(fd, (void *) &data[i * MAX_RW_DATA_COUNT], MAXREADWRITE) != MAXREADWRITE);

  log_("Closing file.");
  FAIL_IF(Myfs_close(fd) < 0);
  
  myfs_umount();
  free(data);

  return test_result(SUCCESS);
}

/* Test 4.2: Read file with 10 blocks */
int test_4_2() {
  int i, j, fd;
  long buf[MAX_RW_DATA_COUNT];
  long *data;

  testname = TEST_4_2;

  data = generate_data(TEN_BLOCK_DATA_COUNT);

  FAIL_IF(Myfs_mount(TESTDISK4) < 0);

  log_("Opening file.");
  FAIL_IF((fd = Myfs_open(TESTFILE4)) < 0);

  log_("Reading and comparing content.");
  for (i=0; i < TEN_BLOCK_RW_COUNT; i++) {
    FAIL_IF(Myfs_read(fd, (void *)buf, MAXREADWRITE) != MAXREADWRITE);

    for (j=0; j < MAX_RW_DATA_COUNT; j++)
      FAIL_IF(buf[j] != data[i * MAX_RW_DATA_COUNT + j]);
  }

  log_("Closing file.");
  FAIL_IF(Myfs_close(fd) < 0);
  
  myfs_umount();
  free(data);

  return test_result(SUCCESS);
}

/* Test 5.1: Seek, truncate, file size, write */
int test_5_1() {
  int fd;
  unsigned long data_offset, byte_offset;
  long buf[MAX_RW_DATA_COUNT];
  long *data;

  testname = TEST_5_1;
  
  data_offset = TEN_BLOCK_DATA_COUNT - 1;
  byte_offset = data_offset * sizeof(long);

  data = generate_data(TEN_BLOCK_DATA_COUNT);

  FAIL_IF(Myfs_mount(TESTDISK4) < 0);

  log_("Opening file.");
  FAIL_IF((fd = Myfs_open(TESTFILE4)) < 0);

  log_("Seeking.");
  FAIL_IF(Myfs_seek(fd, byte_offset) < 0);

  log_("Reading content.");
  FAIL_IF(Myfs_read(fd, (void *)buf, sizeof(long)) < 0);

  log_("Comparing content.");
  FAIL_IF(buf[0] != data[data_offset]);

  log_("Truncating file.");
  FAIL_IF(Myfs_truncate(fd, TRUNCATE_SIZE) < 0);

  log_("Checking truncated file size.");
  FAIL_IF(Myfs_filesize(fd) != TRUNCATE_SIZE);

  log_("Writing new content.");
  FAIL_IF(Myfs_write(fd, (void *)data, TRUNCATE_SIZE) < 0);

  /* File now contains first TRUNCATE_COUNT numbers twice */

  log_("Closing file.");
  FAIL_IF(Myfs_close(fd) < 0);
  
  myfs_umount();
  free(data);

  return test_result(SUCCESS);
}

/* Test 5.2: Read and compare */
int test_5_2() {
  int i, fd;
  long buf[MAX_RW_DATA_COUNT];
  long *data;

  testname = TEST_5_2;
  
  data = generate_data(TEN_BLOCK_DATA_COUNT);

  FAIL_IF(Myfs_mount(TESTDISK4) < 0);

  log_("Opening file.");
  FAIL_IF((fd = Myfs_open(TESTFILE4)) < 0);

  log_("Reading content.");
  FAIL_IF(Myfs_read(fd, (void *)buf, MAXREADWRITE) != 2 * TRUNCATE_SIZE);

  log_("Comparing content.");
  for (i=0; i < TRUNCATE_COUNT; i++)
    FAIL_IF(data[i] != buf[i] && data[i] != buf[2*i]);

  log_("Closing file.");
  FAIL_IF(Myfs_close(fd) < 0);
  
  myfs_umount();
  free(data);

  return test_result(SUCCESS);
}

/* Test 6.1: Multiple create/open/close, and writes */
int test_6_1() {
  int i, j;
  int fd[MULTI_FILE_COUNT];
  char filename[MULTI_FILE_COUNT][MAXFILENAMESIZE];
  long *data;

  testname = TEST_6_1;

  data = generate_data(TEN_BLOCK_DATA_COUNT);

  create_and_mount(TESTDISK6);

  log_("Creating files.");
  for (i=0; i < MULTI_FILE_COUNT; i++) {
    snprintf(filename[i], MAXFILENAMESIZE, "%s%d", TESTFILE, i+1);

    FAIL_IF((fd[i] = Myfs_create(filename[i])) < 0);
  }

  log_("Closing created files.");
  for (i=0; i < MULTI_FILE_COUNT; i++)
    FAIL_IF(Myfs_close(fd[i]) < 0);

  log_("Opening closed files.");
  for (i=0; i < MULTI_FILE_COUNT; i++)
    FAIL_IF((fd[i] = Myfs_open(filename[i])) < 0);

  log_("Writing content.");
  for (j=0; j < TEN_BLOCK_RW_COUNT / 2; j++) 
    for (i=0; i < MULTI_FILE_COUNT; i++) {
      FAIL_IF(Myfs_write(fd[i], (void *) &data[j * MAX_RW_DATA_COUNT + i], MAXREADWRITE) < 0);
    }

  log_("Closing files.");
  for (i=0; i < MULTI_FILE_COUNT; i++)
    FAIL_IF(Myfs_close(fd[i]) < 0);

  myfs_umount();
  free(data);

  return test_result(SUCCESS);
}

/* Test 6.2: Multiple reads */
int test_6_2() {
  int i, j, k;
  int fd[MULTI_FILE_COUNT];
  char filename[MULTI_FILE_COUNT][MAXFILENAMESIZE];
  long buf[MAX_RW_DATA_COUNT];
  long *data;

  testname = TEST_6_2;

  data = generate_data(TEN_BLOCK_DATA_COUNT);

  FAIL_IF(Myfs_mount(TESTDISK6) < 0);

  log_("Opening files.");
  for (i=0; i < MULTI_FILE_COUNT; i++) {
    snprintf(filename[i], MAXFILENAMESIZE, "%s%d", TESTFILE, i+1);

    FAIL_IF((fd[i] = Myfs_open(filename[i])) < 0);
  }

  log_("Reading and comparing content.");
  for (j=0; j < TEN_BLOCK_RW_COUNT / 2; j++) 
    for (i=0; i < MULTI_FILE_COUNT; i++) {
      FAIL_IF(Myfs_read(fd[i], (void *)buf, MAXREADWRITE) < 0);

      for (k=0; k < MAX_RW_DATA_COUNT; k++)
	FAIL_IF(buf[k] != data[j * MAX_RW_DATA_COUNT + i + k]);
    }

  log_("Closing files.");
  for (i=0; i < MULTI_FILE_COUNT; i++)
    FAIL_IF(Myfs_close(fd[i]) < 0);

  myfs_umount();
  free(data);

  return test_result(SUCCESS);
}

/* Main */

int main(int argc, char *argv[])
{
  int (*test_fp[])(void) = {
    test_1_1, test_1_2,
    test_2_1, test_2_2,
    test_3_1, test_3_2,
    test_4_1, test_4_2,
    test_5_1, test_5_2,
    test_6_1, test_6_2
  };

  if (argc != 2) {
    fprintf(stderr, "usage: %s test_number", argv[0]);
    return 1;
  }

  test_fp[atoi(argv[1])]();
  
  return 0;
}
