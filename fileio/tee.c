/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-3 */

/* tee.c

   Demonstrate the use of lseek() and file I/O system calls.

   Usage: tee file {r<length>|R<length>|w<string>|s<offset>}...

   This program opens the file named on its command line, and then performs
   the file I/O operations specified by its remaining command-line arguments:

           r<length>    Read 'length' bytes from the file at current
                        file offset, displaying them as text.

           R<length>    Read 'length' bytes from the file at current
                        file offset, displaying them in hex.

           w<string>    Write 'string' at current file offset.

           s<offset>    Set the file offset to 'offset'.

   Example:

        tee myfile wxyz s1 r2
*/

#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

int
main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags, openFlagsBis, opt;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];

    int optionCount = 0;
    if (argc > 4 || strcmp(argv[1], "--help") == 0)
        usageErr("%s old-file new-file\n", argv[0]);
    if (argc == 4 && strcmp(argv[1], "-a") == 0){
	optionCount = 1;
    }else if (argc == 4){
        errExit("wrong argument name");
    }

    /* Open input and output files */

    inputFd = open(argv[optionCount + 1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", argv[optionCount + 1]);

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    openFlagsBis = O_WRONLY | O_APPEND;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */

    if (argc == 3){
        outputFd = open(argv[2], openFlags, filePerms);
    }

    while ((opt = getopt(argc, argv, "a")) != -1) {
	    switch(opt){
                case 'a':
    	            outputFd = open(argv[3], openFlagsBis, filePerms);
	    }
    }

    if (outputFd == -1)
        errExit("opening file %s", argv[2]);

    /* Transfer data until we encounter end of input or an error */

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0){
        if (write(outputFd, buf, numRead) != numRead){
            fatal("couldn't write whole buffer");
	}else{
            printf("%s", buf);    
	}
    }

    if (numRead == -1)
        errExit("read");

    if (close(inputFd) == -1)
        errExit("close input");
    if (close(outputFd) == -1)
        errExit("close output");

    exit(EXIT_SUCCESS);
}
