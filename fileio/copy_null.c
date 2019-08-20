/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-1 */

/* copy_null.c

   Copy the file named argv[1] to a new file named in argv[2].
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
    int inputFd, outputFd, openFlags, endOffSet, apparentSize;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];
    endOffSet = 0;
    FILE *file;

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s old-file new-file\n", argv[0]);

    /* Open input and output files */

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", argv[1]);

    file = fdopen(inputFd, "r");
    if (file == NULL) {
        perror("Error opening input file descriptor");
        return (-1);
    }

    lseek(inputFd, 0, SEEK_SET);

    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1)
        errExit("opening file %s", argv[2]);


    /* Determine the file_size of the input file */
    if ((apparentSize = lseek(inputFd, 0, SEEK_END)) < 0) {
        errExit("Unable to determine accurate size of file");
        return (-1);
    }

    if (outputFd == -1)
        errExit("opening file %s", argv[2]);

    /* Create a file using ftruncate and lets carry on */
    if (ftruncate(outputFd, apparentSize) < 0) {
        errExit("Unable to create a file that will have a similar size to the original");
        return (-1);
    }

    /* Transfer data until we encounter end of input or an error */

    int i = 0;

    while ((numRead = fread(buf, sizeof(char), sizeof(buf), file)) > 0){
        for (i = 0; i < numRead; i++) {
            if (buf[i] == '\0'){
                endOffSet++;
            }else if (endOffSet > 0){
                lseek(outputFd, endOffSet, SEEK_CUR);
                if (write(outputFd, &buf[i], 1) != 1){
                    errExit("write\n");
                    break;
                }
                endOffSet = 0;
            }else{
                if (write(outputFd, &buf[i], sizeof(char)) != sizeof(char)){
                    fatal("couldn't write whole buffer");
                }
            }
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
