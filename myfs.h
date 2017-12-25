#ifndef MYFS_H
#define MYFS_H

#define BLOCKSIZE          4096     // bytes
#define MAXFILECOUNT       128      // files
#define DISKSIZE         (1<<27)  // 256 MB
#define MAXFILENAMESIZE    32  // characters - max that FS can support
#define BLOCKCOUNT      (DISKSIZE / BLOCKSIZE)
#define MAXOPENFILES       64      // files
#define MAXREADWRITE      1024     // bytes; max read/write amount

// The following will be use to create and format a disk
int myfs_diskcreate(char *diskname);
int myfs_makefs (char *diskname);

// The following will be used by a program to work with files
int myfs_mount (char *vdisk);
int myfs_umount();

int myfs_create(char *filename);
int myfs_open(char *filename);
int myfs_close(int fd);
int myfs_delete(char *filename);
int myfs_read(int fd, void *buf, int n);
int myfs_write(int fd, void *buf, int n);
int myfs_truncate(int fd, int size);
int myfs_seek(int fd, int offset);
int myfs_filesize(int fd);
void myfs_print_dir();
void myfs_print_blocks(char *filename);

#endif
