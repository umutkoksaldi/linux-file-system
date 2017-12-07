#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>

#include "myfs.h"

int main (int argc, char *argv[])
{
	int n, size ,ret, i;
	int fd;  
	char vdiskname[128]; 
	char buf[BLOCKSIZE];     
	int numblocks = 0; 

	if (argc != 2) {
		printf ("usage: createdisk <vdiskname>"); 
		exit (1); 
	}

	strcpy (vdiskname, argv[1]); 
        size = DISKSIZE; 
	numblocks = DISKSIZE / BLOCKSIZE; 

	printf ("diskname=%s size=%d blocks=%d\n", 
		vdiskname, size, numblocks); 
       
	ret = open (vdiskname,  O_CREAT | O_RDWR, 0666); 	
	if (ret == -1) {
		printf ("could not create disk\n"); 
		exit(1); 
	}
	
	bzero ((void *)buf, BLOCKSIZE); 
	fd = open (vdiskname, O_RDWR); 
	for (i=0; i < (size / BLOCKSIZE); ++i) {
		printf ("block=%d\n", i); 
		n = write (fd, buf, BLOCKSIZE); 
		if (n != BLOCKSIZE) {
			printf ("write error\n"); 
			exit (1); 
		}
	}	
	close (fd); 
	
	printf ("created a virtual disk=%s of size=%d\n", vdiskname, size);	
	return (0);
}
