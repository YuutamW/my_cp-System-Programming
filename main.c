#include <stdio.h>  // used for I/O and perror
#include <stdlib.h> // used for malloc & free
#include <fcntl.h>  // contains the file control options(O_RDONLY,O_WRONLY,...)
#include <unistd.h> // Provides access to the POSIX operating system API, which includes the read(), write(), and close() system calls
#include <errno.h>  // allows us to check the errno variable against EINTR to properly handle interrupt signals if a read or write operation fails.
#include <time.h>   // internal timing measurements using clock_gettime() and the struct timespec data structure

/* Verify correct line arguments - returns boolean value */
int verifyArgs(int argc, char* argv[]){
    if(argc != 4){
        fprintf(stderr, "Usage: %s <source-file> <destination-file> <granularity_bytes>\n" , argv[0]);
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int srcFD , dstFD; /*file discriptors : integers representing open files*/
    size_t bytesRead, bytesWritten;
    if(!verifyArgs(argc,argv))  exit(EXIT_FAILURE);
}