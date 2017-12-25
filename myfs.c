#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "myfs.h"

// Global Variables
char disk_name[128];   // name of virtual disk file
int  disk_size;        // size in bytes - a power of 2
int  disk_fd;          // disk file handle
int  disk_blockcount;  // block count on disk
int blockIndex;

char **fileAllocations;
int **fileBlockNumbers;
int *open_file_table;


/* 
   Reads block blocknum into buffer buf.
   You will not modify the getblock() function. 
   Returns -1 if error. Should not happen.
*/
int getblock (int blocknum, void *buf)
{      
	int offset, n; 
	
	if (blocknum >= disk_blockcount) 
		return (-1); //error

	offset = lseek (disk_fd, blocknum * BLOCKSIZE, SEEK_SET); 
	if (offset == -1) {
		printf ("lseek error\n"); 
		exit(0); 

	}

	n = read (disk_fd, buf, BLOCKSIZE); 
	if (n != BLOCKSIZE) 
		return (-1); 

	return (0); 
}


/*  
    Puts buffer buf into block blocknum.  
    You will not modify the putblock() function
    Returns -1 if error. Should not happen. 
*/
int putblock (int blocknum, void *buf)
{
	int offset, n;
	
	if (blocknum >= disk_blockcount) 
		return (-1); //error

	offset = lseek (disk_fd, blocknum * BLOCKSIZE, SEEK_SET);
	if (offset == -1) {
		printf ("lseek error\n"); 
		exit (1); 
	}
	
	n = write (disk_fd, buf, BLOCKSIZE); 
	if (n != BLOCKSIZE) 
		return (-1); 

	return (0); 
}


/* 
   IMPLEMENT THE FUNCTIONS BELOW - You can implement additional 
   internal functions. 
 */


int myfs_diskcreate (char *vdisk)
{
	disk_fd = open(vdisk, O_CREAT | O_RDWR, 0666);
	int ret = ftruncate(disk_fd, 1000*1000*128);

	return ret; 
}


/* format disk of size dsize */
int myfs_makefs(char *vdisk)
{
	strcpy (disk_name, vdisk); 
	disk_size = DISKSIZE; 
	disk_blockcount = disk_size / BLOCKSIZE; 

	disk_fd = open (disk_name, O_RDWR); 
	if (disk_fd == -1) {
		printf ("disk open error %s\n", vdisk); 
		exit(1); 
	}
	
	// perform your format operations here. 
	printf ("formatting disk=%s, size=%d\n", vdisk, disk_size); 
	// initialize the names of files allocated
	fileAllocations = (char**) malloc(sizeof(char*) * MAXFILECOUNT);
	for (int i = 0; i < MAXFILECOUNT; i++) {
		fileAllocations[i] = (char*) malloc(sizeof(char) * 32);
	}
	for (int i = 0; i < MAXFILECOUNT; i++) {
		fileAllocations[i] = "";
	}
	// initialize the file locations in a 2d array
	fileBlockNumbers = (int **) malloc(sizeof(char*) * MAXFILECOUNT);
	for (int i = 0; i < MAXFILECOUNT; i++) {
		fileBlockNumbers[i] = (int*) malloc(sizeof(int) * 1024);
	}
	for (int i = 0; i < MAXFILECOUNT; i++) {
		for(int j = 0; j < 1024; j++) {
			fileBlockNumbers[i][j] = 0;
		}
	}
	// put the FAT into the disk
	blockIndex = 0;
	for(int i = 0; i < MAXFILECOUNT; i++) {
		char *buf = fileAllocations[i];
		putblock(blockIndex, (void*) buf);
		blockIndex++;
	}
	// put block numbers into the disk
	for(int i = 0; i < MAXFILECOUNT; i++) {
		int *buf = (int*) malloc(sizeof(int) * 1024);
		for(int j = 0; j < 1024; j++) {
			buf[j] = fileBlockNumbers[i][j];
		}
		putblock(blockIndex, (void *)buf);
		blockIndex++;
	}

	fsync (disk_fd); 
	close (disk_fd); 

	return (0); 
}

