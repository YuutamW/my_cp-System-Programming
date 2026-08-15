/**
 * Assignement 1 - System Programming Course
 * Yotam Weintraub 
 * ID: 
 */


#include <stdio.h>   /* used for I/O and perror */
#include <stdlib.h>  /* used for malloc & free */
#include <fcntl.h>   /* contains the file control options(O_RDONLY,O_WRONLY,...) */
#include <unistd.h>  /* Provides access to the POSIX operating system API, which includes the read(), write(), and close() system calls */
#include <errno.h>   /* allows us to check the errno variable against EINTR to properly handle interrupt signals if a read or write operation fails. */
#include <time.h>    /* internal timing measurements using clock_gettime() and the struct timespec data structure */

#define ALLOC_BUFF(gran_val)   (char*)malloc(gran_val)    
#define FREE_BUFF(buff) free(buff)

/* checks if the passed 'int' is positive-nonZero if not-returns -1 */
int granParamValidation(int gran_val){   
    if(gran_val > 0) return gran_val;
    fprintf(stderr, "Granularity value: %d must be a positive value\n" , gran_val);
    return -1;
}

/* Verify correct line arguments - returns gran value if valid */
int verifyArgs(int argc, char* argv[]){
    if(argc != 4){
        fprintf(stderr, "Usage: %s <source-file> <destination-file> <granularity_bytes>\n" , argv[0]);
        return -1;
    }
    else if( argv && argv[3] ) {
        char *endPtr;
        long val = strtol(argv[3] , &endPtr, 10); /* convert The granularity arg from string to int in base 10 */ 
        
        if(endPtr == argv[3]) { /* Check the validity of the passed granularity argument */
            fprintf(stderr, "No digits found in <granularity_bytes>: %s Granularity argument must be an integer value\n", argv[3]);
            return -1;
        }
        else    /* If the argument was passed correctly, we check if the passed 'int' is positive-nonZero if not-return -1 */
            return granParamValidation(val);
    }
    else return -1;
}

int main(int argc, char* argv[]) {
    int srcFD , dstFD; /*file discriptors : integers representing open files*/
    ssize_t bytesRead, bytesWritten, totalWritten;
    int granularity_bytes;
    if((granularity_bytes = verifyArgs(argc,argv)) == -1)  exit(EXIT_FAILURE); /* parses through the line args sets gran value when passed correctly */
    

    /* Allocating Buffer by granularity param */
    char* buff;
    if(!(buff = ALLOC_BUFF(granularity_bytes))) { 
        perror("Memory allocation failed, exiting program");
        exit(EXIT_FAILURE);
    }
    

    /* Opening source file with owner read permisions only. */
   srcFD = open(argv[1], O_RDONLY);
   if(srcFD < 0) {
    perror("Failed to open the src file!");
    FREE_BUFF(buff);
    exit(EXIT_FAILURE);
   }

   
   /** Open the destination file, If exists truncates it. if not creates a new file with owner can read/write, others can read permissions
    O_WRONLY: open for writing. = 0000 0001
    O_CREAT: create the file if doesnt exist = 0000 0100 
    O_TRUNC: if the file exists wipe its content (truncate to 0) = 0001 0000
    0644: permissions(owner can read/write, others can read);
    0 = 'This is an octal number!' 
    4 = 'r' , 2 = 'w'. therefore: 
    USER - 4 + 2 = 6 : user can read+write rw-
    Group - 4 : anyone in your user group can read r--
    Other(world) - 4 : anyone else on the system can only read r--
    therefeore: 0644 = -rw-r--r--
    */  
   dstFD = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
   if(dstFD < 0) {
    perror("Error opening destination file!");
    FREE_BUFF(buff);
    close(srcFD);
    exit(EXIT_FAILURE);
   }


   /**  read is a sysCall that requests from the krnl to cpy data from the file into the bufer.
        The krnl puts the process to sleep until the disk/fileSystem dlivers the data.
   */
  while((bytesRead = read(srcFD, buff, granularity_bytes)) != 0 ) {
    if(bytesRead == -1) {
        if(errno == EINTR)  /* The read was interrupted by a signal, try again! */
            continue;
        else {
            /* A real error occurred, print it and exit */
            perror("Read failed");
            FREE_BUFF(buff);
            close(srcFD);
            close(dstFD);
            exit(EXIT_FAILURE);
        }
    }
    totalWritten = 0;   /* This is a variable to track progress of written bytes for each chunk we need to write in the current granularity batch */
    while(totalWritten < bytesRead) {
        if((bytesWritten = write(dstFD , buff + totalWritten, bytesRead - totalWritten)) == -1) { /* sums up total written to 'bytesWriten' while checking for failure */
            if(errno == EINTR) continue; /* a signal has interrupted the current chunk, nothing to report - continues */
            else {
                /* A real error occurred, print it and exit */
                perror("Write failed");
                close(srcFD);
                close(dstFD);
                FREE_BUFF(buff);
                exit(EXIT_FAILURE);
            }
        } else {
            totalWritten += bytesWritten;
        }
    } 
  }

    close(srcFD);
    close(dstFD);
    FREE_BUFF(buff);
    return EXIT_SUCCESS;
}