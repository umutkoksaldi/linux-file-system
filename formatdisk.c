
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "myfs.h"



int main (int argc, char *argv[])
{
	char vdiskname [128]; 

	if (argc != 2) {
		printf ("usage: formatdisk <vdiskname>\n"); 
		exit (1); 
	}

	strcpy (vdiskname, argv[1]); 
	myfs_makefs (vdiskname); 
	return (0); 
}
