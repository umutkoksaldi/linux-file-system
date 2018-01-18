# linux-file-system

## Description
This is a simple file system implemented as a Linux library. It features most of the file operations provided by the system calls in C such as open, close, read, write and create. All file operations are done inside of a 128 MB virtual disk that can be created by executing ```myfs_create(char *diskname)```

## Use
A Makefile is provided in the repo, so directly clone the repo and run the command ```make``` from the terminal. Then, you can compile whatever program you want to link this library with, making sure to add ```-lmyfs``` as an argument to your compiler.

## Functionality Description
```int myfs_diskcreate (char * diskname)```

When the application calls this, a
Linux file called diskname that will act as a disk will be created. The size of
the Linux file will be 128 MB.

```int myfs_makefs(char *diskname)``` 

This will make a myfs file system on the
disk diskname. If success, returns 0, otherwise -1.

```int myfs_mount(char *diskname)```

This will mount the file system to be used by your program, initializing the inodes and unallocated disk block indices.

```int myfs_umount()```

Unmount the file system. Write back cached metada into disk, if any. After this operation, no file operation can be performed until the
file system is mounted again.

```int myfs_create(char *filename)```

Create a file with name filename and initialize its File Control Block

```int myfs_open(char *filename)```

Open file filename. Returns a non-negative
integer file descriptor (file handle) if success. Otherwise returns -1. The file
descriptor returned will be used by subsequent operations on the file.

```int myfs_close(int fd)```

Close the file whose descriptor (handle) is fd. If
success returns 0, otherwise returns -1.

```int myfs_delete(char *filename)```

Delete the file filename. If success returns 0,
otherwise returns -1. File must be closed before deletion, otherwise deletion
will fail.

```int myfs_read(int fd, void *buf, int n)```

Read n bytes from file fd. The read
bytes are put into an application buffer (array) buf whose size is at least n
characters (bytes). Space must have been allocated for buf by the application.
The function will return the number of bytes read (which can be less than n), if
success; otherwise it will return -1. File position pointer is advanced by the
number of bytes read. The max value of n is 1024.

```int myfs_write(int fd, void *buf, int n)```

Write n bytes from application buffer
buf into file fd. The file position pointer will be advanced by the number of
bytes written. If success, returns the number of bytes written, otherwise
returns -1. The max value of n is 1024.

```int myfs_truncate(int fd, int size) ```

Reduce the size of the file to size. If size of the file was
smaller or equal to the size parameter, the call has no effect. Returns 0 if
success, -1 if error. It is not possible to extend the file (increase file size) with
truncate operation.

```int myfs_seek(int fd, int offset)```

Set the file position pointer to offset. If offset
is bigger than or equal to the file size, file position pointer is set to the file
size.

```int myfs_filesize (int fd)```

Returns the current size of the file.

```void myfs_print_dir() ```

Print the names of the files in the root directory. One
filename per line.

```void myfs_print_blocks(filename) ```

Print block numbers of the blocks
allocated to file filename.