/* 
   Mount disk and its file system. This is not the same mount
   operation we use for real file systems:  in that the new filesystem
   is attached to a mount point in the default file system. Here we do
   not do that. We just prepare the file system in the disk to be used
   by the application. For example, we get FAT into memory, initialize
   an open file table, get superblock into into memory, etc.
*/

int myfs_mount (char *vdisk)
{
	struct stat finfo; 

	strcpy (disk_name, vdisk);
	disk_fd = open (disk_name, O_RDWR); 
	if (disk_fd == -1) {
		printf ("myfs_mount: disk open error %s\n", disk_name); 
		exit(1); 
	}
	
	fstat (disk_fd, &finfo); 

	printf ("myfs_mount: mounting %s, size=%d\n", disk_name, 
		(int) finfo.st_size);  
	disk_size = (int) finfo.st_size; 
	disk_blockcount = disk_size / BLOCKSIZE; 

	// perform your mount operations here

	// write your code

	// initialize open file table
	open_file_table = (int*) malloc(sizeof(int) * MAXFILECOUNT);
	for(int i = 0; i < MAXFILECOUNT; i++) {
		open_file_table[i] = 0;
	}
	// read file allocations from memory
	fileAllocations = (char**) malloc(sizeof(char*) * MAXFILECOUNT);
	for (int i = 0; i < MAXFILECOUNT; i++) {
		fileAllocations[i] = (char*) malloc(sizeof(char) * 32);
	}
	blockIndex = 0;
	for (int i = 0; i < MAXFILECOUNT; i++) {
		void *buf = malloc(sizeof(char) * 32);
		if (getblock(blockIndex, buf) != 0) {
			printf("get block failed\n");
			exit(1);
		}
		char *temp = (char*) buf;
		fileAllocations[i] = temp;
		blockIndex++;
	}
	// read file block numbers from memory
	fileBlockNumbers = (int **) malloc(sizeof(char*) * MAXFILECOUNT);
	for (int i = 0; i < MAXFILECOUNT; i++) {
		fileBlockNumbers[i] = (int*) malloc(sizeof(int) * 1024);
	}
	for(int i = 0; i < MAXFILECOUNT; i++ ) {
		void *buf = malloc(sizeof(int) * 1024);
		if (getblock(blockIndex, buf) != 0) {
			printf("get block failed\n");
			exit(1);
		}
		int* temp = (int*) buf;
		for (int j = 0; j < 1024; j++) {
			fileBlockNumbers[i][j] = temp[j];
		}
		blockIndex++;
	}
	
	
	/* you can place these returns wherever you want. Below
	   we put them at the end of functions so that compiler will not
	   complain.
        */
  	return (0); 
}


int myfs_umount()
{
	// perform your unmount operations here

	// write your code

	fsync (disk_fd); 
	close (disk_fd); 
	return (0); 
}


/* create a file with name filename */
int myfs_create(char *filename)
{

	// write your code 

	return (0); 
}


/* open file filename */
int myfs_open(char *filename)
{
	int index = -1; 
	
	// write your code
       
	return (index); 
}

/* close file filename */
int myfs_close(int fd)
{

	// write your code

	return (0); 
}

int myfs_delete(char *filename)
{

	// write your code

	return (0); 
}

int myfs_read(int fd, void *buf, int n)
{
	int bytes_read = -1; 

	// write your code
	
	return (bytes_read); 

}

int myfs_write(int fd, void *buf, int n)
{
	int bytes_written = -1; 

	// write your code

	return (bytes_written); 
} 

int myfs_truncate(int fd, int size)
{

	// write your code

	return (0); 
} 


int myfs_seek(int fd, int offset)
{
	int position = -1; 

	// write your code

	return (position); 
} 

int myfs_filesize (int fd)
{
	int size = -1; 
	
	// write your code

	return (size); 
}


void myfs_print_dir ()
{

	// write your code
	
}


void myfs_print_blocks (char *  filename)
{

	// write your code

}


